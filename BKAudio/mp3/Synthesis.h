#pragma once

#include <vector>
using std::vector;

/*
* 子带多相合成滤波。
*/
class BKMP3_Synthesis
{
public:
	/*
	* 子带多相合成滤波构造器。
	* @param nch 声道数，用于计算输出PCM时的步长值。
	*/
	BKMP3_Synthesis(int nch);
	~BKMP3_Synthesis();

	/*
	* 一个子带多相合成滤波。
	* @param samples 源数据，为32个样本值。
	* @param ch 当前的声道。左声道0，右声道1。
	* @param b 接收输出结果的PCM缓冲区。
	* @param off 缓冲区 b 的下标，本次开始向 b 写入数据的起始位置。
	* @return 缓冲区 b 的下标，本次向 b 写入的结束位置。
	*/
	int synthesisSubBand(float* samples, int ch, char *b, int off);
private:
	int nch;

	/*
	* 暂存DCT输出结果的FIFO队列。
	*/
	float(*fifobuf)[1024];

	/*
	* fifobuf的偏移量，用它完成FIFO队列的移位操作。
	*/
	int fifoIndex[2];

	/*
	* dewin: D[i] * 32767 (i=0..511), 然后重新排序
	* D[]: Coefficients Di of the synthesis window. ISO/IEC 11172-3 ANNEX_B Table 3-B.3
	*/
	static const float dewin[32][16];

	/*
	* 一个子带的矩阵运算。
	* @param src 输入的32个样本值。
	* @param dest 暂存输出值的长度为1024个元素的FIFO队列。
	* @param off FIFO队列的偏移量。一个子带一次矩阵运算输出64个值连续存储到FIFO队列，存储的起始位置由off指定。
	*/
	void dct32to64(float *src, float *dest, int off);
};

