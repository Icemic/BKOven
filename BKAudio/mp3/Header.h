#pragma once

/*
* 帧同步及帧头信息解码。源代码中解析帧头32位用到的变量及含义：
*
* 偏移量	长度	变量名				含义
* 0			11		帧同步时直接解析	11位全设置为'1'的帧同步字
* 11		2		verID				MPEG版本
* 13		2		layer				MPEG压缩层
* 15		1		protection_bit		是否CRC
* 16		4		bitrate_index		位率索引
* 20		2		sampling_frequency	采样率索引
* 22		1		padding				当前帧是否附加填充一槽数据
* 23		1		未解析				告知是否私有
* 24		2		mode				声道模式
* 26		2		mode_extension		声道扩展模式
* 28		1		未解析				告知是否有版权
* 29		1		未解析				告知是否为原版
* 30		2		不常用，未解析		预加重
*/

class BKMP3_Decoder;
class BKMP3_Header
{
public:
	
	enum {
		/*
		* MPEG版本MPEG-1。
		*/
		MPEG1 = 3,
		/*
		* MPEG版本MPEG-2。
		*/
		MPEG2 = 2,
		/*
		* MPEG版本MPEG-2.5（非官方版本）。
		*/
		MPEG25 = 0,
	};
	
	//public  final int MAX_FRAMESIZE = 1732;

private:

	/*
	* 用lsf,layer,bitrate_index索引访问: bitrate[lsf][layer-1][bitrate_index]
	*/
	static const int bitrate[2][3][15];

	/*
	* samplingRate[verID][sampling_frequency]
	*/
	static const int samplingRate[4][4];

	/*
	* verID: 2-bit
	* '00'  MPEG-2.5 (unofficial extension of MPEG-2);
	* '01'  reserved;
	* '10'  MPEG-2 (ISO/IEC 13818-3);
	* '11'  MPEG-1 (ISO/IEC 11172-3).
	*/
	int verID;

	/*
	* layer: 2-bit
	* '11'	 Layer I
	* '10'	 Layer II
	* '01'	 Layer III
	* '00'	 reserved
	*
	* 已换算layer=4-layer: 1--Layer I; 2--Layer II; 3--Layer III; 4--reserved
	*/
	int layer;

	/*
	* protection_bit: 1-bit
	* '1'  no CRC;
	* '0'  protected by 16-bit CRC following header.
	*/
	int protection_bit;

	/*
	* bitrate_index: 4-bit
	*/
	int bitrate_index;

	/*
	* sampling_frequency: 2-bit
	* '00'	 44.1kHz
	* '01'	 48kHz
	* '10'	 32kHz
	* '11'  reserved
	*/
	int sampling_frequency;

	int padding;

	/*
	* mode: 2-bit
	* '00'  Stereo;
	* '01'  Joint Stereo (Stereo);
	* '10'  Dual channel (Two mono channels);
	* '11'  Single channel (Mono).
	*/
	int mode;

	/*
	* mode_extension: 2-bit
	* 		 intensity_stereo	MS_stereo
	* '00'	 off				off
	* '01'	 on					off
	* '10'	 off				on
	* '11'	 on					on
	*/
	int mode_extension;

	int framesize;
	int maindatasize;
	int sideinfosize;
	int lsf;
	bool m_isMS, isIntensity;

protected:
	/*
	* 初始化。
	*/
	void initialize();

	/*
	* 帧头解码。
	* @param h 帧头，大头在上的4字节(32位)整数。
	*/
	void decode(int h);

	friend class BKMP3_Decoder;

public:
	/*
	* 是否有循环冗余校验码。
	* @return 返回true表示有循环冗余校验码，帧头之后邻接有2字节的数据用于CRC。
	*/
	bool isProtected();

	/*
	* 获取声道模式是否为中/侧立体声（Mid/Side stereo）模式。
	*
	* @return true表示是中/侧立体声模式。
	*/
	bool isMS();

	/*
	* 获取声道模式是否为强度立体声（Intensity Stereo）模式。
	*
	* @return true表示是强度立体声模式。
	*/
	bool isIntensityStereo();

	/*
	* 获取当前帧的位率。
	*
	* @return 当前帧的位率，单位为“千位每秒（Kbps）”。
	*/
	int getBitrate();

	/*
	* 获取当前帧的位率的索引值。
	*
	* @return 当前帧的位率的索引值，位率的索引值范围是1至14的某一整数。
	*/
	int getBitrateIndex();

	/*
	* 获取声道数。
	*
	* @return 声道数：1或2。
	*/
	int getChannels();

	/*
	* 获取声道模式。
	*
	* @return 声道模式，其值表示的含义：
	* 返回值	声道模式
	* 0			立体声（stereo)
	* 1			联合立体声（joint stereo）
	* 2			双声道（dual channel）
	* 3			单声道（mono channel）
	*/
	int getMode();

	/*
	* 获取声道扩展模式。
	*
	* @return 声道扩展模式，该值表示当前声道使用的立体声编码方式：
	* 返回值	强度立体声	中/侧立体声
	* 0			off			off
	* 1			on			off
	* 2			off			on
	* 3			on			on
	* @see #getMode()
	*/
	int getModeExtension();

	/*
	* 获取MPEG版本。
	*
	* @return MPEG版本：MPEG1、 MPEG2 或 MPEG25 。
	*/
	int getVersion();

	/*
	* 获取MPEG编码层。
	*
	* @return MPEG编码层：返回值1表示LayerⅠ，2表示LayerⅡ，3表示LayerⅢ。
	*/
	int getLayer();

	/*
	* 获取PCM样本采样率的索引值。
	*
	* @return PCM样本采样率的索引值。
	*/
	int getSamplingFrequency();

	/*
	* 获取PCM样本采样率。
	*
	* @return 获取PCM样本采样率，单位“赫兹（Hz）”
	*/
	int getSamplingRate();

	/*
	* 获取主数据长度。
	*
	* @return 当前帧的主数据长度，单位“字节”。
	*/
	int getMainDataSize();

	/*
	* 获取边信息长度。
	*
	* @return 当前帧边信息长度，单位“字节”。
	*/
	int getSideInfoSize();

	/*
	* 获取帧长度。帧的长度 = 4字节帧头 + CRC（如果有的话，2字节） + 音乐数据长度。
	* 其中音乐数据长度 = 边信息长度 + 主数据长度。
	* 无论是可变位率（VBR）编码的文件还是固定位率（CBR）编码的文件，每帧的长度不一定同。
	*
	* @return 当前帧的长度，单位“字节”。
	*/
	int getFrameSize();

	/*
	* 获取当前帧解码后得到的PCM样本长度。通常情况下同一文件每一帧解码后得到的PCM样本长度是相同的。
	*
	* @return 当前帧解码后得到的PCM样本长度，单位“字节”。
	*/
	int getPcmSize();

	/*
	* 获取当前文件一帧的播放时间长度。
	*
	* @return 当前文件一帧的播放时间长度，单位“秒”。
	*/
	float getFrameDuration();
};