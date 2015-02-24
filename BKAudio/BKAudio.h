#pragma once

#include "bkaudio_global.h"

enum AudioError
{
    NoError,
    FileNotFound = -1,
    InvalidFile = -2,
    NoMusicLoaded = -3,
    NoSeLoaded = -4,
    NoVoiceLoaded = -5,
};

class BKAUDIOSHARED_EXPORT QBKAudio
{
    QBKAudio();
    ~QBKAudio();
public:
    static QBKAudio *getInstance();

    AudioError playMusic(const QString &filename, int vol = 100, int times = -1, double loopto = 0., int fadein = 0);
    AudioError stopMusic(int fadeout = 0);
    AudioError stop(int channel, int fadeout = 0);
    void pause(int channel);
    void resume(int channel);
    AudioError playVoice(const QString &filename, int vol = 100);
    AudioError stopVoice(int fadeout = 0);
    AudioError playSound(const QString &filename, int channel, int vol, int times, bool automatic_cleanup, bool calcRealTimeVolume = false);
    AudioError stopSound(int channel, int fadeout = 0);
    AudioError stopAll(int fadeout = 0);
    AudioError pauseAll();
    AudioError resumeAll();
    void waitForChannel(int channel);
    void setNotifyAtEnd(int channel, volatile bool *b);
    bool getRealTimeVolume(int channel, float *v);
    int musicVolume(int vol);
    int soundVolume(int vol);
    int voiceVolume(int vol);
    int channelVolume(int channel,int vol);
    bool isPlaying(int channel);
    void fade(int channel, int duration, int endvol, bool stopAtEnd);
    void stopFade(int channel);
    void stopAllFade();
};
