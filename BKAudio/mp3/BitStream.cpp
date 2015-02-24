#include "BitStream.h"
#include <cstring>

BKMP3_BitStream::BKMP3_BitStream(int len, int extra)
{
	maxOff = len;
	bitReservoir = new char[len + extra];
	memset(bitReservoir, 0, len + extra);
	bitOwner = 1;
	endPos = 0;
	bitPos = bytePos = 0;
}

BKMP3_BitStream::~BKMP3_BitStream()
{
	if (bitOwner)
	{
		delete[] bitReservoir;
	}
}


int BKMP3_BitStream::append(char *b, int off, int len)
{
	if (len + endPos > maxOff)
	{
		// 将缓冲区bytePos及之后的(未处理过的)数据移动到缓冲区首
		//System.arraycopy(bitReservoir, bytePos, bitReservoir, 0, endPos - bytePos);
		memcpy(bitReservoir + 0, bitReservoir + bytePos, endPos - bytePos);

		endPos -= bytePos;
		bitPos = bytePos = 0;
	}
	if (len + endPos > maxOff)
		len = maxOff - endPos;
	//System.arraycopy(b, off, bitReservoir, endPos, len);
	memcpy(bitReservoir + endPos, b + off, len);

	endPos += len;
	return len;
}

void BKMP3_BitStream::feed(char b[], int off)
{
	if (bitOwner)
	{
		delete[] bitReservoir;
	}
	bitOwner = 0;
	bitReservoir = b;
	bytePos = off;
	bitPos = 0;
}

int BKMP3_BitStream::get1Bit()
{
	int bit = bitReservoir[bytePos] << bitPos;
	bit >>= 7;
	bit &= 0x1;
	bitPos++;
	bytePos += bitPos >> 3;
	bitPos &= 0x7;
	return bit;
}

int BKMP3_BitStream::getBits17(int n)
{
	int iret = bitReservoir[bytePos];
	iret <<= 8;
	iret |= bitReservoir[bytePos + 1] & 0xff;
	iret <<= 8;
	iret |= bitReservoir[bytePos + 2] & 0xff;
	iret <<= bitPos;
	iret &= 0xffffff; // 高8位置0
	iret >>= 24 - n;
	bitPos += n;
	bytePos += bitPos >> 3;
	bitPos &= 0x7;
	return iret;
}

int BKMP3_BitStream::getBits9(int n)
{
	int iret = bitReservoir[bytePos];
	iret <<= 8;
	iret |= bitReservoir[bytePos + 1] & 0xff;
	iret <<= bitPos;
	iret &= 0xffff; // 高16位置0
	iret >>= 16 - n;
	bitPos += n;
	bytePos += bitPos >> 3;
	bitPos &= 0x7;
	return iret;
}

int BKMP3_BitStream::getBytePos()
{
	return bytePos;
}


int BKMP3_BitStream::getSize()
{
	return endPos;
}

void BKMP3_BitStream::skipBytes(int n)
{
	bytePos += n;
	bitPos = 0;
}

void BKMP3_BitStream::skipBits(int n)
{
	bitPos += n;
	bytePos += bitPos >> 3;
	bitPos &= 0x7;
}

void BKMP3_BitStream::mark()
{
	markPos = bytePos;
}

int BKMP3_BitStream::getMark()
{
	return bytePos - markPos;
}

void BKMP3_BitStream::clear()
{
	bitPos = bytePos = endPos = 0;
}