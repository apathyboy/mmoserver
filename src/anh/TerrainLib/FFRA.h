#include "Layer.h"

class FFRA : public TRNLib::LAYER
{
public:
	FFRA(unsigned char* data, unsigned int dataSize);

	unsigned char* data;
	unsigned int size;
};