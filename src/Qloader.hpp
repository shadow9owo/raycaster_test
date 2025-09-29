#pragma once

#include <string>
#include <vector>

#include "ExtData.h"

namespace Quick
{
	extern map LoadMapIntoBuffer(const std::string& name);
	extern bool WriteMapIntoFile(const std::string& name);
}