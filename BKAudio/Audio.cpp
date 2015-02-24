#include "Audio.h"
#include <iostream>

#define BKE_Audio_FOREACH() for (chan_t::iterator i=this->channels.begin(),e=this->channels.end();i!=e;++i)

const bklong BKE_Audio::initial_channel_counter=1<<20;
const bklong BKE_Audio::max_valid_channel=(bklong)initial_channel_counter-1;
BKE_Audio *BKE_Audio::_interface=NULL;

BKE_Audio::BKE_Audio(){
	this->uninitialized=1;
    this->threadInitialFinished = false;
	this->channel_counter=initial_channel_counter;
	this->mvol=this->svol=this->vvol=1.f;
	this->stop_thread=0;
	this->thread.call(std::bind(&BKE_Audio::update_thread,this),1);
	while (threadInitialFinished == false)
		BKE_Thread::sleep(1);
}

BKE_Audio::~BKE_Audio(){
	if (this->uninitialized)
		return;
	this->stop_thread=1;
	this->thread.join();
	BKE_MutexLocker ml(mutex);
	for (auto i = fadeParameters.begin(), e = fadeParameters.end(); i != e; i++)
	{
        delete i.value();
	}
}
/*

#ifdef WIN32
class CSEHException2 {

public:

	CSEHException2(unsigned int code, PEXCEPTION_POINTERS pep)
	{

		m_exceptionCode = code;

		m_exceptionRecord = *pep->ExceptionRecord;

		m_context = *pep->ContextRecord;

		assert(m_exceptionCode == m_exceptionRecord.ExceptionCode);

	}

	operator unsigned int() { return m_exceptionCode; }



	//same as exceptionRecord.ExceptionCode

	unsigned int m_exceptionCode;

	//exception code, crash address, etc,

	EXCEPTION_RECORD m_exceptionRecord;

	// CPU registers and flags

	CONTEXT m_context;

};

void _cdecl TranslateSEHtoCE2(UINT code, PEXCEPTION_POINTERS pep)
{
	throw CSEHException2(code, pep);
}

#endif

extern const char* CompileTime;*/

void BKE_Audio::update_thread(){
/*
#ifdef WIN32
	_set_se_translator(TranslateSEHtoCE2);
	try
	{
#endif*/
	this->dev = new audio_device;
	if (!*this->dev){
		delete this->dev;
		this->dev = 0;
		this->threadInitialFinished = true;
		return;
	}
	this->uninitialized = 0;
	this->threadInitialFinished = true;	
	while (!this->stop_thread)
	{
		{
			BKE_MutexLocker ml(mutex);
			auto i = fadeParameters.begin();
			while (i != fadeParameters.end())
			{
                audiofade_param *p = i.value();
                audio_stream *stream = get_channel(i.key());
				if (!stream)
				{
					delete p;
					auto e = i++;
					fadeParameters.erase(e);
					continue;
				}
				BKE_Clock::t elapsed = p->clock.get() - p->begin_time;
                float progress = qMax(0.0,                                  // needed for rewind. elapsed could be negative
                    qMin(1.0, elapsed /
                    qMax(p->duration, 1)   // division by 0
					)
					);
				float nowvol = p->startVol + (p->endVol - p->startVol) * progress;
				{
					stream->set_volume(nowvol);
				}
				if (elapsed >= p->duration)
				{
					if (p->stopAtEnd)
					{
						stream->stop();
					}
					delete p;
					auto e = i++;
					fadeParameters.erase(e);
					continue;
				}
				i++;
			}
		}
		{
			BKE_MutexLocker ml(this->mutex);
			std::vector<audio_stream *> removed_streams;
			this->dev->update(removed_streams);
			for (size_t a = 0; a < removed_streams.size(); a++){
				BKE_Audio_FOREACH(){
                    if (i.value() == removed_streams[a]){
						this->channels.erase(i);
						break;
					}
				}
			}
		}
		BKE_Thread::sleep(5);
	}
	delete this->dev;
	this->dev = 0;
/*
#ifdef WIN32
	}
	catch (CSEHException2 &e)
	{
		FILE *fp = fopen("BKEexception.txt", "w");
		fprintf(fp, "Exe version : %s\n", CompileTime);
		fprintf(fp, "crash address : %08X\n", e.m_exceptionRecord.ExceptionAddress);
		void* stack[3];
		CaptureStackBackTrace(0, 3, stack, NULL);
		int pos = (int)stack[0];
		int offset = *(int*)(pos - 4);
		int pos2 = pos + offset;
		fprintf(fp, "reference address : %08X  (BKE_Audio::update_thread)\n", pos2);
		fprintf(fp, "crash code : %08X\n", e.m_exceptionCode);
		fprintf(fp, "crash parameters:\n");
		for (unsigned int i = 0; i < e.m_exceptionRecord.NumberParameters; i++)
			fprintf(fp, "%08X\n", e.m_exceptionRecord.ExceptionInformation[i]);
		char msg[1024];
		msg[FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, e.m_exceptionCode & 0xFFFF, 0, msg, 1024, NULL)] = '\0';
		if (msg[0])
		{
			fprintf(fp, "\nerror explanation : %s", msg);
		}
		//unwind call stack
		fprintf(fp, "\ncall stack:\n");
		int ebp = e.m_context.Ebp;
		int esp = e.m_context.Esp;
		while (1)
		{
			int newebp = *(int*)ebp;
			int newesp = *(int*)(ebp + 4);
			if (!newesp || !newebp)
				break;
			int offset = *(int*)(newesp - 4);
			if (*(unsigned char *)(newesp - 5) == 0xE8)
				fprintf(fp, "%08X => %08X\n", newesp - 5, newesp + offset);
			else //simple think it is a immediately call 0xFF 0xXX
			{
				//we guess target by search 0x55 0x8B 0xEC
				unsigned char s[] = { 0x55, 0x8B, 0xEC };
				int curpos = esp - 5;
				int maxsearchpos = curpos - 10000;
				if (maxsearchpos < 0)
					maxsearchpos = 0;
				while (curpos >= maxsearchpos)
				{
					if (!memcmp((void*)curpos, s, 3))
						break;
					else
						curpos--;
				}
				if (curpos > 0)
					fprintf(fp, "dynamic call from %08X => %08X (most possible)\n", newesp - 2, curpos);
				else
					fprintf(fp, "dynamic call from %08X\n", newesp - 2);
			}
			ebp = newebp;
			esp = newesp;
		}
		//dump registers
		fprintf(fp, "\nregisters:\nEAX:%08X\nEBX:%08X\nECX:%08X\nEDX:%08X\nEDI:%08X\nESI:%08X\nEBP:%08X\nESP:%08X\nEIP:%08X\n",
			e.m_context.Eax, e.m_context.Ebx, e.m_context.Ecx, e.m_context.Edx, e.m_context.Edi, e.m_context.Esi, e.m_context.Ebp, e.m_context.Esp, e.m_context.Eip);
		//dump local stack
		fprintf(fp, "\nstack:\n");
		ebp = e.m_context.Ebp;
		esp = e.m_context.Esp;
		while (esp < ebp)
		{
			fprintf(fp, "%08X\n", *(int*)esp);
			esp += 4;
		}
		fclose(fp);
		MessageBox(NULL, L"音频线程发生了错误，请将BKEexception.txt里的信息反馈给开发者，谢谢！", L"出错啦", MB_OK);
		exit(-1);
	}
	catch (...)
	{
		MessageBox(NULL, L"音频线程发生了未知错误", L"出错啦", MB_OK);
		exit(-1);
	}
#endif*/
}

audio_stream *BKE_Audio::get_channel(int channel){
	BKE_MutexLocker ml(this->mutex);
	chan_t::iterator i=this->channels.find(channel);
    return (i==this->channels.end())?0:i.value();
}

static const char* audioSuffix[]=
{
    ".ogg",
    ".mp3",
#ifdef USE_OPUS
    ".opus",
#endif
    ".wav",
    NULL
};

bool BKE_Audio::prepare(const QString &filename, QString &found_path)
{
    if (QFile::exists(filename))
    {
        found_path=filename;
        return true;
    }
    for (int i = 0; audioSuffix[i]; i++)
    {
        found_path=filename+audioSuffix[i];
        if (QFile::exists(found_path))
        {
            return true;
        }
    }
    found_path.clear();
    return false;
}

#define RETURNNONE() return NoError
#define RETURNERROR(tag, ...) return tag
#define RETURNWARNING(tag, ...) return tag

AudioError BKE_Audio::play_music(const QString &filename, bklong vol /*= 100*/, bklong times /*= -1*/, double loopto /*= 0.*/, bklong fadein /*= 0*/)
{
	if (this->uninitialized)
		RETURNNONE();
	const int channel=MUSIC_CHANNEL;
    QString found_path;
	if (!prepare(filename, found_path))
        return FileNotFound;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (stream)
	{
		stopfade(channel);
		this->dev->remove(stream);
	}
	stream=new audio_stream(found_path);
	if (!*stream){
		delete stream;
        RETURNERROR(InvalidFile, found_path);
	}
	stream->loop=times;
	stream->cleanup=0;
	stream->set_volume(vol / 100.0);
	stream->set_general_volume(this->mvol);
	stream->loopto=loopto;
	stream->start();
	this->dev->add(stream);
	this->channels[MUSIC_CHANNEL]=stream;
	if (fadein)
	{
		stream->set_volume(0.f);
		fade(MUSIC_CHANNEL, fadein, vol ,false);
	}
	RETURNNONE();
}

AudioError BKE_Audio::play_voice(const QString &filename, bklong vol /*= 100*/){
	if (this->uninitialized)
		RETURNNONE();
	const int channel=VOICE_CHANNEL;
    QString found_path;
	if (!prepare(filename, found_path))
        return FileNotFound;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (stream)
	{
		stopfade(channel);
		this->dev->remove(stream);
	}
	stream=new audio_stream(found_path);
	if (!*stream){
		delete stream;
        RETURNERROR(InvalidFile, found_path);
	}
	stream->loop=0;
	stream->cleanup=1;
	stream->set_general_volume(this->vvol);
	stream->start();
	this->dev->add(stream);
	this->channels[channel]=stream;
	RETURNNONE();
}

AudioError BKE_Audio::stop(bklong channel, bklong fadeout /*= 0*/){
	switch (channel)
	{
	case VOICE_CHANNEL:
		return stop_voice(fadeout);
	case MUSIC_CHANNEL:
		return stop_music(fadeout);
	case ALL_SOUND:
		return stop_all(fadeout);
	default:
		return stop_sound(channel, fadeout);
	}
}

AudioError BKE_Audio::stop_music(bklong fadeout /*= 0*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(MUSIC_CHANNEL);
	if (!stream)
	{
        RETURNWARNING(NoMusicLoaded);
	}
	if (fadeout)
	{
		fade(MUSIC_CHANNEL, fadeout, 0, true);
	}
	else
	{
		stream->stop();
	}
	RETURNNONE();
}

void BKE_Audio::pause(bklong channel){
	if (this->uninitialized)
		return;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
		return;
	stream->pause();
	return;
}

void BKE_Audio::resume(bklong channel){
	if (this->uninitialized)
		return;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
		return;
	stream->start();
	return;
}

//大括号怎么不换行！楼上楼下都是异端！
void BKE_Audio::seek(bklong channel, double position)
{
    if (this->uninitialized)
        return;
    BKE_MutexLocker ml(this->mutex);
    audio_stream *stream=this->get_channel(channel);
    if (!stream)
        return;
    stream->seek(position);
    return;
}

double BKE_Audio::tell(bklong channel)
{
    if (this->uninitialized)
        return 0;
    BKE_MutexLocker ml(this->mutex);
    audio_stream *stream=this->get_channel(channel);
    if (!stream)
        return 0;
    return stream->tell();;
}

AudioError BKE_Audio::play_sound(const QString &filename, bklong channel, bklong vol, bklong times, bool automatic_cleanup, bool calcRealTimeVolume /*= false*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
    AudioError e;
	e = this->load_sound_on_a_channel(filename,channel);
    if (e != NoError)
		return e;
	return this->play(channel, vol, times, automatic_cleanup, calcRealTimeVolume);
}

AudioError BKE_Audio::load_sound_on_a_channel(const QString &filename,bklong channel){
	if (this->uninitialized)
		RETURNNONE();
    QString found_path;
	if (!prepare(filename, found_path))
        return FileNotFound;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (stream)
	{
		stopfade(channel);
		this->dev->remove(stream);
	}
	stream = new audio_stream(found_path);
	if (!*stream){
		delete stream;
        RETURNERROR(InvalidFile, found_path);
	}
	this->dev->add(stream);
	this->channels[channel]=stream;
	RETURNNONE();
}

AudioError BKE_Audio::unload_sound_from_channel(bklong channel){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
	{
        RETURNWARNING(NoSeLoaded, channel);
	}
	this->dev->remove(stream);
    this->channels.remove(channel);
	RETURNNONE();
}

AudioError BKE_Audio::play(bklong channel, bklong vol, bklong time, bool sautomatic_cleanup, bool calcRealTimeVolume /*= false*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
	{
        RETURNWARNING(NoSeLoaded, channel);
	}
	stream->loop=time;
	stream->cleanup=sautomatic_cleanup;
	stream->set_general_volume(channel==MUSIC_CHANNEL?this->mvol : channel==VOICE_CHANNEL?this->vvol : this->svol);
	stream->set_volume(vol / 100.f);
	stream->calcRealTimeVolume = calcRealTimeVolume;
	stream->start();
	RETURNNONE();
}

bool BKE_Audio::getRealTimeVolume(bklong channel, float *v)
{
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream = this->get_channel(channel);
	if (!stream)
	{
		*v = 0.f;
		return false;
	}
	*v = stream->getRealTimeVolume();
	return true;
}

void BKE_Audio::wait_for_channel(bklong channel){
	BKE_Notify event;
	event.init();
	{
		BKE_MutexLocker ml(this->mutex);
		audio_stream *stream=this->get_channel(channel);
		stream->notify_on_stop(&event);
	}
	event.wait();
}

AudioError BKE_Audio::stop_sound(bklong channel ,bklong fadeout /*= 0*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
	{
        RETURNWARNING(NoSeLoaded, channel);
	}
	if (fadeout)
	{
		fade(channel, fadeout, 0, true);
	}
	else
	{
		stream->stop();
	}
	RETURNNONE();
}

AudioError BKE_Audio::stop_all(bklong fadeout /*= 0*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	BKE_Audio_FOREACH()
	{
		if (fadeout)
		{
            fade(i.key(), fadeout, 0, true);
		}
		else
		{
            i.value()->stop();
		}
	}
	RETURNNONE();
}

AudioError BKE_Audio::pause_all()
{
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	BKE_Audio_FOREACH()
        i.value()->pause();
	RETURNNONE();
}

AudioError BKE_Audio::resume_all()
{
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	BKE_Audio_FOREACH()
        i.value()->start();
	RETURNNONE();
}


AudioError BKE_Audio::stop_voice(bklong fadeout /*= 0*/){
	if (this->uninitialized)
		RETURNNONE();
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(VOICE_CHANNEL);
	if (!stream)
	{
        RETURNWARNING(NoVoiceLoaded);
	}
	if (fadeout)
	{
		fade(VOICE_CHANNEL, fadeout, 0, true);
	}
	else
	{
		stream->stop();
	}
	RETURNNONE();
}

bklong BKE_Audio::set_volume(float &p, bklong vol, bklong channel){
	if (this->uninitialized)
		return 0;
	if (vol<0){
		BKE_MutexLocker ml(this->mutex);
		return int(p*100.f);
	}
	BKE_MutexLocker ml(this->mutex);
	p=vol/100.f;
	if (channel==ALL_SOUND)
	{
		BKE_Audio_FOREACH()
            if (i.key() >= 0 && i.value())
			{
                i.value()->set_general_volume(p);
			}
		return vol;
	}
	audio_stream *stream=this->get_channel(channel);
	if(stream)
		stream->set_general_volume(p);
	return vol;
}

bklong BKE_Audio::channel_volume(bklong channel,bklong vol){
	if (this->uninitialized)
		return -1;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
		return -1;
	if (vol<0){
		return int(stream->get_volume()*100.f);
	}
	stream->set_volume(vol/100.f);
	return vol;
}

bool BKE_Audio::is_playing(bklong channel){
	if (this->uninitialized)
		return 0;
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream=this->get_channel(channel);
	if (!stream)
		return 0;
	return stream->is_sink_playing();
}

void BKE_Audio::set_notify_at_end(bklong channel, BKE_Notify *b)
{
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream = this->get_channel(channel);
	stream->notify_on_stop(b);
}

void BKE_Audio::set_notify_at_end(bklong channel, volatile bool *b)
{
	BKE_MutexLocker ml(this->mutex);
	audio_stream *stream = this->get_channel(channel);
	stream->notify_on_stop(b);
}

void BKE_Audio::fade(bklong channel, bklong duration, bklong endvol ,bool stopAtEnd)
{
	audio_stream *stream = get_channel(channel);
	if (!stream)
	{
		return;
	}
	audiofade_param * p = new audiofade_param;
	p->duration = duration;
	p->startVol = stream->get_volume();
	p->endVol = endvol / 100.f;
	p->stopAtEnd = stopAtEnd;
	p->begin_time = p->clock.get();
	BKE_MutexLocker ml(mutex);
	auto i = fadeParameters.find(channel);
	if (i != fadeParameters.end())
	{
        delete i.value();
        i.value() = p;
	}
	else
	{
		fadeParameters[channel] = p;
	}
}

void BKE_Audio::stopfade(int channel)
{
	BKE_MutexLocker ml(mutex);
	auto i = fadeParameters.find(channel);
	if (i != fadeParameters.end())
	{
        if (i.value()->stopAtEnd)
		{
			stop(channel);
		}
        delete i.value();
		fadeParameters.erase(i);
	}
}

void BKE_Audio::stopallfade()
{
	BKE_MutexLocker ml(mutex);
	for (auto it = fadeParameters.begin(); it != fadeParameters.end();it++)
	{
        if (it.value()->stopAtEnd)
		{
            stop(it.key());
		}
        delete it.value();
	}
	fadeParameters.clear();
}

/*
TiXmlElement *save_channel(int no,const audio_stream &stream){
	bool is_playing=stream.is_playing(),
		loop=!!stream.loop;
	if (!loop && is_playing)
		return 0;
	TiXmlElement *channel=new TiXmlElement("channel");
	channel->SetAttribute("no",no);
	channel->SetAttribute("path",stream.filename);
	channel->SetAttribute("loop",loop);
	channel->SetAttribute("volume",itoac(stream.get_volume()));
	channel->SetAttribute("playing",is_playing);
	return channel;
}

void BKE_Audio::load_channel(TiXmlElement *element){
	int index=element->QueryIntAttribute("no");
	audio_stream *stream=new audio_stream(element->QueryWStringAttribute("path"),index==BKE_Audio::music_channel);
	if (!*stream){
		delete stream;
		return;
	}
	this->channels[index]=stream;
	this->dev->add(stream);
	stream->loop=(element->QueryIntAttribute("loop"))?-1:0;
	stream->set_volume(element->QueryFloatAttribute("volume"));
	stream->set_general_volume((index==this->music_channel)?this->mvol:this->svol);
	stream->mute(!this->notmute);
	if (element->QueryIntAttribute("playing"))
		stream->start();
}

TiXmlElement *BKE_Audio::save_channels(){
	TiXmlElement *channels=new TiXmlElement("channels");
	BKE_MutexLocker ml(this->mutex);
	BKE_Audio_FOREACH(){
        TiXmlElement *el=save_channel(i.key(),*i.value());
		if (!el)
			continue;
		channels->LinkEndChild(el);
	}
	return channels;
}

void BKE_Audio::load_channels(TiXmlElement *parent){
    TiXmlElement *channels=parent.key()ChildElement("channels");
    for (TiXmlElement *i=channels.key()ChildElement();i;i=i->NextSiblingElement())
		this->load_channel(i);
}

TiXmlElement *BKE_Audio::save(){
	TiXmlElement *audio=new TiXmlElement("audio");
	if (this->uninitialized)
		return audio;
	audio->SetAttribute("music_volume",this->music_volume(-1));
	audio->SetAttribute("sfx_volume",this->sound_volume(-1));
	audio->LinkEndChild(this->save_channels());
	return audio;
}

void BKE_Audio::load(TiXmlElement *parent){
	if (this->uninitialized)
		return;
    TiXmlElement *audio=parent.key()ChildElement("audio");
	this->music_volume(audio->QueryIntAttribute("music_volume"));
	this->sound_volume(audio->QueryIntAttribute("sfx_volume"));
	BKE_MutexLocker ml(this->mutex);
	BKE_Audio_FOREACH(){
        this->dev->remove(i.value());
	}
	this->channels.clear();
	this->load_channels(audio);
}
*/
