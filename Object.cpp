#include "Object.h"
#include <string>
#include "CradleCradle.h"

Object::Object(std::string& meshPath)
	: mesh(meshPath)
{
	std::string path = meshPath;
	mesh = path;

	//body = P::gpPhysics->createRigidDynamic();

}