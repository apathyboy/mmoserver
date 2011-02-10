#include "Layer.h"

//Boundries
#include "BREC.h"
#include "BCIR.h"
#include "BPOL.h"
#include "BPLN.h"

//Filters
#include "FDIR.h"
#include "FFRA.h"
#include "FHGT.h"
#include "FSLP.h"

//Shader
#include "ASCN.h"
#include "ACCN.h"
#include "FSHD.h"

//Height
#include "AHCN.h"
#include "AHFR.h"
#include "AHTR.h"

//Flora
#include "AFSC.h"
#include "AFSN.h"
#include "AFDN.h"
#include "AFDF.h"

//Other
#include "AENV.h"
#include "AEXC.h"
#include "AROA.h"
#include "ARIV.h"
#include "ACRF.h"
#include "ACRH.h"
#include "ASRP.h"

// Math
#include <cmath>
#include <limits>

using namespace TRNLib;
using namespace IFFLib;

LAYER* LAYER::LOAD(IFF::NODE* node)
{
	LAYER* layer = NULL;

	//See if the name is prefixed funkily or something
	char* nameStart = node->name;

	//Do it
	if(nameStart[0] == 'A')
	{
		if(nameStart[1] == 'C')
		{
			if(nameStart[2] == 'C')
			{
				if(nameStart[3] == 'N')
				{
					layer = new ACCN(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'R')
			{
				if(nameStart[3] == 'F')
				{
					layer = new ACRF(node->children[1]->children[0]->data, node->children[1]->children[0]->size);
				}
				else if(nameStart[3] == 'H')
				{
					layer = new ACRH(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'E')
		{
			if(nameStart[2] == 'N')
			{
				if(nameStart[3] == 'V')
				{
					layer = new AENV(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'X')
			{
				if(nameStart[3] == 'C')
				{
					layer = new AEXC();
				}
			}
		}
		else if(nameStart[1] == 'F')
		{
			if(nameStart[2] == 'D')
			{
				if(nameStart[3] == 'F')
				{
					layer = new AFDF(node->children[1]->data, node->children[1]->size);
				}
				else if(nameStart[3] == 'N')
				{
					layer = new AFDN(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'S')
			{
				if(nameStart[3] == 'C')
				{
					layer = new AFSC(node->children[1]->data, node->children[1]->size);
				}
				else if(nameStart[3] == 'N')
				{
					layer = new AFSN(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'H')
		{
			if(nameStart[2] == 'C')
			{
				if(nameStart[3] == 'N')
				{
					layer = new AHCN(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'F')
			{
				if(nameStart[3] == 'R')
				{
					layer = new AHFR(node->children[1]->children[0]->data, node->children[1]->children[0]->size);
				}
			}
			else if(nameStart[2] == 'T')
			{
				if(nameStart[3] == 'R')
				{
					layer = new AHTR(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'R')
		{
			if(nameStart[2] == 'I')
			{
				if(nameStart[3] == 'V')
				{
					layer = new ARIV(	node->children[1]->children[0]->children[0]->children[0]->data, 
										node->children[1]->children[0]->children[0]->children[0]->size, 
										node->children[1]->children[1]->data, 
										node->children[1]->children[1]->size);
				}
			}
			else if(nameStart[2] == 'O')
			{
				if(nameStart[3] == 'A')
				{
					layer = new AROA(	node->children[1]->children[0]->children[0]->children[0]->data, 
										node->children[1]->children[0]->children[0]->children[0]->size, 
										node->children[1]->children[1]->data, 
										node->children[1]->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'S')
		{
			if(nameStart[2] == 'C')
			{
				if(nameStart[3] == 'N')
				{
					layer = new ASCN(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'R')
			{
				if(nameStart[3] == 'P')
				{
					layer = new ASRP(node->children[1]->data, node->children[1]->size);
				}
			}
		}
	}
	else if(nameStart[0] == 'B')
	{
		if(nameStart[1] == 'C')
		{
			if(nameStart[2] == 'I')
			{
				if(nameStart[3] == 'R')
				{
					layer = new BCIR(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'P')
		{
			if(nameStart[2] == 'L')
			{
				if(nameStart[3] == 'N')
				{
					layer = new BPLN(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'O')
			{
				if(nameStart[3] == 'L')
				{
					layer = new BPOL(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'R')
		{
			if(nameStart[2] == 'E')
			{
				if(nameStart[3] == 'C')
				{
					layer = new BREC(node->children[1]->data, node->children[1]->size);
				}
			}
		}
	}
	else if(nameStart[0] == 'F')
	{
		if(nameStart[1] == 'D')
		{
			if(nameStart[2] == 'I')
			{
				if(nameStart[3] == 'R')
				{
					layer = new FDIR(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'F')
		{
			if(nameStart[2] == 'R')
			{
				if(nameStart[3] == 'A')
				{
					layer = new FFRA(node->children[1]->children[0]->data, node->children[1]->children[0]->size);
				}
			}
		}
		else if(nameStart[1] == 'H')
		{
			if(nameStart[2] == 'G')
			{
				if(nameStart[3] == 'T')
				{
					layer = new FHGT(node->children[1]->data, node->children[1]->size);
				}
			}
		}
		else if(nameStart[1] == 'S')
		{
			if(nameStart[2] == 'H')
			{
				if(nameStart[3] == 'D')
				{
					layer = new FSHD(node->children[1]->data, node->children[1]->size);
				}
			}
			else if(nameStart[2] == 'L')
			{
				if(nameStart[3] == 'P')
				{
					layer = new FSLP(node->children[1]->data, node->children[1]->size);
				}
			}
		}
	}

	if(layer)
	{
		//Find it's data.
		unsigned char* data = node->children[0]->children[0]->children[0]->data;

		layer->enabled    = (data[0] == 1) ? true : false;
		
		unsigned int strLen = strlen(const_cast<const char*>((char*)&data[4])) + 1;
		layer->customName = new unsigned char[strLen];
		memcpy(layer->customName, &data[4], strLen);
	}

	return layer;
}

ACCN::ACCN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ACCN;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

ASCN::ASCN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ASCN;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

FSHD::FSHD(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ACCN;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

ACRF::ACRF(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ACRF;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

ACRH::ACRH(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ACRH;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

AENV::AENV(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AENV;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

AEXC::AEXC()
{
	type = LAYER_AEXC;
}

AROA::AROA(unsigned char* data, unsigned int dataSize, unsigned char* data2, unsigned int dataSize2)
{
	type = LAYER_AROA;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;

	this->data2 = new unsigned char[dataSize2];
	memcpy(this->data2, data2, dataSize2);
	this->size2 = dataSize2;
}

AHCN::AHCN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AHCN;

	memcpy(&transform_type, &data[0], 4);
	memcpy(&height_val, &data[4], 4);

	//printf("C: %d %f\n", unk1, unk2);
}

void AHCN::getBaseHeight(float x, float z, float transform_value, float& base_value, TerrainManager* tm)
{
	base_value = height_val * transform_value;
}

AHFR::AHFR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AHFR;

	memcpy(&fractal_id, &data[0], 4);
	memcpy(&transform_type, &data[4], 4);
	memcpy(&height_val, &data[8], 4);

	//printf("F: %d %d %f\n", unk1, unk2, unk3);
}

void AHFR::getBaseHeight(float x, float z, float transform_value, float& base_value, TerrainManager* tm)
{
	TRNLib::MFAM* fractal = tm->getFractal(fractal_id);
	
	float noise_result = fractal->getNoise(x, z) * height_val;

	float result;

	switch (transform_type)
	{
		case 1:
		result = base_value + noise_result * transform_value;
		break;
	case 2:
		result = base_value - noise_result * transform_value;
		break;
	case 3:
		result = base_value + (noise_result * base_value - base_value) * transform_value;
		break;
	case 4:
		result = base_value;
		break;
	default:
		result = base_value + (noise_result - base_value) * transform_value;
		break;
	}

	base_value = result;
}

FFRA::FFRA(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_FFRA;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

FHGT::FHGT(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_FHGT;

	unsigned int i=0;

	memcpy(&minHeight, &data[i], 4); i+=4;

	memcpy(&maxHeight, &data[i], 4); i+=4;

	memcpy(&feather_type, &data[i], 4); i+=4;
	
	memcpy(&feather_amount, &data[i], 4); i+=4;
}

FSLP::FSLP(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_FSLP;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

BREC::BREC(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_BREC;

	memcpy(&x1, &data[0], 4);
	memcpy(&z1, &data[4], 4);
	
	memcpy(&x2, &data[8], 4);
	memcpy(&z2, &data[12], 4);

	memcpy(&feather_type, &data[16], 4);
	memcpy(&feather_amount, &data[20], 4);

	//printf("==BREC==\nPOS1: (%f,%f)\nPOS2: (%f,%f)\nFeather Type: (%d)\nFeather Amount: (%f)\n", x1, y1, x2, y2, feather_type, feather_amount);

}

bool BREC::isContained(float x, float z)
{
	float max_x, max_z, min_x, min_z;

	if (x1 < x2)
	{
		max_x = x2;
		min_x = x1;
	}
	else
	{
		max_x = x1;
		min_x = x2;
	}

	if (z1 < z2)
	{
		max_z = z2;
		min_z = z1;
	}
	else
	{
		max_z = z1;
		min_z = z2;
	}

	if (max_x >= x && min_x <= x && max_z >= z && min_z <= z)
		return true;

	return false;
}

BPOL::BPOL(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_BPOL;

	unsigned int sizeTemp;
	unsigned int i=0;
	memcpy(&sizeTemp, &data[0], 4); i+=4;

	for(unsigned int j = 0; j < sizeTemp; j++)
	{
		float tempX;
		float tempZ;
		memcpy(&tempX, &data[i], 4); i+=4;
		memcpy(&tempZ, &data[i], 4); i+=4;

		verts.push_back(new VERTEX(tempX, tempZ));
	}

	memcpy(&feather_type, &data[i], 4); i+=4;
	memcpy(&shore_smoothness, &data[i], 4); i+=4;
	memcpy(&use_water_height, &data[i], 4); i+=4;
	memcpy(&water_height, &data[i], 4); i+=4;
	memcpy(&water_shader_size, &data[i], 4); i+=4;

	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	water_shader = new unsigned char[strLen];
	memcpy(water_shader, &data[i], strLen); i+= strLen;
}

bool BPOL::isContained(float x, float z)
{
	int j;
	bool odd_nodes = false;
	float x1, x2;

	for ( unsigned int i = 0; i < verts.size(); ++i )
	{
		j = (i+1) % verts.size();

		if ( verts.at(i)->x < verts.at(j)->x)
		{
			x1 = verts.at(i)->x;
			x2 = verts.at(j)->x;
		} 
		else 
		{
			x1 = verts.at(j)->x;
			x2 = verts.at(i)->x;
		}

			/* First check if the ray is possible to cross the line */
			if ( x > x1 && x <= x2 && ( z < verts.at(i)->z || z <= verts.at(j)->z ) ) {
				static const float eps = 0.000001f;

				/* Calculate the equation of the line */
				float dx = verts.at(j)->x - verts.at(i)->x;
				float dz = verts.at(j)->z - verts.at(i)->z;
				float k;

				if ( fabs(dx) < eps ){
					k = std::numeric_limits<float>::infinity();
				} else {
					k = dz/dx;
				}

				float m = verts.at(i)->z - k * verts.at(i)->x;

				/* Find if the ray crosses the line */
				float z2 = k * x + m;
				if ( z <= z2 )
				{
					odd_nodes=!odd_nodes;
				}
			}
		}


  return odd_nodes;
}

BPLN::BPLN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_BPLN;
	unsigned int sizeTemp;
	unsigned int i=0;
	memcpy(&sizeTemp, &data[0], 4); i+=4;

	for(unsigned int j = 0; j < sizeTemp; j++)
	{
		float tempX;
		float tempY;
		memcpy(&tempX, &data[i], 4); i+=4;
		memcpy(&tempY, &data[i], 4); i+=4;

		verts.push_back(new VERTEX(tempX, tempY));
	}

	memcpy(&feather_type, &data[i], 4); i+=4;

	memcpy(&feather_amount, &data[i], 4); i+=4;
	memcpy(&line_width, &data[i], 4); i+=4;
}

bool BPLN::isContained(float x, float z)
{
	return false;
}

BCIR::BCIR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_BCIR;
	memcpy(&x, &data[0], 4);
	memcpy(&z, &data[4], 4);
	
	memcpy(&rad, &data[8], 4);

	memcpy(&feather_type, &data[12], 4);
	memcpy(&feather_amount, &data[16], 4);
	//printf("==BCIR==\nPOS: (%f,%f)\nRAD: %f\nFeather Type: (%d)\nFeather Amount: (%f)\n", x, y, rad, feather_type, feather_amount);
}

bool BCIR::isContained(float px, float pz)
{
	float distx = pow(px-x,2);
	float distz = pow(pz-z,2);
	float r2 = pow(rad,2);

	if ( distx + distz < r2)
		return true;

	return false;
}

AFSC::AFSC(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AFSC;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

AFSN::AFSN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AFSN;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

AFDF::AFDF(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AFDF;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

AFDN::AFDN(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AFDN;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

ARIV::ARIV(unsigned char* data, unsigned int dataSize, unsigned char* data2, unsigned int dataSize2)
{
	type = LAYER_ARIV;

	//These are generally unused and can be ignored.
	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;

	this->data2 = new unsigned char[dataSize2];
	memcpy(this->data2, data2, dataSize2);
	this->size2 = dataSize2;
}

AHTR::AHTR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AHTR;
	
	memcpy(&flat_ratio, &data[0], 4);
	memcpy(&height_delta, &data[4], 4);
}

ASRP::ASRP(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_ASRP;
	
	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}

FDIR::FDIR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_FDIR;

	this->data = new unsigned char[dataSize];
	memcpy(this->data, data, dataSize);
	this->size = dataSize;
}