/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

  This file is part of DarkStar-server source code.

  z16:
  Not anymore! Adjusted slightly for DLL use.

===========================================================================
*/

/*
The NavMesh class will load and find paths given a start point and end point.
*/
#pragma once

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

#include <cstdint>
#include <memory>
#include <string>
#include <array>
#include <vector>

namespace constants {
    static const int magic = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
    static const int version = 1;
    static const int max_nav_polygons = 0x100;
}

struct NavMeshSetHeader
{
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader
{
    dtTileRef tileRef;
    int dataSize;
};

struct vector
{
    float X;
    float Y;
    float Z;

    vector() {}
    vector(float x, float y, float z)
    {
    	X = x;
    	Y = y;
    	Z = z;
    }
};

class nav_mesh
{
public:
    std::vector<vector> FindPath(std::string filename, vector start, vector end);

private:
    std::string Filename;
    std::unique_ptr<dtNavMesh> Mesh;
    std::unique_ptr<dtNavMeshQuery> Query;

    bool Load(std::string filename);

    static vector ToFFXIPos(vector const out);
    static std::array<float, 3> ToDetourPos(vector const pos);
};

struct FindResult {
    std::size_t size;
    vector* data;

    FindResult(std::vector<vector> path) : size{path.size()}, data{new vector[size]} {
        std::copy(path.begin(), path.end(), data);
    }
};

extern "C" __declspec(dllexport) void* __stdcall FindPath(char const* filename, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z);
