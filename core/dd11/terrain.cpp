#include "terrain.h"

int p[512];

float dd11::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float dd11::lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

float dd11::grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float dd11::perlin(float x, float y)
{
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = p[X] + Y, AA = p[A], AB = p[A + 1];
    int B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];

    return lerp(lerp(grad(p[AA], x, y), grad(p[BA], x - 1, y), u),
        lerp(grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1), u),
        v);
}

void dd11::initPerlinNoise() {

    std::vector<int> permutation;
    permutation.reserve(256);
    for (int i = 0; i < 256; ++i) {
        permutation.push_back(i);
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(permutation.begin(), permutation.end(), generator);

    for (int i = 0; i < 256; ++i) {
        p[i] = p[i + 256] = permutation[i];
    }
}

ew::MeshData dd11::generateTerrain(int width, int depth, float scale, int subdivisions)
{
    ew::MeshData terrain;
    ew::Vertex v;

    for (int z = 0; z <= subdivisions; ++z) {
        for (int x = 0; x <= subdivisions; ++x) {
            float height = perlin((x + width) * scale, (z + depth) * scale) * 10.0f;

            v.pos = ew::Vec3((float)(x) * width, height, (float)(z) * depth);
            v.normal = ew::Vec3(0, 1, 0);
            v.uv = ew::Vec2((float)(x) / subdivisions, (float)(z) / subdivisions);

            terrain.vertices.push_back(v);
        }
    }

    // Generate indices for triangles
    for (int z = 0; z < subdivisions; ++z) {
        for (int x = 0; x < subdivisions; ++x) {
            int topLeft = z * (subdivisions + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (subdivisions + 1) + x;
            int bottomRight = bottomLeft + 1;

            terrain.indices.push_back(topLeft);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(topRight);

            terrain.indices.push_back(topRight);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(bottomRight);
        }
    }

    return terrain;
}
