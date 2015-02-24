#pragma once
#include "IReader.h"

#include <vector>

class BKMP3_AbstractLayer;
class BKMP3_Header;
struct BKMP3_IndexCache;
/*
* BKEMP3解码器。
* @version 0.01
*/
class BKMP3_Decoder
{
public:
	/*
	* 用指定的读入接口类创建一个MP3Decoder对象。
	* @param fd 音频读入接口类。
	*/
	BKMP3_Decoder(IReader *);

	~BKMP3_Decoder();

	/*
	* 解码一帧。
	* @param outPtr 指向输出缓冲区指针的指针。若为 <b>null</b>，则跳过输出。
	* @param size   储存输出缓冲区大小的指针。若为 <b>null</b>，则跳过输出。
	* @return 是否成功。若为0则解码继续，否则解码结束。
	*/
	int decodeOneFrame(char **outPtr, int *size);

	/*
	* @param ms seek到的秒数
	*/
	void seek(double s);

	/*
	* @return 获取声道数。
	*/
	int getChannelCount() const;

	/*
	* @return 获取采样率。
	*/
	int getSamplingRate() const;

private:
	bool m_bEOF; //如果找不到下一帧头，则此flag为true。
	bool m_bIOEOF; //若读取IO达到末尾，此flag为true。
	bool m_bSync;	// 若帧头的特征未改变，此flag为true。
	bool m_bFrameFound;
	int m_iHeaderMask;
	int m_iCurrentFrame;	//当前帧序号
	static const int sDefaultIgnoreCount = 4; //忽略帧数，用于seek，默认为4（fixme：貌似需要进一步计算
	int m_iIgnoreFrameCount; //忽略帧数，用于seek。
	int m_iIgnorePCMSizeCount; //忽略输出字节数，用于seek中精确定位
	int m_iFileOffset; //当前文件偏移
	//读取缓冲区及偏移指针
	int pos, endPos;
	char buf[8192];
	//读取缓冲区更新函数
	void updateBuffer();
	//读取缓冲区重置函数
	void resetBuffer();

	//帧头解释器
	BKMP3_Header *header;
	//解码器
	BKMP3_AbstractLayer *layer;
	//读入接口
	IReader *fd;

	//帧索引，用于快速seek用
	BKMP3_IndexCache *indexCache;

	/*
	* 寻找下一帧头。导致m_iCurrentFrame++。
	*/
	void nextFrameHeader();

	/*
	* 忽略该帧的解码。
	*/
	void ignoreThisFrame();
};