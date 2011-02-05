#include "Layer.h"

class AHFR : public TRNLib::LAYER
{
public:
	AHFR(unsigned char* data, unsigned int dataSize);

	int   fractal_id;
	int   unk2;
	float unk3;
};