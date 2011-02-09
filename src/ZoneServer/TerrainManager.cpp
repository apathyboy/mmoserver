/*
---------------------------------------------------------------------------------------
This source file is part of SWG:ANH (Star Wars Galaxies - A New Hope - Server Emulator)

For more information, visit http://www.swganh.com

Copyright (c) 2006 - 2010 The SWG:ANH Team
---------------------------------------------------------------------------------------
Use of this source code is governed by the GPL v3 license that can be found
in the COPYING file or at http://www.gnu.org/licenses/gpl-3.0.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/

#include "TerrainManager.h"
#include "anh\TerrainLib\Boundary.h"
#include "anh\TerrainLib\BPOL.h"
#include "anh\TerrainLib\AHCN.h"
#include "anh\TerrainLib\AHFR.h"

#include <iostream>

#include <glog\logging.h>


TerrainManager::TerrainManager(std::string terrain_filename)
	: terrain_file(terrain_filename)
{
	if (terrain_file.getError() == 0)
	{
		LOG(INFO) << "Successfully loaded the terrain file.";
		float x = 1307.160034f; float z = 3131.459961f;
		float y = getHeight(x, z);
		LOG(ERROR) << "Height at [" << x << ", " << z << "] is [" << y << "]";
	}
	else
	{
		LOG(ERROR) << "Unable to load the terrain file.";
		abort();
	}
}


TerrainManager::~TerrainManager(void)
{
}

TRNLib::HEADER* TerrainManager::getHeaders(void)
{
	return terrain_file.getHeaderData();
}

float TerrainManager::getWaterHeight(float x, float z, float& water_height)
{
	TRNLib::HEADER* header = getHeaders();
	std::vector<TRNLib::Boundary*>* boundaries = terrain_file.getWaterBoundaries();
	TRNLib::Boundary* boundary;

	for (unsigned int i = 0; i < boundaries->size(); i++)
	{
		boundary = boundaries->at(i);
		if (boundary->isContained(x, z))
		{
			water_height = ((BPOL*)(boundary))->water_height;
			return true;
		}
	}

	if (header->use_global_water_height != 0)
	{
		water_height = header->global_water_height;
		return true;
	}

	return false;
}

TRNLib::CONTAINER_LAYER* TerrainManager::findLayer(float x, float z)
{
	std::vector<TRNLib::CONTAINER_LAYER*>* layers = terrain_file.getLayers();
	TRNLib::CONTAINER_LAYER* layer;
	std::vector<TRNLib::LAYER*> boundaries;
	TRNLib::Boundary* boundary;

	for (unsigned int i = 0; i < layers->size(); i++)
	{
		layer = (TRNLib::CONTAINER_LAYER*)layers->at(i);
		boundaries = layer->boundaries;

		for (unsigned int j = 0; j < boundaries.size(); j++)
		{
			boundary = (TRNLib::Boundary*)boundaries.at(j);
			if (boundary->isContained(x, z))
				return findLayerRecursive(x, z, layer);
		}
	}

	return (TRNLib::CONTAINER_LAYER*)layers->at(0);
}

TRNLib::CONTAINER_LAYER* TerrainManager::findLayerRecursive(float x, float z, TRNLib::CONTAINER_LAYER* rootLayer)
{
	std::vector<TRNLib::CONTAINER_LAYER*> layers = rootLayer->children;
	TRNLib::LAYER* test_layer;
	TRNLib::CONTAINER_LAYER* layer;
	std::vector<TRNLib::LAYER*> boundaries;
	TRNLib::Boundary* boundary;

	for (unsigned int i = 0; i < layers.size(); i++)
	{
		test_layer = layers.at(i);

		if (test_layer->type == TRNLib::LAYER_CONTAINER)
		{
			layer = (TRNLib::CONTAINER_LAYER*)test_layer;
			boundaries = layer->boundaries;

			for (unsigned int j = 0; j < boundaries.size(); j++)
			{
				boundary = (TRNLib::Boundary*)boundaries.at(j);
				if (boundary->isContained(x, z))
					return findLayerRecursive(x, z, layer);
			}
		}
	}

	return rootLayer;
}

TRNLib::MFAM* TerrainManager::getFractal(int fractal_id)
{
	return terrain_file.getFractalFamilies()->at(fractal_id);
}

float TerrainManager::getHeight(float x, float z)
{
	TRNLib::LAYER* height_layer = findLayer(x, z)->getHeight();
	float base_height;

	if (height_layer->type == TRNLib::LAYER_AHCN)
	{
		base_height = ((AHCN*)height_layer)->getBaseHeight(x,z, this);
	}
	else
	{
		base_height = ((AHFR*)height_layer)->getBaseHeight(x,z, this);
	}

	return base_height;
}