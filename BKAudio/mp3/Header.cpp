#include "Header.h"

const int BKMP3_Header::bitrate[2][3][15] =
{
	{
		//MPEG-1
		//Layer I
		{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
		//Layer II
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
		//Layer III
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }
	},
	{
		//MPEG-2/2.5
		//Layer I
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 },
		//Layer II
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },
		//Layer III
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }
	}
};

const int BKMP3_Header::samplingRate[4][4] =
{
	{ 11025, 12000, 8000, 0 },	//MPEG-2.5
	{ 0, 0, 0, 0, },			//reserved
	{ 22050, 24000, 16000, 0 },	//MPEG-2 (ISO/IEC 13818-3)
	{ 44100, 48000, 32000, 0 }	//MPEG-1 (ISO/IEC 11172-3)
};

void BKMP3_Header::initialize()
{
	layer = sideinfosize = framesize = 0;
	verID = 1;
}

void BKMP3_Header::decode(int h)
{
	verID = (h >> 19) & 3;
	layer = 4 - ((h >> 17) & 3);
	protection_bit = (h >> 16) & 0x1;
	bitrate_index = (h >> 12) & 0xF;
	sampling_frequency = (h >> 10) & 3;
	padding = (h >> 9) & 0x1;
	mode = (h >> 6) & 3;
	mode_extension = (h >> 4) & 3;

	m_isMS = mode == 1 && (mode_extension & 2) != 0;
	isIntensity = mode == 1 && (mode_extension & 0x1) != 0;
	lsf = (verID == MPEG1) ? 0 : 1;

	switch (layer) {
	case 1:
		framesize = bitrate[lsf][0][bitrate_index] * 12000;
		framesize /= samplingRate[verID][sampling_frequency];
		framesize += padding;
		framesize <<= 2; // 1-slot = 4-byte
		break;
	case 2:
		framesize = bitrate[lsf][1][bitrate_index] * 144000;
		framesize /= samplingRate[verID][sampling_frequency];
		framesize += padding;
		break;
	case 3:
		framesize = bitrate[lsf][2][bitrate_index] * 144000;
		framesize /= samplingRate[verID][sampling_frequency] << lsf;
		framesize += padding;

		//计算帧边信息长度
		if (verID == MPEG1)
			sideinfosize = (mode == 3) ? 17 : 32;
		else
			sideinfosize = (mode == 3) ? 9 : 17;
		break;
	}

	//计算主数据长度
	maindatasize = framesize - 4 - sideinfosize;

	if (protection_bit == 0)
		maindatasize -= 2;	//CRC-word
}

bool BKMP3_Header::isProtected()
{
	return (protection_bit == 0);
}

bool BKMP3_Header::isMS()
{
	return m_isMS;
}

bool BKMP3_Header::isIntensityStereo()
{
	return isIntensity;
}

int BKMP3_Header::getBitrate()
{
	return bitrate[lsf][layer - 1][bitrate_index];
}

int BKMP3_Header::getBitrateIndex()
{
	return bitrate_index;
}

int BKMP3_Header::getChannels()
{
	return (mode == 3) ? 1 : 2;
}
int BKMP3_Header::getMode()
{
	return mode;
}
int BKMP3_Header::getModeExtension()
{
	return mode_extension;
}
int BKMP3_Header::getVersion() {
	return verID;
}
int BKMP3_Header::getLayer() {
	return layer;
}
int BKMP3_Header::getSamplingFrequency() {
	return sampling_frequency;
}
int BKMP3_Header::getSamplingRate() {
	return samplingRate[verID][sampling_frequency];
}
int BKMP3_Header::getMainDataSize() {
	return maindatasize;
}
int BKMP3_Header::getSideInfoSize() {
	return sideinfosize;
}
int BKMP3_Header::getFrameSize() {
	return framesize;
}
int BKMP3_Header::getPcmSize() {
	int pcmsize = (verID == MPEG1) ? 4608 : 2304;
	if (mode == 3) // if channels == 1
		pcmsize >>= 1;
	return pcmsize;
}
float BKMP3_Header::getFrameDuration() {
	return ((float)1152 / (getSamplingRate() << lsf));
}