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
			v.normal = ew::Normalize(v.pos);
			v.uv.x = (float)(col) / numSegments;
			v.uv.y = (float)(row) / numSegments;
			sphere.vertices.push_back(v);
		}
	}

	int poleStart = 0;
	int sideStart = numSegments + 1;
	int columns = numSegments + 1;

	for (int i = 0; i < numSegments; i++)
	{
		sphere.indices.push_back(poleStart + i);
		sphere.indices.push_back(sideStart + i);
		sphere.indices.push_back(sideStart + i + 1);
	}

	poleStart = numSegments * (numSegments + 1);
	sideStart = poleStart - (numSegments + 1);

	for (int i = 0; i < numSegments; i++)
	{
		sphere.indices.push_back(sideStart + i);
		sphere.indices.push_back(poleStart + i + 1);
		sphere.indices.push_back(sideStart + i + 1);
	}

	for (int row = 0;  row < numSegments;  row++)
	{
		for (int col = 0; col < numSegments; col++)
		{
			int start = row * columns + col;
			//Triangle 1
			sphere.indices.push_back(start);
			sphere.indices.push_back(start + columns);
			sphere.indices.push_back(start + 1);
			//Triangle 2
			sphere.indices.push_back(start + 1);
			sphere.indices.push_back(start + columns);
			sphere.indices.push_back(start + columns + 1);
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
	float u, vTop, vBottom;

	//Top Center
	v.pos = ew::Vec3(0, topY, 0);
	v.normal = ew::Vec3(0, 1, 0);
	v.uv = ew::Vec2(0.5f, 0.5f);
	cylinder.vertices.push_back(v);

	//Top Ring (Top Face)
	/*for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
		v.normal = ew::Vec3(0, 1, 0);
		u = float(i) / numSegments;
		vTop = 1.0f;
		v.uv = ew::Vec2(u, vTop);
		cylinder.vertices.push_back(v);
	}*/

	//Top Ring (Side Face)
	for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
		v.normal = ew::Normalize(v.pos - ew::Vec3(0, topY, 0));
		u = float(i) / numSegments;
		vTop = 0.0f;
		v.uv = ew::Vec2(u, vTop);
		cylinder.vertices.push_back(v);
	}	

	//Bottom Center
	v.pos = ew::Vec3(0, bottomY, 0);
	v.normal = ew::Vec3(0, -1, 0);
	v.uv = ew::Vec2(0.5f, 0.5f);
	cylinder.vertices.push_back(v);

	//Bottom Ring (Side Face)
	for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
		v.normal = ew::Normalize(v.pos - ew::Vec3(0, bottomY, 0));
		u = float(i) / numSegments;
		vBottom = 0.0f;
		v.uv = ew::Vec2(u, vBottom);
		cylinder.vertices.push_back(v);
	}

	//Bottom Ring (Bottom Face)
	/*for (int i = 0; i <= numSegments; i++)
	{
		float theta = i * thetaStep;
		v.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
		v.normal = ew::Vec3(0, -1, 0);
		u = float(i) / numSegments;
		vBottom = 1.0f;
		v.uv = ew::Vec2(u, vBottom);
		cylinder.vertices.push_back(v);
	}*/

	//Cap Indices
	int start = 1, center = 0;
	for (int i = 0; i < numSegments; i++) {
		cylinder.indices.push_back(start + i);
		cylinder.indices.push_back(center);
		cylinder.indices.push_back(start + i + 1);
	}
	
	start += numSegments + 2;
	center+= numSegments + 2;
	
	for (int i = 0; i < numSegments; i++) {

		cylinder.indices.push_back(center);
		cylinder.indices.push_back(start + i);
		cylinder.indices.push_back(start + i + 1);
	}
	start = 1;
	
	int sideStart = start, columns = numSegments + 2;
	for (int i = 0; i < numSegments; i++)
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
			v.pos = ew::Vec3(size * ((float)col / subdivisions), 0, -size * ((float)row / subdivisions));
			v.normal = ew::Vec3(0, 1, 0);
			v.uv = ew::Vec2(1 - (float)row / subdivisions, (float)col / subdivisions);
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
