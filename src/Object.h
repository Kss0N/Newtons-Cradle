#pragma once

#include "framework.h"
#include "Mesh.h"
#include <string>

/*Simple wrapper for PhysX Objects*/
class Object
{
	Mesh mesh;
	physx::PxRigidDynamic* body;

	Object(std::string& meshPath);
	

};

