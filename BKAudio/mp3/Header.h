#pragma once

/*
* ֡ͬ����֡ͷ��Ϣ���롣Դ�����н���֡ͷ32λ�õ��ı��������壺
*
* ƫ����	����	������				����
* 0			11		֡ͬ��ʱֱ�ӽ���	11λȫ����Ϊ'1'��֡ͬ����
* 11		2		verID				MPEG�汾
* 13		2		layer				MPEGѹ����
* 15		1		protection_bit		�Ƿ�CRC
* 16		4		bitrate_index		λ������
* 20		2		sampling_frequency	����������
* 22		1		padding				��ǰ֡�Ƿ񸽼����һ������
* 23		1		δ����				��֪�Ƿ�˽��
* 24		2		mode				����ģʽ
* 26		2		mode_extension		������չģʽ
* 28		1		δ����				��֪�Ƿ��а�Ȩ
* 29		1		δ����				��֪�Ƿ�Ϊԭ��
* 30		2		�����ã�δ����		Ԥ����
*/

class BKMP3_Decoder;
class BKMP3_Header
{
public:
	
	enum {
		/*
		* MPEG�汾MPEG-1��
		*/
		MPEG1 = 3,
		/*
		* MPEG�汾MPEG-2��
		*/
		MPEG2 = 2,
		/*
		* MPEG�汾MPEG-2.5���ǹٷ��汾����
		*/
		MPEG25 = 0,
	};
	
	//public  final int MAX_FRAMESIZE = 1732;

private:

	/*
	* ��lsf,layer,bitrate_index��������: bitrate[lsf][layer-1][bitrate_index]
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
	* �ѻ���layer=4-layer: 1--Layer I; 2--Layer II; 3--Layer III; 4--reserved
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
	* ��ʼ����
	*/
	void initialize();

	/*
	* ֡ͷ���롣
	* @param h ֡ͷ����ͷ���ϵ�4�ֽ�(32λ)������
	*/
	void decode(int h);

	friend class BKMP3_Decoder;

public:
	/*
	* �Ƿ���ѭ������У���롣
	* @return ����true��ʾ��ѭ������У���룬֡ͷ֮���ڽ���2�ֽڵ���������CRC��
	*/
	bool isProtected();

	/*
	* ��ȡ����ģʽ�Ƿ�Ϊ��/����������Mid/Side stereo��ģʽ��
	*
	* @return true��ʾ����/��������ģʽ��
	*/
	bool isMS();

	/*
	* ��ȡ����ģʽ�Ƿ�Ϊǿ����������Intensity Stereo��ģʽ��
	*
	* @return true��ʾ��ǿ��������ģʽ��
	*/
	bool isIntensityStereo();

	/*
	* ��ȡ��ǰ֡��λ�ʡ�
	*
	* @return ��ǰ֡��λ�ʣ���λΪ��ǧλÿ�루Kbps������
	*/
	int getBitrate();

	/*
	* ��ȡ��ǰ֡��λ�ʵ�����ֵ��
	*
	* @return ��ǰ֡��λ�ʵ�����ֵ��λ�ʵ�����ֵ��Χ��1��14��ĳһ������
	*/
	int getBitrateIndex();

	/*
	* ��ȡ��������
	*
	* @return ��������1��2��
	*/
	int getChannels();

	/*
	* ��ȡ����ģʽ��
	*
	* @return ����ģʽ����ֵ��ʾ�ĺ��壺
	* ����ֵ	����ģʽ
	* 0			��������stereo)
	* 1			������������joint stereo��
	* 2			˫������dual channel��
	* 3			��������mono channel��
	*/
	int getMode();

	/*
	* ��ȡ������չģʽ��
	*
	* @return ������չģʽ����ֵ��ʾ��ǰ����ʹ�õ����������뷽ʽ��
	* ����ֵ	ǿ��������	��/��������
	* 0			off			off
	* 1			on			off
	* 2			off			on
	* 3			on			on
	* @see #getMode()
	*/
	int getModeExtension();

	/*
	* ��ȡMPEG�汾��
	*
	* @return MPEG�汾��MPEG1�� MPEG2 �� MPEG25 ��
	*/
	int getVersion();

	/*
	* ��ȡMPEG����㡣
	*
	* @return MPEG����㣺����ֵ1��ʾLayer��2��ʾLayer��3��ʾLayer��
	*/
	int getLayer();

	/*
	* ��ȡPCM���������ʵ�����ֵ��
	*
	* @return PCM���������ʵ�����ֵ��
	*/
	int getSamplingFrequency();

	/*
	* ��ȡPCM���������ʡ�
	*
	* @return ��ȡPCM���������ʣ���λ�����ȣ�Hz����
	*/
	int getSamplingRate();

	/*
	* ��ȡ�����ݳ��ȡ�
	*
	* @return ��ǰ֡�������ݳ��ȣ���λ���ֽڡ���
	*/
	int getMainDataSize();

	/*
	* ��ȡ����Ϣ���ȡ�
	*
	* @return ��ǰ֡����Ϣ���ȣ���λ���ֽڡ���
	*/
	int getSideInfoSize();

	/*
	* ��ȡ֡���ȡ�֡�ĳ��� = 4�ֽ�֡ͷ + CRC������еĻ���2�ֽڣ� + �������ݳ��ȡ�
	* �����������ݳ��� = ����Ϣ���� + �����ݳ��ȡ�
	* �����ǿɱ�λ�ʣ�VBR��������ļ����ǹ̶�λ�ʣ�CBR��������ļ���ÿ֡�ĳ��Ȳ�һ��ͬ��
	*
	* @return ��ǰ֡�ĳ��ȣ���λ���ֽڡ���
	*/
	int getFrameSize();

	/*
	* ��ȡ��ǰ֡�����õ���PCM�������ȡ�ͨ�������ͬһ�ļ�ÿһ֡�����õ���PCM������������ͬ�ġ�
	*
	* @return ��ǰ֡�����õ���PCM�������ȣ���λ���ֽڡ���
	*/
	int getPcmSize();

	/*
	* ��ȡ��ǰ�ļ�һ֡�Ĳ���ʱ�䳤�ȡ�
	*
	* @return ��ǰ�ļ�һ֡�Ĳ���ʱ�䳤�ȣ���λ���롱��
	*/
	float getFrameDuration();
};