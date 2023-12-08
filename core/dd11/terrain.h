#pragma once
#include "../ew/mesh.h"
#include <vector>
#include <random>
#include <cmath>

namespace dd11 {
	float fade(float t);
	float lerp(float a, float b, float t);
	float grad(int hash, float x, float y);
	float perlin(float x, float y);
	void initPerlinNoise();
	ew::MeshData generateTerrain(int width, int depth, float scale, int subdivisions);
	ew::MeshData generateBoulder(float radius, int numSegments, float roughness);
}