#include "WaterBazookaCapsule.h"

WaterBazookaCapsule::WaterBazookaCapsule (LiveActor *pHost, const char *pName) : PartsModel(pHost, pName, "WaterBazookaCapsule", 0, 18, 0) {

}

void WaterBazookaCapsule::init (const JMapInfoIter &rIter) {
    initHitSensor(1);
    MR::addHitSensor(this, "body", ATYPE_WATER_BAZOOKA_CAPSULE, 4, 50.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::initCollisionParts(this, "WaterBazookaCapsule", getSensor(0), 0);
    initEffectKeeper(0, 0, false);
    initSound(2, 0, 0, TVec3f(0));
    MR::addToAttributeGroupSearchTurtle(this);
    MR::startBrk(this, "Damage");
    MR::setBrkFrameAndStop(this, 0.0f);
    initNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvNormal::sInstance, 0);
    appear();
}

void WaterBazookaCapsule::crackCapsule () {
    setNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvCrack::sInstance);
}

void WaterBazookaCapsule::breakCapsule () {
    setNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvBreak::sInstance);
}

bool WaterBazookaCapsule::isPlayerOnCapsule () const {
    return MR::isOnPlayer(getSensor(0));
}

void WaterBazookaCapsule::exeCrack () {
    if (MR::isFirstStep(this)) {
        MR::stopScene(12);
        MR::startBva(this, "Crack");
        MR::emitEffect(this, "Crack");
        MR::startSound(this, "SE_EM_WATERBAZ_CAPSULE_DAMAGE", -1, -1);
    }
}

void WaterBazookaCapsule::exeBreak () {
    if (MR::isFirstStep(this)) {
        MR::stopScene(12);
        MR::startBva(this, "Break");
        MR::emitEffect(this, "Break");
        MR::startSound(this, "SE_EM_WATERBAZ_CAPSULE_BREAK", -1, -1);
    }
}

bool WaterBazookaCapsule::receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (isNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvBreak::sInstance)) 
        return false;
    if (MR::isEffectValid(this, "Crack")) 
        return false;
    // Does this compile with a bctrl??
    return mHost->receiveMsgPlayerAttack(msg, pSender, pReceiver);
}

bool WaterBazookaCapsule::receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (isNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvBreak::sInstance)) 
        return false;
    if (MR::isEffectValid(this, "Crack")) 
        return false;
    // Does this compile with a bctrl??
    return mHost->receiveMsgEnemyAttack(msg, pSender, pReceiver);
}

bool WaterBazookaCapsule::receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (isNerve(&NrvWaterBazookaCapsule::WaterBazookaCapsuleNrvBreak::sInstance)) 
        return false;
    if (MR::isEffectValid(this, "Crack")) 
        return false;
    // Does this compile with a bctrl??
    return mHost->receiveMsgPlayerAttack(msg, pSender, pReceiver);
}

WaterBazookaCapsule::~WaterBazookaCapsule () {

}

namespace NrvWaterBazookaCapsule {
    void WaterBazookaCapsuleNrvBreak::execute (Spine *pSpine) const {
        WaterBazookaCapsule *pActor = (WaterBazookaCapsule *)pSpine->mExecutor;
        pActor->exeBreak();
    }
    void WaterBazookaCapsuleNrvCrack::execute (Spine *pSpine) const {
        WaterBazookaCapsule *pActor = (WaterBazookaCapsule *)pSpine->mExecutor;
        pActor->exeCrack();
    }
    void WaterBazookaCapsuleNrvNormal::execute (Spine *pSpine) const {
        WaterBazookaCapsule *pActor = (WaterBazookaCapsule *)pSpine->mExecutor;
        if (MR::isFirstStep(pActor)) 
            MR::startBva(pActor, "Normal");
    }

    WaterBazookaCapsuleNrvNormal(WaterBazookaCapsuleNrvNormal::sInstance);
    WaterBazookaCapsuleNrvCrack(WaterBazookaCapsuleNrvCrack::sInstance);
    WaterBazookaCapsuleNrvBreak(WaterBazookaCapsuleNrvBreak::sInstance);
}