#include "ChannelInformation.h"

BKMP3_ChannelInformation::BKMP3_ChannelInformation()
{
	buf = new float[32 * 18];
}


BKMP3_ChannelInformation::~BKMP3_ChannelInformation()
{
	delete[] buf;
}
