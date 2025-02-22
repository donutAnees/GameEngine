#include <vector>

struct vec2
{
	float x;
	float y;
};

struct vec3
{
	float x;
	float y;
	float z;
};

struct triangle
{
	vec3 p[3];
};

struct mesh
{
	std::vector<triangle> triangles;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class cube : public mesh
{
public:
	cube(float x, float y, float z, float s)
	{
		// Create 6 faces of the cube
		// Front face
		triangles.push_back({ { {x, y, z}, {x + s, y, z}, {x, y + s, z} } });
		triangles.push_back({ { {x + s, y, z}, {x + s, y + s, z}, {x, y + s, z} } });
		// Back face
		triangles.push_back({ { {x, y, z + s}, {x + s, y, z + s}, {x, y + s, z + s} } });
		triangles.push_back({ { {x + s, y, z + s}, {x + s, y + s, z + s}, {x, y + s, z + s} } });
		// Top face
		triangles.push_back({ { {x, y, z}, {x + s, y, z}, {x, y, z + s} } });
		triangles.push_back({ { {x + s, y, z}, {x + s, y, z + s}, {x, y, z + s} } });
		// Bottom face
		triangles.push_back({ { {x, y + s, z}, {x + s, y + s, z}, {x, y + s, z + s} } });
		triangles.push_back({ { {x + s, y + s, z}, {x + s, y + s, z + s}, {x, y + s, z + s} } });
		// Left face
		triangles.push_back({ { {x, y, z}, {x, y, z + s}, {x, y + s, z} } });
		triangles.push_back({ { {x, y, z + s}, {x, y + s, z + s}, {x, y + s, z} } });
		// Right face
		triangles.push_back({ { {x + s, y, z}, {x + s, y, z + s}, {x + s, y + s, z} } });
		triangles.push_back({ { {x + s, y, z + s}, {x + s, y + s, z + s}, {x + s, y + s, z + s} } });
	}
};