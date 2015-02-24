#include "MP3Decoder.h"
#include "Layer3.h"
#include "header.h"
#include <algorithm>
#include <vector>

struct BKMP3_IndexCache
{
	std::vector<int> cache;
	static const int step = 8;
	void add(int frame, int fileoffset);
	int get(int frame, int &fileoffset);
};

BKMP3_Decoder::BKMP3_Decoder(IReader *fd)
	:fd(fd)
	, layer(nullptr)
{
	header = new BKMP3_Header();
	indexCache = new BKMP3_IndexCache();
	
	m_iHeaderMask = 0xffe00000;
	m_bSync = false;
	m_iCurrentFrame = -1;
	m_iIgnoreFrameCount = 0;
	m_iIgnorePCMSizeCount = 0;
	m_iFileOffset = 0;
	//endPos = pos = buf->length;
	resetBuffer();
	header->initialize();

	nextFrameHeader();

	switch (header->getLayer()) {
		/*
		case 1:
		layer = new Layer1(&header);
		break;
		case 2:
		layer = new Layer2(&header);
		break;
		*/
	case 3:
		layer = new BKMP3_Layer3(header);
		break;
	default:
		m_bEOF = true;
	}


	return;
}

BKMP3_Decoder::~BKMP3_Decoder()
{
	delete header;
	delete layer;
	delete indexCache;
}

int BKMP3_Decoder::decodeOneFrame(char **outPtr, int *size)
{
	if (!m_bEOF)
	{
		while (m_iIgnoreFrameCount)
		{
			pos = layer->ignoreOneFrame(buf, pos, true);
			nextFrameHeader();
			m_iIgnoreFrameCount--;
			if (m_bEOF)
			{
				goto _failed;
			}
		}
		pos = layer->decodeOneFrame(buf, pos);
		layer->outputPCM(outPtr, size, m_iIgnorePCMSizeCount);
		nextFrameHeader();
	}
	else
	{
_failed:
		*outPtr = NULL;
		*size = NULL;
	}
	return m_bEOF;
}

void BKMP3_Decoder::seek(double s)
{
	double dpf = header->getFrameDuration();
	int frame = (int)(s / dpf);
	m_iIgnorePCMSizeCount = s / dpf * header->getPcmSize() - frame * header->getPcmSize();
	if (m_iIgnorePCMSizeCount < 0)
	{
		m_iIgnorePCMSizeCount = 0;
	}

	//比预计位置后退 sDefaultIgnoreCount 帧来保证正常解码
	int dstFrame = frame - sDefaultIgnoreCount;
	if (dstFrame < 0)
	{
		dstFrame = 0;
	}

	if (m_iCurrentFrame == frame)
	{
		return;
	}
	else if (m_iCurrentFrame < frame && dstFrame <= m_iCurrentFrame)
	{
		m_iIgnoreFrameCount = frame - m_iCurrentFrame;
		//这时候什么都不干，继续解码就好了
	}
	else
	{
		m_iIgnoreFrameCount = frame - dstFrame;
		int fileoffset;
		int closeFrame = indexCache->get(dstFrame, fileoffset);

		m_iFileOffset = fd->seek(fileoffset, SEEK_SET);
		resetBuffer();
		updateBuffer();
		m_iCurrentFrame = closeFrame - 1;
		while (m_iCurrentFrame < dstFrame)
		{
			if (m_bFrameFound)
			{
				ignoreThisFrame();
			}
			nextFrameHeader();
		}
	}
}

int BKMP3_Decoder::getChannelCount() const
{
	return header->getChannels();
}

int BKMP3_Decoder::getSamplingRate() const
{
	return header->getSamplingRate();
}

static int byte2int(char b[], int off) {
	int val = b[off] << 24;
	val |= (b[off + 1] & 0xff) << 16;
	val |= (b[off + 2] & 0xff) << 8;
	val |= (b[off + 3] & 0xff);
	return val;
}

static bool available(int h, int mask)
{
	return (h & mask) == mask
		&& ((h >> 19) & 3) != 1		// version ID:  '01' - reserved
		&& ((h >> 17) & 3) != 0		// Layer index: '00' - reserved
		&& ((h >> 12) & 15) != 15	// Bitrate Index: '1111' - reserved
		&& ((h >> 12) & 15) != 0	// Bitrate Index: '0000' - free
		&& ((h >> 10) & 3) != 3;	// Sampling Rate Index: '11' - reserved
}

#define CHECKPOS(_s) if(endPos - pos <= (_s)) { \
	if (m_bIOEOF){ \
		m_bEOF = true; \
		return; \
	} \
	updateBuffer(); \
	if ( m_bIOEOF || endPos - pos <= (_s)){ \
		m_bEOF = true; \
		return; \
	} \
}

void BKMP3_Decoder::nextFrameHeader()
{
	int h, framesize, mask = 0;
	int skipped;

	CHECKPOS(4);

	h = byte2int(buf, pos);
	pos += 4;
	skipped = 4;

	while (!m_bEOF) {
		//1. 查找帧同步头
		while (!available(h, m_iHeaderMask)) {
			CHECKPOS(0);
			h = (h << 8) | (buf[pos++] & 0xff);
			skipped++;
			m_bSync = false;
		}

		//2. 帧头解码
		header->decode(h);

		framesize = header->getFrameSize();
		CHECKPOS(framesize);

		if (m_bSync)	// version ID等帧的特征未改变,不用与下一帧的同步头比较
			break;

		//3. 与下一帧的同步头比较,确定是否找到有效的同步字.
		
		mask = 0xffe00000; 		// syncword
		mask |= h & 0x180000;	// version ID
		mask |= h & 0x60000;	// Layer index
		mask |= h & 0xc00;		// sampling_frequency
		// mode, mode_extension 不是每帧都相同.

		if (available(byte2int(buf, pos - 4 + framesize), mask)) {
			if (m_iHeaderMask == 0xffe00000) {
				m_iHeaderMask = mask;
			}
			m_bSync = true;
			break; // 找到有效的帧同步字段，结束查找
		}

		//4. 移动到下一字节，继续重复1-3
		CHECKPOS(0);
		h = (h << 8) | (buf[pos++] & 0xff);
		skipped++;
	}

	if (header->isProtected()){
		pos += 2; // CRC-word
		skipped += 2;
	}
		
	m_bFrameFound = true;
	m_iCurrentFrame++;
	indexCache->add(m_iCurrentFrame, m_iFileOffset - endPos + pos - skipped);

	return ;
}

void BKMP3_Decoder::ignoreThisFrame()
{
	pos = layer->ignoreOneFrame(buf, pos, false);
}

void BKMP3_Decoder::updateBuffer()
{
	int len, chunk = 0;
	if ((len = endPos - pos) > 0)
		memcpy(buf + 0, buf + pos, len);
	//System.arraycopy(buf, pos, buf, 0, len);
	endPos = len + fd->read(buf + len, pos);
	chunk += pos;
	pos = 0;
	if (endPos <= len || chunk > 0x10000){//64K
		m_bIOEOF = true;
		return;
	}
	m_iFileOffset += endPos - len;
	return;
}

void BKMP3_Decoder::resetBuffer()
{
	endPos = pos = 8192;
	m_bEOF = false;
	m_bIOEOF = false;
	m_bFrameFound = false;
}

void BKMP3_IndexCache::add(int frame, int fileoffset)
{
	//从1开始 变换为 从0开始
	if (frame % step)
	{
		return;
	}
	frame /= step;
	if (frame >= (int)this->cache.size())
	{
		this->cache.resize(frame + 1);
	}
	this->cache[frame] = fileoffset;
}

int BKMP3_IndexCache::get(int frame, int &fileoffset)
{
	frame /= step;
	if (frame >= this->cache.size())
	{
		fileoffset = this->cache.back();
		return (this->cache.size()-1)*step;
	}
	fileoffset = this->cache[frame];
	return frame * step;
}