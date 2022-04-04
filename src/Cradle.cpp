#include "Cradle.h"
#include "CradleCradle.h"
#include "math.h"

#define _USE_MATH_DEFINES
#include <math.h>


std::string BALL_MESH_PATH      = "Sphere.obj";
std::string HANGER_MESH_PATH    = "Hanger.obj";

float hangerWorldDistance = 10;
Cradle cradle;




constexpr float volumeOfSphere(float radius) {
    return 4 / 3 * M_PI * radius * radius * radius;
}


/// @brief Creates a distance joint between the two actors A and B
/// @param distance is the distance between the orgins of the two actors
/// @param pActorA 
/// @param pActorB 
/// @return PxDistanceJoint reference
static physx::PxDistanceJoint* initDistanceJoint(float distance, physx::PxRigidActor* pActorA, physx::PxRigidActor* pActorB)
{
    auto pDistJoint = physx::PxDistanceJointCreate(*P::gpPhysics,
        //Actor A
        pActorA, physx::PxTransform(),
        pActorB, physx::PxTransform());
    pDistJoint->setMaxDistance(distance);
    pDistJoint->setMinDistance(distance);
    pDistJoint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eMAX_DISTANCE_ENABLED, true);
    pDistJoint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eMIN_DISTANCE_ENABLED, true);

    return pDistJoint;
}

void Cradle::addSegments(uint32_t nSegments)
{
    //Append the Segments to the last one
    for (uint32_t i = 0; i < nSegments; i++) {
        Segment s;



        //the position is the next one in X-direction
        PxVec3 position;
        if (segments.end() == segments.begin()) {
            position = PxVec3(0, 0, 0);
        }
        else{
            position = 
            (*(segments.end()-1)).pBall->getGlobalPose().p 
            +PxVec3(ballRadius,0,0);
        }
        s.pBall = P::gpPhysics->createRigidDynamic(
            physx::PxTransform(position));
        s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        s.pBall->attachShape(*pBallShape);
        s.pBall->setMass(density * volumeOfSphere(ballRadius));



        PxVec3 posA = s.pBall->getGlobalPose().p + 
            PxVec3(0, hangerHeight, hangerHeight * tanf(angleBetweenBallAndHanger));
        PxVec3 posB = s.pBall->getGlobalPose().p +
            PxVec3(0, hangerHeight, -hangerHeight * tanf(angleBetweenBallAndHanger));
        s.pHangerA = P::gpPhysics->createRigidStatic(physx::PxTransform(posA));
        s.pHangerB = P::gpPhysics->createRigidStatic(physx::PxTransform(posB));
        s.pHangerA->attachShape(*pHangerShape);
        s.pHangerB->attachShape(*pHangerShape);
        physx::PxActorFlags flags;
        flags.set(physx::PxActorFlag::Enum::eDISABLE_GRAVITY);
        flags.set(physx::PxActorFlag::Enum::eDISABLE_SIMULATION);
        s.pHangerA->setActorFlags(flags);
        s.pHangerB->setActorFlags(flags);



        auto length = sqrtf(
            pow(hangerHeight, 2) 
            + pow(hangerHeight*(double)tanf(angleBetweenBallAndHanger), 2));
        s.pJointA = initDistanceJoint(length, s.pBall, s.pHangerA);
        s.pJointB = initDistanceJoint(length, s.pBall, s.pHangerB);



        segments.push_back(s);
    }

    //Move the entire cradle so that the new center is located at the old center


}

Cradle::Cradle(uint32_t nBalls)
    : Cradle()
{
    this->init(nBalls);

}

Cradle::Cradle()
    : ballMesh(BALL_MESH_PATH), hangerMesh(HANGER_MESH_PATH)
{
}

void Cradle::init(uint32_t nBalls)
{
    pBallMaterial = P::gpPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
    


    physx::PxShapeFlags ballShapeFlags;
    auto ballGeometry = ballMesh.createPxGeometry(1);
    ballShapeFlags.set(physx::PxShapeFlag::Enum::eVISUALIZATION);
    pBallShape = P::gpPhysics->createShape(
        physx::PxSphereGeometry(1),
        *pBallMaterial, false, ballShapeFlags);


    auto pHangerMaterial = P::gpPhysics->createMaterial(0, 0, 0);
    physx::PxShapeFlags hangerShapeFlags;
    hangerShapeFlags.clear(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE);
    hangerShapeFlags.set(physx::PxShapeFlag::Enum::eVISUALIZATION);

    pHangerShape = P::gpPhysics->createShape(hangerMesh.createPxGeometry(1),
        *pHangerMaterial, false, hangerShapeFlags);
    pHangerMaterial->release();

    

    addSegments(nBalls);
}

void Cradle::simulate()
{

}

Cradle::worldPositions Cradle::getWorldPositions()
{
    worldPositions wrldPoses;


    return wrldPoses;
}

Cradle::~Cradle() {
    pBallShape->release();
    pBallMaterial->release();

    pHangerShape->release();

}