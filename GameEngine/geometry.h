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

struct Triangle
{
	// Any shape can be represented by a triangle
	// A triangle is defined by 3 points
	vec3 p[3];
};

struct Mesh
{
	// A mesh is a collection of triangles, which can be used to represent a 3D object
	std::vector<Triangle> triangles;
};

class mat4x4
{
private:
	float m[4][4] = { 0 };
public:
	void matrixMultiplyVector(vec3& input, vec3& output) const {
		output.x = input.x * m[0][0] + input.y * m[1][0] + input.z * m[2][0] + m[3][0];
		output.y = input.x * m[0][1] + input.y * m[1][1] + input.z * m[2][1] + m[3][1];
		output.z = input.x * m[0][2] + input.y * m[1][2] + input.z * m[2][2] + m[3][2];
		float w = input.x * m[0][3] + input.y * m[1][3] + input.z * m[2][3] + m[3][3];
		if (w != 0.0f) {
			output.x /= w;
			output.y /= w;
			output.z /= w;
		}
	}

	void initProjectionMatrix(float fNear, float fFar, float fFov, float fAspectRatio)
	{
		// The projection matrix is used to convert 3D coordinates into 2D coordinates

		/*	We have to bring all points to the range[-1, 1]
		* Screen space x = x * fAspectRatio, where fAspectRatio = width / height
		* The x coordinate is scaled by the aspect ratio
		* The reason for normalizing the x coordinate is to make the x coordinate independent of the aspect ratio
		* 
		* Next thing to do here is account for the field of view(FOV)
		* The field of view(FOV) is the angle of the camera’s vision
		* 
		* If the FOV is wider, then we see more of the scene/object and therefore the object appears smaller
		* If the FOV is narrower, then we see less of the scene/object and therefore the object appears larger
		* 
		* tan(FOV / 2) gives you the ratio of the opposite side (the half-height of the view frustum at a certain depth) 
		* to the adjacent side (the distance from the camera).
		* Now if we consider tan(FOV / 2) as the scaling factor, then objects farther will be bigger only
		* But we want the inverse of this, so we take the reciprocal of this (1/tan(FOV / 2))
		*
		* Next thing to account is z coordinate, that is the depth of the object from the camera
		* Screen space x = fAspectRatio * (x / z) * tan(fov / 2)
		* Screen space y = (y / z) * tan(fov / 2)
		* This ensures that objects farther from the camera appear smaller.
		* 
		* Finally for 
		* Screen space z = z * (fFar / (fFar - fNear))  - (fNear * fFar / (fFar - fNear))
		* Basically, normalizing the z coordinate to be in the range[-1, 1], this is not used
		* for scaling the object to the screen but rather for visibility, that is pixels closer to the camera
		* are drawn over pixels farther from the camera
		* 
		* (fFar / (fFar - fNear)) gives you the scaling factor for the z coordinate
		* (fNear * fFar / (fFar - fNear)) is the offset to make the z coordinate negative or positive
		* 
		* Our transform is [screen space x, screen space y, screen space z]
		* 
		* We can use matrix multiplication
		* The projection matrix is a 4x4 matrix
		* 
		* [[ fAspectRatio * (1 / tan(fov / 2)), 0, 0, 0 ],
		* [[ 0, 1 / tan(fov / 2), 0, 0 ],
		* [[ 0, 0, fFar / (fFar - fNear), 1 ],
		* [[ 0, 0, -fNear * fFar / (fFar - fNear), 0 ]]
		* 
		* 
		*/
			
		float tanHalfFov = tanf(fFov / 2.0f);
		m[0][0] = fAspectRatio / tanHalfFov;
		m[1][1] = 1.0f / tanHalfFov;
		m[2][2] = fFar / (fFar - fNear);
		m[3][2] = (-fNear * fFar) / (fFar - fNear);
		m[2][3] = 1.0f;
		}
};


class Cube : public Mesh
{
public:
	Cube(float x, float y, float z, float s)
	{
		// SOUTH face
		triangles.push_back({
			{ {x, y, z}, {x, y + s, z}, {x + s, y + s, z} }
			});
		triangles.push_back({
			{ {x, y, z}, {x + s, y + s, z}, {x + s, y, z} }
			});

		// EAST face
		triangles.push_back({
			{ {x + s, y, z}, {x + s, y + s, z}, {x + s, y + s, z + s} }
			});
		triangles.push_back({
			{ {x + s, y, z}, {x + s, y + s, z + s}, {x + s, y, z + s} }
			});

		// NORTH face
		triangles.push_back({
			{ {x + s, y, z + s}, {x + s, y + s, z + s}, {x, y + s, z + s} }
			});
		triangles.push_back({
			{ {x + s, y, z + s}, {x, y + s, z + s}, {x, y, z + s} }
			});

		// WEST face
		triangles.push_back({
			{ {x, y, z}, {x, y + s, z}, {x, y + s, z + s} }
			});
		triangles.push_back({
			{ {x, y, z}, {x, y + s, z + s}, {x, y, z + s} }
			});

		// TOP face
		triangles.push_back({
			{ {x, y + s, z}, {x, y + s, z + s}, {x + s, y + s, z + s} }
			});
		triangles.push_back({
			{ {x, y + s, z}, {x + s, y + s, z + s}, {x + s, y + s, z} }
			});

		// BOTTOM face
		triangles.push_back({
			{ {x, y, z}, {x + s, y, z}, {x + s, y, z + s} }
			});
		triangles.push_back({
			{ {x, y, z}, {x + s, y, z + s}, {x, y, z + s} }
			});
	}
};