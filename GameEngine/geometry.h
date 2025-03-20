#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <strstream>

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

vec3 vec3_add(const vec3& v1, const vec3& v2);
vec3 vec3_sub(const vec3& v1, const vec3& v2);
vec3 vec3_mul(const vec3& v, float k);
vec3 vec3_div(const vec3& v, float k);
float dot_product(const vec3& v1, const vec3& v2);
float vec3_length(const vec3& v);
vec3 vec3_invert_vec3(const vec3& v);
void normalize(vec3& v);
vec3 cross_product(const vec3& v1, const vec3& v2);


struct Triangle
{
	// Any shape can be represented by a triangle
	// A triangle is defined by 3 points
	vec3 p[3];
	// The normal of the triangle, doesnt matter which point of the triangle we choose, the normal will be the same
	vec3 normal;
	// Compute the normal of the triangle
	// The normal of a triangle is the cross product of two sides of the triangle
	// The cross product of two vectors gives a vector that is perpendicular to the plane formed by the two vectors
	void computeNormal();
};

struct Mesh
{
	// A mesh is a collection of triangles, which can be used to represent a 3D object
	std::vector<Triangle> triangles;
	bool loadFromObjectFile(const std::string& filename);
};

class mat4x4
{
public:
	float m[4][4] = { 0 };
	/*
	* The projection matrix is used to convert 3D coordinates into 2D coordinates, use this function to create
	* a projection matrix
	* @param fNear: The near clipping plane distance. This determines the closest distance from the camera where objects are rendered.
	*               Objects closer than this distance to the camera will not be rendered.
	* 
	* @param fFar: The far clipping plane distance. This determines the farthest distance from the camera where objects are rendered.
	*              Objects farther than this distance will not be rendered.
	* 
	* @param fFov: The field of view (in degrees) for the camera. This defines how wide the camera's viewing angle is. A larger field of view allows for a wider perspective, while a smaller field of view makes objects appear closer and more zoomed in.
	* 
	* @param fAspectRatio: The aspect ratio of the view (width divided by height). This ensures the projection matrix is correctly scaled for different screen sizes or viewport configurations.
	*/
	void initProjectionMatrix(float fNear, float fFar, float fFov, float fAspectRatio);
	
	/*
	* A translation matrix is used to move objects in 3D space. This function initializes a translation matrix with the specified translation values.
	*/
	void initTranslationMatrix(float x, float y, float z);

	/*
	* A view matrix is used to transform objects in the world space to the camera's view space. This function initializes a view matrix based on the camera's position, target, and up vector.
	* 
	* @param pos: The position of the camera in world space.
	* 
	* @param target: The target point the camera is looking at in world space.
	* 
	* @param up: The up vector of the camera.
	*/
	void initViewMatrix(const vec3& pos, const vec3& target, const vec3& up, const vec3& right);
	mat4x4 operator*(const mat4x4& rhs) const;
	mat4x4 operator=(const mat4x4& rhs);
	void matrixMultiplyVector(vec3& input, vec3& output) const;

};

class Cube : public Mesh
{
public:
	Cube(float x, float y, float z, float s);
};