#include "Cradle.h"
#include "CradleCradle.h"
#include "math.h"

#include "timeCounter.h"
#include "logger.h"



std::string BALL_MESH_PATH      = "Sphere.obj";
std::string HANGER_MESH_PATH    = "Hanger.obj";

Cradle cradle;

#define PI 3.1415926535897932


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

        //s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
        s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, true);
        s.pBall->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);

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



    physx::PxShapeFlags ballShapeFlags = physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
    //auto ballGeometry = ballMesh.createPxGeometry(1);
    pBallShape = P::gpPhysics->createShape(
        physx::PxSphereGeometry(ballRadius),
        *pBallMaterial, false, ballShapeFlags);
    segments.clear(); 


    addSegments(nBalls);
}

static volatile bool pleaseTerminate = false;
#define X86_PAGE_SIZE 4096 
struct simulateInput_t
{
    std::vector<Segment> segments;
};
DWORD WINAPI Cradle::simulationProcedure(LPVOID input)
{
    std::ofstream log("Debug.txt", std::ios::app);
    SYSTEMTIME st;
    GetSystemTime(&st);
    log << '[' << st.wHour << '|' << st.wMinute << ']' << "\r\n";


    auto segments = ((simulateInput_t*)input)->segments;
    unsigned __int64 milliseconds = 0;
    while (!cradle.isResting() && !pleaseTerminate)
    {
        if (milliseconds % 100 == 0) {
            log << "time = " << milliseconds << ":\r\n";
        }
        //Apply drag and log
        for (Segment& s : segments)
        {
            auto velocity = s.pBall->getLinearVelocity();
            //Drag force direction is opposite of the velocity's
            PxVec3 force = -1 * getDragMagnitude(s) * velocity.getNormalized();
            s.pBall->addForce(force);
            
            if (milliseconds % 100 == 0) {
                log << s.pBall->getGlobalPose().p.y << "\r\n";
            }
        }
        if (milliseconds % 100 == 0) log << "\r\n\r\n\r\n";
        tc.updateTime(milliseconds++);

        P::gpScene->simulate(.001f);
        P::gpScene->fetchResults(true);

        RECT mainR;
        GetClientRect(ghMainWindow, &mainR);
        InvalidateRect(ghMainWindow, &mainR, TRUE);

    }
    logger::addLogEntry(static_cast<double>(milliseconds));

    log.close();
    delete(input);
    return 0;
}



Cradle::milliseconds Cradle::simulate()
{
    //MessageBeep(MB_COMPOSITE);
    auto oldPos = segments.cbegin()->pBall->getGlobalPose().p;
    segments.cbegin()->pBall->setGlobalPose(physx::PxTransform(
        PxVec3(oldPos.x - jointHeight,
            oldPos.y + jointHeight,
            0)));
    wakeUp();
    pleaseTerminate = false;
    std::ofstream log("Debug.txt", std::ios::app);

    unsigned __int64 milliseconds = 0;
    while (!cradle.isResting() && !pleaseTerminate)
    {
        //Apply drag and log
        if (milliseconds % 1000 == 0) log << "time = " << milliseconds << "\r\n";
        for (Segment& s : segments)
        {
            auto velocity = s.pBall->getLinearVelocity();
            //Drag force direction is opposite of the velocity's
            PxVec3 force = -1 * getDragMagnitude(s) * velocity.getNormalized();
            s.pBall->addForce(force);
            if (milliseconds % 1000 == 0) log << s.pBall->getGlobalPose().p.y << "\r\n";
        }
        if (milliseconds % 1000 == 0) log << "\r\n\r\n";
        milliseconds++;
        P::gpScene->simulate(.001f);
        P::gpScene->fetchResults(true);

    }
    return milliseconds;
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
    for (auto& segment : segments)
        disposeOfSegment(segment);
        segments.pop_back();
    pBallShape->release();
    pBallMaterial->release();
}

void Cradle::adjustHeightOfJoints()
{
    for (auto& s : segments)
    {
        s.adjustJointHeight(jointHeight, angleBetweenBallAndJoint);
    }
}

void Cradle::stop()
{
    pleaseTerminate = true;
}



void Cradle::updateMaterial()
{
    size_t nSegments = segments.size();
    dispose();
    init(nSegments);
}

void Cradle::wakeUp()
{
    for (auto& s : segments) {
        s.pBall->wakeUp();
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
        //to not go on forever, minimal speed is rounded down to .1mm/s
        if (velocity.magnitude() >= 0.005 or (abs(pos.y) >= 0.1)) 
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
