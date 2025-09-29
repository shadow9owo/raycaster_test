// template - https://github.com/raylib-extras/raylib-quickstart

extern "C"
{
	#include "raylib.h"
}
#include <iostream>

#include "Qloader.hpp"
#include "Qrender.hpp"

#include "Log.hpp"

#include "Player.hpp"

map map_buffer;

RenderTexture2D ren;

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(1280, 720, "render");

	ren = LoadRenderTexture(1280, 720);

	SetTargetFPS(24);

	Quick::WriteMapIntoFile("A");
	map_buffer = Quick::LoadMapIntoBuffer("A");

	Quick::map2renderbuffer = map_buffer; //load to renderer

	for (tile a : map_buffer.mapdata) //debug
	{
		Log("TextureID: " + std::to_string(a.TextureID));
		Log("xywh: " +
			std::to_string(a.xywh.x) + " " +
			std::to_string(a.xywh.y) + " " +
			std::to_string(a.xywh.z) + " " +
			std::to_string(a.xywh.w));
		Log("TileType: " + std::to_string(static_cast<int>(a._TileType)));
	}

	while (!WindowShouldClose())
	{
		pollinputs();

		BeginDrawing();

		ClearBackground(BLACK);

		BeginTextureMode(ren);

		Quick::RenderCellfloor();

		renderperspective();

		EndTextureMode();

		DrawTextureRec(
			ren.texture,
			Rectangle{ 0, 0, (float)ren.texture.width, (float)-ren.texture.height }, // source rectangle (invert Y)
			Vector2{ 0, 0 },
			WHITE
		);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
