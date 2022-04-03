#pragma once

#include "resource.h"
#include "framework.h"

/// @brief current instance
extern HINSTANCE ghInstance;
/// @brief Main Window
extern HWND ghMainWindow;
/// @brief Currently Active accelerator. Windows that use accelerators should update this value once they get focus.
extern HACCEL ghActiveAccelTable;


//PhysX constants
namespace P {
	/// @brief Foundation object, containing the low level implementations
	extern physx::PxFoundation* gpFoundation;
	/// @brief PhysX main object for everything in the SDK
	extern physx::PxPhysics* gpPhysics;
	/// @brief Library reference that creates custom meshes
	extern physx::PxCooking* gpCooking;
	/// @brief the Scene
	extern physx::PxScene* gpScene;
	/// @brief Object that makes sure PhysX uses the same units of measurement.
	extern physx::PxTolerancesScale gToleranceScale;
}


