#include "procGen.h"

ew::MeshData dd11::createSphere(float radius, int numSegments)
{
	ew::MeshData sphere;
	ew::Vertex v;

	float thetaStep = (2 * ew::PI) / numSegments;
	float phiStep = ew::PI / numSegments;
	float theta, phi;

	for (int row = 0; row <= numSegments; row++)
	{
		phi = row * phiStep;
		
		for (int col = 0; col <= numSegments; col++)
		{
			theta = col * thetaStep;
			v.pos = ew::Vec3(radius * sin(phi) * sin(theta), radius * cos(phi), radius * sin(phi) * cos(theta));
			sphere.vertices.push_back(v);
		}
	}

	return sphere;
}

ew::MeshData dd11::createCylinder(float height, float radius, int numSegments)
{

	return ew::MeshData();
}

ew::MeshData dd11::createPlane(float size, int subdivisions)
{
	ew::MeshData plane;
	ew::Vertex v;

	//Vertices
	for (int row = 0; row <= subdivisions; row++)
	{
		for (int col = 0; col <= subdivisions; col++)
		{
			v.pos = ew::Vec3(size * (col / subdivisions), 0, -size * (row / subdivisions));
			plane.vertices.push_back(v);
		}
	}

	//Indices
	int columns = subdivisions + 1;

	for (int row = 0; row < subdivisions; row++)
	{
		for (int col = 0; col < subdivisions; col++)
		{
			int start = row * columns + col;

			//Bottom Right Triangle
			plane.indices.push_back(start);
			plane.indices.push_back(start + 1);
			plane.indices.push_back(start + columns + 1);

			//Top Left Triangle
			plane.indices.push_back(start + columns);
			plane.indices.push_back(start + columns + 1);
			plane.indices.push_back(start);
		}
	}

	return plane;
}
