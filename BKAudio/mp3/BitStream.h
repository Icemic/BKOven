#pragma once

/*
* ��������ֽ������λ����
* <p>����Layer3�ļ����룬 BitStream �����ڶ�ȡ֡����Ϣ��Ҫ��ȡLayer3�ļ��������ݣ�����ʹ��{@link BitStreamMainData}��
* <p>����Layer1��Layer2�ļ����룬�� BitStream ��õ�λ�������ڽ���ȫ�����ݡ�
* @version 0.400
*/
class BKMP3_BitStream
{
protected:
	int bitPos;
	int bytePos;
	char *bitReservoir;
	bool bitOwner;
	void mark();
	int getMark();

public:
	/*
	* ����һ��λ��BitStream����λ���Ļ�������Сlenָ����λ���Ļ�����β���ճ��ĳ�����extraָ����
	*
	* @param len �������ɷ��ʳ��ȡ�<br>
	*            ���������ڽ���֡����ϢʱlenΪ9��17��32��<br>
	*            ���������ڽ���������(main_data)ʱ���Ȳ�С�����֡��512+1732��
	*
	* @param extra ������β���ճ����ֽ�������ֹ����������ʱλ���д����»����������
	* β���ճ�512�ֽ�(part2_3_length���ȣ�2^12λ)��
	*/
	BKMP3_BitStream(int len, int extra);

	virtual ~BKMP3_BitStream();

	/*
	* �򻺳������len�ֽڡ�
	*
	* @param b Դ���ݡ�
	* @param off Դ����ƫ������
	* @param len Դ���ݳ��ȡ�
	* @return ʵ����䵽���������ֽ�����
	*/
	int append(char *b, int off, int len);

	/*
	* ������ָ��Ϊb����������ʼƫ������offָ������ {@link #append(byte[], int, int)}
	* �����������ǣ������������Դ����b�������ݡ�
	*
	* @param b Դ���ݡ�
	* @param off Դ����ƫ������
	*/
	void feed(char *b, int off);

	/*
	* �ӻ�������ȡһλ��
	* @return 0��1��
	*/
	int get1Bit();

	/*
	* �ӻ�������ȡnλ����������ʱ�ٶȷ����ԭ������ȡ��λ��������9���뿼����{@link #getBits9(int)}��������Ч��
	* @param n ��������n=2..17ʱ���ô˷�����
	* @return nλ��ֵ��
	*/
	int getBits17(int n);

	/*
	* �ӻ�������ȡnλ��
	* @param n ��������n=2..9ʱ���ô˷�����
	* @return nλ��ֵ��
	*/
	int getBits9(int n);

	/*
	* ��ȡ�������ֽ�ָ�롣
	* @return �������ֽ�ָ�롣
	*/
	int getBytePos();

	/*
	* ��ȡ�������Ѿ�������ֽ�����
	* @return �������Ѿ�������ֽ�����
	*/
	int getSize();

	/*
	* ����������n�ֽڣ�����������ָ�븴λ��
	* @param n �������ֽ�����
	*/
	void skipBytes(int n);

	/*
	* ���������������ָ�����ء�
	* @param n ��n>0����n���أ���n<0�����-n���ء�
	*/
	void skipBits(int n);

	/*
	* ��������գ�����������ָ�븴λ��
	*/
	void clear();


private:
	int endPos;		//bitReservoir��������ֽ���
	int maxOff;
	int markPos;
};