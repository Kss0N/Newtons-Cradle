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



/*
Aggregates
An aggregate is a collection of actors. 
Aggregates do not provide extra simulation or query features, but allow you to tell the SDK that a set of actors will be clustered together, which in turn allows the SDK to optimize its spatial data operations. 
A typical use case is a ragdoll, made of multiple different actors. Without aggregates, this gives rise to as many broad-phase entries as there are shapes in the ragdoll. 
It is typically more efficient to represent the ragdoll in the broad-phase as a single entity, and perform internal overlap tests in a second pass if necessary. 
Another potential use case is a single actor with a large number of attached shapes.
*/