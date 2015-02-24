#pragma once

/*
* һ��������һ����������Ϣ
*/
class BKMP3_ChannelInformation
{
public:
	friend class BKMP3_Layer3;
	~BKMP3_ChannelInformation();
private:
	// ��λ����ȡ���ݰ�����˳�����γ�ʼ����14������,��getSideInfo����.
	int part2_3_length;
	int big_values;
	int global_gain;
	int scalefac_compress;
	int window_switching_flag;
	int block_type;
	int mixed_block_flag;
	int table_select[3];
	int subblock_gain[3];
	int region0_count;
	int region1_count;
	int preflag;
	int scalefac_scale;
	int count1table_select;

	int part2_length;	//��������(scale-factor)������
	int nonzero;		//��ֵ����Сֵ���ܳ���
	float *buf;		//�ݴ�һ�������ڵ�һ����������������������Ӵ��ϳɵȲ���������м���
	BKMP3_ChannelInformation();
};

