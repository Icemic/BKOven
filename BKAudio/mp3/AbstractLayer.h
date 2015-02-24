#pragma once

#include "Synthesis.h"
#include "Header.h"

class BKMP3_Decoder;
class BKMP3_AbstractLayer
{
private:
	int channels;

	/*
	* �˲�����
	*/
	BKMP3_Synthesis *filter;

	/*
	* ����PCM�������ı���
	*/
	char *pcmbuf;
	int size;			// һ������Ƶ������pcmbufд��ĳ���

public:
	/*
	* ��ָ��֡ͷh����Ƶ���audio���󴴽�һ��֡��������
	* @param h �Ѿ�����֡ͷ��Ϣ��֡ͷ����
	* @param audio ��Ƶ����������ָ��Ϊ <b>null</b> ����� {@link #outputPCM()}
	* ��������������ƶ�PCM������ָ�루��ʾ�������Ѿ�ȡ�ߣ�������д�����ݣ���
	*/
	BKMP3_AbstractLayer(BKMP3_Header *h);

	virtual ~BKMP3_AbstractLayer();

	/*
	* �Ӵ˻�����b�и���ƫ����off����ʼ����һ֡����֡ͷ��ģ��������ݡ�
	* @param b Դ���ݻ�������
	* @param off Դ���ݻ�������ƫ������
	* @return Դ���ݻ������µ�ƫ���������ڼ��������һ֡���ݵĿ�ʼλ����Դ���ݻ�������ƫ������
	*/
	virtual int decodeOneFrame(char *b, int off) = 0;

	/*
	* ������֡�Ľ��롣����֡β��ƫ������
	* @param b Դ���ݻ�������
	* @param off Դ���ݻ�������ƫ������
	* @param fillMainData �Ƿ����mainData��
	* @return Դ���ݻ������µ�ƫ���������ڼ��������һ֡���ݵĿ�ʼλ����Դ���ݻ�������ƫ������
	*/
	virtual int ignoreOneFrame(char *b, int off, bool fillMainData) = 0;

	/*
	* һ���Ӵ�����ϳ����ˡ�
	* @param samples �����32������ֵ��
	* @param ch ��ǰ������0��ʾ��������1��ʾ��������
	*/
protected:
	void synthesisSubBand(float *samples, int ch);

	/*
	* ���������������
	* ����ϳ��˲������PCM����д�뻺�������������������������4֡�õ���PCM���ݲŲ���һ�������
	* �������߲�����Ҫ֪����ǰ�������Ƿ��Ѿ������㹻���ݡ� ��ֹ�����������ÿ�������4֡Ӧ���ñ�����1�Σ�
	* ��ȻҲ����ÿ����1֡�͵��ñ�����1�Ρ�
	* ��������Ƶ����������ӻ�����ȡ�߽���4֡�õ���PCM���ݡ�
	* ���ܲ�������������Ƶ����Ѿ�ֹͣ���Ա������ĵ��ý���������ֱ����ʼ��Ƶ�������������������꣬�������Զ������
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