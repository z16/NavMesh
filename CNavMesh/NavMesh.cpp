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

===========================================================================
*/

/* Selfishly modified by z16 to suit my own needs! */

#include "NavMesh.hpp"
#include "DetourNavMeshQuery.h"
#include <memory>
#include <algorithm>
#include <fstream>
#include <array>
#include <vector>

template<typename T>
T read(std::ifstream& file) {
    T value;
    file.read(reinterpret_cast<char*>(&value), sizeof value);
    return value;
}

vector nav_mesh::ToFFXIPos(vector const out) {
    return {out.X, -out.Y, -out.Z};
}

std::array<float, 3> nav_mesh::ToDetourPos(vector const pos) {
    return {pos.X, pos.Y * -1, pos.Z * -1};
}

bool nav_mesh::Load(std::string filename) {
    std::ifstream file{filename.c_str(), std::ifstream::binary | std::ifstream::in};

    if (!file) {
        return false;
    }

    // Read header.
    auto header = read<NavMeshSetHeader>(file);
    if (header.magic != constants::magic) {
        return false;
    }
    if (header.version != constants::version) {
        return false;
    }

    Mesh = std::unique_ptr<dtNavMesh>{dtAllocNavMesh()};
    if (!Mesh) {
        return false;
    }

    auto status = Mesh->init(&header.params);
    if (dtStatusFailed(status)) {
        return false;
    }

    // Read tiles.
    for (int i = 0; i < header.numTiles; ++i) {
        auto tileHeader = read<NavMeshTileHeader>(file);
        if (!tileHeader.tileRef || !tileHeader.dataSize)
            break;

        auto data = reinterpret_cast<unsigned char*>(dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM));
        if (!data) {
            break;
        }

        std::fill(data, data + tileHeader.dataSize, 0);
        file.read(reinterpret_cast<char*>(data), tileHeader.dataSize);

        Mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
    }

    Query = std::make_unique<dtNavMeshQuery>();

    // init detour nav mesh path finder
    status = Query->init(Mesh.get(), constants::max_nav_polygons);

    if (dtStatusFailed(status)) {
        return false;
    }

    return true;
}

std::vector<vector> nav_mesh::FindPath(std::string filename, vector start, vector end) {
    std::vector<vector> path;

    if (Filename != filename) {
        if (Load(filename)) {
            Filename = filename;
        }
        else {
            Filename = "";
        }
    }

    if (Filename.empty()) {
        return path;
    }

    auto start_pos = nav_mesh::ToDetourPos(start);
    auto end_pos = nav_mesh::ToDetourPos(end);

    dtQueryFilter filter;
    filter.setIncludeFlags(0xFFFF);
    filter.setExcludeFlags(0);

    float polyPickExt[3] = {10, 20, 10};

    dtPolyRef start_ref;
    dtPolyRef end_ref;

    std::array<float, 3> start_nearest;
    std::array<float, 3> end_nearest;

    if (dtStatusFailed(Query->findNearestPoly(start_pos.data(), polyPickExt, &filter, &start_ref, start_nearest.data()))) {
        return path;
    }

    if (dtStatusFailed(Query->findNearestPoly(end_pos.data(), polyPickExt, &filter, &end_ref, end_nearest.data()))) {
        return path;
    }

    if (!Mesh->isValidPolyRef(start_ref) || !Mesh->isValidPolyRef(end_ref)) {
        return path;
    }

    dtPolyRef polygons[constants::max_nav_polygons];
    float straightPath[constants::max_nav_polygons * 3];
    unsigned char straightPathFlags[constants::max_nav_polygons];
    dtPolyRef straightPathPolys[constants::max_nav_polygons];
    int nstraightPath = 0;

    // not sure what this is for?
    std::int32_t pathCount = 0;

    if (dtStatusFailed(Query->findPath(start_ref, end_ref, start_nearest.data(), end_nearest.data(), &filter, polygons, &pathCount, constants::max_nav_polygons))) {
        return path;
    }

    if (pathCount == 0) {
        return path;
    }

    std::int32_t straightPathCount = constants::max_nav_polygons * 3;

    if (dtStatusFailed(Query->findStraightPath(start_nearest.data(), end_nearest.data(), polygons, pathCount, straightPath, straightPathFlags, straightPathPolys, &straightPathCount, constants::max_nav_polygons))) {
        return path;
    }

    // i starts at 3 so the start position is ignored
    for (int i = 3; i < straightPathCount * 3; i += 3) {
        path.push_back(nav_mesh::ToFFXIPos({straightPath[i], straightPath[i + 1], straightPath[i + 2]}));
    }

    return path;
}

nav_mesh mesh{};

extern "C" __declspec(dllexport) void* __stdcall FindPath(char const* filename, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z) {
    return new FindResult{mesh.FindPath(filename, {start_x, start_y, start_z}, {end_x, end_y, end_z})};
}
