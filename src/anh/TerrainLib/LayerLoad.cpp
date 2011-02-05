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

	memcpy(&unk1, &data[0], 4);
	memcpy(&unk2, &data[4], 4);

	//printf("C: %d %f\n", unk1, unk2);
}

AHFR::AHFR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_AHFR;

	memcpy(&fractal_id, &data[0], 4);
	memcpy(&unk2, &data[4], 4);
	memcpy(&unk3, &data[8], 4);

	//printf("F: %d %d %f\n", unk1, unk2, unk3);
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
	memcpy(&y1, &data[4], 4);
	
	memcpy(&x2, &data[8], 4);
	memcpy(&y2, &data[12], 4);

	memcpy(&feather_type, &data[16], 4);
	memcpy(&feather_amount, &data[20], 4);

	//printf("==BREC==\nPOS1: (%f,%f)\nPOS2: (%f,%f)\nFeather Type: (%d)\nFeather Amount: (%f)\n", x1, y1, x2, y2, feather_type, feather_amount);

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
		float tempY;
		memcpy(&tempX, &data[i], 4); i+=4;
		memcpy(&tempY, &data[i], 4); i+=4;

		verts.push_back(new VERTEX(tempX, tempY));
	}

	memcpy(&feather_type, &data[i], 4); i+=4;
	memcpy(&shore_smoothness, &data[i], 4); i+=4;
	memcpy(&is_water, &data[i], 4); i+=4;
	memcpy(&water_height, &data[i], 4); i+=4;
	memcpy(&water_shader_size, &data[i], 4); i+=4;

	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	water_shader = new unsigned char[strLen];
	memcpy(water_shader, &data[i], strLen); i+= strLen;
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

BCIR::BCIR(unsigned char* data, unsigned int dataSize)
{
	type = LAYER_BCIR;
	memcpy(&x, &data[0], 4);
	memcpy(&y, &data[4], 4);
	
	memcpy(&rad, &data[8], 4);

	memcpy(&feather_type, &data[12], 4);
	memcpy(&feather_amount, &data[16], 4);

	//printf("==BCIR==\nPOS: (%f,%f)\nRAD: %f\nFeather Type: (%d)\nFeather Amount: (%f)\n", x, y, rad, feather_type, feather_amount);
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