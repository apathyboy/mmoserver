#include "Layer.h"

class BCIR : public TRNLib::LAYER
{
public:
	BCIR(unsigned char* data, unsigned int dataSize);

	float x;
	float y;
	float rad;

	int feather_type;
	float feather_amount;
};