#pragma once
#include "IReader.h"

#include <vector>

class BKMP3_AbstractLayer;
class BKMP3_Header;
struct BKMP3_IndexCache;
/*
* BKEMP3��������
* @version 0.01
*/
class BKMP3_Decoder
{
public:
	/*
	* ��ָ���Ķ���ӿ��ഴ��һ��MP3Decoder����
	* @param fd ��Ƶ����ӿ��ࡣ
	*/
	BKMP3_Decoder(IReader *);

	~BKMP3_Decoder();

	/*
	* ����һ֡��
	* @param outPtr ָ�����������ָ���ָ�롣��Ϊ <b>null</b>�������������
	* @param size   ���������������С��ָ�롣��Ϊ <b>null</b>�������������
	* @return �Ƿ�ɹ�����Ϊ0����������������������
	*/
	int decodeOneFrame(char **outPtr, int *size);

	/*
	* @param ms seek��������
	*/
	void seek(double s);

	/*
	* @return ��ȡ��������
	*/
	int getChannelCount() const;

	/*
	* @return ��ȡ�����ʡ�
	*/
	int getSamplingRate() const;

private:
	bool m_bEOF; //����Ҳ�����һ֡ͷ�����flagΪtrue��
	bool m_bIOEOF; //����ȡIO�ﵽĩβ����flagΪtrue��
	bool m_bSync;	// ��֡ͷ������δ�ı䣬��flagΪtrue��
	bool m_bFrameFound;
	int m_iHeaderMask;
	int m_iCurrentFrame;	//��ǰ֡���
	static const int sDefaultIgnoreCount = 4; //����֡��������seek��Ĭ��Ϊ4��fixme��ò����Ҫ��һ������
	int m_iIgnoreFrameCount; //����֡��������seek��
	int m_iIgnorePCMSizeCount; //��������ֽ���������seek�о�ȷ��λ
	int m_iFileOffset; //��ǰ�ļ�ƫ��
	//��ȡ��������ƫ��ָ��
	int pos, endPos;
	char buf[8192];
	//��ȡ���������º���
	void updateBuffer();
	//��ȡ���������ú���
	void resetBuffer();

	//֡ͷ������
	BKMP3_Header *header;
	//������
	BKMP3_AbstractLayer *layer;
	//����ӿ�
	IReader *fd;

	//֡���������ڿ���seek��
	BKMP3_IndexCache *indexCache;

	/*
	* Ѱ����һ֡ͷ������m_iCurrentFrame++��
	*/
	void nextFrameHeader();

	/*
	* ���Ը�֡�Ľ��롣
	*/
	void ignoreThisFrame();
};