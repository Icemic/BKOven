#pragma once

#include <vector>
#include <list>
#include <cstdlib>
#include <cassert>
#include <AL/al.h>
#include <AL/alc.h>
#include <algorithm>
#include "Thread.h"
#include <QIODevice>

class decoder;
class audio_sink;
class audio_stream{
protected:
	audio_sink *sink;
	decoder *dec;
	volatile bool good,
		playing,
		paused;
	float volume,
		*general_volume;
	float get_compound_volume(){
		if (!this->general_volume)
			return this->volume;
		return this->volume* *this->general_volume;
	}
	void set_internal_volume();
	std::vector<BKE_Notify *> notify;
	std::vector<volatile bool *> notify_bool;
public:
	double loopto;
	bool calcRealTimeVolume;
    QString filename;
	volatile int loop;
	bool cleanup;
    audio_stream(const QString &filename);
	virtual ~audio_stream();
	operator bool(){ return this->good; }
	void start();
	void stop();
	void pause();
	virtual bool update();
	bool is_playing() const{ return this->playing; }
	bool is_sink_playing() const;
	bool is_paused() const{ return this->paused; }
	void set_volume(float);
	void set_general_volume(float &);
	float get_volume() const{ return this->volume; }
	void notify_on_stop(BKE_Notify *event){ this->notify.push_back(event); }
	void notify_on_stop(volatile bool *b){ this->notify_bool.push_back(b); }
	int needs_update();
	//返回播放进度（误差在微秒级）（单位：ms）
	double tell();
	//调整播放进度至（单位：ms）
	void seek(double);

	float getRealTimeVolume();
};



class audio_sink{
	ALuint source;
	static const size_t n=16;
	ALuint freebuffer;
public:
	void seek(double s);
	double time_offset_played;
	double time_offset_buffered;
	float realTimeVolume;
	bool calcRealTimeVolume;
	audio_sink(bool calcRealTimeVolume);
	~audio_sink();
	operator bool(){ return !!this->source; }
	void push(const void *buffer, bkulong length, bkulong freq, bkulong channels, bkulong bit_depth);
	bool needs_more_data();
	bool flush(); //返回值表示是否需要播放(alSourcePlay
	void pause(){ alSourcePause(this->source); }
	void unpause(){ alSourcePlay(this->source); }
	void set_volume(float vol){ alSourcef(this->source,AL_GAIN,vol); }
	ALint get_state(){
		ALint state;
		alGetSourcei(this->source,AL_SOURCE_STATE,&state);
		return state;
	}
	friend void audio_stream::seek(double);
	friend double audio_stream::tell();
};

struct audio_buffer{
	void *buffer;
	bkulong length;
	bkulong frequency;
	bkulong channels;
	bkulong bit_depth;
	int ownership;
	audio_buffer(const void *buffer, bkulong length, bkulong freq, bkulong channels, bkulong bit_depth, int ownership = 0);
	~audio_buffer(){
		if (ownership!=2)
		{
			free(this->buffer);
		}
	}
	void push(audio_sink &sink){
		sink.push(this->buffer, this->length, this->frequency, this->channels, this->bit_depth);
	}
	static void *allocate(bkulong samples, bkulong bytes_per_channel, bkulong channels){
		return malloc(samples*bytes_per_channel*channels);
	}
	static void deallocate(void *buffer){
		free(buffer);
	}
	void cut(double t);
};

class decoder{
protected:
    QIODevice *stream;
	bool good;
	friend void audio_stream::seek(double);
public:
    decoder(const QString &file);
	virtual void seek(double s) = 0;
	virtual ~decoder();
	virtual operator bool(){ return this->good; }
	virtual audio_buffer *get_buffer(bool &error)=0;
	
};

class audio_device{
public:
	typedef std::list<audio_stream *> list_t;
private:
	ALCdevice *device;
	ALCcontext *context;
	list_t streams;
	bool good;
public:
	audio_device();
	~audio_device();
	operator bool(){ return this->good; }
	void update(std::vector<audio_stream *> &removed_streams);
	void add(audio_stream *);
	void remove(audio_stream *);
};

inline void audio_stream::set_internal_volume()
{
    if(this->sink)
        this->sink->set_volume(this->get_compound_volume());
}

