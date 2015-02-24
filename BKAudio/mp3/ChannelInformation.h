#pragma once

/*
* 一个粒度内一个声道的信息
*/
class BKMP3_ChannelInformation
{
public:
	friend class BKMP3_Layer3;
	~BKMP3_ChannelInformation();
private:
	// 从位流读取数据按下列顺序依次初始化的14个变量,见getSideInfo方法.
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

	int part2_length;	//增益因子(scale-factor)比特数
	int nonzero;		//大值区和小值区总长度
	float *buf;		//暂存一个粒度内的一个声道逆量化、消混叠、子带合成等步骤产生的中间结果
	BKMP3_ChannelInformation();
};

