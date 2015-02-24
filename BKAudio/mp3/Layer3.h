#pragma once

#include <cmath>

#include "AbstractLayer.h"
#include "BitStream.h"
#include "ChannelInformation.h"
#include "BitStreamMainData.h"

/*
* ����Layer ��
*/
class BKMP3_Layer3 :
	public BKMP3_AbstractLayer
{
private:
	int ngr;		// Number of granules
	int nch;		// Number of channels
	BKMP3_Header *header;
	BKMP3_BitStream *bsSI;	// ��֡����Ϣ(Side Information)λ��
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
	* ����һ�� Layer �� ֡��������Ĭ������cpu����Ľ����Ż���
	* @param h �Ѿ�����֡ͷ��Ϣ��֡ͷ����
	* @param audio ��Ƶ�������
	* @see #Layer3(Header, IAudio, boolean)
	*/

	/*
	* ����һ�� Layer �� ֡��������
	* <br>��������������ڶ����CPUӲ��ƽ̨�ϣ��������ö����CPU���룬�����ܹ��������߽����ٶȡ�
	* <br>��������������ڵ�����CPUӲ��ƽ̨�ϣ�������ö����CPU���룬�����ʹ�����ٶ������½���
	* <p>����� {@link #Layer3(Header, IAudio)} ����Layer3����ȱʡ�����������cpu������Ż���
	* @param h �Ѿ�����֡ͷ��Ϣ��֡ͷ����
	* @param audio ��Ƶ�������
	* @param coresEnabled ��Ϊ<b>true</b>���ö����CPU�����Ż�����Ϊ<b>false</b>���ö����CPU�����Ż���
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
	// x_slen: ��������(scalefactor)������
	//�ƶ���Layer3StaticData
	
	void getScaleFactorsLSF(int gr, int ch);
	//<<<<SCALE FACTORS========================================================

	//3.
	//>>>>HUFFMAN BITS=========================================================
	int region[4];	// ĳһ������ڽ��������ݵ�����(���3������)
	int hv[32 * 18 + 4];		// [32 * 18 + 4], �ݴ��õĹ�����ֵ

	void huffBits(int gr, int ch);
	//<<<<HUFFMAN BITS=========================================================

	//4.
	//>>>>REQUANTIZATION & REORDER=============================================
	int widthLong[22];	// [22] �������������Ƶ��(��һ����������������Ƶ���ߵ�����)
	int widthShort[13];	// [13] �̿����������Ƶ��
	int rzeroBandLong;
	int rzeroBandShort[3];

	// ISO/IEC 11172-3 ANNEX B,Table 3-B.6. Layer III Preemphasis
	static const int pretab[22];

	/*
	* ���������Զ̿�(���̿�ͻ�Ͽ��еĶ̿�)������.��������ʱ��ֵ�ı���:<br>
	* rzero_bandL -- ������������ֵ��Ƶ����,����ǿ��������(intensity stereo)����<br>
	* rzero_bandS -- �̿���������ֵ��Ƶ����,����ǿ������������<br>
	* rzero_index -- ���������ֵ��"�Ӵ�"��
	* <p>
	* Layer3 ��������ʽISO/IEC 11172-3, 2.4.3.4
	* <p>
	* XRi = pow(2, 0.25 * (global_gain - 210)) <br>
	* if (LONG block types) <br>
	*����XRi *= pow(2, -0.5 * (1 + scalefac_scale) * (L[sfb] + preflag * pretab[sfb])) <br>
	* if (SHORT block types) { <br>
	*����XRi *= pow(2, 0.25 * -8 * subblock_gain[sfb]) <br>
	*����XRi *= pow(2, 0.25 * -2 * (1 + scalefac_scale) * S[scf]) } <br>
	* XRi *= sign(haffVal) * pow(abs(haffVal), 4/3) <br>
	*
	*/
	void requantizer(int ch, BKMP3_ChannelInformation *ci);
	//<<<<REQUANTIZATION & REORDER=============================================


	//5.
	//>>>>STEREO===============================================================

	// ��requantizer�������Ѿ����˳��Ը�2����,ms_stereo�ڲ��ٳ��Ը�2.
	void ms_stereo(int gr);

	static const float is_coef_lsf[2][15];
	static const float is_coef[7];

	// ����һ��Ƶ��ǿ��������,MPEG-1
	void is_lines(int pos, int idx0, int width, int step, int gr);

	// ����һ��Ƶ��ǿ��������,MPEG-2
	void is_lines_lsf(int tab2, int pos, int idx0, int width, int step, int gr);

	/*
	* ǿ��������(intensity stereo)����
	*
	* ISO/IEC 11172-3���Ի�Ͽ��еĳ�����ǿ������������,���ܶ�MP3����������˴���.
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

	float *preBlckCh0; // [32*18],������FIFO����
	float *preBlckCh1; // [32*18],������FIFO

	void hybrid(int ch, BKMP3_ChannelInformation *ci);
	//<<<<HYBRID(synthesize via iMDCT)=========================================

	//8.
	//>>>>INVERSE QUANTIZE SAMPLES=============================================
	//
	// �� decodeAudioDataChannel ������ʵ�ֶ���Ƶ�ʵ���
	//
	//<<<<INVERSE QUANTIZE SAMPLES=============================================

	//9.
	//>>>>SYNTHESIZE VIA POLYPHASE MDCT========================================
	//
	// jmp123.decoder.Synthesis.synthesisSubBand ����
	//
	//<<<<SYNTHESIZE VIA POLYPHASE MDCT========================================

	//10.
	//>>>>OUTPUT PCM SAMPLES===================================================
	//
	// jmp123.output.Audio ��
	//
	//<<<<OUTPUT PCM SAMPLES===================================================
public:
	/*
	* ����1֡Layer ���֡ͷ����������ݡ�
	*/
	virtual int decodeOneFrame(char *b, int off);

	/*
	* ����1֡��
	*/

	virtual int ignoreOneFrame(char *b, int off, bool fillMainData);

private:
	//private int owt; //----debug

	/*
	* samplesChx: �ݴ���Ӵ��ϳ�(hybrid)�Ľ���г�ȡ������32������ֵ,
	* ��32������ֵ��Ϊ�������ݱ��������ϳ��˲���.
	*/
	float *samplesCh0, *samplesCh1;
	// һ��������������Ӵ��ϳɡ�����Ƶ�ʵ��úͶ���ϳ��˲�
	void decodeAudioDataChannel(BKMP3_ChannelInformation *ci, int ch);

public:
	void close(bool interrupted);
};

