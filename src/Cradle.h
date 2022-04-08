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
#include <thread>
#define _USE_MATH_DEFINES
#include <math.h>

//For a ball with iron as material, 10 balls
namespace CradleDefVals {
	constexpr float ballRadius = 1;
	constexpr float density = 8.050;

	constexpr float staticFriction = 0.7;
	constexpr float dynamicFriction = 0.6;
	constexpr float restitution = 1;

	//in radians
	constexpr float angleBetweenBallAndJoint = M_PI/4;
	constexpr float jointHeight = 10;
	constexpr int nBalls = 10;
}

/// @brief Segment sub class. All Segments in the cradle are identical so no constructor is needed.
struct Segment {
	physx::PxRigidDynamic* pBall = NULL;

	physx::PxDistanceJoint* pJointA = NULL;

	physx::PxDistanceJoint* pJointB = NULL;

	/// @brief Creates new joint Objects with new global frames
	/// @param newHeight is the distance on the Y-axis above the ball, where the joints reside
	/// @param newAngle is the angle between the heightLine and the jointLine 
	void adjustJointHeight(float newHeight, float newAngle);
};

class Cradle
{
	HANDLE hSimulationThread = NULL;
	physx::PxShape* pBallShape;
	physx::PxMaterial* pBallMaterial;

	std::vector<Segment> segments;
	
	void adjustEntirePosition();

	static DWORD WINAPI simulationProcedure(LPVOID);

	PxVec3 centerPos;


	/// @brief returns the drag force calculated by the 
	/// @param speed velocity magnitude
	/// @return force magnitude. To apply the force, the force should have opposite direction to the current resulting force
	static double getDragMagnitude(Segment& s);

	/// @brief determines if the cradles  mechanical energy is 0
	/// @return true if the mechanical energy of the cradle is 0
	bool isResting();

public:
	constexpr static double densityOfAir = 1.2041;
	constexpr static double dragCoefficient = 0.47835;
	Mesh ballMesh;

	float ballRadius;
	float density;

	float staticFriction;
	float dynamicFriction;
	float restitution;

	//in radians
	float angleBetweenBallAndJoint;
	float jointHeight;



	Cradle(uint32_t nBalls);

	Cradle();

	~Cradle();

	/// @brief 
	/// @return 
	void init(uint32_t nBalls = 10);

	/// @brief 
	/// @param  
	void addSegments(uint32_t = 1);

	/// @brief 
	void removeSegments(uint32_t = 1);

	/// @brief 
	inline size_t getSize() { return segments.size(); }

	/// @brief sets all member fields to default values (of a ball of steel)
	void reset();

	/// @brief 
	void simulate();
	
	/// @brief 
	void dispose();


	void adjustHeightOfJoints();

	void stop();

	void updateMaterial();

	void wakeUp();

	/// @brief Returns worldposition of all balls. because they all use the same 
	/// Mesh, the position to offset is only needed.
	std::vector<PxVec3> getPositions();
};

extern Cradle cradle;