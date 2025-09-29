#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <random>

#include "ExtData.h"
#include "STD.hpp"

#include "Qloader.hpp"

namespace Quick
{
    map LoadMapIntoBuffer(const std::string& name)
    {
        map m{};
        std::string raw = DG2D::SmartTableDatabase::LoadValue(name);
        if (raw.empty()) return m;

        const char* ptr = raw.data();
        size_t offset = 0;

        if (raw.size() < sizeof(long) + sizeof(uint32_t))
            return m;

        std::memcpy(&m.checksum, ptr + offset, sizeof(long));
        offset += sizeof(long);

        uint32_t count = 0;
        std::memcpy(&count, ptr + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        for (uint32_t i = 0; i < count && offset + sizeof(tile) <= raw.size(); i++)
        {
            tile t{};
            std::memcpy(&t, ptr + offset, sizeof(tile));
            offset += sizeof(tile);
            m.mapdata.push_back(t);
        }

        std::strncpy(m.name, name.c_str(), sizeof(m.name));
        return m;
    }

    bool WriteMapIntoFile(const std::string& name)
    {
        map m{};
        std::strncpy(m.name, name.c_str(), sizeof(m.name));
        m.checksum = 0xBEEF;

        const int tileCount = 20;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(-256, 256);

        for (int i = 0; i < tileCount; i++)
        {
            tile t{};
            t._TileType = TileType::Normal;
            t.TextureID = 1;

            float x = static_cast<float>(dist(gen));
            float y = static_cast<float>(dist(gen));

            t.xywh = { x, y, 32.0f, 32.0f };
            m.mapdata.push_back(t);
        }

        std::string raw;
        raw.resize(sizeof(long) + sizeof(uint32_t) + m.mapdata.size() * sizeof(tile));
        size_t offset = 0;

        std::memcpy(&raw[offset], &m.checksum, sizeof(long));
        offset += sizeof(long);

        uint32_t count = static_cast<uint32_t>(m.mapdata.size());
        std::memcpy(&raw[offset], &count, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        for (auto& t : m.mapdata)
        {
            std::memcpy(&raw[offset], &t, sizeof(tile));
            offset += sizeof(tile);
        }

        return DG2D::SmartTableDatabase::SetValue(name, raw);
    }
}
