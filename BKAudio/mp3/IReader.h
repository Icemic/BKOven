#pragma once

class IReader
{
public:
	virtual int read(char b[], int len) = 0;
	virtual int seek(int pos, int wherece) = 0;
	virtual ~IReader(){}
};