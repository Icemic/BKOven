#include "BKAudio.h"
#include "Audio.h"

QBKAudio::QBKAudio()
{
    BKE_Audio::create();
}

QBKAudio::~QBKAudio()
{
    BKE_Audio::purge();
}

QBKAudio *QBKAudio::getInstance()
{
    static QBKAudio a;
    return &a;
}

AudioError QBKAudio::playMusic(const QString &filename, int vol, int times, double loopto, int fadein)
{
    return BKE_Audio::getInstance()->play_music(filename, vol, times, loopto, fadein);
}

AudioError QBKAudio::stopMusic(int fadeout)
{
    return BKE_Audio::getInstance()->stop_music(fadeout);
}

AudioError QBKAudio::stop(int channel, int fadeout)
{
    return BKE_Audio::getInstance()->stop(channel, fadeout);
}

void QBKAudio::pause(int channel)
{
    return BKE_Audio::getInstance()->pause(channel);
}

void QBKAudio::resume(int channel)
{
    return BKE_Audio::getInstance()->resume(channel);
}

AudioError QBKAudio::playVoice(const QString &filename, int vol)
{
    return BKE_Audio::getInstance()->play_voice(filename, vol);
}

AudioError QBKAudio::stopVoice(int fadeout)
{
    return BKE_Audio::getInstance()->stop_voice(fadeout);
}

AudioError QBKAudio::playSound(const QString &filename, int channel, int vol, int times, bool automatic_cleanup, bool calcRealTimeVolume)
{
    return BKE_Audio::getInstance()->play_sound(filename, channel, vol, times, automatic_cleanup, calcRealTimeVolume);
}

AudioError QBKAudio::stopSound(int channel, int fadeout)
{
    return BKE_Audio::getInstance()->stop_sound(channel, fadeout);
}

AudioError QBKAudio::stopAll(int fadeout)
{
    return BKE_Audio::getInstance()->stop_all(fadeout);
}

AudioError QBKAudio::pauseAll()
{
    return BKE_Audio::getInstance()->pause_all();
}

AudioError QBKAudio::resumeAll()
{
    return BKE_Audio::getInstance()->resume_all();
}

void QBKAudio::waitForChannel(int channel)
{
    return BKE_Audio::getInstance()->wait_for_channel(channel);
}

void QBKAudio::setNotifyAtEnd(int channel, volatile bool *b)
{
    return BKE_Audio::getInstance()->set_notify_at_end(channel, b);
}

bool QBKAudio::getRealTimeVolume(int channel, float *v)
{
    return BKE_Audio::getInstance()->getRealTimeVolume(channel, v);
}

int QBKAudio::musicVolume(int vol)
{
    return BKE_Audio::getInstance()->music_volume(vol);
}

int QBKAudio::soundVolume(int vol)
{
    return BKE_Audio::getInstance()->sound_volume(vol);
}

int QBKAudio::voiceVolume(int vol)
{
    return BKE_Audio::getInstance()->voice_volume(vol);
}

int QBKAudio::channelVolume(int channel,int vol)
{
    return BKE_Audio::getInstance()->channel_volume(channel, vol);
}

bool QBKAudio::isPlaying(int channel)
{
    return BKE_Audio::getInstance()->is_playing(channel);
}

void QBKAudio::fade(bklong channel, bklong duration, bklong endvol, bool stopAtEnd)
{
    return BKE_Audio::getInstance()->fade(channel, duration, endvol, stopAtEnd);
}

void QBKAudio::stopFade(int channel)
{
    return BKE_Audio::getInstance()->stopfade(channel);
}

void QBKAudio::stopAllFade()
{
    return BKE_Audio::getInstance()->stopallfade();
}
