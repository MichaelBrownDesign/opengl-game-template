#pragma once
#include "Mesh.h"

namespace gfx
{
	namespace primitive
	{
		MeshData Quad(float width, float height);
		MeshData Box(float width, float height, float depth);
		MeshData Sphere(int segments, float radius);
		MeshData Cylinder(float radius, float height, int segments);
		MeshData Capsule(float radius, float height, int latitudeSegments, int longitudeSegments, int rings);
	}
}