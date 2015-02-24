#pragma once

#include "Synthesis.h"
#include "Header.h"

class BKMP3_Decoder;
class BKMP3_AbstractLayer
{
private:
	int channels;

	/*
	* 滤波器。
	*/
	BKMP3_Synthesis *filter;

	/*
	* 管理PCM缓冲区的变量
	*/
	char *pcmbuf;
	int size;			// 一次向音频缓冲区pcmbuf写入的长度

public:
	/*
	* 用指定帧头h和音频输出audio对象创建一个帧解码器。
	* @param h 已经解码帧头信息的帧头对象。
	* @param audio 音频输出对象。如果指定为 <b>null</b> 则调用 {@link #outputPCM()}
	* 不产生输出，仅移动PCM缓冲区指针（表示缓冲区已经取走，可重新写入数据）。
	*/
	BKMP3_AbstractLayer(BKMP3_Header *h);

	virtual ~BKMP3_AbstractLayer();

	/*
	* 从此缓冲区b中给定偏移量off处开始解码一帧（除帧头外的）音乐数据。
	* @param b 源数据缓冲区。
	* @param off 源数据缓冲区的偏移量。
	* @return 源数据缓冲区新的偏移量，用于计算解码下一帧数据的开始位置在源数据缓冲区的偏移量。
	*/
	virtual int decodeOneFrame(char *b, int off) = 0;

	/*
	* 跳过该帧的解码。返回帧尾的偏移量。
	* @param b 源数据缓冲区。
	* @param off 源数据缓冲区的偏移量。
	* @param fillMainData 是否填充mainData。
	* @return 源数据缓冲区新的偏移量，用于计算解码下一帧数据的开始位置在源数据缓冲区的偏移量。
	*/
	virtual int ignoreOneFrame(char *b, int off, bool fillMainData) = 0;

	/*
	* 一个子带多相合成滤滤。
	* @param samples 输入的32个样本值。
	* @param ch 当前声道。0表示左声道，1表示右声道。
	*/
protected:
	void synthesisSubBand(float *samples, int ch);

	/*
	* 尝试输出解码结果。
	* 多相合成滤波输出的PCM数据写入缓冲区，当缓冲区至少填入解码4帧得到的PCM数据才产生一次输出，
	* 但调用者并不需要知道当前缓冲区是否已经填入足够数据。 防止缓冲区溢出，每解码最多4帧应调用本方法1次，
	* 当然也可以每解码1帧就调用本方法1次。
	* 若产生音频输出将，将从缓冲区取走解码4帧得到的PCM数据。
	* 可能产生阻塞：若音频输出已经停止，对本方法的调用将被阻塞，直到开始音频输出。如果输入流解码完，阻塞被自动清除。
	* @see #startAudio()
	*/
	void outputPCM(char **ptr, int *size, int &ignorePCMSize);

private:
	int writeCursor[2];
	void resetOffset() {
		writeCursor[0] = 0;
		writeCursor[1] = 2;
	}
	friend class BKMP3_Decoder;
};