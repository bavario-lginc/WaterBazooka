#include "MogucchiShooterBazooka.h"

// obj_arg???
f32 sExplosionBlurOffset = 100.0f;

MogucchiShooterBazooka::MogucchiShooterBazooka (LiveActor *pHost, const char *pName) : PartsModel(pHost, pName, "MogucchiShooter", 0, 18, false) {
    
}

void MogucchiShooterBazooka::init (const JMapInfoIter &rIter) {
    initEffectKeeper(1, 0, false);
    MR::addEffectHitNormal(this, 0);
    initSound(1, 0, 0, TVec3f(0));
    MR::calcFrontVec(&_A0, this);
    MR::createCenterScreenBlur();
    MR::startBrk(this, "Normal");
    initNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance, 0);
    appear();
}

void MogucchiShooterBazooka::panicDeath () {
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvDeathPanic::sInstance);
}

void MogucchiShooterBazooka::explosion () {
    resetDirection();
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvExplosion::sInstance);
}

void MogucchiShooterBazooka::hitShock () {
    resetDirection();
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvHitShock::sInstance);
}

void MogucchiShooterBazooka::anger () {
    resetDirection();
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvAnger::sInstance);
}

void MogucchiShooterBazooka::stormStart () {
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvStormStart::sInstance);
}

void MogucchiShooterBazooka::storm () {
    setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvStorm::sInstance);
}

bool MogucchiShooterBazooka::isLaughed () const {
    return isNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvLaugh::sInstance);
}

void MogucchiShooterBazooka::exeWait () {
    WaterBazooka *pHost = (WaterBazooka *)mHost;
    if (MR::isFirstStep(this)) {
        if (pHost->isBazookaPinch()) {
            MR::startAction(this, "WaitPinch");
            return;
        } else {
            MR::startAction(this, "Wait");
            return;
        }
    }
    faceToMario();
    if (MR::isPlayerInRush()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvLaugh::sInstance);
    else if (pHost->isPanic()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvShock::sInstance);
    else if (pHost->isTired()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvTire::sInstance);
    else if (pHost->isFirstShoot()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvShot::sInstance);
}

void MogucchiShooterBazooka::exeShot () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Shot");
    else if (MR::isPlayerInRush()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvLaugh::sInstance);
    else if (MR::isActionEnd(this)) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeTire () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Tire");
    WaterBazooka *pHost = (WaterBazooka *)mHost;
    if (pHost->isPanic()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvShock::sInstance);
    else if (pHost->isTired()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeDeathPanic () {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Panic");
        MR::startBrk(this, "CoolDown");
    }
}

void MogucchiShooterBazooka::exeExplosion () {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Down");
        MR::startCenterScreenBlur(50, sExplosionBlurOffset, 80, 5, 30);
    }
    if (MR::isActionEnd(this)) 
        kill();
}

void MogucchiShooterBazooka::exeLaugh () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Laugh");
    if (MR::isStep(this, 120)) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeShock () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Shock");
    if (MR::isStep(this, 120)) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvPanic::sInstance);
}

void MogucchiShooterBazooka::exePanic () {
    WaterBazooka *pHost = (WaterBazooka *)mHost;
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Panic");
        if (pHost->isBazookaLifeOut()) 
            MR::setBckRate(this, 2.0f);
    }
    MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_PANIC", -1, -1, -1);
    faceToMario();
    if (!pHost->isPanic() && MR::isOnGroundPlayer() && !pHost->isBazookaLifeOut()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeHitShock () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Shock");
    faceToMario();
    if (MR::isActionEnd(this)) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvHitPanic::sInstance);
}

void MogucchiShooterBazooka::exeHitPanic () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Panic");
    MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_PANIC", -1, -1, -1);
    if (MR::isDemoActive()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeAnger () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "Angry");
    if (MR::isStep(this, 40)) 
        MR::startCenterScreenBlur(60, sExplosionBlurOffset, 80, 5, 30);
    faceToMario();
    if (MR::isDemoActive()) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::exeStorm () {
    if (MR::isFirstStep(this)) 
        MR::startAction(this, "SpinAttack");
    faceToMario();
    if (MR::isActionEnd(this)) 
        setNerve(&NrvMogucchiShooterBazooka::MogucchiShooterNrvWait::sInstance);
}

void MogucchiShooterBazooka::calcAndSetBaseMtx () {
    if (!mCalcOwnMatrix) 
        PartsModel::calcAndSetBaseMtx();
    else {
        TPos3f v0;
        v0.identity();
        TPos3f v1;
        JMath::gekko_ps_copy12(&v1, &mFixedPos->_1C);
        TVec3f v2;
        v2.set(v1.mMtx[0][1], v1.mMtx[1][1], v1.mMtx[2][1]);
        mTranslation.set(mFixedPos->_1C.mMtx[0][3], mFixedPos->_1C.mMtx[1][3], mFixedPos->_1C.mMtx[2][3]);
        MR::makeMtxUpFrontPos(&v0, v2, _A0, mTranslation);
        MR::setBaseTRMtx(this, v0);
    }
}

void MogucchiShooterBazooka::resetDirection () {
    TPos3f v0;
    JMath::gekko_ps_copy12(&v0, MR::getJointMtx(mHost, "Cockpit"));
    v0.getZDir(_A0);
}

void MogucchiShooterBazooka::faceToMario () {
    if (!MR::isPlayerHidden() && !MR::isStageStateScenarioOpeningCamera() && !MR::isDemoActive()) {
        TPos3f v0;
        JMath::gekko_ps_copy12(&v0, &mFixedPos->_1C);
        MR::turnDirectionToTargetDegree(this, &_A0, *MR::getPlayerPos(), 1.0f);
        TVec3f v1;
        v0.getZDir(v1);
        MR::clampVecAngleDeg(&_A0, v1, 30.0f);
    }
}

MogucchiShooterBazooka::~MogucchiShooterBazooka () {

}

namespace NrvMogucchiShooterBazooka {
    void MogucchiShooterNrvStorm::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeStorm();
    }
    void MogucchiShooterNrvStormStart::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        if (MR::isFirstStep(pActor)) 
            MR::startAction(pActor, "SpinAttackStart");
        pActor->faceToMario();
    }
    void MogucchiShooterNrvAnger::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeAnger();
    }
    void MogucchiShooterNrvHitPanic::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeHitPanic();
    }
    void MogucchiShooterNrvHitShock::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeHitShock();
    }
    void MogucchiShooterNrvPanic::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exePanic();
    }
    void MogucchiShooterNrvShock::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeShock();
    }
    void MogucchiShooterNrvLaugh::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeLaugh();
    }
    void MogucchiShooterNrvExplosion::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeExplosion();
    }
    void MogucchiShooterNrvDeathPanic::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeDeathPanic();
    }
    void MogucchiShooterNrvTire::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeTire();
    }
    void MogucchiShooterNrvShot::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeShot();
    }
    void MogucchiShooterNrvWait::execute(Spine *pSpine) const {
        MogucchiShooterBazooka *pActor = (MogucchiShooterBazooka *)pSpine->mExecutor;
        pActor->exeWait();
    }

    MogucchiShooterNrvWait(MogucchiShooterNrvWait::sInstance);
    MogucchiShooterNrvShot(MogucchiShooterNrvShot::sInstance);
    MogucchiShooterNrvTire(MogucchiShooterNrvTire::sInstance);
    MogucchiShooterNrvDeathPanic(MogucchiShooterNrvDeathPanic::sInstance);
    MogucchiShooterNrvExplosion(MogucchiShooterNrvExplosion::sInstance);
    MogucchiShooterNrvLaugh(MogucchiShooterNrvLaugh::sInstance);
    MogucchiShooterNrvShock(MogucchiShooterNrvShock::sInstance);
    MogucchiShooterNrvPanic(MogucchiShooterNrvPanic::sInstance);
    MogucchiShooterNrvHitShock(MogucchiShooterNrvHitShock::sInstance);
    MogucchiShooterNrvHitPanic(MogucchiShooterNrvHitPanic::sInstance);
    MogucchiShooterNrvAnger(MogucchiShooterNrvAnger::sInstance);
    MogucchiShooterNrvStormStart(MogucchiShooterNrvStormStart::sInstance);
    MogucchiShooterNrvStorm(MogucchiShooterNrvStorm::sInstance);
}