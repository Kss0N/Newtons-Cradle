/**
@file cpuDispatcher.cpp
@author  J.Keson <>
@version 1.0

@section LICENSE
This program is free software; you can redistribute itand /or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU
General Public License for more details at
https ://www.gnu.org/copyleft/gpl.html

@section DESCRIPTION
	Implements the SceneDesc CPU dispatcher Interface for task management
*/


#include <physX/PxPhysicsAPI.h>

class CpuDispatcher : public physx::PxCpuDispatcher
{
public:
	void submitTask(physx::PxBaseTask& task);

	uint32_t getWorkerCount() const;

	//static void init();
	CpuDispatcher();
	~CpuDispatcher();
};

