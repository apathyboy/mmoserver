#include "Layer.h"

class BREC : public TRNLib::LAYER
{
public:
	BREC(unsigned char* data, unsigned int dataSize);

	float x1;
	float y1;

	float x2;
	float y2;

	int feather_type;
	float feather_amount;
};