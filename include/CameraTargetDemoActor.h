#pragma once

#include "Game/LiveActor.h"

class CameraTargetDemoActor : public LiveActor {
public:
    CameraTargetDemoActor(MtxPtr pMtx, const char *pName);
    ~CameraTargetDemoActor();
    virtual void init(const JMapInfoIter &rIter);
    void setTargetMtx(MtxPtr pNewTargetMtx);

    TSMtxf _90;
};

namespace MR {
    void makeRTFromMtxPtr(TVec3f *, TVec3f *, MtxPtr, bool);
}