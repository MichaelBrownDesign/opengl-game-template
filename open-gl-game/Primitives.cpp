#include <vector>
#include <map>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Primitives.h"

namespace gfx
{
	namespace primitive
	{
		MeshData Quad(float width, float height)
		{
			const glm::vec2 size(width/2, height/2);
			MeshData meshData{};

			meshData.vertices =
			{
				glm::vec3(-size.x,  -size.y,  0),
				glm::vec3(-size.x,   size.y,  0),
				glm::vec3( size.x,   size.y,  0),
				glm::vec3( size.x,  -size.y,  0)
			};

			meshData.normals =
			{
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1)
			};

			meshData.uvs =
			{
				glm::vec2(0, 0),
				glm::vec2(0, 1),
				glm::vec2(1, 1),
				glm::vec2(1, 0)
			};

			meshData.indices =
			{
				0, 1, 2,
				3, 0, 2
			};

			return meshData;
		}

		MeshData Box(float width, float height, float depth)
		{
			const glm::vec3 size(width/2, height/2, depth/2);
			MeshData meshData{};
			meshData.vertices =
			{  
				// Back face
				glm::vec3(-size.x, -size.y, -size.z),
				glm::vec3( size.x, -size.y, -size.z),
				glm::vec3( size.x,  size.y, -size.z),
				glm::vec3(-size.x,  size.y, -size.z),

				// Front face
				glm::vec3(-size.x,  size.y, size.z),
				glm::vec3( size.x,  size.y, size.z),
				glm::vec3( size.x, -size.y, size.z),
				glm::vec3(-size.x, -size.y, size.z),

				// Right face
				glm::vec3(size.x, -size.y, -size.z),
				glm::vec3(size.x, -size.y,  size.z),
				glm::vec3(size.x,  size.y,  size.z),
				glm::vec3(size.x,  size.y, -size.z),

				// Left face
				glm::vec3(-size.x, -size.y, -size.z),
				glm::vec3(-size.x, -size.y,  size.z),
				glm::vec3(-size.x,  size.y,  size.z),
				glm::vec3(-size.x,  size.y, -size.z),

				// Top face
				glm::vec3(-size.x, size.y, -size.z),
				glm::vec3(-size.x, size.y,  size.z),
				glm::vec3( size.x, size.y,  size.z),
				glm::vec3( size.x, size.y, -size.z),

				// Bottom face
				glm::vec3(-size.x, -size.y, -size.z),
				glm::vec3(-size.x, -size.y,  size.z),
				glm::vec3( size.x, -size.y,  size.z),
				glm::vec3( size.x, -size.y, -size.z)
			};

			meshData.normals =
			{
				// Back face
				glm::vec3(0, 0, -1),
				glm::vec3(0, 0, -1),
				glm::vec3(0, 0, -1),
				glm::vec3(0, 0, -1),

				// Front face
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, 1),

				// Right face
				glm::vec3(1, 0, 0),
				glm::vec3(1, 0, 0),
				glm::vec3(1, 0, 0),
				glm::vec3(1, 0, 0),

				// Left face
				glm::vec3(-1, 0, 0),
				glm::vec3(-1, 0, 0),
				glm::vec3(-1, 0, 0),
				glm::vec3(-1, 0, 0),

				// Top face
				glm::vec3(0, 1, 0),
				glm::vec3(0, 1, 0),
				glm::vec3(0, 1, 0),
				glm::vec3(0, 1, 0),

				// Bottom face
				glm::vec3(0, -1, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, -1, 0),
			};

			meshData.indices =
			{
				// Top face
				0, 2, 1,
				0, 3, 2,

				// Bottom face
				4, 6, 5,
				4, 7, 6,

				// Right face
				8, 10, 9,
				8, 11, 10,

				// Left face
				12, 13, 14,
				12, 14, 15,

				// Top face
				16, 17, 18,
				16, 18, 19,

				// Bottom face
				20, 22, 21,
				20, 23, 22,
			};

			return meshData;
		}
	
		void subdivide(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices) 
		{
			std::map<std::pair<GLuint, GLuint>, GLuint> midPointIndexCache;
			auto getMidPointIndex = [&midPointIndexCache, &vertices](GLuint i1, GLuint i2) -> GLuint {
				auto key = std::minmax(i1, i2);
				if (midPointIndexCache.find(key) != midPointIndexCache.end()) {
					return midPointIndexCache[key];
				}
				glm::vec3 midPoint = glm::normalize(vertices[i1] + vertices[i2]) * 0.5f;
				vertices.push_back(midPoint);
				int newIndex = vertices.size() - 1;
				midPointIndexCache[key] = newIndex;
				return newIndex;
			};

			std::vector<GLuint> newIndices;
			for (int i = 0; i < indices.size(); i += 3) {
				GLuint v1 = indices[i];
				GLuint v2 = indices[i + 1];
				GLuint v3 = indices[i + 2];

				GLuint a = getMidPointIndex(v1, v2);
				GLuint b = getMidPointIndex(v2, v3);
				GLuint c = getMidPointIndex(v3, v1);

				newIndices.insert(newIndices.end(), { v1, a, c });
				newIndices.insert(newIndices.end(), { v2, b, a });
				newIndices.insert(newIndices.end(), { v3, c, b });
				newIndices.insert(newIndices.end(), { a, b, c });
			}
			indices = newIndices;
		}

		MeshData Sphere(int subdivisions, float radius)
		{
			const float phi = (1.0 + std::sqrt(5.0)) / 2.0; // Golden ratio

			std::vector<glm::vec3> vertices = 
			{
				{-1,  phi,  0}, { 1,  phi,  0}, {-1, -phi,  0}, { 1, -phi,  0},
				{ 0, -1,  phi}, { 0,  1,  phi}, { 0, -1, -phi}, { 0,  1, -phi},
				{ phi,  0, -1}, { phi,  0,  1}, {-phi,  0, -1}, {-phi,  0,  1}
			};

			std::vector<GLuint> indices = 
			{
				0, 11, 5,  0, 5, 1,  0, 1, 7,  0, 7, 10,  0, 10, 11,
				1, 5, 9,  5, 11, 4, 11, 10, 2, 10, 7, 6,  7, 1, 8,
				3, 9, 4,  3, 4, 2,  3, 2, 6,  3, 6, 8,  3, 8, 9,
				4, 9, 5,  2, 4, 11, 6, 2, 10, 8, 6, 7,  9, 8, 1
			};

			for (auto& vertex : vertices)
			{
				vertex = glm::normalize(vertex) * radius;
			}

			for (int i = 0; i < subdivisions; ++i)
			{
				subdivide(vertices, indices);
			}

			std::vector<glm::vec3> normals(vertices.size());
			
			for (int i = 0; i < vertices.size(); ++i) 
			{
				vertices[i] = glm::normalize(vertices[i]) * radius;
				normals[i] = vertices[i];
			}

			MeshData meshData;
			meshData.vertices = vertices;
			meshData.normals = normals;
			meshData.indices = indices;
			return meshData;
		}

		MeshData Cylinder(float radius, float height, int segments)
		{
			segments = glm::max(segments, 3);

			const float halfHeight = height / 2;

			std::vector<glm::vec3> vertices(segments * 4);
			std::vector<glm::vec3> normals(segments * 4);
			std::vector<GLuint> indices(segments * 6 + (segments - 2) * 6);

			for (int i = 0; i < segments; ++i)
			{
				float t = i / (float)segments;
				t *= glm::pi<float>() * 2;
				float x = glm::cos(t) * radius;
				float z = glm::sin(t) * radius;

				vertices[(segments * 0) + i]  = glm::vec3(x, halfHeight, z);
				vertices[(segments * 1) + i] = glm::vec3(x, -halfHeight, z);
				vertices[(segments * 2) + i] = glm::vec3(x, halfHeight, z);
				vertices[(segments * 3) + i] = glm::vec3(x, -halfHeight, z);

				normals[(segments * 0) + i] = glm::normalize(glm::vec3(x, 0, z));
				normals[(segments * 1) + i] = glm::normalize(glm::vec3(x, 0, z));
				normals[(segments * 2) + i] = glm::normalize(glm::vec3(0, 1, 0));
				normals[(segments * 3) + i] = glm::normalize(glm::vec3(0, 1, 0));
			}

			// Edge faces
			for (int i = 0; i < segments - 1; ++i)
			{
				indices[i * 6 + 0] = i;
				indices[i * 6 + 1] = i + 1;
				indices[i * 6 + 2] = i + segments;
				indices[i * 6 + 3] = i + segments + 1;
				indices[i * 6 + 4] = i + segments;
				indices[i * 6 + 5] = i + 1;
			}

			indices[(segments - 1) * 6 + 0] = (segments - 1);
			indices[(segments - 1) * 6 + 1] = 0;
			indices[(segments - 1) * 6 + 2] = (segments - 1 + segments);
			indices[(segments - 1) * 6 + 3] = segments;
			indices[(segments - 1) * 6 + 4] = (segments - 1 + segments);
			indices[(segments - 1) * 6 + 5] = 0;

			// Top/bottom faces
			int idx = (segments - 1) * 6 + 6;

			for (int i = 1; i < segments - 1; ++i)
			{
				indices[idx++] = (segments * 2) + 0;
				indices[idx++] = (segments * 2) + i + 1;
				indices[idx++] = (segments * 2) + i;

				indices[idx++] = segments * 3;
				indices[idx++] = segments * 3 + i;
				indices[idx++] = segments * 3 + i + 1;
			}

			MeshData meshData;
			meshData.vertices = vertices;
			meshData.normals = normals;
			meshData.indices = indices;
			return meshData;
		}

		MeshData Capsule(float radius, float height, int latitudeSegments, int longitudeSegments, int rings)
		{
			latitudeSegments = latitudeSegments % 2 != 0 ? latitudeSegments + 1 : latitudeSegments;

			const int halfLats = latitudeSegments / 2;

			// vertext index offsets
			int vOffsetNorthHemi = longitudeSegments;
			int vOffsetNorthEquator = vOffsetNorthHemi + (longitudeSegments + 1) * (halfLats - 1);
			int vOffsetCylinder = vOffsetNorthEquator + (longitudeSegments + 1);
			int vOffsetSouthEquator = (rings > 0) ?
				vOffsetCylinder + (longitudeSegments + 1) * rings :
				vOffsetCylinder;
			int vOffsetSouthHemi = vOffsetSouthEquator + (longitudeSegments + 1);
			int vOffsetSouthPolar = vOffsetSouthHemi + (longitudeSegments + 1) * (halfLats - 2);
			int vOffsetSouthCap = vOffsetSouthPolar + (longitudeSegments + 1);

			int vCount = vOffsetSouthCap + longitudeSegments;
			std::vector<glm::vec3> vertices(vCount);
			std::vector<glm::vec3> normals (vCount);

			const float toTheta = 2.f * glm::pi<float>() / longitudeSegments;
			const float toPhi = glm::pi<float>() / latitudeSegments;
			const float toTexHorizontal = 1.f / longitudeSegments;
			const float toTexVertical = 1.f / halfLats;

			const float vtAspectRatio = 1.f / 3.f;
			const float vtAspectNorth = 1.f - vtAspectRatio;
			const float vtAspectSouth = vtAspectRatio;

			std::vector<glm::vec2> thetaCartesian(longitudeSegments);
			std::vector<glm::vec2> rhoThetaCartesian(longitudeSegments);
			std::vector<float> sTexCache(longitudeSegments + 1);

			// Polar vertices
			for (int lon = 0; lon < longitudeSegments; ++lon)
			{
				float lf = lon;
				float sTexPolar = 1.f - ((lf + .5f) * toTexHorizontal);
				float cosTheta = glm::cos(lf * toTheta);
				float sinTheta = glm::sin(lf * toTheta);

				thetaCartesian[lon] = glm::vec2(cosTheta, sinTheta);
				rhoThetaCartesian[lon] = radius * glm::vec2(cosTheta, sinTheta);

				// North
				vertices[lon] = glm::vec3(0.f, height / 2.f + radius, 0.f);
				normals[lon]  = glm::vec3(0.f, 1.f, 0.f);

				// South
				int i = vOffsetSouthCap + lon;
				vertices[i] = glm::vec3(0.f, -(height / 2.f + radius), 0.f);
				normals[i]  = glm::vec3(0.f, -1.f, 0.f);
			}

			// Equatorial vertices
			for (int lon = 0; lon <= longitudeSegments; ++lon)
			{
				float sTex = 1.f - lon * toTexHorizontal;
				sTexCache[lon] = sTex;

				int ll = lon % longitudeSegments;
				glm::vec2 tc = thetaCartesian[ll];
				glm::vec2 rtc = rhoThetaCartesian[ll];

				// North equator
				int idxn = vOffsetNorthEquator + lon;
				vertices[idxn] = glm::vec3(rtc.x, height / 2.f, -rtc.y);
				normals[idxn]  = glm::vec3(tc.x, 0.f, -tc.y);

				// South equator
				int idxs = vOffsetSouthEquator + lon;
				vertices[idxs] = glm::vec3(rtc.x, -height / 2.f, -rtc.y);
				normals[idxs]  = glm::vec3(tc.x, 0.f, -tc.y);
			}

			// Hemisphere vertices
			for (int lat = 0; lat < halfLats - 1; ++lat)
			{
				const float phi = (lat + 1.f) * toPhi;
				const float cosPhiSouth = glm::cos(phi);
				const float sinPhiSouth = glm::sin(phi);
				const float cosPhiNorth = sinPhiSouth;
				const float sinPhiNorth = -cosPhiSouth;

				const float rhoCosPhiNorth = radius * cosPhiNorth;
				const float rhoSinPhiNorth = radius * sinPhiNorth;
				const float zOffsetNorth = height / 2.f - rhoSinPhiNorth;
				const float rhoCosPhiSouth = radius * cosPhiSouth;
				const float rhoSinPhiSouth = radius * sinPhiSouth;
				const float zOffsetSouth = -height / 2.f - rhoSinPhiSouth;

				const float tTexFac = (lat + 1.f) * toTexVertical;
				const float cmplTexFac = 1.f - tTexFac;
				const float tTexNorth = cmplTexFac + vtAspectNorth * tTexFac;
				const float tTexSouth = cmplTexFac * vtAspectSouth;

				const int vCurrentLatNorth = vOffsetNorthHemi + (lat * (longitudeSegments + 1));
				const int vCurrentLatSouth = vOffsetSouthHemi + (lat * (longitudeSegments + 1));

				for (int lon = 0; lon <= longitudeSegments; ++lon)
				{
					float sTex = sTexCache[lon];
					glm::vec2 tc = thetaCartesian[lon % longitudeSegments];

					// North hemisphere
					int idxn = vCurrentLatNorth + lon;
					vertices[idxn] = glm::vec3(
						rhoCosPhiNorth * tc.x,
						zOffsetNorth,
						-rhoCosPhiNorth * tc.y
					);
					normals[idxn] = glm::vec3(
						cosPhiNorth * tc.x,
						-sinPhiNorth,
						-cosPhiNorth * tc.y
					);

					// South hemisphere
					int idxs = vCurrentLatSouth + lon;
					vertices[idxs] = glm::vec3(
						rhoCosPhiSouth * tc.x,
						zOffsetSouth,
						-rhoCosPhiSouth * tc.y
					);
					normals[idxs] = glm::vec3(
						cosPhiSouth * tc.x,
						-sinPhiSouth,
						-cosPhiSouth * tc.y
					);
				}
			}

			// Cylinder vertices
			if (rings > 0)
			{
				float toFac = 1.f / (rings + 1);
				int index = vOffsetCylinder;

				for (int h = 1; h <= rings; ++h)
				{
					float fac = h * toFac;
					float cmplFac = 1.f - fac;
					float tTex = cmplFac * vtAspectNorth + fac * vtAspectSouth;
					float z = (height / 2.f) - height * fac;

					for (int lon = 0; lon <= longitudeSegments; ++lon)
					{
						glm::vec2 tc = thetaCartesian[lon % longitudeSegments];
						glm::vec2 rtc = rhoThetaCartesian[lon % longitudeSegments];
						float sTex = sTexCache[lon];

						vertices[index] = glm::vec3(rtc.x, z, -rtc.y);
						normals[index]  = glm::vec3(tc.x, 0.f, -tc.y);
						++index;
					}
				}
			}

			// Triangle indices
			const int long3 = longitudeSegments * 3;
			const int long6 = longitudeSegments * 6;
			const int hemiLong = (halfLats - 1) * long6;

			const int tOffsetNorthHemi = long3;
			const int tOffsetCylinder = tOffsetNorthHemi + hemiLong;
			const int tOffsetSouthHemi = tOffsetCylinder + (rings + 1) * long6;
			const int tOffsetSouthCap = tOffsetSouthHemi + hemiLong;

			int triCount = tOffsetSouthCap + long3;
			std::vector<GLuint> indices(triCount);

			// Polar caps
			for (int i = 0, k = 0, m = tOffsetSouthCap; i < longitudeSegments; ++i, k += 3, m += 3)
			{
				// North
				indices[k] = i;
				indices[k + 1] = vOffsetNorthHemi + i;
				indices[k + 2] = vOffsetNorthHemi + i + 1;
				// South
				indices[m] = vOffsetSouthCap + i;
				indices[m + 1] = vOffsetSouthPolar + i + 1;
				indices[m + 2] = vOffsetSouthPolar + i;
			}

			// Hemispheres
			for (int i = 0, k = tOffsetNorthHemi, m = tOffsetSouthHemi; i < (halfLats - 1); ++i)
			{
				const int vCurrentLatNorth = vOffsetNorthHemi + (i * (longitudeSegments + 1));
				const int vNextLatNorth = vCurrentLatNorth + (longitudeSegments + 1);
				const int vCurrentLatSouth = vOffsetSouthEquator + (i * (longitudeSegments + 1));
				const int vNextLatSouth = vCurrentLatSouth + (longitudeSegments + 1);

				for (int j = 0; j < longitudeSegments; ++j, k += 6, m += 6)
				{
					// North
					const int n00 = vCurrentLatNorth + j;
					const int n01 = vNextLatNorth + j;
					const int n11 = vNextLatNorth + j + 1;
					const int n10 = vCurrentLatNorth + j + 1;

					indices[k] = n00;
					indices[k + 1] = n11;
					indices[k + 2] = n10;

					indices[k + 3] = n00;
					indices[k + 4] = n01;
					indices[k + 5] = n11;

					// South
					const int s00 = vCurrentLatSouth + j;
					const int s01 = vNextLatSouth + j;
					const int s11 = vNextLatSouth + j + 1;
					const int s10 = vCurrentLatSouth + j + 1;

					indices[m] = s00;
					indices[m + 1] = s11;
					indices[m + 2] = s10;

					indices[m + 3] = s00;
					indices[m + 4] = s01;
					indices[m + 5] = s11;
				}
			}

			// Cylinder
			for (int i = 0, k = tOffsetCylinder; i <= rings; ++i)
			{
				const int vCurrentLat = vOffsetNorthEquator + i * (longitudeSegments + 1);
				const int vNextLat = vCurrentLat + (longitudeSegments + 1);

				for (int j = 0; j < longitudeSegments; ++j, k += 6)
				{
					int cy00 = vCurrentLat + j;
					int cy01 = vNextLat + j;
					int cy11 = vNextLat + j + 1;
					int cy10 = vCurrentLat + j + 1;

					indices[k] = cy00;
					indices[k + 1] = cy11;
					indices[k + 2] = cy10;
					indices[k + 3] = cy00;
					indices[k + 4] = cy01;
					indices[k + 5] = cy11;
				}
			}

			MeshData meshData;
			meshData.vertices = vertices;
			meshData.normals = normals;
			meshData.indices = indices;
			return meshData;
		}
	}
}