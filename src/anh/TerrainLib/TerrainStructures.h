#include <vector>

namespace TRNLib
{

class SFAM
{
public:

	SFAM(unsigned char* data);
	~SFAM()
	{
		for(unsigned int i=0; i < children.size(); i++)
			delete children[i];
	}

	int family_id;
	unsigned char* family_name;
	unsigned char* file_name;
	char  unk1;
	char  unk2;
	char  unk3;
	float unk4;
	float unk5;
	//int child_count

	static class SFAM_CHILD
	{
	public:
		unsigned char* shader_name;
		float unk1;
	};

	std::vector<SFAM_CHILD*> children;
};

class FFAM
{
public:

	FFAM(unsigned char* data);

	int family_id;
	unsigned char* family_name;
	char  unk1;
	char  unk2;
	char  unk3;
	float unk4;
	float unk5;
	//int child_count;

	static class FFAM_CHILD
	{
	public:

		unsigned char* appearance_name;
		float unk1;
		int   unk2;
		float unk3;
		float unk4;
		int   unk5;
		int   unk6;
		float unk7;
		float unk8;
	};

	std::vector<FFAM_CHILD*> children;
};

class RFAM
{
public:

	RFAM(unsigned char* data);

	int family_id;
	unsigned char* family_name;
	char unk1;
	char unk2;
	char unk3;
	float unk4;
	//int child_count;

	static class RFAM_CHILD
	{
	public:

		unsigned char* shader_name;
		float unk1;
		float unk2;
		float unk3;
		float unk4;
		int   unk5;
		float unk6;
		float unk7;
		int   unk8;
		int   unk9;
	};

	std::vector<RFAM_CHILD*> children;
};

class EFAM
{
public:

	EFAM(unsigned char* data);

	int family_id;
	unsigned char* family_name;
	char  unk1;
	char  unk2;
	char  unk3;
	float unk4;
};

class MFAM
{
public:

	MFAM(unsigned char* data, unsigned char* data2);

	int fractal_id;
	unsigned char* fractal_label;

	//MFRCFORM
	int				unk1;		//seed ??
	int				unk2;		//Bias type??
	float			unk3;		//bias amount??
	int				unk4;		//Gain type??
	float			unk5;		//Gain amount??
	unsigned int	unk6;		//Octave count??
	float			unk7;		//Octave Parameters or something??
	float			unk8;		//Amplitude??
	float			unk9;		//Frequency X?
	float			unk10;		//Frequency Y?
	unsigned int	unk11;		//X offset??
	unsigned int	unk12;		//y offset?
	unsigned int	unk13;		//Use both?
};

class HEADER
{
public:
	//Basic Data
	unsigned char* filename;
	float map_width;
	float chunk_width;
	int   tiles_per_chunk;
	int   header_type;
	float global_water_height;
	float water_shader_size;
	unsigned char* water_shader_name;
	float seconds_per_world_cycle;

	float Collidable_MinDist;
	float Collidable_MaxDist;
	float Collidable_TileSize;
	float Collidable_TileBorder;
	float Collidable_Seed;

	float NonCollidable_MinDist;
	float NonCollidable_MaxDist;
	float NonCollidable_TileSize;
	float NonCollidable_TileBorder;
	float NonCollidable_Seed;

	float NearRadial_MinDist;
	float NearRadial_MaxDist;
	float NearRadial_TileSize;
	float NearRadial_TileBorder;
	float NearRadial_Seed;

	float FarRadial_MinDist;
	float FarRadial_MaxDist;
	float FarRadial_TileSize;
	float FarRadial_TileBorder;
	float FarRadial_Seed;
};

class FOOTER
{
public:
	float gameWidth;
	float   chunkGameWidth; //width of a chunk
	int   chunkBitFieldSize;
	int   bitMapWidth;

	unsigned char* WMAP;
	unsigned int WMAP_SIZE;
	unsigned char* SMAP;
	unsigned int SMAP_SIZE;
};

};