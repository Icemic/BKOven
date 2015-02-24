#include "OpenAL.h"
#include "AudioFormats.h"
#include <iostream>
#include "Audio.h"
#include <algorithm>
#include <QFile>

template <typename T1,typename T2>
void saturate_value(T1 &dst,const T2 &min,const T2 &max){
    if (dst<min)
        dst=min;
    else if (dst>max)
        dst=max;
}

decoder::decoder(const QString &file)
{
    QFile *fp = new QFile(file);
    if(fp->open(QIODevice::ReadOnly))
    {
        stream = fp;
        good = 1;
    }
    else
    {
        delete fp;
        good = 0;
    }
}

decoder::~decoder(){
	if (stream)
		this->stream->close();
}

#ifdef DEBUG
#define CHECK_AL_ERROR() {int error = alGetError(); assert(error==AL_NO_ERROR);}
#else
#define CHECK_AL_ERROR()
#endif

audio_sink::audio_sink(bool calcRealTimeVolume){
	this->time_offset_played=0;
	this->time_offset_buffered = 0;
	this->realTimeVolume = 0.f;
	this->calcRealTimeVolume = calcRealTimeVolume;
	alGenSources(1,&this->source);
    CHECK_AL_ERROR();
	freebuffer = 0;
}

audio_sink::~audio_sink(){
	if (!*this)
		return;
	alSourceStop(this->source);
    CHECK_AL_ERROR();
	ALint finished;
	alGetSourcei(this->source,AL_BUFFERS_PROCESSED,&finished);
    CHECK_AL_ERROR();
	ALuint buffers[n];
	alSourceUnqueueBuffers(this->source,finished,buffers);
    CHECK_AL_ERROR();
    alDeleteSources(1,&this->source);
    CHECK_AL_ERROR();
	alDeleteBuffers(finished,buffers);
    CHECK_AL_ERROR();
	if (freebuffer)
	{
		alDeleteBuffers(1, &freebuffer);
        CHECK_AL_ERROR();
	}
}

ALenum make_format(bkulong channels,bkulong bit_depth){
	if (channels<=2){
		static const ALenum array[]={
			AL_FORMAT_MONO8,
			AL_FORMAT_MONO16,
			AL_FORMAT_STEREO8,
			AL_FORMAT_STEREO16
		};
		return array[((channels==2)<<1)|(bit_depth==16)];
	}
    return alGetEnumValue("");
}

bool audio_sink::needs_more_data(){
	if (!*this)
		return 0;
	ALint finished,
		queued,
		state;
	alGetSourcei(this->source,AL_BUFFERS_PROCESSED,&finished);
    CHECK_AL_ERROR();
	alGetSourcei(this->source,AL_BUFFERS_QUEUED,&queued);
    CHECK_AL_ERROR();
	state=this->get_state();
	return !(!finished && (size_t)queued>=n && (state==AL_PLAYING || state==AL_PAUSED || state== AL_INITIAL));
}

bool audio_sink::flush()
{
	if (!*this)
		return false;
	ALint queued,
		finished,
		state;
	alGetSourcei(this->source, AL_BUFFERS_PROCESSED, &finished);
    CHECK_AL_ERROR();
	alGetSourcei(this->source, AL_BUFFERS_QUEUED, &queued);
    CHECK_AL_ERROR();
	state = this->get_state();
	bool call_play = (finished == queued || state != AL_PLAYING);
	std::vector<ALuint> temp(queued);
	if (finished){
		ALuint *temp_p = &temp[0];
		alSourceUnqueueBuffers(this->source, finished, temp_p);
        CHECK_AL_ERROR();
		for (size_t a = 0; a < (size_t)finished; a++){
			ALint local_freq,
				local_bits,
				local_channels,
				local_size;
			alGetBufferi(temp[a], AL_FREQUENCY, &local_freq);
            CHECK_AL_ERROR();
			alGetBufferi(temp[a], AL_BITS, &local_bits);
            CHECK_AL_ERROR();
			local_bits >>= 3;
			alGetBufferi(temp[a], AL_CHANNELS, &local_channels);
            CHECK_AL_ERROR();
			alGetBufferi(temp[a], AL_SIZE, &local_size);
            CHECK_AL_ERROR();
			this->time_offset_played += double(local_size / (local_channels*local_bits)) / double(local_freq) * 1000.0;		
		}
		if (freebuffer)
		{
			alDeleteBuffers(finished, &temp[0]);
            CHECK_AL_ERROR();
		}
		else
		{
			alDeleteBuffers(finished - 1, &temp[0]);
			freebuffer = temp[finished - 1];
            CHECK_AL_ERROR();
		}
	}
	return call_play;
}

#define MYABS(A) ((A<0)?-A:A) 

void audio_sink::push(const void *buffer, bkulong length, bkulong freq, bkulong channels, bkulong bit_depth){
	if (!*this)
		return;
	bool call_play = this->flush();
	if (!freebuffer)
    {
		alGenBuffers(1, &freebuffer);
        CHECK_AL_ERROR();
    }
	alBufferData(
		freebuffer,
		make_format(channels,bit_depth),
		buffer,
		length*channels*bit_depth/8,
		freq
	);
    CHECK_AL_ERROR();
	time_offset_buffered += 1000.0 * length / freq;
	if (calcRealTimeVolume)
	{
		switch (bit_depth)
		{
		case 8:
		{
			char *data = (char *)buffer;
			static float maxVol = pow(2.0, 7) - 1;
#pragma region calc1
			switch (channels)
			{
			case 1:
			{
				bklong maxv = data[0];
				bklong minv = data[0];
				for (size_t i = 0; i < length; i++)
				{
					bklong v = MYABS(data[i]);
					if (v > maxv)
					{
						maxv = v;
					}
					else if (v < minv)
					{
						minv = v;
					}
				}
				realTimeVolume = (maxv + minv) / 2 / maxVol;
				break;
			}
			case 2:
			{
				bklong maxv1 = data[0];
				bklong minv1 = data[0];
				bklong maxv2 = data[1];
				bklong minv2 = data[1];
				for (size_t i = 0; i < length; i++)
				{
					bklong v = MYABS(data[i << 1]);
					if (v > maxv1)
					{
						maxv1 = v;
					}
					else if (v < minv1)
					{
						minv1 = v;
					}

					bklong v2 = MYABS(data[(i << 1) + 1]);
					if (v2 > maxv2)
					{
						maxv2 = v2;
					}
					else if (v2 < minv2)
					{
						minv2 = v2;
					}
				}
				realTimeVolume = (maxv1 + minv1 + maxv2 + minv2) / 4 / maxVol;
				break;
			}
			default:
				break;
			}
#pragma endregion
			break;
		}
		case 16:
		{
			short *data = (short *)buffer;
			static float maxVol = pow(2.0, 15) - 1;
#pragma region calc2
			switch (channels)
			{
			case 1:
			{
				bklong maxv = data[0];
				bklong minv = data[0];
				for (size_t i = 0; i < length; i++)
				{
					bklong v = MYABS(data[i]);
					if (v > maxv)
					{
						maxv = v;
					}
					else if (v < minv)
					{
						minv = v;
					}
				}
				realTimeVolume = (maxv + minv) / 2 / maxVol;
				break;
			}
			case 2:
			{
				bklong maxv1 = data[0];
				bklong minv1 = data[0];
				bklong maxv2 = data[1];
				bklong minv2 = data[1];
				for (size_t i = 0; i < length; i++)
				{
					bklong v = MYABS(data[i << 1]);
					if (v > maxv1)
					{
						maxv1 = v;
					}
					else if (v < minv1)
					{
						minv1 = v;
					}

					bklong v2 = MYABS(data[(i << 1) + 1]);
					if (v2 > maxv2)
					{
						maxv2 = v2;
					}
					else if (v2 < minv2)
					{
						minv2 = v2;
					}
				}
				realTimeVolume = (maxv1 + minv1 + maxv2 + minv2) / 4 / maxVol;
				break;
			}
			default:
				break;
			}
#pragma endregion
		}
		default:
			break;
		}
	}
	alSourceQueueBuffers(this->source,1,&freebuffer);
    CHECK_AL_ERROR();
	freebuffer = 0;
	if (call_play){
		alSourcePlay(this->source);
        CHECK_AL_ERROR();
	}
}

void audio_sink::seek(double s)
{
	time_offset_buffered = s;
}

audio_buffer::audio_buffer(const void *buffer, bkulong length, bkulong freq, bkulong channels, bkulong bit_depth, int ownership){
	bkulong n=length*(bit_depth/8)*channels;
	if (ownership == 1){
		this->buffer=malloc(n);
		memcpy(this->buffer,buffer,n);
	}else
		this->buffer=(void *)buffer;
	this->ownership=ownership;
	this->length=length;
	this->frequency=freq;
	this->channels=channels;
	this->bit_depth=bit_depth;
}

void audio_buffer::cut(double t)
{
	size_t n = t / 1000 * frequency;
	if (length > n)
	{
		length = n;
	}
}

template <typename T>
static decoder *new_decoder(const QString &filename){
    return new T(filename);
}

/*
669 (Composer 669, Unis 669),
AMF (DSMI Advanced Module Format),
AMF (ASYLUM Music Format V1.0),
APUN (APlayer),
DSM (DSIK internal format),
FAR (Farandole Composer),
GDM (General DigiMusic),
IT (Impulse Tracker),
IMF (Imago Orpheus),
MOD (15 and 31 instruments),
MED (OctaMED),
MTM (MultiTracker Module editor),
OKT (Amiga Oktalyzer),
S3M (Scream Tracker 3),
STM (Scream Tracker),
STX (Scream Tracker Music Interface Kit),
ULT (UltraTracker),
UNI (MikMod),
XM (FastTracker 2) 
*/

inline bool ends_with(const QString &s1,const QString &s2){
    return s1.size()>=s2.size() && s1.right(s2.size())==s2;
}

decoder *initialize_decoder(const QString &filename){
	struct pair{
        QString ext;
        decoder *(*f)(const QString &);
	};
	static pair array[]={
        {".ogg",new_decoder<ogg_decoder>},
        {".mp3",new_decoder<mp3_decoder>},
#ifdef USE_OPUS
        {".opus",new_decoder<opus_decoder>},
#endif
        {".wav",new_decoder<wav_decoder>}
/*
        {".flac",new_decoder<flac_decoder>},
*/
	};
	const size_t n=sizeof(array)/sizeof(*array);
	for (size_t a=0;a<n;a++)
		if (ends_with(filename,array[a].ext))
			return array[a].f(filename);
	return 0;
}

audio_stream::audio_stream(const QString &filename){
	this->filename=filename;
	this->dec=initialize_decoder(filename);
	if (this->dec && !*this->dec){
		delete this->dec;
		this->dec=0;
	}
	this->loopto=0.0;
	this->sink=0;
	this->loop=0;
	this->playing=0;
	this->paused=0;
	this->good=!!this->dec;
	this->volume=1.f;
	this->general_volume=0;
	this->cleanup=0;
	this->calcRealTimeVolume = 0;
}

audio_stream::~audio_stream(){
	this->stop();
	delete this->dec;
}

void audio_stream::start(){
	if (this->playing)
	{
		if (this->paused)
		{
			this->sink->unpause();
			this->paused = 0;
		}
	}
	else
	{
		this->sink = new audio_sink(calcRealTimeVolume);
		if (!*this->sink){
			delete this->sink;
			this->sink = 0;
		}
		else{
			this->playing = 1;
			if (!this->paused){
				this->seek(0.0);
			}
			else
				this->paused = 0;
			this->set_internal_volume();
		}
	}
}

void audio_stream::stop(){
	delete this->sink;
	this->sink=0;
	this->playing=0;
	this->paused=0;
}

void audio_stream::pause(){
	if (!this->paused)
	{
        if(this->sink)
             this->sink->pause();
		this->paused = 1;
	}
}

bool audio_stream::update(){
	{
		int a=this->needs_update();
		if (a>=0)
			return !!a;
	}
	while (this->sink->needs_more_data()){
		bool error;
		audio_buffer *buffer;
		buffer=this->dec->get_buffer(error);
		bool stop=0;
		if (!buffer && !error){
			if (this->loop){
				this->seek(loopto);
				this->loop--;
				continue;
			}else
				stop=1;
		}
		if (stop){
			this->playing=0;
			return 0;
		}
		if (error){
			delete buffer;
			delete this->sink;
			this->sink=0;
			this->good=0;
			return 1;
		}
		buffer->push(*this->sink);
		delete buffer;
	}
    if(this->sink->get_state()==AL_INITIAL)
        this->sink->unpause();
	return 0;
}

bool audio_stream::is_sink_playing() const{
	if (!this->sink)
		return 0;
	return this->sink->get_state()==AL_PLAYING;
}

void audio_stream::set_volume(float vol){
	saturate_value(vol,0.f,1.f);
	this->volume=vol;
	this->set_internal_volume();
}

void audio_stream::set_general_volume(float &vol){
	this->general_volume=&vol;
	this->set_internal_volume();
}

int audio_stream::needs_update(){
	if (!this->playing){
		if (!this->sink || this->sink->get_state()==AL_STOPPED){
			while (this->notify.size()){
				this->notify.back()->set();
				this->notify.pop_back();
			}
			while (this->notify_bool.size()){
				*this->notify_bool.back() = true;
				this->notify_bool.pop_back();
			}
			return 1;
		}
		return 0;
	}
	if (this->paused || !*this)
		return 0;
	return -1;
}

double audio_stream::tell(){
	return (!this->sink) ? 0 : this->sink->time_offset_buffered;
}

void audio_stream::seek(double s)
{
	if (this->dec && this->sink)
	{
		this->dec->seek(s);
		this->sink->seek(s);
	}
}

float audio_stream::getRealTimeVolume()
{
	return (!this->sink) ? 0 : this->sink->realTimeVolume;
}

#ifdef AL_STATIC_BUILD
extern void al_static_init();
extern void al_static_uninit();
#endif

audio_device::audio_device(){
	this->device=0;
	this->context=0;
	this->good=0;
#ifdef AL_STATIC_BUILD
	al_static_init();
#endif
	this->device=alcOpenDevice(0);
	if (!this->device)
		return;
	this->context=alcCreateContext(this->device,0);
	if (!this->context)
		return;
	this->good=1;
	alcMakeContextCurrent(this->context);
}

audio_device::~audio_device(){
	while (this->streams.size()){
		delete this->streams.front();
		this->streams.pop_front();
	}
	alcMakeContextCurrent(0);
	if (this->context)
		alcDestroyContext(this->context);
	if (this->device)
		alcCloseDevice(this->device);
#ifdef AL_STATIC_BUILD
	al_static_uninit();
#endif
}

void audio_device::update(std::vector<audio_stream *> &removed_streams){
	std::vector<audio_stream *> remove;
	for (list_t::iterator i=this->streams.begin(),e=this->streams.end();i!=e;i++){
		audio_stream &stream=**i;
		if (stream.update() && stream.cleanup)
			remove.push_back(&stream);
	}
	removed_streams=remove;
	while (remove.size()){
		this->remove(remove.back());
		remove.pop_back();
	}
}

void audio_device::add(audio_stream *stream){
	this->streams.push_front(stream);
}

void audio_device::remove(audio_stream *stream){
	for (list_t::iterator i=this->streams.begin(),e=this->streams.end();i!=e;++i){
		if (stream!=*i)
			continue;
		delete stream;
		this->streams.erase(i);
		break;
	}
}

