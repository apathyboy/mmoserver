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

TRNLib::LAYER* TerrainManager::findLayer(float x, float z)
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

	return layers->at(0);
}

TRNLib::LAYER* TerrainManager::findLayerRecursive(float x, float z, TRNLib::LAYER* rootLayer)
{
	std::vector<TRNLib::CONTAINER_LAYER*> layers = ((TRNLib::CONTAINER_LAYER*)rootLayer)->children;
	TRNLib::CONTAINER_LAYER* layer;
	std::vector<TRNLib::LAYER*> boundaries;
	TRNLib::Boundary* boundary;

	for (unsigned int i = 0; i < layers.size(); i++)
	{
		layer = (TRNLib::CONTAINER_LAYER*)layers.at(i);
		boundaries = layer->boundaries;

		for (unsigned int j = 0; j < boundaries.size(); j++)
		{
			boundary = (TRNLib::Boundary*)boundaries.at(j);
			if (boundary->isContained(x, z))
				return findLayerRecursive(x, z, layer);
		}
	}

	return rootLayer;
}

TRNLib::MFAM* TerrainManager::getFractal(int fractal_id)
{
	TRNLib::MFAM* fractal;
	std::vector<TRNLib::MFAM*>* fractals = terrain_file.getFractalFamilies();

	for (unsigned int i = 0; i < fractals->size(); i++)
	{
		fractal = fractals->at(i);
		if (fractal->fractal_id == fractal_id)
			return fractal;
	}

	return NULL;
}

float TerrainManager::getHeight(float x, float z)
{
	std::vector<TRNLib::CONTAINER_LAYER*>* layers = terrain_file.getLayers();

	float affector_transform = 1.0f;
	float transform_value = 0.0f;
	float height_result = 0.0f;

	for (unsigned int i = 0; i < layers->size(); i++)
	{
		TRNLib::CONTAINER_LAYER* layer = layers->at(i);
		transform_value = processLayerHeight(layer, x, z, height_result, affector_transform);
	}

	return height_result;
}

float TerrainManager::processLayerHeight(TRNLib::CONTAINER_LAYER* layer, float x, float z, float& base_value, float affector_transform)
{
	std::vector<TRNLib::LAYER*> boundaries = layer->boundaries;
	std::vector<TRNLib::LAYER*> height_affector = layer->heights;

	float transform_value = 0.0f;
	bool has_boundaries = false;
	float result = 0.0f;

	for (unsigned int i = 0; i < boundaries.size(); i++)
	{
		TRNLib::Boundary* boundary = (TRNLib::Boundary*)boundaries.at(i);

		if (boundary->enabled == false)
			continue;
		else
			has_boundaries = true;

		if (boundary->isContained(x, z))
			result = 1.0f;

		if (result > transform_value)
			transform_value = result;
	}

	if (has_boundaries == false)
		transform_value = 1.0f;

	if (transform_value != 0)
	{
		for (unsigned int i = 0; i < height_affector.size(); i++)
		{
			TRNLib::Height* affector = (TRNLib::Height*)height_affector.at(i);
			
			if (affector->enabled)
			{
				base_value = 0;
				affector->getBaseHeight(x, z, transform_value, base_value, this);
			}
		}

		std::vector<TRNLib::CONTAINER_LAYER*> children = layer->children;

		for (unsigned int i = 0; i < children.size(); i++)
		{
			TRNLib::CONTAINER_LAYER* child = children.at(i);

			if (child->enabled)
				processLayerHeight(child, x, z, base_value, affector_transform * transform_value);
		}
	}

	return transform_value;
}