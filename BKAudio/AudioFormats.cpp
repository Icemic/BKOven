#include "AudioFormats.h"
#include <iostream>
#include <cerrno>
#include <QDebug>

bool stdioSeek(QIODevice *stream, bklong offset, int direction){
    switch (direction){
    case SEEK_CUR:
        offset += stream->pos();
        break;
    case SEEK_END:
        offset = stream->size() - offset;
        break;
    case SEEK_SET:
        break;
    default:
        break;
    }
    return stream->seek(offset);
}

size_t ogg_read(void *buffer,size_t size,size_t nmemb,void *s){
    QIODevice *stream=(QIODevice *)s;
    qint64 r = stream->read((char*)buffer, size*nmemb);
    if (r < 0){
		errno=1;
		return 0;
	}
	errno=0;
	return r/size;
}

int ogg_seek(void *s,ogg_int64_t offset,int whence){
    QIODevice *stream=(QIODevice *)s;
    stdioSeek(stream,offset,whence);
	return 0;
}

long ogg_tell(void *s){
    return (long)((QIODevice *)s)->pos();
}

QString ogg_code_to_string(int e){
	switch (e){
		case 0:
			return "[OGG Decoder]no error\n";
		case OV_EREAD:
			return "[OGG Decoder]a read from media returned an error\n";
		case OV_ENOTVORBIS:
			return "[OGG Decoder]bitstream does not contain any Vorbis data\n";
		case OV_EVERSION:
			return "[OGG Decoder]vorbis version mismatch\n";
		case OV_EBADHEADER:
			return "[OGG Decoder]invalid Vorbis bitstream header\n";
		case OV_EFAULT:
			return "[OGG Decoder]internal logic fault; indicates a bug or heap/stack corruption\n";
		default:
			return "[OGG Decoder]unknown error\n";
	}
}

ogg_decoder::ogg_decoder(const QString &file):decoder(file){
	if (!*this)
		return;
	this->bitstream=0;
	ov_callbacks cb;
	cb.read_func=ogg_read;
	cb.seek_func=ogg_seek;
	cb.tell_func=ogg_tell;
	cb.close_func=0;
    int error=ov_open_callbacks(stream,&this->file,0,0,cb);
	if (error<0){
        qWarning()<<ogg_code_to_string(error);
		this->good=0;
		return;
	}
}

ogg_decoder::~ogg_decoder(){
	if (!*this)
		return;
	ov_clear(&this->file);
}

audio_buffer *ogg_decoder::get_buffer(bool &error){
	error = 0;
	vorbis_info *i = ov_info(&this->file, this->bitstream);
	const size_t n=1<<12;
	char *temp=(char *)audio_buffer::allocate(n/4,2,2);
	size_t size=0;
	while (size<n){
        int r=ov_read(&this->file,temp+size,n-size,0,2,1,&this->bitstream);
		if (r<0){
			error=1;
			audio_buffer::deallocate(temp);
			return 0;
		}
		if (!r){
			if (!size){
				audio_buffer::deallocate(temp);
				return 0;
			}
			break;
		}
		size+=r;
	}
	return new audio_buffer(temp, size / (i->channels * 2), i->rate, i->channels, 16, 0);
}

void ogg_decoder::seek(double loopto){
	ov_time_seek(&this->file,loopto / 1000.0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//opus

#ifdef USE_OPUS
// Convert an opus error code to a string.
const char *opus_error_to_string(int error)
{
	switch (error)
	{
		case OP_FALSE:
			return "OP_FALSE: A request did not succeed.";
		case OP_HOLE:
			return "OP_HOLE: There was a hole in the page sequence numbers.";
		case OP_EREAD:
			return "OP_EREAD: An underlying read, seek or tell operation failed.";
		case OP_EFAULT:
			return "OP_EFAULT: A NULL pointer was passed where none was expected, or an internal library error was encountered.";
		case OP_EIMPL:
			return "OP_EIMPL: The stream used a feature which is not implemented.";
		case OP_EINVAL:
			return "OP_EINVAL: One or more parameters to a function were invalid.";
		case OP_ENOTFORMAT:
			return "OP_ENOTFORMAT: This is not a valid Ogg Opus stream.";
		case OP_EBADHEADER:
			return "OP_EBADHEADER: A required header packet was not properly formatted.";
		case OP_EVERSION:
			return "OP_EVERSION: The ID header contained an unrecognised version number.";
		case OP_EBADPACKET:
			return "OP_EBADPACKET: An audio packet failed to decode properly.";
		case OP_EBADLINK:
			return "OP_EBADLINK: We failed to find data we had seen before or the stream was sufficiently corrupt that seeking is impossible.";
		case OP_ENOSEEK:
			return "OP_ENOSEEK: An operation that requires seeking was requested on an unseekable stream.";
		case OP_EBADTIMESTAMP:
			return "OP_EBADTIMESTAMP: The first or last granule position of a link failed basic validity checks.";
		default:
			return "Unknown error.";
	}
}

int opus_read(void *_stream, unsigned char *_ptr, int _nbytes){
	BKE_DataStream *stream = (BKE_DataStream *)_stream;
	size_t bytes_read;
	if (!stream->read(_ptr, bytes_read, _nbytes)){
		errno = 1;
		return 0;
	}
	errno = 0;
	return bytes_read;
}

int opus_seek(void *_stream, opus_int64 _offset, int _whence){
	BKE_DataStream *stream = (BKE_DataStream *)_stream;
	stream->stdioSeek(_offset, _whence);
	return 0;
}

opus_int64 opus_tell(void *_stream){
	return (long)((BKE_DataStream *)_stream)->seek(0, 0);
}

opus_decoder::opus_decoder(BKE_DataStream *stream) :decoder(stream){
	if (!*this)
		return;
	OpusFileCallbacks cb;
	cb.read = opus_read;
	cb.seek = opus_seek;
	cb.tell = opus_tell;
	cb.close = NULL;

	int error = 0;
	// Open the file.
	file = op_open_callbacks(stream, &cb, NULL, 0, &error);
	if (error != 0)
	{
		o_stderr << L"播放" << stream->getName() << "文件失败：(" << (long)error << L")" << opus_error_to_string(error);
		good = 0;
		return;
	}
}

audio_buffer *opus_decoder::get_buffer(bool &error){
	const int buffer_size = 960 * 2 * 2;
	int16_t *buf = (int16_t *)malloc(buffer_size);

	int samples_read = 0;

	int num_channels = op_channel_count(file, -1);

	while (samples_read < buffer_size)
	{
		// op_read returns number of samples read (per channel), and accepts number of samples which fit in the buffer, not number of bytes.
		int ns = op_read(file, buf + samples_read*num_channels, (buffer_size - samples_read*num_channels), 0);
		if (ns < 0)
		{
			o_stderr << "Opus : 在" << samples_read << L"处解码错误：(" << ns << L")" << opus_error_to_string(ns);
			error = 1;
			return NULL;
		}
		if (ns == 0) break;
		samples_read += ns;
	}
	error = 0;
	return new audio_buffer(buf, samples_read / (num_channels * 2), 48000, num_channels, 16, 1);
}

opus_decoder::~opus_decoder()
{
	op_free(file);
}

void opus_decoder::seek(double loopto)
{
	uint64_t offest;
	offest = 2 * 48000 * op_channel_count(file, -1) * loopto /8 / 1000.0;
	op_pcm_seek(offest);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//wav
/*
wav_decoder::wav_decoder(BKE_DataStream *stream):decoder(stream) {
	if (!*this)
		return;
	bkulong realread,singelread;
	int offest;
	char test[4],singel;
	good = 0;

	do{
		stream->read(&singel, singelread, sizeof(char));
	} while (singel != 'R');
	offest = -(int)(sizeof(Byte));
	stream->seek(offest, 0);

	stream->read(&file.RRIFFWAVEChunk,realread,sizeof(RIFF_HEADER));
	if (file.RRIFFWAVEChunk.RiffID[0]!='R'||file.RRIFFWAVEChunk.RiffID[1]!='I'||file.RRIFFWAVEChunk.RiffID[2]!='F'||file.RRIFFWAVEChunk.RiffID[3]!='F')
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Bad header: Cannot find RIFF file marker\n");
		return;
	}
	if (file.RRIFFWAVEChunk.WaveID[0]!='W'||file.RRIFFWAVEChunk.WaveID[1]!='A'||file.RRIFFWAVEChunk.WaveID[2]!='V'||file.RRIFFWAVEChunk.WaveID[3]!='E' )
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Bad header: Cannot find WAVE file marker\n");
		return;
	}
	//错误处理

	do{
		stream->read(&singel, singelread, sizeof(char));
	} while (singel != 'f');
	offest = -(int)(sizeof(Byte));
	stream->seek(offest, 0);

	stream->read(&file.FMT,realread,sizeof(FMT_BLOCK));
	if (file.FMT.FmtID[0]!='f'||file.FMT.FmtID[1]!='m'||file.FMT.FmtID[2]!='t'||file.FMT.FmtID[3]!=' ')
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Bad format: Cannot find 'fmt' file marker*2\n");
		return;
	}
	if ( file.FMT.FmtSize < (sizeof (WAVE_FORMAT) - sizeof (uint32_t)) )
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Bad format: Bad fmt size\n");
		return;
	}
	else
		if (file.FMT.FmtSize!=16)
		{
			offest=file.FMT.FmtSize-16;
			stream->seek(offest,0);
		}
	if ( file.FMT.wavFormat.FormatTag != 1 )
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Only supports PCM wave format\n");
		return;
	}
	//错误处理

	stream->read(&test,realread,4*sizeof(Byte));
	offest=-(int)(4*sizeof(Byte));
	stream->seek(offest,0);
	if (test[0]!='d'||test[1]!='a'||test[2]!='t'||test[3]!='a')
	{
		stream->read(&file.FactChunk,realread,sizeof(FACT_BLOCK));
		offest=file.FactChunk.FactSize;
		stream->seek(offest,0);
	}

	do{
		stream->read(&singel, singelread, sizeof(char));
	} while (singel != 'd');
	offest = -(int)(sizeof(Byte));
	stream->seek(offest, 0);

	stream->read(&file.DataChunk,realread,sizeof(DATA_BLOCK));
	if (file.DataChunk.DataID[0]!='d'||file.DataChunk.DataID[1]!='a'||file.DataChunk.DataID[2]!='t'||file.DataChunk.DataID[3]!='a')
	{
		o_stderr.print(L_ERROR, "[WAV Decoder]Bad format: unable to find 'data' file marker\n");
		return;
	}
	DataStartOffset = stream->getOffset();
	good = 1;
	//错误处理
}

audio_buffer *wav_decoder::get_buffer(bool &error){
	bkulong a = (stream->getOffset() - this->DataStartOffset);	//已读data数据量
	if (this->DataStartOffset > stream->getOffset()){
		stream->seek(-1 * a, 0);
		a = 0;
	}
	error = false;
	const bkulong n = WavReadSize;
	bkulong freq = file.FMT.wavFormat.SamplesPerSec;
	bkulong channels = file.FMT.wavFormat.Channels;
	bkulong bit_depth = file.FMT.wavFormat.BitsPerSample;
	Byte *temp = (Byte *)malloc(n);
	bkulong realread = 0;
	if (file.DataChunk.DataSize < a)
	{
		free(temp);
		return NULL;
	}
	stream->read(temp, realread, n);
	if (realread == 0)
	{
		free(temp);
		return NULL;
	}
	a = file.DataChunk.DataSize - a;
	if (a<WavReadSize)
	{
		realread = (int)a;
	}
	
	//if (realread < WavReadSize)
	//{
	//	if (realread == 0)
	//	{
	//		free(temp);
	//		return NULL;
	//	}
	//	temp = (Byte *)realloc(temp, realread);
	//}
	bkulong length = 8 * realread / file.FMT.wavFormat.Channels / file.FMT.wavFormat.BitsPerSample;
	return new audio_buffer(temp, length, freq, channels, bit_depth, 1);
}

wav_decoder::~wav_decoder()
{
}

void wav_decoder::seek(double loopto){
	uint64_t offest;
	offest=sizeof(WAVEChunk)+(file.FMT.wavFormat.BitsPerSample * file.FMT.wavFormat.SamplesPerSec * file.FMT.wavFormat.Channels * loopto / 1000.0)/8;
	//WAV格式文件所占容量（KB) = （取样频率*量化位数*声道）*时间 / 8 (字节= 8bit)
	stream->seek(offest,1);
}
*/
wav_decoder::wav_decoder(const QString &file) :decoder(file) {
	uint32_t test = 0;
	int was_error = 0;
	ChunkInf temChunkInf;
	bkulong realread = 0;
	temChunkInf.length = 0;
	//int IEEE_float_encoded, MS_ADPCM_encoded, IMA_ADPCM_encoded;
	//IEEE_float_encoded = MS_ADPCM_encoded = IMA_ADPCM_encoded = 0;
	int IEEE_float_encoded;
	IEEE_float_encoded = 0;
    realread = stream->read((char*)&test, sizeof(uint32_t));
	if (realread != 4 || test != RIFF)
	{
        qWarning() << (L"[BKWAV]RIFF头错误\n");
		return;
	}

    stdioSeek(stream, sizeof(uint32_t), SEEK_CUR);
    realread = stream->read((char*)&test, sizeof(uint32_t));

	if (realread != 4 || test != WAVE)
	{
        qWarning() << (L"[BKWAV]WAVE头错误\n");
		return;
	}

	do{
        stdioSeek(stream, temChunkInf.length, SEEK_CUR);
        realread = stream->read((char*)&temChunkInf, sizeof(ChunkInf));
		if (realread != sizeof(ChunkInf)){
            qWarning() << (L"[BKWAV]未找到fmt头\n");
			return;
		}
	} while (temChunkInf.magic != FMT);

	if (temChunkInf.length < (sizeof(WaveFMT) - sizeof(uint32_t)))
	{
        qWarning() << (L"[BKWAV]fmt块大小错误\n");
		return;
	}

    realread = stream->read((char*)&wavefmt, sizeof(WaveFMT));

	switch (wavefmt.encoding) {
	case PCM_CODE:
		
		break;
	case IEEE_FLOAT_CODE:
        qWarning() << (L"[BKWAV]未支持解码的PCM数据\n");
		//IEEE_float_encoded = 1;
		return;
		//break;
	case MS_ADPCM_CODE:
		/* Try to understand this */
		/*
		if (InitMS_ADPCM(&wavefmt) < 0) {
		was_error = 1;
		o_stderr.print(L_ERROR, "未知的PCM数据\n");
		return;
		}
		*/
        qWarning() << (L"[BKWAV]未支持解码的PCM数据\n");
		//MS_ADPCM_encoded = 1;
		//break;
		return;
	case IMA_ADPCM_CODE:
		/* Try to understand this */
		/*
		if (InitIMA_ADPCM(&wavefmt) < 0) {
		was_error = 1;
		o_stderr.print(L_ERROR, "未知的PCM数据\n");
		return;
		}
		*/
        qWarning() << (L"[BKWAV]未支持解码的PCM数据\n");
		//IMA_ADPCM_encoded = 1;
		//break;
		return;
	case MP3_CODE:
        qWarning() << (L"[BKWAV]无法接受MPEG Layer 3类型数据\n");
		was_error = 1;
		return;
	default:
        qWarning() << (L"[BKWAV]未知的wave数据类型\n");
		was_error = 1;
		return;
	}

	if (IEEE_float_encoded) {
		if (((wavefmt.bitspersample)) != 32) {
			was_error = 1;
		}
		else {
			format = AUDIO_F32;
		}
	}
	else {
		switch ((wavefmt.bitspersample)) {
			/*
		case 4:
			if (MS_ADPCM_encoded || IMA_ADPCM_encoded) {
				format = AUDIO_S16;
			}
			else {
				was_error = 1;
			}
			break;
			*/
		case 8:
			format = AUDIO_U8;
			break;
		case 16:
			format = AUDIO_S16;
			break;
		case 32:
			format = AUDIO_S32;
			break;
		default:
			was_error = 1;
			break;
		}
	}

	if (was_error) {
        qWarning() << (L"[BKWAV]未知的PCM数据\n");
		return;
	}

	if (realread != sizeof(WaveFMT)){
        qWarning() << (L"[BKWAV]fmt数据不完整\n");
		return;
	}
	if (temChunkInf.length != 16)
	{
        uint32_t offset = temChunkInf.length - 16;
        stdioSeek(stream, offset, SEEK_CUR);
	}
	temChunkInf.length = 0;
	do{
        stdioSeek(stream, temChunkInf.length, SEEK_CUR);
        realread = stream->read((char*)&temChunkInf, sizeof(ChunkInf));
		if (realread != sizeof(ChunkInf)){
            qWarning() << (L"[BKWAV]未找到data头\n");
			return;
		}
	} while (temChunkInf.magic != DATA);
    DataStartOffset = stream->pos();
	datasize = temChunkInf.length;
}

audio_buffer *wav_decoder::get_buffer(bool &error){

    bkulong DataOffset = (stream->pos() - this->DataStartOffset);	//已读data数据量
    if (this->DataStartOffset > stream->pos()){
        qWarning() << (L"[BKWAV]读取data开始位置错误\n");
        stdioSeek(stream, DataStartOffset, SEEK_SET);
		DataOffset = 0;
	}
	/*
	switch (wavefmt.encoding) {
	case PCM_CODE:
	// We can understand this
	break;
	case IEEE_FLOAT_CODE:
	// We can understand this
	break;
	case MS_ADPCM_CODE:
	// Try to understand this
	return NULL;
	break;
	case IMA_ADPCM_CODE:
	// Try to understand this
	return NULL;
	break;
	case MP3_CODE:
	return NULL;
	default:
	return NULL;
	}
	*/
	error = false;
	bkulong freq = wavefmt.frequency;
	bkulong channels = wavefmt.channels;
	bkulong bit_depth = wavefmt.bitspersample;
	Byte temp[WavReadSize];
	bkulong realread = 0;
	if (datasize < DataOffset)
	{
		return NULL;
	}
    realread = stream->read((char*)temp, WavReadSize);
	if (realread == 0)
	{
		return NULL;
	}
	DataOffset = datasize - DataOffset;
	if (DataOffset < WavReadSize)
	{
		realread = (int)DataOffset;
	}
	bkulong length = realread * 8 / wavefmt.channels / wavefmt.bitspersample;
	return new audio_buffer(temp, length, freq, channels, bit_depth, 1);
}

wav_decoder::~wav_decoder()
{
}

void wav_decoder::seek(double loopto){
	//bkulong offset = DataStartOffset + (wavefmt.bitspersample * wavefmt.frequency * wavefmt.channels * loopto / 1000.0) / 8;
	bkulong offset = DataStartOffset + (wavefmt.byterate * loopto / 1000.0);
	//数据量 = （采样频率×采样位数×声道数×时间） / 8
	//每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／8。播放软件利用此值可以估计缓冲区的大小。
    stdioSeek(stream, offset, SEEK_SET);
}
/*
int wav_decoder::InitMS_ADPCM(WaveFMT * format)
{
	uint8_t *rogue_feel;
	int i;
	// Set the rogue pointer to the MS_ADPCM specific data
	rogue_feel = (uint8_t *)format + sizeof(*format);
	if (sizeof(*format) == 16) {
		// const Uint16 extra_info = ((rogue_feel[1] << 8) | rogue_feel[0]);
		rogue_feel += sizeof(uint16_t);
	}
	MS_ADPCM_state.wSamplesPerBlock = ((rogue_feel[1] << 8) | rogue_feel[0]);
	rogue_feel += sizeof(uint16_t);
	MS_ADPCM_state.wNumCoef = ((rogue_feel[1] << 8) | rogue_feel[0]);
	rogue_feel += sizeof(uint16_t);
	if (MS_ADPCM_state.wNumCoef != 7) {
		o_stderr.print(L_ERROR,"未知的ADPCM设置系数");
		return (-1);
	}
	for (i = 0; i < MS_ADPCM_state.wNumCoef; ++i) {
		MS_ADPCM_state.aCoeff[i][0] = ((rogue_feel[1] << 8) | rogue_feel[0]);
		rogue_feel += sizeof(uint16_t);
		MS_ADPCM_state.aCoeff[i][1] = ((rogue_feel[1] << 8) | rogue_feel[0]);
		rogue_feel += sizeof(uint16_t);
	}
	return (0);
}
int wav_decoder::InitIMA_ADPCM(WaveFMT * format)
{
	uint8_t *rogue_feel;
	// Set the rogue pointer to the IMA_ADPCM specific data
	rogue_feel = (uint8_t *)format + sizeof(*format);
	if (sizeof(*format) == 16) {
		// const Uint16 extra_info = ((rogue_feel[1] << 8) | rogue_feel[0]);
		rogue_feel += sizeof(uint8_t);
	}
	IMA_ADPCM_state.wSamplesPerBlock = ((rogue_feel[1] << 8) | rogue_feel[0]);
	return (0);
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
inline int16_t fix_24bit_sample(FLAC__int32 v){
	return (v+0x80)>>8;
}

inline int16_t fix_32bit_sample(FLAC__int32 v){
	const FLAC__int32 max_i32=0xFFFFFFFF;
	return ((max_i32-0x80<v)?max_i32:v+0x80)>>8;
}

flac_decoder::flac_decoder(BKE_DataStream *stream):decoder(stream),buffer(0){
	if (!*this)
		return;
	this->set_md5_checking(1);
	this->good=0;
	if (this->init()!=FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return;
	this->good=1;
}

flac_decoder::~flac_decoder(){
	delete this->buffer;
}

FLAC__StreamDecoderWriteStatus flac_decoder::write_callback(const FLAC__Frame *frame,const FLAC__int32 * const *buffer){
	void *push_me=0;
	bool stereo=frame->header.channels!=1;
	ulong channels=stereo?2:1,
		bits=0;
	switch (frame->header.bits_per_sample){
		case 8:
			bits=8;
			{
				push_me=audio_buffer::allocate(frame->header.blocksize,bits/8,channels);
				int8_t *temp_buffer=(int8_t *)push_me;
				for (size_t i=0;i<frame->header.blocksize;i++){
					temp_buffer[i*channels]=buffer[0][i];
					if (stereo)
						temp_buffer[i*channels+1]=buffer[1][i];
				}
			}
			break;
		case 16:
			bits=16;
			{
				push_me=audio_buffer::allocate(frame->header.blocksize,bits/8,channels);
				int16_t *temp_buffer=(int16_t *)push_me;
				for (size_t i=0;i<frame->header.blocksize;i++){
					temp_buffer[i*channels]=buffer[0][i];
					if (stereo)
						temp_buffer[i*channels+1]=buffer[1][i];
				}
			}
			break;
		case 24:
			bits=16;
			{
				push_me=audio_buffer::allocate(frame->header.blocksize,bits/8,channels);
				int16_t *temp_buffer=(int16_t *)push_me;
				for (size_t i=0;i<frame->header.blocksize;i++){
					temp_buffer[i*channels]=fix_24bit_sample(buffer[0][i]);
					if (stereo)
						temp_buffer[i*channels+1]=fix_24bit_sample(buffer[1][i]);
				}
			}
			break;
		case 32:
			bits=16;
			{
				push_me=audio_buffer::allocate(frame->header.blocksize,bits/8,channels);
				int16_t *temp_buffer=(int16_t *)push_me;
				for (size_t i=0;i<frame->header.blocksize;i++){
					temp_buffer[i*channels]=fix_32bit_sample(buffer[0][i]);
					if (stereo)
						temp_buffer[i*channels+1]=fix_32bit_sample(buffer[1][i]);
				}
			}
			break;
	}
	assert(!this->buffer);
	this->buffer=new audio_buffer(push_me,frame->header.blocksize,frame->header.sample_rate,channels,bits,1);
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderReadStatus flac_decoder::read_callback(FLAC__byte *buffer,size_t *bytes){
	return
		(!this->stream || !this->stream->read(buffer,*bytes,*bytes))
		?FLAC__STREAM_DECODER_READ_STATUS_ABORT
		:FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus flac_decoder::seek_callback(FLAC__uint64 absolute_byte_offset){
	if (!this->stream)
		return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
	this->stream->seek(absolute_byte_offset,1);
	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

bool flac_decoder::eof_callback(){
	return (!this->stream)?1:this->stream->get_offset()>=this->stream->get_size();
}

FLAC__StreamDecoderTellStatus flac_decoder::tell_callback(FLAC__uint64 *absolute_byte_offset){
	if (!this->stream)
		return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
	*absolute_byte_offset=this->stream->get_offset();
	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus flac_decoder::length_callback(FLAC__uint64 *stream_length){
	if (!this->stream)
		return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
	*stream_length=this->stream->get_size();
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

audio_buffer *flac_decoder::get_buffer(bool &error){
	bool ok=1;
	while (!this->buffer && (ok=this->process_single()) && this->get_state()!=FLAC__STREAM_DECODER_END_OF_STREAM);
	error=!ok;
	audio_buffer *ret=this->buffer;
	this->buffer=0;
	return ret;
}

void flac_decoder::loop(){
	this->seek_absolute(0);
}
*/

mp3_decoder::mp3_decoder(const QString &file) :decoder(file){
	if (!*this)
		return;
	this->good = 0;
	//this->has_played=0;
	dec = new BKMP3_Decoder(this);
	this->good = 1;
}

mp3_decoder::~mp3_decoder(){
	delete dec;
}

int mp3_decoder::read(char b[], int len)
{
    bkulong l = stream->read(b,len);
	return l;
}

int mp3_decoder::seek(int o, int whence){
    return stdioSeek(stream, o, whence);
}

audio_buffer *mp3_decoder::get_buffer(bool &there_was_an_error){
	there_was_an_error=0;
	char *buffer = NULL;
	int size;
	int err = dec->decodeOneFrame(&buffer, &size);
	if (err != 0){
		return NULL;
	}
	return new audio_buffer(buffer, size / (dec->getChannelCount() * 2), dec->getSamplingRate(), dec->getChannelCount(), 16, 2);
}

void mp3_decoder::seek(double s){
	//if (this->has_played){
	//off_t t = ( 44100 * s / 1000.0);
//		mpg123_timeframe(this->handle,s / 1000.0);
	dec->seek(s / 1000.0);
	//}
}


/*
static const size_t BUFFERSIZE=32768;
static std::vector<SBYTE> audiobuffer;

void openal_Exit(){
	VC_Exit();
	audiobuffer.clear();
}

void openal_Update(){
	audiobuffer.resize(BUFFERSIZE);
	size_t n=VC_WriteBytes(&audiobuffer[0],BUFFERSIZE);
	audiobuffer.resize(n);
}

BOOL openal_Reset(){
	VC_Exit();
	return VC_Init();
}

BOOL openal_IsPresent(){
	return 1;
}

MDRIVER drv_openal={
	NULL,
	"openal",
	"OpenAL",
	0,255,
	"openal",

	0,
	openal_IsPresent,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	VC_Init,
	openal_Exit,
	openal_Reset,
	VC_SetNumVoices,
	VC_PlayStart,
	VC_PlayStop,
	openal_Update,
	0,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};

bool mod_static_data::perform_initialization(){
	MikMod_RegisterDriver(&drv_openal);
	md_mode|=DMODE_SOFT_MUSIC|DMODE_16BITS|DMODE_STEREO;
	md_mixfreq=44100;
	md_volume=100;
	MikMod_RegisterAllLoaders();
	//Shut GCC up:
	char s[]="";
	if (MikMod_Init(s))
		return 0;
	return 1;
}

mod_static_data::~mod_static_data(){
	if (this->initialized)
		MikMod_Exit();
}

struct BKE_MREADER{
	MREADER base;
	BKE_DataStream *stream;
};

BOOL mod_seek(MREADER *stream,long offset,int whence){
	((BKE_MREADER *)stream)->stream->stdio_seek(offset,whence);
	return 1;
}

long mod_tell(MREADER *stream){
	return (long)((BKE_MREADER *)stream)->stream->get_offset();
}

BOOL mod_read(MREADER *stream,void *dst,size_t n){
	return ((BKE_MREADER *)stream)->stream->read(dst,n,n);
}

int mod_get(MREADER *stream){
	uchar byte;
	size_t n=1;
	if (!((BKE_MREADER *)stream)->stream->read(&byte,n,n) || !n)
		return -1;
	return byte;
}

BOOL mod_eof(MREADER *stream){
	return ((BKE_MREADER *)stream)->stream->get_offset()>=((BKE_MREADER *)stream)->stream->get_size();
}

mod_decoder::mod_decoder(BKE_DataStream *stream):decoder(stream){
	if (!*this)
		return;
	this->good=0;
	if (!mod_decoder::static_data.init())
		return;
	BKE_MREADER reader;
	reader.base.Eof=mod_eof;
	reader.base.Get=mod_get;
	reader.base.Read=mod_read;
	reader.base.Seek=mod_seek;
	reader.base.Tell=mod_tell;
	reader.stream=stream;
	this->module=Player_LoadGeneric((MREADER *)&reader,64,0);
	this->good=this->module;
	if (!this->good){
		o_stderr <<MikMod_strerror(MikMod_errno)<<"\n";
		return;
	}
	Player_Start(this->module);
}

mod_decoder::~mod_decoder(){
	if (this->good)
		Player_Free(this->module);
}

audio_buffer *mod_decoder::get_buffer(bool &error){
	error=0;
	if (!Player_Active())
		return 0;
	MikMod_Update();
	return new audio_buffer(&audiobuffer[0],audiobuffer.size()/4,44100,2,16);
}

void mod_decoder::loop(){
	Player_SetPosition(0);
	Player_Start(this->module);
}

custom_FILE BKE_fopen(const char *filename){
	return midi_decoder::static_data.get_file(filename);
}

int BKE_fclose(custom_FILE file){
	return 0;
}

size_t BKE_fread(void *ptr,size_t size,size_t nmemb,custom_FILE file){
	((BKE_DataStream *)file)->read(ptr,nmemb,size*nmemb);
	return nmemb/size;
}

int BKE_fseek(custom_FILE stream,long int offset,int origin){
	return (int)((BKE_DataStream *)stream)->stdio_seek(offset,origin);
}

custom_stdio init_stdio(){
	custom_stdio stdio;
	stdio.fopen=BKE_fopen;
	stdio.fclose=BKE_fclose;
	stdio.fread=BKE_fread;
	stdio.fseek=BKE_fseek;
	return stdio;
}

bool midi_static_data::perform_initialization(){
	custom_stdio stdio=init_stdio();
	if (mid_init(0,&stdio))
		return 0;
	if (!general_archive.addArchive(L"timidity.zip"))
		general_archive.addArchive(L"timidity.oaf");
	return 1;
}

midi_static_data::~midi_static_data(){
	if (this->initialized)
		mid_exit();
}

void midi_static_data::cache_file(const char *path,BKE_DataStream *stream){
    QString temp=UniFromUTF8(std::string(path));
	tolower(temp);
	toforwardslash(temp);
	BKE_MutexLocker ml(this->mutex);
	this->cached_files[temp]=stream;
}

BKE_DataStream *midi_static_data::get_file(const char *path){
    QString temp=UniFromUTF8(std::string(path));
	tolower(temp);
	toforwardslash(temp);
	BKE_MutexLocker ml(this->mutex);
	midi_static_data::map_t::iterator i=this->cached_files.find(temp);
	if (i!=this->cached_files.end()){
		i->second->reset();
		return i->second;
	}
	BKE_DataStream *stream=general_archive.open(temp,KEEP_IN_MEMORY);
	if (!stream)
		return 0;
	return this->cached_files[temp]=stream;
}

size_t midi_read(void *ctx,void *ptr,size_t size,size_t nmemb){
	((BKE_DataStream *)ctx)->read(ptr,nmemb,size*nmemb);
	return nmemb/size;
}

int midi_close(void *ctx){
	return 0;
}

midi_decoder::midi_decoder(BKE_DataStream *stream):decoder(stream){
	if (!*this)
		return;
	this->good=0;
	if (!midi_decoder::static_data.init())
		return;
	MidIStream *file=mid_istream_open_callbacks(midi_read,midi_close,stream);
	if (!file)
		return;
	MidSongOptions options;
	options.buffer_size=1024;
	options.channels=2;
	options.format=MID_AUDIO_S16LSB;
	options.rate=44100;
	custom_stdio stdio=init_stdio();
	this->song=mid_song_load(file,&options,&stdio);
	mid_istream_close(file);
	if (!this->song)
		return;
	this->good=1;
	mid_song_start(this->song);
}

midi_decoder::~midi_decoder(){
	if (*this)
		mid_song_free(this->song);
}

audio_buffer *midi_decoder::get_buffer(bool &error){
	error=0;
	audio_buffer *buffer=(audio_buffer *)audio_buffer::allocate(1024,2,2);
	size_t bytes_read=mid_song_read_wave(this->song,buffer,1024*4);
	if (!bytes_read){
		audio_buffer::deallocate(buffer);
		return 0;
	}
	return new audio_buffer(buffer,bytes_read/4,44100,2,16,1);
}

void midi_decoder::loop(){
	mid_song_seek(this->song,0);
	mid_song_start(this->song);
}
*/
