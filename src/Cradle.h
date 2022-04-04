/** 
* @file Cradle.h
* @author J. Keson <jakob.kristersson@bredband.net>
* @version 1.0
* 
* @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 * 
 * @section DESCRIPTION
 * 
 * This is the actual implementation of the Cradle that the project is based on.
 * 
 * The Cradle has N ammound of spheres which are PxRigidDynamics
 * They are all connected in joints to PxRigidStatics which makes the circular path of the 
 * spheres of constant radius.
*/
#pragma once

#include <vector>
#include "framework.h"
#include "Mesh.h"


/// @brief Segment sub class. All Segments in the cradle are identical so no constructor is needed.
struct Segment {
	physx::PxRigidDynamic* pBall;

	physx::PxRigidStatic* pHangerA;
	physx::PxDistanceJoint* pJointA;

	physx::PxRigidStatic* pHangerB;
	physx::PxDistanceJoint* pJointB;
};

class Cradle
{
	Mesh ballMesh;
	physx::PxShape* pBallShape;
	physx::PxMaterial* pBallMaterial;
	
	/*Hangers:
	Hangers don't serve a role in the simulation. They are thus Rigid static,
	The are massless, not affected by gravity or anything else physics related
	*/
	Mesh hangerMesh;
	physx::PxShape* pHangerShape;


	float ballRadius = 1;
	float ropeLength = 1;
	float density = 1;

	float staticFriction = 1;
	float dynamicFriction = 1;
	float restitution = 1;

	//in radians
	float angleBetweenBallAndHanger = 0;
	float hangerHeight = 10;

	std::vector<Segment> segments;
	


public:
	PxVec3 centerPos;

	Cradle(uint32_t nBalls);

	Cradle();

	~Cradle();

	void init(uint32_t nBalls = 10);

	void addSegments(uint32_t = 1);
	inline void addSegment() { addSegments(1); }


	void update();

	void simulate();
	

	struct worldPositions {
		std::vector<physx::PxVec3> balls;
		std::vector<physx::PxVec3> hangersA;
		std::vector<physx::PxVec3> hangersB;
	};

	/// @brief gets the Positions of all Cradle components
	/// @return 
	worldPositions getWorldPositions();
};

extern Cradle cradle;