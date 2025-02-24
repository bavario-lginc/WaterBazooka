#pragma once

#include "syati.h"

class WaterBazookaCapsule : public PartsModel {
public:
    WaterBazookaCapsule(LiveActor *pHost, const char *pName);
    ~WaterBazookaCapsule();
    virtual void init(const JMapInfoIter &rIter);
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver);

    void exeCrack();
    void exeBreak();

    void crackCapsule();
    void breakCapsule();
    bool isPlayerOnCapsule() const;
};

namespace NrvWaterBazookaCapsule {
    NERVE(WaterBazookaCapsuleNrvNormal);
    NERVE(WaterBazookaCapsuleNrvCrack);
    NERVE(WaterBazookaCapsuleNrvBreak);
}