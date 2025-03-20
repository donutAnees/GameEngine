#include "geometry.h"

vec3 vec3_add(const vec3& v1, const vec3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vec3 vec3_sub(const vec3& v1, const vec3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

vec3 vec3_mul(const vec3& v, float k) {
	return { v.x * k, v.y * k, v.z * k };
}

vec3 vec3_div(const vec3& v, float k) {
	return { v.x / k, v.y / k, v.z / k };
}

float dot_product(const vec3& v1, const vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float vec3_length(const vec3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3_invert_vec3(const vec3& v) {
	return { -v.x, -v.y, -v.z };
}

void normalize(vec3& v) {
	float l = vec3_length(v);
	v.x /= l;
	v.y /= l;
	v.z /= l;
}

vec3 cross_product(const vec3& v1, const vec3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
	    v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

void Triangle::computeNormal() {
	vec3 line1 = { p[1].x - p[0].x, p[1].y - p[0].y, p[1].z - p[0].z };
	vec3 line2 = { p[2].x - p[0].x, p[2].y - p[0].y, p[2].z - p[0].z };
	normal = cross_product(line1, line2);
	// Normalizing the normal vector to unit length
	float l = vec3_length(normal);
	normalize(normal);
}

mat4x4 mat4x4::operator*(const mat4x4& rhs) const {
	mat4x4 res;
	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			res.m[c][r] = m[0][r] * rhs.m[c][0] + m[1][r] * rhs.m[c][1] + m[2][r] * rhs.m[c][2] + m[3][r] * rhs.m[c][3];
		}
	}
	return res;
}

mat4x4 mat4x4::operator=(const mat4x4& rhs) {
	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			m[c][r] = rhs.m[c][r];
		}
	}
	return *this;
}

void mat4x4::matrixMultiplyVector(vec3& input, vec3& output) const {
	output.x = input.x * m[0][0] + input.y * m[1][0] + input.z * m[2][0] + m[3][0];
	output.y = input.x * m[0][1] + input.y * m[1][1] + input.z * m[2][1] + m[3][1];
	output.z = input.x * m[0][2] + input.y * m[1][2] + input.z * m[2][2] + m[3][2];
	// Since this a 4d matrix and the 4th element represents the w coordinate, which is used to represent projective space
	// that is it represents the size of the coordinate (space) in 3D
	// W = 1, it does not change the size of x, y, z
	// W < 1, it shrinks the size of x, y, z
	// W > 1, it enlarges the size of x, y, z

	// For rotation and transformation we dont need w
	// But for translation we need w
	float w = input.x * m[0][3] + input.y * m[1][3] + input.z * m[2][3] + m[3][3]; // w is always 1
	if (w != 0.0f) {
		output.x /= w;
		output.y /= w;
		output.z /= w;
	}
}

void mat4x4::initProjectionMatrix(float fNear, float fFar, float fFov, float fAspectRatio) {
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

void mat4x4::initTranslationMatrix(float x, float y, float z) {
	/*	Translation matrix is used to move the object in 3D space */
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

void mat4x4::initViewMatrix(const vec3& pos, const vec3& forward, const vec3& up, const vec3& right) {
	/*	View matrix is used to transform objects in the world space to the camera's view space
	* For this we need to multiply a Rotation Matrix and a Translation Matrix
	*/

	// Rotation Matrix
	mat4x4 matRot;
	matRot.m[0][0] = right.x;
	matRot.m[0][1] = right.y;
	matRot.m[0][2] = right.z;
	matRot.m[1][0] = up.x;
	matRot.m[1][1] = up.y;
	matRot.m[1][2] = up.z;
	matRot.m[2][0] = -forward.x;
	matRot.m[2][1] = -forward.y;
	matRot.m[2][2] = -forward.z;
	matRot.m[3][3] = 1.0f;

	// Translation Matrix
	mat4x4 matTrans;
	matTrans.m[0][0] = 1.0f;
	matTrans.m[1][1] = 1.0f;
	matTrans.m[2][2] = 1.0f;
	matTrans.m[3][3] = 1.0f;
	matTrans.m[3][0] = -pos.x;
	matTrans.m[3][1] = -pos.y;
	matTrans.m[3][2] = -pos.z;

	// The view matrix is the product of the rotation matrix and the translation matrix
	*this = matRot * matTrans;
}

Cube::Cube(float x, float y, float z, float s) {
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

bool Mesh::loadFromObjectFile(const std::string& filename) {
	std::ifstream f(filename);
	if (!f.is_open())
		return false;

	std::vector<vec3> verts;

	std::string line;
	while (std::getline(f, line)) {
		std::istringstream s(line);

		char junk;
		if (line[0] == 'v') {
			vec3 v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}
		else if (line[0] == 'f') {
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			if (f[0] > 0 && f[1] > 0 && f[2] > 0 &&
				f[0] <= verts.size() && f[1] <= verts.size() && f[2] <= verts.size()) {
				triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
	}
	return true;
}
