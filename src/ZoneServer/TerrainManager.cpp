#include "TerrainManager.h"


TerrainManager::TerrainManager(std::string zonename)
{
	mZoneFile = "terrain/" + zonename + ".trn";
}


TerrainManager::~TerrainManager(void)
{
}
