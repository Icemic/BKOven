#pragma once
#include "BitStream.h"
/*
* ����MPEG-1/MPEG-2/MPEG-2.5 Audio Layer �� ������(main_data)����ʱ��λ�����Թ��������ݽ��롣
*/
class BKMP3_BitStreamMainData :public BKMP3_BitStream
{
private:
	// ���ڹ���������Ĵ�ֵ��
	int lin[32];		// lin[0..15]=0
	const short *htbv[32];

	// ���ڽ���Сֵ��
	const short *htc0, *htc1;

public:
	/*
	* ����һ��λ��BitStreamMainData����λ���Ļ�������Сlenָ����λ���Ļ�����β���ճ��ĳ�����extraָ����
	* @param len �������ɷ��ʳ��ȡ�
	* @param extra ������β���ճ����ֽ�����
	*/
	BKMP3_BitStreamMainData(int len, int extra);
	~BKMP3_BitStreamMainData();
	/*
	* һ���������ڵ�һ���������������롣
	*
	* @param region region[0..2]�ǽ���ʱ������ڽ�������������ĳ��ȣ�region[3]��ɶ����λ�ܳ��ȡ�
	* @param table_select �����ֵ������������
	* @param count1table_select ����Сֵ��(count1��)��������
	* @param hv ���ս���õ���576��ֵ��
	* @return 576��ȥ��ֵ�����ȡ�
	*/
	int decodeHuff(int *region, int *table_select, int count1table_select, int *hv);
	//-------------------------------------------------------------------------
private:
	void initialize();
};