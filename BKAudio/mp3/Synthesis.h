#pragma once

#include <vector>
using std::vector;

/*
* �Ӵ�����ϳ��˲���
*/
class BKMP3_Synthesis
{
public:
	/*
	* �Ӵ�����ϳ��˲���������
	* @param nch �����������ڼ������PCMʱ�Ĳ���ֵ��
	*/
	BKMP3_Synthesis(int nch);
	~BKMP3_Synthesis();

	/*
	* һ���Ӵ�����ϳ��˲���
	* @param samples Դ���ݣ�Ϊ32������ֵ��
	* @param ch ��ǰ��������������0��������1��
	* @param b ������������PCM��������
	* @param off ������ b ���±꣬���ο�ʼ�� b д�����ݵ���ʼλ�á�
	* @return ������ b ���±꣬������ b д��Ľ���λ�á�
	*/
	int synthesisSubBand(float* samples, int ch, char *b, int off);
private:
	int nch;

	/*
	* �ݴ�DCT��������FIFO���С�
	*/
	float(*fifobuf)[1024];

	/*
	* fifobuf��ƫ�������������FIFO���е���λ������
	*/
	int fifoIndex[2];

	/*
	* dewin: D[i] * 32767 (i=0..511), Ȼ����������
	* D[]: Coefficients Di of the synthesis window. ISO/IEC 11172-3 ANNEX_B Table 3-B.3
	*/
	static const float dewin[32][16];

	/*
	* һ���Ӵ��ľ������㡣
	* @param src �����32������ֵ��
	* @param dest �ݴ����ֵ�ĳ���Ϊ1024��Ԫ�ص�FIFO���С�
	* @param off FIFO���е�ƫ������һ���Ӵ�һ�ξ����������64��ֵ�����洢��FIFO���У��洢����ʼλ����offָ����
	*/
	void dct32to64(float *src, float *dest, int off);
};

