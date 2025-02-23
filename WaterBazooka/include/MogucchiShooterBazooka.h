#pragma once

#include "syati.h"
#include "WaterBazooka.h"

class MogucchiShooterBazooka : public PartsModel {
public:
    MogucchiShooterBazooka(LiveActor *pHost, const char *pName);
    ~MogucchiShooterBazooka();
    virtual void init(const JMapInfoIter &rIter);
    virtual void calcAndSetBaseMtx();

    void panicDeath();
    void explosion();
    void resetDirection();
    void hitShock();
    void anger();
    void stormStart();
    void storm();
    void faceToMario();
    bool isLaughed() const;

    void exeWait();
    void exeShot();
    void exeTire();
    void exeDeathPanic();
    void exeExplosion();
    void exeLaugh();
    void exeShock();
    void exePanic();
    void exeHitShock();
    void exeHitPanic();
    void exeAnger();
    void exeStorm();

    TVec3f _A0;
};

namespace NrvMogucchiShooterBazooka {
    NERVE(MogucchiShooterNrvWait);
    NERVE(MogucchiShooterNrvShot);
    NERVE(MogucchiShooterNrvTire);
    NERVE(MogucchiShooterNrvDeathPanic);
    NERVE(MogucchiShooterNrvExplosion);
    NERVE(MogucchiShooterNrvLaugh);
    NERVE(MogucchiShooterNrvShock);
    NERVE(MogucchiShooterNrvPanic);
    NERVE(MogucchiShooterNrvHitShock);
    NERVE(MogucchiShooterNrvHitPanic);
    NERVE(MogucchiShooterNrvAnger);
    NERVE(MogucchiShooterNrvStormStart);
    NERVE(MogucchiShooterNrvStorm);
}