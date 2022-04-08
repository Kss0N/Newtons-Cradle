#pragma once
#include "framework.h"
#include "math.h"


class Camera
{
	

public:
	PxVec3 position = physx::PxZero;

	PxVec3 orientation = physx::PxZero;
	PxVec3 up = physx::PxZero;

	PxMat44 cameraMatrix = physx::PxIdentity;


	void move(WPARAM input);
};
extern Camera camera;

