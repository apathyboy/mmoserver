#include "Layer.h"

class FHGT : public TRNLib::LAYER
{
public:
	FHGT(unsigned char* data, unsigned int dataSize);

	float minHeight;
	float maxHeight;
	int feather_type;
	float feather_amount;
};