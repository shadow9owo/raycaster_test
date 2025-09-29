#pragma once

#include <vector>
#include "raylib.h"

enum TileType
{
	Special, //special is undefined as of rn
	Normal
};

struct tile
{
	TileType _TileType = TileType::Normal;
	unsigned char TextureID = 0;
	Vector4 xywh = { };
};

struct map
{
	char name[16] = {};
	std::vector<tile> mapdata = {};
	long checksum = 0;
};