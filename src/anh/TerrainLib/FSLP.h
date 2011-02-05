#include "Layer.h"

class FSLP : public TRNLib::LAYER
{
public:
	FSLP(unsigned char* data, unsigned int dataSize);

	unsigned char* data;
	unsigned int size;
};