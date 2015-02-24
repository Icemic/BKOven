#pragma once
#include "BitStream.h"
/*
* 用于MPEG-1/MPEG-2/MPEG-2.5 Audio Layer Ⅲ 主数据(main_data)解码时读位流并对哈夫曼数据解码。
*/
class BKMP3_BitStreamMainData :public BKMP3_BitStream
{
private:
	// 用于哈夫曼解码的大值区
	int lin[32];		// lin[0..15]=0
	const short *htbv[32];

	// 用于解码小值区
	const short *htc0, *htc1;

public:
	/*
	* 创建一个位流BitStreamMainData对象，位流的缓冲区大小len指定，位流的缓冲区尾部空出的长度由extra指定。
	* @param len 缓冲区可访问长度。
	* @param extra 缓冲区尾部空出的字节数。
	*/
	BKMP3_BitStreamMainData(int len, int extra);
	~BKMP3_BitStreamMainData();
	/*
	* 一个粒度组内的一个声道哈夫曼解码。
	*
	* @param region region[0..2]是解码时码表用于解码主数据区域的长度，region[3]是啥夫曼位总长度。
	* @param table_select 解码大值区码表的索引。
	* @param count1table_select 解码小值区(count1区)码表的索引
	* @param hv 接收解码得到的576个值。
	* @return 576减去零值区长度。
	*/
	int decodeHuff(int *region, int *table_select, int count1table_select, int *hv);
	//-------------------------------------------------------------------------
private:
	void initialize();
};