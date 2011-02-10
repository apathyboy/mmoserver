/*
This file is part of MMOServer. For more information, visit http://swganh.com
Copyright (c) 2006 - 2010 The SWG:ANH Team

MMOServer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MMOServer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MMOServer. If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include "TerrainManager.h"

class TerrainManagerTest : public testing::Test {};

/// By default a new instance of the dispatcher should not have any listeners.
TEST_F(TerrainManagerTest, getWaterHeight) {
	// Construct a terrain manager for Tattooine.
	TerrainManager tm_test("build/bin/Debug/terrain/tatooine.trn");

	// Obtain headers from the terrain manager.
	TRNLib::HEADER* header = tm_test.getHeaders();

	// Test if we are getting values we expect for global water height from the header
	EXPECT_EQ(header->use_global_water_height, 0) << "Expected use_global_water_height of 0";
	EXPECT_EQ(header->global_water_height,0) << "Expected global_water_height of 0";
}

TEST_F(TerrainManagerTest, getHeight) {
	// Construct a terrain manager for Tatooine
	TerrainManager tm_test("build/bin/Debug/terrain/tatooine.trn");

	std::vector<float*> coords;
	float* coord = new float[3];

	coord[0] = -5051.0f; coord[1] = 75.0f; coord[2] = -6629.0f;
	coords.push_back(coord);
	coord[0] = -5274.0f; coord[1] = 75.0f; coord[2] = -6547.0f;
	coords.push_back(coord);
	coord[0] = -1278.5f; coord[1] = 12.53f; coord[2] = -3643.1f;
	coords.push_back(coord);
	coord[0] = -1130.9f; coord[1] = 12.2f; coord[2] = -3611.6f;
	coords.push_back(coord);
	coord[0] = 3472.22f; coord[1] = 5.0f; coord[2] = -4918.59f;

	for (unsigned int i = 0; i < coords.size(); i++)
	{
		float* pos = coords.at(i);
		float height_test = tm_test.getHeight(pos[0], pos[2]);
		EXPECT_EQ(height_test, pos[1]) << "Expected height of " << pos[1] << " and got " << height_test;
	}
}