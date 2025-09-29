#pragma once

#include "raylib.h"

#include "ExtData.h"

namespace Quick
{
	map map2renderbuffer;
	
	void RenderCellfloor()
	{
		Color cellingcolor = RED;
		Color floorcolor = GREEN;

		DrawRectangle(0, 0, 1280, 360, cellingcolor);
		DrawRectangle(0, 360, 1280, 360, floorcolor);

		return;
	}
}