#include "Cradle.h"
#include "CradleCradle.h"
#include "math.h"

#include "timeCounter.h"
#include "logger.h"



std::string BALL_MESH_PATH      = "Sphere.obj";
std::string HANGER_MESH_PATH    = "Hanger.obj";

Cradle cradle;



constexpr float volumeOfSphere(float radius) {
    return 4 / 3 * M_PI * radius * radius * radius;
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
            +PxVec3(2*ballRadius,0,0);
        }
        s.pBall = P::gpPhysics->createRigidDynamic(
            physx::PxTransform(position));
        s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        s.pBall->attachShape(*pBallShape);
        s.pBall->setMass(density * volumeOfSphere(ballRadius));
        P::gpScene->addActor(*s.pBall);

        s.adjustJointHeight(jointHeight, angleBetweenBallAndJoint);

        segments.push_back(s);
    }

    //Move the entire cradle so that the new center is located at the old center
    adjustEntirePosition();

    InvalidateRect(ghMainWindow, NULL, TRUE); //invalidate to force redraw
    UpdateWindow(ghMainWindow);
}

#define PI 3.1415926535897932

Cradle::Cradle(uint32_t nBalls)
    : Cradle()
{
    this->init(nBalls);

}

Cradle::Cradle()
    : ballMesh(BALL_MESH_PATH)
{
}

void Cradle::init(uint32_t nBalls)
{
    pBallMaterial = P::gpPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
    cradle.reset();


    physx::PxShapeFlags ballShapeFlags;
    //auto ballGeometry = ballMesh.createPxGeometry(1);
    ballShapeFlags.set(physx::PxShapeFlag::Enum::eVISUALIZATION);
    pBallShape = P::gpPhysics->createShape(
        physx::PxSphereGeometry(1),
        *pBallMaterial, false, ballShapeFlags);



    addSegments(nBalls);
}

#define X86_PAGE_SIZE 4096 
struct simulateInput_t
{
    std::vector<Segment> segments;
};
DWORD WINAPI Cradle::simulationProcedure(LPVOID input)
{
    auto segments = ((simulateInput_t*)input)->segments;
    unsigned __int64 milliseconds = 0;
    while (!cradle.isResting())
    {
        //Apply drag
        for (Segment& s : segments)
        {
            auto velocity = s.pBall->getLinearVelocity();
            //Drag force direction is opposite of the velocity's
            PxVec3 force = -1 * getDragMagnitude(s) * velocity.getNormalized();
            s.pBall->addForce(force);
        }
        tc.updateTime(milliseconds++);

        P::gpScene->simulate(.001f);
        P::gpScene->fetchResults(true);

        RECT mainR;
        GetClientRect(ghMainWindow, &mainR);
        InvalidateRect(ghMainWindow, &mainR, TRUE);

    }
    logger::addLogEntry(static_cast<double>(milliseconds));


    return 0;
}

void Cradle::simulate()
{
    if (logger::logIsOpen()) logger::newLog();
    /*Move 0th Ball to vertical alignment*/
    auto oldPos = segments.cbegin()->pBall->getGlobalPose().p;
    segments.cbegin()->pBall->setGlobalPose(physx::PxTransform(
        PxVec3( oldPos.x - jointHeight, 
                oldPos.y + jointHeight, 
                0)));
    
    //Wake em up
    for (auto& s : segments) s.pBall->wakeUp();

    simulateInput_t si = {
    };
    DWORD threadID;
    hSimulationThread = CreateThread(NULL, 8 * 8 * X86_PAGE_SIZE, 
        simulationProcedure, static_cast<LPVOID>(&si), 0, &threadID);

    

}


static void disposeOfSegment(Segment& s)
{
    s.pJointA->release();
    s.pJointB->release();
    s.pBall->release();
}




void Cradle::removeSegments(uint32_t nSegments)
{
    for (int i = 0; i < nSegments && getSize() != 2; i++) 
    {
        auto& segment = *(segments.end() - 1);
        disposeOfSegment(segment);
        segments.pop_back();
    }
    adjustEntirePosition();
}





#define RESET_VAR(v) v = CradleDefVals::v

void Cradle::adjustEntirePosition()
{

}

void Cradle::reset() {
    RESET_VAR(ballRadius);
    RESET_VAR(density);
    RESET_VAR(staticFriction);
    RESET_VAR(dynamicFriction);
    RESET_VAR(restitution);

    RESET_VAR(angleBetweenBallAndJoint);
    RESET_VAR(jointHeight);

    
    

}



void Cradle::dispose() {
    pBallShape->release();
    pBallMaterial->release();

    logger::closeLog();

    for (auto& segment : segments)
        disposeOfSegment(segment);
}

void Cradle::adjustHeightOfJoints()
{
    for (auto& s : segments)
    {
        s.adjustJointHeight(jointHeight, angleBetweenBallAndJoint);
    }
}

double Cradle::getDragMagnitude(Segment& s)
{
    return 0.5 * densityOfAir * 
        s.pBall->getLinearVelocity().magnitudeSquared() * 
        dragCoefficient * 
        cradle.ballRadius * cradle.ballRadius* PI;
}



bool Cradle::isResting()
{
    for (Segment& s : segments) 
    {
        auto velocity = s.pBall->getLinearVelocity();
        auto pos = s.pBall->getGlobalPose().p;
        if (velocity.magnitudeSquared() != 0 or (pos.y != 0)) 
            return false;
    }
    return true;
}

std::vector<PxVec3> Cradle::getPositions()
{
    std::vector<PxVec3> v;
    for (auto& s : segments)
        v.push_back(s.pBall->getGlobalPose().p);
    return v;
}

Cradle::~Cradle() 
{

}









static void applyJointFlags(physx::PxDistanceJoint* pJoint, float length)
{
    using namespace physx;
    PxDistanceJointFlags jointFlags
    (PxDistanceJointFlag::Enum::eMAX_DISTANCE_ENABLED | PxDistanceJointFlag::Enum::eMIN_DISTANCE_ENABLED);
    pJoint->setDistanceJointFlags(jointFlags);
    pJoint->setMaxDistance(length);
    pJoint->setMinDistance(length);
}

void Segment::adjustJointHeight(float newHeight, float newAngle)
{
    if (pJointA) pJointA->release();
    if (pJointB) pJointB->release();
    auto length = newHeight / cosf(newAngle);
    auto ballPos = pBall->getGlobalPose().p;

    auto jointAPos = ballPos + PxVec3(0, newHeight, newHeight * tanf(newAngle));
    pJointA = physx::PxDistanceJointCreate(*P::gpPhysics,
        //A is at the ball.
        pBall, physx::PxTransform(PxVec3(0)),
        //B is a global position
        NULL, physx::PxTransform(jointAPos)
    );
    pJointB = physx::PxDistanceJointCreate(*P::gpPhysics,
        //A is at the ball.
        pBall, physx::PxTransform(PxVec3(0)),
        //B is a global position, it's Z-composant is the negative of the A Joint's 
        NULL, physx::PxTransform(ballPos + PxVec3(0, newHeight, -1  * newHeight * tanf(newAngle)))
    );
    applyJointFlags(pJointA, length);
    applyJointFlags(pJointB, length);
}
