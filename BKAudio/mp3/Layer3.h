#pragma once

#include <cmath>

#include "AbstractLayer.h"
#include "BitStream.h"
#include "ChannelInformation.h"
#include "BitStreamMainData.h"

/*
* 解码Layer Ⅲ。
*/
class BKMP3_Layer3 :
	public BKMP3_AbstractLayer
{
private:
	int ngr;		// Number of granules
	int nch;		// Number of channels
	BKMP3_Header *header;
	BKMP3_BitStream *bsSI;	// 读帧边信息(Side Information)位流
	BKMP3_BitStreamMainData *maindataStream;
	int main_data_begin;
	int scfsi[2]; // [nch],scale-factor selector information
	BKMP3_ChannelInformation *infoCh0; // [ngr]
	BKMP3_ChannelInformation *infoCh1; // [ngr]
	const int *sfbIndexLong;
	const int *sfbIndexShort;
	bool isMPEG1;

	bool coresEnabled;
	int pcmbufCount;
	//AudioDataConcurrent audiodataCh0, audiodataCh1;
	int nadc;
public:

	/*
	* 创建一个 Layer Ⅲ 帧解码器，默认启用cpu多核心解码优化。
	* @param h 已经解码帧头信息的帧头对象。
	* @param audio 音频输出对象。
	* @see #Layer3(Header, IAudio, boolean)
	*/

	/*
	* 创建一个 Layer Ⅲ 帧解码器。
	* <br>如果本程序运行在多核心CPU硬件平台上，建议启用多核心CPU解码，这样能够大幅度提高解码速度。
	* <br>如果本程序运行在单核心CPU硬件平台上，建议禁用多核心CPU解码，否则会使解码速度略有下降。
	* <p>如果用 {@link #Layer3(Header, IAudio)} 创建Layer3对象，缺省情况下启用了cpu多核心优化。
	* @param h 已经解码帧头信息的帧头对象。
	* @param audio 音频输出对象。
	* @param coresEnabled 若为<b>true</b>启用多核心CPU解码优化，若为<b>false</b>禁用多核心CPU解码优化。
	*/
	BKMP3_Layer3(BKMP3_Header *h, bool coresEnabled = true);
	~BKMP3_Layer3();
protected:
	void initialize();

private:
	//1.
	//>>>>SIDE INFORMATION (part1)=============================================
	//private int part2_3_bits;//----debug
	int getSideInfo(char *b, int off);
	//<<<<SIDE INFORMATION=====================================================

	//2.
	//>>>>SCALE FACTORS========================================================
	int(*scalefacLong)[23];		// [nch][23];
	int(*scalefacShort)[39];		// [nch][13*3];

	// -------------------------------- MPEG-1 --------------------------------
	static const int slen0[16];
	static const int slen1[16];

	void getScaleFactors(int gr, int ch);


	// -------------------------------- MPEG-2 --------------------------------
	// x_slen: 增益因子(scalefactor)比特数
	//移动至Layer3StaticData
	
	void getScaleFactorsLSF(int gr, int ch);
	//<<<<SCALE FACTORS========================================================

	//3.
	//>>>>HUFFMAN BITS=========================================================
	int region[4];	// 某一码表用于解码主数据的区域(最多3个区段)
	int hv[32 * 18 + 4];		// [32 * 18 + 4], 暂存解得的哈夫曼值

	void huffBits(int gr, int ch);
	//<<<<HUFFMAN BITS=========================================================

	//4.
	//>>>>REQUANTIZATION & REORDER=============================================
	int widthLong[22];	// [22] 长块的增益因子频带(用一个增益因子逆量化频率线的条数)
	int widthShort[13];	// [13] 短块的增益因子频带
	int rzeroBandLong;
	int rzeroBandShort[3];

	// ISO/IEC 11172-3 ANNEX B,Table 3-B.6. Layer III Preemphasis
	static const int pretab[22];

	/*
	* 逆量化并对短块(纯短块和混合块中的短块)重排序.在逆量化时赋值的变量:<br>
	* rzero_bandL -- 长块非零哈夫曼值的频带数,用于强度立体声(intensity stereo)处理<br>
	* rzero_bandS -- 短块非零哈夫曼值的频带数,用于强度立体声处理<br>
	* rzero_index -- 非零哈夫曼值的"子带"数
	* <p>
	* Layer3 逆量化公式ISO/IEC 11172-3, 2.4.3.4
	* <p>
	* XRi = pow(2, 0.25 * (global_gain - 210)) <br>
	* if (LONG block types) <br>
	*　　XRi *= pow(2, -0.5 * (1 + scalefac_scale) * (L[sfb] + preflag * pretab[sfb])) <br>
	* if (SHORT block types) { <br>
	*　　XRi *= pow(2, 0.25 * -8 * subblock_gain[sfb]) <br>
	*　　XRi *= pow(2, 0.25 * -2 * (1 + scalefac_scale) * S[scf]) } <br>
	* XRi *= sign(haffVal) * pow(abs(haffVal), 4/3) <br>
	*
	*/
	void requantizer(int ch, BKMP3_ChannelInformation *ci);
	//<<<<REQUANTIZATION & REORDER=============================================


	//5.
	//>>>>STEREO===============================================================

	// 在requantizer方法内已经作了除以根2处理,ms_stereo内不再除以根2.
	void ms_stereo(int gr);

	static const float is_coef_lsf[2][15];
	static const float is_coef[7];

	// 解码一个频带强度立体声,MPEG-1
	void is_lines(int pos, int idx0, int width, int step, int gr);

	// 解码一个频带强度立体声,MPEG-2
	void is_lines_lsf(int tab2, int pos, int idx0, int width, int step, int gr);

	/*
	* 强度立体声(intensity stereo)解码
	*
	* ISO/IEC 11172-3不对混合块中的长块作强度立体声处理,但很多MP3解码程序都作了处理.
	*/
	void intensity_stereo(int gr);
	//<<<<STEREO===============================================================

	//6.
	//>>>>ANTIALIAS============================================================

	void antialias(int ch, BKMP3_ChannelInformation *ci);
	//<<<<ANTIALIAS============================================================

	//7.
	//>>>>HYBRID(synthesize via iMDCT)=========================================
	static const float imdctWIN[4][36];

	void imdct12(float *preBlck, float *recv, int off);
	void imdct36(float *preBlck, float *recv, int off, int block_type);

	float *preBlckCh0; // [32*18],左声道FIFO队列
	float *preBlckCh1; // [32*18],右声道FIFO

	void hybrid(int ch, BKMP3_ChannelInformation *ci);
	//<<<<HYBRID(synthesize via iMDCT)=========================================

	//8.
	//>>>>INVERSE QUANTIZE SAMPLES=============================================
	//
	// 在 decodeAudioDataChannel 方法内实现多相频率倒置
	//
	//<<<<INVERSE QUANTIZE SAMPLES=============================================

	//9.
	//>>>>SYNTHESIZE VIA POLYPHASE MDCT========================================
	//
	// jmp123.decoder.Synthesis.synthesisSubBand 方法
	//
	//<<<<SYNTHESIZE VIA POLYPHASE MDCT========================================

	//10.
	//>>>>OUTPUT PCM SAMPLES===================================================
	//
	// jmp123.output.Audio 类
	//
	//<<<<OUTPUT PCM SAMPLES===================================================
public:
	/*
	* 解码1帧Layer Ⅲ除帧头外的音乐数据。
	*/
	virtual int decodeOneFrame(char *b, int off);

	/*
	* 忽略1帧。
	*/

	virtual int ignoreOneFrame(char *b, int off, bool fillMainData);

private:
	//private int owt; //----debug

	/*
	* samplesChx: 暂存从子带合成(hybrid)的结果中抽取出来的32个样本值,
	* 这32个样本值作为输入数据被送入多相合成滤波器.
	*/
	float *samplesCh0, *samplesCh1;
	// 一个声道消混叠、子带合成、多相频率倒置和多相合成滤波
	void decodeAudioDataChannel(BKMP3_ChannelInformation *ci, int ch);

public:
	void close(bool interrupted);
};

