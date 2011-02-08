#include "Boundary.h"

class BCIR : public TRNLib::Boundary
{
public:
	BCIR(unsigned char* data, unsigned int dataSize);

	float x;
	float z;
	float rad;

	int feather_type;
	float feather_amount;

	bool isContained(float x, float z);
};