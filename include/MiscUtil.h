#pragma once

#include "syati.h"

class AudBgmConductorExt : public AudBgmConductor {
public:
    // matching (different IDs to Galaxy 1)
    void playPinchEventBgmLevel (bool v0) {
        if (MR::isPlayingSubBgmID(0x100000B) || MR::isPlayingSubBgmID(0x100000C) || MR::isPlayingSubBgmID(0x100000F) || MR::isPlayingSubBgmID(0x1000010) || MR::isPlayingSubBgmID(0x100000D) || MR::isPlayingSubBgmID(0x100000E)) {
            _20 = -1;
            _24 = -1;
            return;
        }
        if (!MR::isPlayerDead() && !MR::isPowerStarGetDemoActive()) {
            if (_20 < 0) {
                if (v0) 
                    MR::startSubBGM("BGM_PINCH_2", false);
                else 
                    MR::startSubBGM("BGM_PINCH_1", false);
                if (v0) {
                    MR::moveVolumeSubBGM(0.0f, 0);
                    _24 = 2;
                } 
                _28 = 0;
            }
            _20 = 3;
        }
    }
};

namespace MR {
    // needs to be added to a header
    u32 setupAlreadyDoneFlag(int *, const char *, const JMapInfoIter &, s32);
    void updateAlreadyDoneFlag(int);

    
    // inlined funcs
    template <class T>
    inline JointControlDelegator<T>* createJointDelegatorWithNullChildFunc(T *pHost, bool (T::*calcFunc)(TPos3f *, const JointControllerInfo &), const char *pName) {
        JointControlDelegator<T>* delegator = new JointControlDelegator<T>();
        delegator->mObjPtr = pHost;
        delegator->mCalcJointMtxFunc = calcFunc;
        delegator->mCalcJointMtxAfterChildFunc = 0;
        setJointControllerParam(delegator, pHost, pName);
        return delegator;
    }
    
    template <class T>
    inline JointControlDelegator<T>* createJointDelegatorWithNullMtxFunc(T *pHost, bool (T::*calcFunc)(TPos3f *, const JointControllerInfo &), const char *pName) {
        JointControlDelegator<T>* delegator = new JointControlDelegator<T>(calcFunc, pHost, 0);
        setJointControllerParam(delegator, pHost, pName);
        return delegator;
    }
}