#include "TerrainStructures.h"
#include <string>
#include <string.h>

using namespace TRNLib;

SFAM::SFAM(unsigned char* data)
{
	unsigned int i = 0;
	
	//Read family_id
	memcpy(&family_id, &data[i], 4); i += 4;

	//Read family_name

	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	family_name = new unsigned char[strLen];
	memcpy(family_name, &data[i], strLen); i+= strLen;

	//Read file_name
	strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	file_name = new unsigned char[strLen];
	memcpy(file_name, &data[i], strLen); i+= strLen;

	//Read unk1
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk2
	memcpy(&unk2, &data[i], 1); ++i;
	//Read unk3
	memcpy(&unk3, &data[i], 1); ++i;
	//Read unk4
	memcpy(&unk4, &data[i], 4); i+=4;
	//Read unk5
	memcpy(&unk5, &data[i], 4); i+=4;

	unsigned int child_count;
	memcpy(&child_count, &data[i], 4); i+=4;

	for(unsigned int j=0; j < child_count; ++j)
	{
		SFAM_CHILD* pChild = new SFAM_CHILD();

		//Read shader_name
		unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
		pChild->shader_name = new unsigned char[strLen];
		memcpy(pChild->shader_name, &data[i], strLen); i+= strLen;

		//Read unk1
		memcpy(&pChild->unk1, &data[i], 4); i+=4;

		children.push_back(pChild);
	}
}

FFAM::FFAM(unsigned char* data)
{
	unsigned int i=0;

	//Read family_id
	memcpy(&family_id, &data[i], 4); i += 4;

	//Read family_name
	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	family_name = new unsigned char[strLen];
	memcpy(family_name, &data[i], strLen); i+= strLen;

	//Read unk1
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk2
	memcpy(&unk2, &data[i], 1); ++i;
	//Read unk3
	memcpy(&unk3, &data[i], 1); ++i;
	//Read unk4
	memcpy(&unk4, &data[i], 4); i+=4;
	//Read unk5
	memcpy(&unk5, &data[i], 4); i+=4;

	unsigned int child_count;
	memcpy(&child_count, &data[i], 4); i+=4;

	for(unsigned int j=0; j < child_count; ++j)
	{
		FFAM_CHILD* pChild = new FFAM_CHILD();

		unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
		pChild->appearance_name = new unsigned char[strLen];
		memcpy(pChild->appearance_name, &data[i], strLen); i+= strLen;

		//Read unk1;
		memcpy(&pChild->unk1, &data[i], 4); i+=4;
		//Read unk2; //Type unknown!
		memcpy(&pChild->unk2, &data[i], 4); i+=4;
		//Read unk3;
		memcpy(&pChild->unk3, &data[i], 4); i+=4;
		//Read unk4;
		memcpy(&pChild->unk4, &data[i], 4); i+=4;
		//Read unk5;
		memcpy(&pChild->unk5, &data[i], 4); i+=4;
		//Read unk6;
		memcpy(&pChild->unk6, &data[i], 4); i+=4;
		//Read unk7;
		memcpy(&pChild->unk7, &data[i], 4); i+=4;
		//Read unk8;
		memcpy(&pChild->unk8, &data[i], 4); i+=4;

		children.push_back(pChild);
	}
}

RFAM::RFAM(unsigned char* data)
{
	unsigned int i=0;

	//Read family_id
	memcpy(&family_id, &data[i], 4); i += 4;

	//Read family_name
	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	family_name = new unsigned char[strLen];
	memcpy(family_name, &data[i], strLen); i+= strLen;

	//Read unk1
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk2
	memcpy(&unk2, &data[i], 1); ++i;
	//Read unk3
	memcpy(&unk3, &data[i], 1); ++i;
	//Read unk4
	memcpy(&unk4, &data[i], 4); i+=4;

	unsigned int child_count;
	memcpy(&child_count, &data[i], 4); i+=4;

	for(unsigned int j=0; j < child_count; ++j)
	{
		RFAM_CHILD* pChild = new RFAM_CHILD();

		unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
		pChild->shader_name = new unsigned char[strLen];
		memcpy(pChild->shader_name, &data[i], strLen); i+= strLen;

		//Read unk1;
		memcpy(&pChild->unk1, &data[i], 4); i+=4;
		//Read unk2; //Type unknown!
		memcpy(&pChild->unk2, &data[i], 4); i+=4;
		//Read unk3;
		memcpy(&pChild->unk3, &data[i], 4); i+=4;
		//Read unk4;
		memcpy(&pChild->unk4, &data[i], 4); i+=4;
		//Read unk5;
		memcpy(&pChild->unk5, &data[i], 4); i+=4;
		//Read unk6;
		memcpy(&pChild->unk6, &data[i], 4); i+=4;
		//Read unk7;
		memcpy(&pChild->unk7, &data[i], 4); i+=4;
		//Read unk8;
		memcpy(&pChild->unk8, &data[i], 4); i+=4;
		//Read unk9;
		memcpy(&pChild->unk9, &data[i], 4); i+=4;

		children.push_back(pChild);
	}
}

EFAM::EFAM(unsigned char* data)
{
	unsigned int i=0;

	//Read family_id;
	memcpy(&family_id, &data[i], 4); i+=4;
	//Read family_name;
	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	family_name = new unsigned char[strLen];
	memcpy(family_name, &data[i], strLen); i+= strLen;

	//Read unk1;
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk2;
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk3;
	memcpy(&unk1, &data[i], 1); ++i;
	//Read unk4;
	memcpy(&unk1, &data[i], 4); i+=4;
}

MFAM::MFAM(unsigned char* data, unsigned char* data2)
{
	unsigned int i = 0;

	//First Part
	//Read fractal_id;
	memcpy(&fractal_id, &data[i], 4); i+=4;
	//Read fractal_label;
	unsigned int strLen = strlen(const_cast<const char*>((char*)&data[i])) + 1;
	fractal_label = new unsigned char[strLen];
	memcpy(fractal_label, &data[i], strLen); i+= strLen;

	//Reset the counter
	i=0;

	//Second Part
	//Read seed_arg;
	memcpy(&seed_arg , &data2[i], 4); i+=4;
	//Read use_bias;
	memcpy(&use_bias , &data2[i], 4); i+=4;
	//Read bias;
	memcpy(&bias , &data2[i], 4); i+=4;
	//Read use_gain;
	memcpy(&use_gain , &data2[i], 4); i+=4;
	//Read gain;
	memcpy(&gain , &data2[i], 4); i+=4;
	//Read octaves;
	memcpy(&octaves , &data2[i], 8); i+=4;
	//Read octaves_arg;
	memcpy(&octaves_arg , &data2[i], 4); i+=4;
	//Read amplitude_arg;
	memcpy(&amplitude , &data2[i], 4); i+=4;
	setAmplitude();
	//Read freq_x;
	memcpy(&freq_x, &data2[i], 4); i+=4;
	//Read freq_z;
	memcpy(&freq_z, &data2[i], 4); i+=4;
	//Read offset_x; //type unknown
	memcpy(&offset_x, &data2[i], 4); i+=4;
	//Read offset_y;
	memcpy(&offset_z, &data2[i], 4); i+=4;
	//Read combination
	memcpy(&combination_type, &data2[i], 4); i+=4;
}

void MFAM::setSeed(int seed)
{
	// Initialize Random
}

void MFAM::setAmplitude()
{
	offset = 0.0f;

	float curr_amplitude = 0.0f;
	float next_amplitude = 1.0f;

	for (int i = 0; i < octaves; i++)
	{
		curr_amplitude = curr_amplitude + next_amplitude;
		next_amplitude = next_amplitude * amplitude;
	}

	offset = curr_amplitude;

	if (offset != 0)
		offset = 1.0/offset;
}