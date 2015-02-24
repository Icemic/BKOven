#include "AbstractLayer.h"

BKMP3_AbstractLayer::BKMP3_AbstractLayer(BKMP3_Header *h) {
	size = h->getPcmSize();

	channels = h->getChannels();
	filter = new BKMP3_Synthesis(channels);
	pcmbuf = new char[size];
	resetOffset();
}


BKMP3_AbstractLayer::~BKMP3_AbstractLayer()
{
	delete this->filter;
	delete[] this->pcmbuf;
}

void BKMP3_AbstractLayer::synthesisSubBand(float *samples, int ch) {
	writeCursor[ch] = filter->synthesisSubBand(samples, ch, pcmbuf, writeCursor[ch]);
}

void BKMP3_AbstractLayer::outputPCM(char **ptr, int *size, int &ignorePCMSize) {
	if (ptr && size)
	{
		*ptr = pcmbuf + ignorePCMSize;
		*size = this->size - ignorePCMSize;
		ignorePCMSize = 0;
	}
	resetOffset();
}
