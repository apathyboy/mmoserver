#ifndef ANH_TERRAINMANAGER_H
#define ANH_TERRAINMANAGER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Utils/typedefs.h"

class TerrainManager
{
public:
	TerrainManager(std::string zonename);
	~TerrainManager(void);

private:
	std::string mZoneFile;
};

#endif
