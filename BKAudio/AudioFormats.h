#pragma once

#include "OpenAL.h"
#include <vorbis/vorbisfile.h>
//#include <FLAC++/decoder.h>
#include <bkmp3/MP3Decoder.h>

//#include <mikmod.h>
//#include "libtimidity/timidity.h"
#ifdef USE_OPUS
#include <opusfile.h>
#endif

class ogg_decoder:public decoder{
protected:
	OggVorbis_File file;
	int bitstream;
	void seek(double s);
public:
    ogg_decoder(const QString &file);
	~ogg_decoder();
	audio_buffer *get_buffer(bool &error);
	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char Byte;
#define WavReadSize 4096

#define RIFF            0x46464952      /* "RIFF" */
#define WAVE            0x45564157      /* "WAVE" */
#define FACT            0x74636166      /* "fact" */
#define LIST            0x5453494c      /* "LIST" */
#define FMT             0x20746D66      /* "fmt " */
#define DATA            0x61746164      /* "data" */

#define PCM_CODE        0x0001
#define MS_ADPCM_CODE   0x0002
#define IEEE_FLOAT_CODE 0x0003
#define IMA_ADPCM_CODE  0x0011
#define MP3_CODE        0x0055

#define WAVE_MONO       1
#define WAVE_STEREO     2

#define AUDIO_F32		0x8120
#define AUDIO_S16		0x8010
#define AUDIO_U8        0x0008
#define AUDIO_S32		0x8020

#pragma pack(push)
#pragma pack(1)
/*
struct RIFF_HEADER
{
	char RiffID[4];			//'R','I','F','F'
	uint32_t RiffSize;
	char WaveID[4];;		//'W','A','V','E'
};

struct WAVE_FORMAT
{
	uint16_t FormatTag;				//格式种类（值为1时，表示数据为线性PCM编码）
	uint16_t Channels;				//通道数，单声道为1，双声道为2
	uint32_t SamplesPerSec;			//采样频率
	uint32_t AvgBytesPerSec;		//波形数据传输速率（每秒平均字节数）
	uint16_t BlockAlign;			//DATA数据块长度，字节。
	uint16_t BitsPerSample;			//比特率
};

struct FMT_BLOCK
{
	char FmtID[4];		//'f','m','t',''
	uint32_t FmtSize;
	WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK
 {
 	char FactID[4];//'f','a','c','t'
 	uint32_t FactSize;
 };

struct DATA_BLOCK
{
	char DataID[4];//'d','a','t','a'
	uint32_t DataSize;
};

struct WAVEChunk
{
	RIFF_HEADER RRIFFWAVEChunk;
	FMT_BLOCK FMT;
	FACT_BLOCK FactChunk;
	DATA_BLOCK DataChunk;
};
*/
struct WaveFMT
{
	uint16_t encoding;				//格式种类（值为1时，表示数据为线性PCM编码）
	uint16_t channels;				//通道数，单声道为1，双声道为2
	uint32_t frequency;				//采样频率 11025, 22050, 44100 Hz
	uint32_t byterate;				//波形数据传输速率（每秒平均字节数） 每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／8。播放软件利用此值可以估计缓冲区的大小。
	uint16_t blockalign;			//数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8。播放软件需要一次处理多个该值大小的字节数据，以便将其值用于缓冲区的调整。
	uint16_t bitspersample;			//比特率(位深Bit Depth)
};
struct ChunkInf
{
	uint32_t magic;
	uint32_t length;
};
struct Chunk
{
	ChunkInf chunkinf;
	bkulong offset;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
struct MS_ADPCM_decodestate
{
	uint8_t hPredictor;
	uint16_t iDelta;
	int16_t iSamp1;
	int16_t iSamp2;
};

struct MS_ADPCM_decoder
{
	uint16_t wSamplesPerBlock;
	uint16_t wNumCoef;
	int16_t aCoeff[7][2];

	struct MS_ADPCM_decodestate state[2];
};
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
struct IMA_ADPCM_decodestate
{
	int32_t sample;
	int8_t index;
};
struct IMA_ADPCM_decoder
{
	uint16_t wSamplesPerBlock;

	struct IMA_ADPCM_decodestate state[2];
};
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
/*
class wav_decoder:public decoder{
protected:
	WAVEChunk file;
	bkulong DataStartOffset = 0;
	virtual void seek(double s) override;
public:
	wav_decoder(BKE_DataStream *stream);
	virtual ~wav_decoder() override;
	virtual audio_buffer *get_buffer(bool &error) override;
};
*/

class wav_decoder :public decoder{
protected:
	WaveFMT wavefmt;
	uint16_t format;
	uint32_t datasize;
	bkulong DataStartOffset;
	//MS_ADPCM_decoder MS_ADPCM_state;
	//IMA_ADPCM_decoder IMA_ADPCM_state;
	//int InitMS_ADPCM(WaveFMT * format);
	//int InitIMA_ADPCM(WaveFMT * format);
	virtual void seek(double s) override;
public:
    wav_decoder(const QString &file);
	virtual ~wav_decoder() override;
	virtual audio_buffer *get_buffer(bool &error) override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class flac_decoder:public decoder,public FLAC::Decoder::Stream{
	audio_buffer *buffer;
	FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame *frame,const FLAC__int32 * const *buffer);
	FLAC__StreamDecoderReadStatus read_callback(FLAC__byte *buffer,size_t *bytes);
	FLAC__StreamDecoderSeekStatus seek_callback(FLAC__uint64 absolute_byte_offset);
	bool eof_callback();
	FLAC__StreamDecoderTellStatus tell_callback(FLAC__uint64 *absolute_byte_offset);
	FLAC__StreamDecoderLengthStatus length_callback(FLAC__uint64 *stream_length);
	void error_callback(::FLAC__StreamDecoderErrorStatus status){
		o_stderr <<"Got error callback: "<<FLAC__StreamDecoderErrorStatusString[status]<<"\n";
	}
public:
	flac_decoder(BKE_DataStream *stream);
	~flac_decoder();
	operator bool(){
		return decoder::operator bool() && FLAC::Decoder::Stream::operator bool();
	}
	audio_buffer *get_buffer(bool &error);
	void loop();
};
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_OPUS
class opus_decoder : public decoder
{
protected:
	OggOpusFile *file;
	virtual void seek(double loopto) override;
public:
	opus_decoder(BKE_DataStream *stream);
	virtual ~opus_decoder() override;
	virtual audio_buffer *get_buffer(bool &error, double t) override;
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class mp3_decoder:public decoder , public IReader{
	BKMP3_Decoder *dec;
public:
    mp3_decoder(const QString &file);
	~mp3_decoder();
	virtual int read(char b[], int len) override;
	virtual int seek(int pos, int wherece) override;
	virtual audio_buffer *get_buffer(bool &error) override;
	virtual void seek(double s) override;
};

/*
class mod_static_data:public static_initializer{
	bool perform_initialization();
public:
	~mod_static_data();
};

class mod_decoder:public decoder{
	static mod_static_data static_data;
	MODULE *module;
public:
	mod_decoder(BKE_DataStream *stream);
	~mod_decoder();
	audio_buffer *get_buffer(bool &error);
	void loop();
};

class midi_static_data:public static_initializer{
	bool perform_initialization();
public:
    typedef std::map<QString,BKE_DataStream *> map_t;

	~midi_static_data();
	void cache_file(const char *path,BKE_DataStream *stream);
	BKE_DataStream *get_file(const char *path);
private:
	map_t cached_files;
};

class midi_decoder:public decoder{
	MidSong *song;
public:
	static midi_static_data static_data;
	midi_decoder(BKE_DataStream *stream);
	~midi_decoder();
	audio_buffer *get_buffer(bool &error);
	void loop();
};
*/

