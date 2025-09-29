#pragma once

#include <raylib.h>
#include <cmath>
#include "Qrender.hpp"

#include "ExtData.h"

#define buffer Quick::map2renderbuffer

constexpr int MAP_MIN = -256;
constexpr int MAP_MAX = 256;
constexpr int TILE_SIZE = 32;
constexpr int MAP_GRID_SIZE = (MAP_MAX - MAP_MIN) / TILE_SIZE + 1;

float fov = 60.0f;
float rotation = 90.0f;
Vector2 position = { 128, 128 };

int worldMap[MAP_GRID_SIZE][MAP_GRID_SIZE] = { 0 };

void buildWorldMap(const map& m) {
    std::memset(worldMap, 0, sizeof(worldMap));
    for (const auto& t : m.mapdata) {
        if (t._TileType != TileType::Normal) continue;

        int tx = static_cast<int>((t.xywh.x - MAP_MIN) / TILE_SIZE);
        int ty = static_cast<int>((t.xywh.y - MAP_MIN) / TILE_SIZE);

        if (tx >= 0 && tx < MAP_GRID_SIZE && ty >= 0 && ty < MAP_GRID_SIZE)
            worldMap[tx][ty] = 1; // wall
    }
}

bool isWall(int x, int y) {
    if (x < 0 || x >= MAP_GRID_SIZE || y < 0 || y >= MAP_GRID_SIZE) return true;
    return worldMap[x][y] != 0;
}

void renderperspective() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    const float halfFOV = fov / 2.0f;
    const float angleStep = fov / static_cast<float>(screenWidth);
    const float projDist = (screenWidth / 2.0f) / tanf(fov * 0.5f * (PI / 180.0f));

    for (int x = 0; x < screenWidth; x++) {
        float rayAngle = rotation - halfFOV + x * angleStep;
        float rayRad = rayAngle * (PI / 180.0f);

        float dx = cosf(rayRad);
        float dy = sinf(rayRad);

        int mapX = static_cast<int>((position.x - MAP_MIN) / TILE_SIZE);
        int mapY = static_cast<int>((position.y - MAP_MIN) / TILE_SIZE);

        float deltaDistX = (dx == 0) ? 1e30f : std::abs(TILE_SIZE / dx);
        float deltaDistY = (dy == 0) ? 1e30f : std::abs(TILE_SIZE / dy);

        int stepX = (dx < 0) ? -1 : 1;
        int stepY = (dy < 0) ? -1 : 1;

        float sideDistX = (dx < 0) ? (position.x - (mapX * TILE_SIZE + MAP_MIN)) * deltaDistX / TILE_SIZE
            : (((mapX + 1) * TILE_SIZE + MAP_MIN) - position.x) * deltaDistX / TILE_SIZE;

        float sideDistY = (dy < 0) ? (position.y - (mapY * TILE_SIZE + MAP_MIN)) * deltaDistY / TILE_SIZE
            : (((mapY + 1) * TILE_SIZE + MAP_MIN) - position.y) * deltaDistY / TILE_SIZE;

        bool hit = false;
        bool hitVertical = false;
        float distance = 0.0f;

        while (!hit && distance < 1000.0f) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                distance = sideDistX;
                hitVertical = true;
            }
            else {
                sideDistY += deltaDistY;
                mapY += stepY;
                distance = sideDistY;
                hitVertical = false;
            }

            if (isWall(mapX, mapY)) hit = true;
        }

        if (hit) {
            float delta = rayRad - rotation * (PI / 180.0f);
            float correctedDist = distance * cosf(delta);
            if (correctedDist < 8.0f) correctedDist = 8.0f;

            float sliceHeight = (TILE_SIZE / correctedDist) * projDist;

            float shade = 1.0f / (0.01f * correctedDist);
            if (shade > 1.0f) shade = 1.0f;
            if (shade < 0.2f) shade = 0.2f;

            Color baseColor = hitVertical ? WHITE : GRAY;
            Color col = {
                static_cast<unsigned char>(baseColor.r * shade),
                static_cast<unsigned char>(baseColor.g * shade),
                static_cast<unsigned char>(baseColor.b * shade),
                255
            };

            DrawLine(x,
                screenHeight / 2 - static_cast<int>(sliceHeight / 2),
                x,
                screenHeight / 2 + static_cast<int>(sliceHeight / 2),
                col);
        }
    }
}

void pollinputs()
{
    float moveSpeed = 1.5f;
    float rotSpeed = 0.5f;

    rotation += GetMouseDelta().x * rotSpeed;
    float rad = rotation * (PI / 180.0f);

    Vector2 moveDelta = { 0, 0 };

    if (IsKeyDown(KEY_W)) {
        moveDelta.x += cosf(rad);
        moveDelta.y += sinf(rad);
    }
    if (IsKeyDown(KEY_S)) {
        moveDelta.x -= cosf(rad);
        moveDelta.y -= sinf(rad);
    }

    float strafeRad = rad + PI / 2;
    if (IsKeyDown(KEY_A)) {
        moveDelta.x -= cosf(strafeRad);
        moveDelta.y -= sinf(strafeRad);
    }
    if (IsKeyDown(KEY_D)) {
        moveDelta.x += cosf(strafeRad);
        moveDelta.y += sinf(strafeRad);
    }

    float len = sqrtf(moveDelta.x * moveDelta.x + moveDelta.y * moveDelta.y);
    if (len > 0.0f) {
        moveDelta.x /= len;
        moveDelta.y /= len;
    }

    position.x += moveDelta.x * moveSpeed;
    position.y += moveDelta.y * moveSpeed;
}
