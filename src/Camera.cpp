#include "Camera.h"
#include "CradleCradle.h"

Camera camera;

constexpr auto speed = 0.1f;


static PxMat44 perspective(float FOV, float AspectRatio, float dNear, float dFar)
{

	//distance to Projection Plane
	float dPP = 1 / tanf(FOV / 2);

	float divisor = 1 / (dNear - dFar);

	float a = -divisor * (dNear + dFar);
	float b = divisor * (2 * dFar * dNear);

	//Construct using columns:
	return PxMat44(	PxVec4(1 / AspectRatio * dPP, 0, 0, 0),
					PxVec4(0, dPP, 0, 0),
					PxVec4(0, 0, a, 1),
					PxVec4(0, 0, b, 0));
	/*
	1 / AspectRatio * dPP,	0.0f,	0.0f,	0.0f,
	0.0f,					dPP,	0.0f,	0.0f,
	0.0f,					0.0f,	a,		b,
	0.0f,					0.0f,	1.0f,	0.0f);
	*/
}


static PxMat44 lookAt(const PxVec3& eye, const PxVec3& center, const PxVec3& up)
{

	PxVec3 X, Y, Z;

	Z = (eye - center).getNormalized();
	X = (up.cross(Z)).getNormalized(); // X = Y x Z
	Y = (Z.cross(X)).getNormalized();  // Y = Z x X

	/*
	X[0],	X[1],	X[2], -dot(X, eye),
	Y[0],	Y[1],	Y[2], -dot(Y, eye),
	Z[0],	Z[1],	Z[2], -dot(Z, eye),
	0,		0,		0,		1
	*/
	
	return PxMat44(PxVec4(X.x, Y.x, Z.x, 0), 
		PxVec4(X.y, Y.y, Z.y, 0), 
		PxVec4(X.z, Y.z, Z.z, 0),
		PxVec4(-(X.dot(eye)), -(Y.dot(eye)), -(Z.dot(eye)), 1));
}

constexpr double toRadians(double x) {
	return x * 3.1415926535 / 180;
}


void Camera::move(WPARAM input)
{
	switch (input)
	{
	case 'W':
	{
		position += speed * orientation;
		break;
	}
	case 'A':
	{
		position += speed * - (orientation.cross(up)).getNormalized();
		break;
	}
	case 'S':
	{
		position += speed * -orientation;
		break;
	}
	case 'D':
	{
		position += speed * (orientation.cross(up)).getNormalized();
		break;
	}
	case VK_SPACE:
	{
		position += speed * up;
	}
	case VK_LSHIFT:
	{
		position += speed * -up;
	}
	default:
		break;
	}
	
	RECT mR;//main window client rectangle
	GetClientRect(ghMainWindow, &mR);

	auto projection = perspective(toRadians(70), (mR.right - mR.left) / (mR.bottom - mR.top), 0.1f, 100.0f);
	auto view = lookAt(position, position + orientation, up);

	cameraMatrix = projection * view;
}

// Matrix = perspective * lookat



/*





float FOV = 70.0f;


void Camera::Matrix(float degFOV, float aspectRatio, Shader& shader, std::string& uniform)
{
	auto camera = Camera::getCamera();

	//I REALLY REALLY REALLY HOPE THERE ARE NO BUGS IN HERE
	
}
*/