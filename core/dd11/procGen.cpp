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
	ew::MeshData cylinder;
	ew::Vertex v;

	float topY = height / 2, bottomY = -topY;
	float thetaStep = (2 * ew::PI) / numSegments;

	//Top Center
	v.pos = ew::Vec3(0, topY, 0);
	cylinder.vertices.push_back(v);

	//Top Ring
	for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
		cylinder.vertices.push_back(v);
	}

	//Bottom Ring
	for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
		cylinder.vertices.push_back(v);
	}

	//Bottom Center
	v.pos = ew::Vec3(0, bottomY, 0);
	cylinder.vertices.push_back(v);

	//Cap Indices
	float start = 0, center = 0;
	for (int i = 0; i < numSegments; i++) {
		cylinder.indices.push_back(start + i);
		cylinder.indices.push_back(center);
		cylinder.indices.push_back(start + i + 1);
	}

	//start = numSegments;
	center++; 

	for (int i = 0; i < numSegments; i++) {
		cylinder.indices.push_back(start + i + numSegments);
		cylinder.indices.push_back(center);
		cylinder.indices.push_back(start + i + 1 + numSegments);
	}

	float sideStart = start, columns = numSegments + 1;
	for (int i = 0; i < columns; i++)
	{
		start = sideStart + i;

		//Triangle 1
		cylinder.indices.push_back(start);
		cylinder.indices.push_back(start + 1);
		cylinder.indices.push_back(start + columns);
		//Triangle 2
		cylinder.indices.push_back(start + columns);
		cylinder.indices.push_back(start + 1);
		cylinder.indices.push_back(start + columns + 1);
	}	

	return cylinder;
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
			v.normal = ew::Vec3(0, 0, 1);
			v.uv = ew::Vec2(1 / subdivisions, 1 / subdivisions);
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
			plane.indices.push_back(start);
			plane.indices.push_back(start + columns + 1);
			plane.indices.push_back(start + columns);
		}
	}

	return plane;
}
