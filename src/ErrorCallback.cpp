#include "ErrorCallback.h"


#include <iostream>

void ErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) 
{
	std::cout << "PhysX Error " << code << " " << message << std::endl;
}