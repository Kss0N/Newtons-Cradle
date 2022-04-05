#include "FilterShader.h"

using namespace physx;




/*
TODO: Actually implement my own Filter shader instead of ripping off the tutorial

*/
PxFilterFlags filterShader(
    PxFilterObjectAttributes attribs0, PxFilterData filterData0,
    PxFilterObjectAttributes attribs1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* cBlock, PxU32 cBlockSize)
{

    // let triggers through
    if (PxFilterObjectIsTrigger(attribs0) || PxFilterObjectIsTrigger(attribs1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    // trigger the contact callback for pairs (A,B) where
    // the filtermask of A contains the ID of B and vice versa.
    if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

    return PxFilterFlag::eDEFAULT;
}