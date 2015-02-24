#pragma once

#include "OpenAL.h"
#include "Thread.h"
#include <QMap>
#include <QFile>
#include "BKAudio.h"

#define MUSIC_CHANNEL -1
#define VOICE_CHANNEL -2
#define ALL_SOUND -3

class BKE_Audio{
	static const bklong initial_channel_counter;

public:
	audio_device *dev;
    typedef QMap<bklong, audio_stream *> chan_t;
	chan_t channels;
//private:
	bool uninitialized;
	bklong channel_counter;
	float mvol,
		svol,
		vvol;

	volatile bool threadInitialFinished;
	BKE_Thread thread;
	void update_thread();
	BKE_Mutex mutex;
	bool stop_thread;

	audio_stream *get_channel(int channel);
	bklong set_volume(float &, bklong, bklong);
public:
	static const bklong max_valid_channel;
	BKE_Audio();
	~BKE_Audio();
    bool prepare(const QString &filename, QString &found_path);
    AudioError play_music(const QString &filename, bklong vol = 100, bklong times = -1, double loopto = 0., bklong fadein = 0);
    AudioError stop_music(bklong fadeout = 0);
    AudioError stop(bklong channel, bklong fadeout = 0);
	void pause(bklong channel);
	void resume(bklong channel);
    AudioError play_voice(const QString &filename, bklong vol = 100);
    AudioError stop_voice(bklong fadeout = 0);
    AudioError play_sound(const QString &filename, bklong channel, bklong vol, bklong times, bool automatic_cleanup, bool calcRealTimeVolume = false);
    AudioError stop_sound(bklong channel, bklong fadeout = 0);
    AudioError stop_all(bklong fadeout = 0);
    AudioError pause_all();
    AudioError resume_all();
    AudioError load_sound_on_a_channel(const QString &filename, bklong channel);
    AudioError unload_sound_from_channel(bklong channel);
    AudioError play(bklong channel, bklong vol, bklong time, bool sautomatic_cleanup, bool calcRealTimeVolume = false);
	void wait_for_channel(bklong channel);
	void set_notify_at_end(bklong channel, volatile bool *b);
	void set_notify_at_end(bklong channel, BKE_Notify *b);
	bool getRealTimeVolume(bklong channel, float *v);
	bklong music_volume(bklong vol){
		return this->set_volume(this->mvol,vol,MUSIC_CHANNEL);
	}
	bklong sound_volume(bklong vol){
		return this->set_volume(this->svol,vol,ALL_SOUND);
	}
	bklong voice_volume(bklong vol){
		return this->set_volume(this->vvol,vol,VOICE_CHANNEL);
	}
	bklong channel_volume(bklong channel,bklong vol);
	bool is_playing(bklong channel);
	bool is_initialized(){
		return !this->uninitialized;
	}


	static BKE_Audio *_interface;
	static void create(){
        Q_ASSERT(_interface==NULL);
		_interface = new BKE_Audio;
	}
	static void purge(){
		delete _interface;
		_interface = NULL;
	}
	static BKE_Audio *getInstance(){
        Q_ASSERT(_interface);
		return _interface;
	}
	static bool valid()
	{
		return !!_interface;
	}

	struct audiofade_param
	{
		BKE_Clock clock;
		BKE_Clock::t begin_time;
		bklong duration;
		float startVol;
		float endVol;
		bool stopAtEnd;
	};
    QMap<bklong, audiofade_param*> fadeParameters;

	void fade(bklong channel, bklong duration, bklong endvol, bool stopAtEnd);
	void stopfade(int channel);
	void stopallfade();
	long getFadeNumber() { BKE_MutexLocker ml(mutex); return fadeParameters.size(); }
};
