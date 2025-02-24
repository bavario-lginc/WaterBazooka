#include "CameraTargetDemoActor.h"

CameraTargetDemoActor::CameraTargetDemoActor (MtxPtr pMtx, const char *pName) : LiveActor(pName) {
    _90.set(pMtx);
}

void CameraTargetDemoActor::init (const JMapInfoIter &rIter) {
    LiveActor::init(rIter);
    makeActorAppeared();
}

void CameraTargetDemoActor::setTargetMtx (MtxPtr pNewTargetMtx) {
    _90.set(pNewTargetMtx);
    MR::makeRTFromMtxPtr(&mTranslation, &mRotation, pNewTargetMtx, true);
}

CameraTargetDemoActor::~CameraTargetDemoActor () {
    
}