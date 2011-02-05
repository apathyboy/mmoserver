#include "Layer.h"

class BPLN : public TRNLib::LAYER
{
public:
	BPLN(unsigned char* data, unsigned int dataSize);
	
	std::vector<TRNLib::VERTEX*> verts;

	int feather_type;
	float feather_amount;
	float line_width;
};