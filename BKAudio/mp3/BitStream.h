#pragma once

/*
* 从输入的字节流获得位流。
* <p>对于Layer3文件解码， BitStream 仅用于读取帧边信息，要读取Layer3文件的主数据，必须使用{@link BitStreamMainData}。
* <p>对于Layer1和Layer2文件解码，用 BitStream 获得的位流，用于解码全部数据。
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
	* 创建一个位流BitStream对象，位流的缓冲区大小len指定，位流的缓冲区尾部空出的长度由extra指定。
	*
	* @param len 缓冲区可访问长度。<br>
	*            缓冲区用于解码帧边信息时len为9、17或32。<br>
	*            缓冲区用于解码主数据(main_data)时长度不小于最大帧长512+1732。
	*
	* @param extra 缓冲区尾部空出的字节数，防止哈夫曼解码时位流有错误导致缓冲区溢出，
	* 尾部空出512字节(part2_3_length长度，2^12位)。
	*/
	BKMP3_BitStream(int len, int extra);

	virtual ~BKMP3_BitStream();

	/*
	* 向缓冲区添加len字节。
	*
	* @param b 源数据。
	* @param off 源数据偏移量。
	* @param len 源数据长度。
	* @return 实际填充到缓冲区的字节数。
	*/
	int append(char *b, int off, int len);

	/*
	* 将缓冲指定为b，缓冲区初始偏移量由off指定。与 {@link #append(byte[], int, int)}
	* 方法的区别是，本方法不会从源数据b复制数据。
	*
	* @param b 源数据。
	* @param off 源数据偏移量。
	*/
	void feed(char *b, int off);

	/*
	* 从缓冲区读取一位。
	* @return 0或1。
	*/
	int get1Bit();

	/*
	* 从缓冲区读取n位。由于运行时速度方面的原因，若读取的位数不大于9，请考虑用{@link #getBits9(int)}方法更高效。
	* @param n 比特数，n=2..17时调用此方法。
	* @return n位的值。
	*/
	int getBits17(int n);

	/*
	* 从缓冲区读取n位。
	* @param n 比特数，n=2..9时调用此方法。
	* @return n位的值。
	*/
	int getBits9(int n);

	/*
	* 获取缓冲区字节指针。
	* @return 缓冲区字节指针。
	*/
	int getBytePos();

	/*
	* 获取缓冲区已经填入的字节数。
	* @return 缓冲区已经填入的字节数。
	*/
	int getSize();

	/*
	* 缓冲区丢弃n字节，缓冲区比特指针复位。
	* @param n 丢弃的字节数。
	*/
	void skipBytes(int n);

	/*
	* 缓冲区丢弃或回退指定比特。
	* @param n 若n>0丢弃n比特，若n<0则回退-n比特。
	*/
	void skipBits(int n);

	/*
	* 缓冲区清空，缓冲区比特指针复位。
	*/
	void clear();


private:
	int endPos;		//bitReservoir已填入的字节数
	int maxOff;
	int markPos;
};