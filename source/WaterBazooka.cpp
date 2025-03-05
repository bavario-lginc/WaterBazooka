#include "WaterBazooka.h"

// sprintf???
const char *sDropPointStringTable[] = {
    "落下点1",
    "落下点2",
    "落下点3",
    "落下点4"
};
// obj_arg???
f32 sElectricBulletSpeed = 12.0f;

namespace MR {
    // not matching
    HitSensor *addMessageSensorEnemy(LiveActor *pActor, const char *pSensorName) {
        return pActor->mSensorKeeper->add(pSensorName, ATYPE_ENEMY, 0, 0.0f, pActor, TVec3f(0.0f, 0.0f, 0.0f));
    }
    // no symbol, matching
    void playLevelMarioPinchBGM (bool v0) {
        if (MR::isPlayerDead()) 
            return;
        AudBgmConductorExt *pConductor = (AudBgmConductorExt *)MR::getSceneObjHolder()->getObj(SCENE_OBJ_AUD_BGM_CONDUCTOR);
        if (!pConductor) 
            return;
        pConductor->playPinchEventBgmLevel(v0);
    }
}

// Matching
WaterBazooka::WaterBazooka (const char *pName) : LiveActor(pName) {
    _90 = 0;
    _94 = 0;
    mBreakModel = 0;
    _9C = 0;
    mCameraInfo = 0;
    _A4 = -1;
    _A8 = 3;
    _AC = 0;
    mCollision = 0;
    mJointControlDelegator = 0;
    _E8 = false;
    mSpawnPowerStar = false;
    mIsElectricBazooka = false;
    mElectricBullets = 0;
    _F0 = -1;
    _B8.identity();
}

extern u32 __ptmf_null;

// different function args and removed hardcoded check, otherwise matching
void WaterBazooka::init (const JMapInfoIter &rIter) {
    MR::initDefaultPos(this, rIter);
    const char *pObjectName;
    MR::getObjectName(&pObjectName, rIter);
    mIsElectricBazooka = MR::isEqualString("ElectricBazooka", pObjectName);
    
    initModelManagerWithAnm(pObjectName, 0, 0, false);
    MR::connectToSceneMapObjStrongLight(this);
    initHitSensor(2);
    MR::addBodyMessageSensorMapObj(this);
    MR::addMessageSensorEnemy(this, "cannon");
    MR::initCollisionParts(this, "Cannon1", getSensor("cannon"), MR::getJointMtx(this, "Cannon1"));
    mCollision = MR::createCollisionPartsFromLiveActor(this, "AllRoot", getSensor("Body"), MR::CollisionScaleType_Default);
    MR::validateCollisionParts(mCollision);

    initEffectKeeper(1, 0, false);
    MR::addEffectHitNormal(this, 0);
    initSound(4, 0, 0, TVec3f(0));
    MR::invalidateClipping(this);
    MR::initActorCamera(this, rIter, &mCameraInfo);
    JointControlDelegator<WaterBazooka> *v0 = MR::createJointDelegatorWithNullChildFunc(this, &WaterBazooka::calcJointCannon, "Cannon1");
    mJointControlDelegator = v0;
    MR::initJointTransform(this);
    JMath::gekko_ps_copy12(_B8.mMtx, MR::getJointMtx(this, "Cannon1"));
    initShooter();
    initBazookaCapsule();
    initBreakModel();
    initCameraTarget();
    initBullet(rIter);

    MR::getJMapInfoArg0NoInit(rIter, &mSpawnPowerStar);
    if (mSpawnPowerStar) 
        MR::declarePowerStar(this);
    if (MR::tryRegisterDemoCast(this, rIter)) {
        MR::tryRegisterDemoCast(_90, rIter);
        MR::tryRegisterDemoCast(_94, rIter);
        MR::tryRegisterDemoCast(_9C, rIter);
        MR::tryRegisterDemoCast(mBreakModel, rIter);
    }
    MR::startBrk(this, "Damage");
    MR::setBrkFrameAndStop(this, 0.0f);
    MR::declareStarPiece(this, 24);
    MR::addToAttributeGroupSearchTurtle(this);
    MR::useStageSwitchReadA(this, rIter);
    MR::useStageSwitchWriteDead(this, rIter);
    initNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance, 0);
    //if (MR::isEqualStageName("OceanPhantomCaveGalaxy")) {
        int v1 = 0;
        _A4 = MR::setupAlreadyDoneFlag(&v1, "SwingStop", rIter, 0);
        if (v1) {
            if (MR::isValidSwitchDead(this)) 
                MR::onSwitchDead(this);
            makeActorDead();
        } else 
            makeActorAppeared();
    //}
}

// Matching
bool WaterBazooka::receiveMsgPlayerAttack (u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (MR::isMsgStarPieceReflect(msg)) 
        return true;
    if (pReceiver == getSensor("Body")) 
        return false;
    if (MR::isDemoActive()) 
        return false;
    if (isNrvDemo()) 
        return false;
    if (MR::isMsgPlayerHipDropFloor(msg) && pReceiver->mSensorType == ATYPE_WATER_BAZOOKA_CAPSULE) {
        if (isElectricLeak()) {
            if (!MR::isPlayerDamaging() || MR::isPlayerParalyzing()) 
                return false;
        }
        damageBazooka(pSender, pReceiver);
        return true;
    } else {
        if (MR::isMsgJetTurtleAttack(msg)) {
            damageBazooka(pSender, pReceiver);
            return true;
        }
    }
    return false;
}

// Matching
bool WaterBazooka::receiveMsgEnemyAttack (u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (pReceiver == getSensor("Body")) 
        return false;
    if (MR::isDemoActive()) 
        return false;
    if (isNrvDemo()) 
        return false;
    if (pSender->mSensorType == ATYPE_COCO_NUT) {
        damageBazooka(pSender, pReceiver);
        return true;
    }
    return false;
}

// Functionally matching
bool WaterBazooka::receiveOtherMsg (u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (!MR::isSensorPlayer(pSender)) 
        return false;
    if (pReceiver == getSensor("Body")) 
        return false;
    if (MR::isDemoActive()) 
        return false;
    if (isNrvDemo()) 
        return false;
    if (!MR::isMsgFloorTouch(msg) && !MR::isMsgWallTouch(msg)) 
        return false;
    if (isElectricLeak() && !MR::isPlayerHipDropFalling() && !MR::isPlayerHipDropLand()) {
        MR::sendMsgEnemyAttackElectric(pSender, pReceiver);
        return true;
    } else if (isNerve(&NrvWaterBazooka::WaterBazookaNrvStorm::sInstance)) {
        MR::sendMsgEnemyAttackFlip(pSender, pReceiver);
        return true;
    }
    return false;
}

// Matching
bool WaterBazooka::isFirstShoot () const {
    if (!MR::isFirstStep(this)) 
        return false;
    return isNerve(&NrvWaterBazooka::WaterBazookaNrvShot::sInstance);
}

// Matching
bool WaterBazooka::isTired () const {
    return isNerve(&NrvWaterBazooka::WaterBazookaNrvTire::sInstance);
}

// CW is too smart to match, functionally matching
bool WaterBazooka::isPanic () const {
    bool returnVal = false;
    if (!isNerve(&NrvWaterBazooka::WaterBazookaNrvPanic::sInstance) && !isNerve(&NrvWaterBazooka::WaterBazookaNrvStorm::sInstance)) 
        return returnVal;
    returnVal = true;
    return returnVal;
}

// Matching
bool WaterBazooka::isBazookaPinch () const {
    return _A8 <= 1;
}

// Not matching but should be the same thing
bool WaterBazooka::isBazookaLifeOut () const {
    return _A8 == 0;
}

// Functionally matching
void WaterBazooka::exeWaitForBattle () {
    if (MR::isFirstStep(this)) 
        MR::tryStartBck(this, "Wait", 0);
    JMath::gekko_ps_copy12(_B8.mMtx, MR::getJointMtx(this, "Cannon1"));
    bool v0 = false;
    if (MR::isValidSwitchA(this) && !MR::isOnSwitchA(this)) {
        if (MR::isStageStateScenarioOpeningCamera()) 
            v0 = true;
    } else 
        v0 = true;
    if (v0) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
}

// Matching
void WaterBazooka::exeWait () {
    if (MR::isFirstStep(this) && _A8 <= 1 && !MR::isBrkPlaying(this, "PowerUp")) 
        MR::startBrk(this, "PowerUp");
    if (_90->isLaughed()) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWaitForLaugh::sInstance);
    else {
        if (MR::isGreaterStep(this, 0) && !MR::isPlayerHidden()) 
            setNerve(&NrvWaterBazooka::WaterBazookaNrvAim::sInstance);
        else 
            tryWaitForBattle();
    }
}

// Matching
void WaterBazooka::exeAim () {
    if (MR::isFirstStep(this)) 
        MR::tryStartBck(this, "ShotStart", 0);
    MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_TURN", -1, -1, -1);
    if (mIsElectricBazooka && MR::isGreaterEqualStep(this, 10)) 
        MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_PRE_SHOOT", -1, -1, -1);
    if (aimAtMario()) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvAimEnd::sInstance);
    else if (MR::isGreaterEqualStep(this, 60)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvAimEnd::sInstance);
}

// Matching
void WaterBazooka::exeAimEnd () {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "SwingStop", 0);
        MR::startSound(this, "SE_EM_WATERBAZ_TURN_END", -1, -1);
    }
    if (mIsElectricBazooka) 
        MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_PRE_SHOOT", -1, -1, -1);
    if (MR::isBckStopped(this) && _A8 <= 1 && !mIsElectricBazooka) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvShotNoMotion::sInstance);
    else 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvShot::sInstance);
}

// Matching
void WaterBazooka::exeShot () {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Shot", 0);
        _AC++;
    }
    if (mIsElectricBazooka) 
        MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_PRE_SHOOT", -1, -1, -1);
    if (MR::isStep(this, 30)) {
        MR::startSound(this, "SE_EM_WATERBAZ_SHOT", -1, -1);
        tryShotBullet();
    }
    if (MR::isBckStopped(this)) {
        if (_AC >= 3) {
            _AC = 0;
            setNerve(&NrvWaterBazooka::WaterBazookaNrvTire::sInstance);
        } else {
            if (_A8 <= 1 && !mIsElectricBazooka) 
                setNerve(&NrvWaterBazooka::WaterBazookaNrvShotNoMotion::sInstance);
            else 
                setNerve(&NrvWaterBazooka::WaterBazookaNrvWaitForLaugh::sInstance);
        }
    }
}

// Matching
void WaterBazooka::exeShotNoMotion () {
    if (MR::isFirstStep(this)) {
        _AC++;
        MR::startBck(this, "ShortShot", 0);
        MR::startSound(this, "SE_EM_WATERBAZ_SHOT", -1, -1);
        tryShotBullet();
    }
    aimAtMario();
    if (_90->isLaughed()) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWaitForLaugh::sInstance);
    else if (MR::isStep(this, 30)) {
        if (_AC >= 5) {
            _AC = 0;
            setNerve(&NrvWaterBazooka::WaterBazookaNrvTire::sInstance);
        } else 
            setNerve(&NrvWaterBazooka::WaterBazookaNrvShotNoMotion::sInstance);
    }
}

// Matching
void WaterBazooka::exeTire () {
    if (MR::isFirstStep(this)) 
        MR::startBck(this, "Tire", 0);
    if (_90->isLaughed()) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWaitForLaugh::sInstance);
    else if (MR::isStep(this, 120)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
}

// stackswap, otherwise matching
void WaterBazooka::exeDemoCrackCapsule () {
    if (MR::isFirstStep(this)) {
        tryJumpBackPlayerFromBazooka();
        MR::startBck(this, "1stDamage", 0);
        _90->hitShock();
    }
    if (MR::isStep(this, 27)) 
        MR::startSound(this, "SE_EV_MOGUBAZ_ANGRY1", -1, -1);
    if (!MR::isDemoActiveRegistered(this)) {
        TVec3f v0;
        MR::calcUpVec(&v0, this);
        TVec3f v1;
        JMAVECScaleAdd(v0, mTranslation, v1, 600.0f);
        MR::appearStarPiece(this, v1, 8, 25.0f, 40.0f, false);
        MR::startSound(this, "SE_OJ_STAR_PIECE_BURST", -1, -1);
        MR::validateCollisionParts(_94);
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
    }
}

// stackswap, otherwise matching
void WaterBazooka::exeDemoAnger () {
    if (MR::isFirstStep(this)) {
        tryJumpBackPlayerFromBazooka();
        MR::startBck(this, "2ndDamage", 0);
        _90->anger();
        MR::startBrk(this, "PowerUp");
        MR::startBrk(_94, "PowerUp");
    }
    if (MR::isStep(this, 20)) 
        MR::startSound(this, "SE_EV_MOGUBAZ_ANGRY2", -1, -1);
    if (mIsElectricBazooka) {
        if (MR::isFirstStep(this)) 
            MR::emitEffect(this, "AngrySign");
        else if (MR::isStep(this, 60)) 
            MR::deleteEffect(this, "AngrySign");
        else if (MR::isStep(this, 90)) 
            MR::emitEffect(this, "Angry");

        if (MR::isLessStep(this, 60)) 
            MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_PRE_ELEC_LEAK", -1, -1, -1);
        else if (MR::isGreaterEqualStep(this, 90)) 
            MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_ELEC_LEAK", -1, -1, -1);
    }
    if (!MR::isDemoActiveRegistered(this)) {
        if (mIsElectricBazooka) {
            MR::deleteEffect(this, "Angry");
            _F0 = 0;
        }
        TVec3f v0;
        MR::calcUpVec(&v0, this);
        TVec3f v1;
        JMAVECScaleAdd(v0, mTranslation, v1, 600.0f);
        MR::appearStarPiece(this, v1, 16, 25.0f, 40.0f, false);
        MR::startSound(this, "SE_OJ_STAR_PIECE_BURST", -1, -1);
        MR::validateCollisionParts(_94);
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
    }
}

// Matching
void WaterBazooka::exeDemoBreakWait () {
    if (MR::isFirstStep(this)) {
        MR::offUpdateCollisionParts(this);
        if (MR::isPlayerInRush()) 
            MR::endBindAndPlayerJumpWithRollLanding(this, *MR::getPlayerVelocity(), 0);
    }
    if (MR::isStep(this, 5)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakSign::sInstance);
}

// Matching
void WaterBazooka::exeDemoBreakSign () {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Down", 0);
        tryJumpBackPlayerFromBazooka();
        if (mIsElectricBazooka) {
            MR::deleteEffect(this, "Angry");
            MR::deleteEffect(this, "AngrySign");
        }
    }
    MR::tryRumblePadWeak(this, 0);
    MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_PRE_EXPLODE", -1, -1, -1);
    if (MR::isGreaterEqualStep(this, 120)) 
        MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_BREAK_FLASH", -1, -1, -1);
    if (MR::isBckStopped(this)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakExplosion::sInstance);
}

// Matching
void WaterBazooka::exeDemoBreakExplosion () {
    if (MR::isFirstStep(this)) {
        MR::hideModel(this);
        MR::invalidateCollisionParts(this);
        MR::invalidateCollisionParts(mCollision);
        MR::invalidateHitSensors(this);
        MR::deleteEffect(this, "DamageSmokeS1");
        MR::deleteEffect(this, "DamageSmokeS2");
        MR::deleteEffect(this, "DamageSmokeL1");
        MR::deleteEffect(this, "DamageSmokeL2");
        _90->mCheckHiddenHost = true;
        _90->explosion();
        _94->kill();
        mBreakModel->makeActorAppeared();
        MR::invalidateClipping(mBreakModel);
        MR::startBck(mBreakModel, "Break", 0);
        MR::startBrk(mBreakModel, "Break");
        MR::tryRumblePadStrong(this, 0);
        MR::shakeCameraNormal();
        MR::startSound(this, "SE_EM_WATERBAZ_EXPLODE", -1, -1);
        MR::startSound(this, "SE_EV_MOGUBAZ_DEAD", -1, -1);
    }
    if (MR::isStep(this, 53)) {
        if (mSpawnPowerStar) {
            TVec3f v0;
            MR::copyJointPos(this, "Cockpit", &v0);
            MR::startAfterBossRushBGM(); // Same function as SMG1???
            MR::requestAppearPowerStar(this, v0);
        } else 
            MR::startSystemSE("SE_SY_READ_RIDDLE_S", -1, -1);        
    }
    if (MR::isBckStopped(mBreakModel)) {
        mBreakModel->kill();
        setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakEnd::sInstance);
    }
}

// Matching
void WaterBazooka::exeDemoBreakEnd () {
    if (MR::isStep(this, 60)) {
        if (MR::isValidSwitchDead(this)) 
            MR::onSwitchDead(this);
        kill();
    }
}

// Matching
void WaterBazooka::exeWaitForLaugh () {
    if (_90->isLaughed()) 
        _AC = 0;
    else 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
}

// Matching
void WaterBazooka::exePanic () {
    if (MR::isFirstStep(this)) {
        MR::tryStartBck(this, "Wait", 0);
        _AC = 0;
    }
    aimAtMario();
    bool v0 = false;
    if (MR::isOnPlayer(getSensor("cannon")) || _94->isPlayerOnCapsule()) 
        v0 = true;
    if (!v0 && MR::isOnGroundPlayer()) {
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
        return;
    }
    if (MR::isStep(this, 180)) {
        _90->stormStart();
        MR::startBrk(this, "Spin");
        MR::startBrk(_94, "Spin");
        MR::startSound(this, "SE_EV_MOGUBAZ_PRE_STORM", -1, -1);
    }
    if (MR::isGreaterEqualStep(this, 180)) 
        MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_PRE_STORM", -1, -1, -1);
    if (MR::isStep(this, 300)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvStorm::sInstance);
}

// Matching
void WaterBazooka::exeStorm () {
    if (MR::isFirstStep(this)) {
        _90->storm();
        MR::emitEffect(this, "Spin");
    }
    TVec3f v0;
    _B8.getTrans(v0);
    TVec3f v1;
    _B8.getXDir(v1);
    f32 v2 = MR::calcNerveEaseOutValue(this, 150, 25.0f, 0.0f);
    MR::rotateVecDegree(&v1, mGravity, v2);
    TVec3f v3;
    JGeometry::negateInternal(mGravity, v3);
    MR::makeMtxSideUpPos(&_B8, v1, v3, v0);
    MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_STORM", 100.0f * v2 / 25.0f, -1, -1);
    if (MR::isStep(this, 90)) 
        MR::deleteEffect(this, "Spin");
    if (MR::isStep(this, 150)) 
        setNerve(&NrvWaterBazooka::WaterBazookaNrvWait::sInstance);
}

// Matching
void WaterBazooka::makeActorDead () {
    _90->makeActorDead();
    _94->makeActorDead();
    mBreakModel->makeActorDead();
    _9C->makeActorDead();
    mCollision->_CC = false;
    LiveActor::makeActorDead();
}

// MR::updateAlreadyDoneFlag args changed, otherwise matching
void WaterBazooka::kill () {
    _9C->kill();
    if (_A4 >= 0) 
        MR::updateAlreadyDoneFlag(_A4);
    LiveActor::kill();
}

// Removed hardcoded check, otherwise matching
void WaterBazooka::control () {
    setCameraTargetMtx();
    switchShowOrHide();
    if (!MR::isStageStateScenarioOpeningCamera()/* && MR::isEqualStageName("OceanPhantomCaveGalaxy")*/) {
        if (MR::isExecScenarioStarter()) {
            MR::playLevelMarioPinchBGM(_E8);
            _E8 = true;
        } else if (MR::isNearPlayer(this, 5000.0f)) {
            MR::playLevelMarioPinchBGM(_E8);
            _E8 = true;
        }
    }
    updateElectricLeak();
    if (getSmokeLevel() != -1) 
        MR::startLevelSound(this, "SE_EM_LV_WATERBAZ_SMOKE", -1, -1, -1);
    tryPanic();
}

// Matching
void WaterBazooka::calcAndSetBaseMtx () {
    LiveActor::calcAndSetBaseMtx();
    mJointControlDelegator->registerCallBack();
}

// Matching
void WaterBazooka::startDemoCrackCapsule () {
    MR::requestStartDemoRegisteredMarioPuppetable(this, 0, 0, "カプセル破壊");
    _94->crackCapsule();
    MR::invalidateCollisionParts(_94);
    setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoCrackCapsule::sInstance);
}

// Matching
void WaterBazooka::startDemoAnger () {
    if (MR::isDemoPartExist(this, "乗組員怒り")) 
        MR::requestStartDemoRegisteredMarioPuppetable(this, 0, 0, "乗組員怒り");
    _94->crackCapsule();
    MR::invalidateCollisionParts(this);
    setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoAnger::sInstance);
}

// Matching
void WaterBazooka::startDemoBreakCapsule () {
    MR::requestStartDemoRegisteredMarioPuppetable(this, 0, 0, "完全破壊");
    _90->panicDeath();
    _94->breakCapsule();
    MR::invalidateCollisionParts(_94);
    setNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakWait::sInstance);
}

// Matching, some funcs are inlined
bool WaterBazooka::aimAtMario () {
    if (MR::isStageStateScenarioOpeningCamera()) 
        return true;
    TVec3f v0;
    _B8.getXDir(v0);
    TVec3f v1;
    _B8.getTrans(v1);
    TVec3f *v2 = MR::getPlayerPos();
    TVec3f v3;
    JGeometry::negateInternal(mGravity, v3);
    TVec3f v4;
    JMAVECScaleAdd(v3, *v2, v4, 100.0f);
    TVec3f v5;
    v5.sub(v4, v1);
    MR::normalize(&v5);
    
    f32 v6 = 1.2f;
    if (v6 < 0.0f) 
        v6 = -v6;
    JMath::TSinCosTable<14, f32>* pTable = &JMath::sSinCosTable;
    MR::turnVecToVecCos(&v0, v0, v5, pTable->get(45.51f * v6), mGravity, 0.02f);
    TVec3f v7;
    JGeometry::negateInternal(mGravity, v7);
    TVec3f v8;
    MR::turnVecToPlane(&v8, v0, v7);
    MR::clampVecAngleDeg(&v0, v8, 15.0f);
    TVec3f v9;
    JGeometry::negateInternal(mGravity, v9);
    if (v0.dot(v9) > 0.0f) {
        TVec3f v10;
        JGeometry::negateInternal(mGravity, v10);
        MR::turnVecToPlane(&v0, v0, v10);
    }
    TVec3f v11;
    JGeometry::negateInternal(mGravity, v11);
    MR::makeMtxSideUpPos(&_B8, v0, v11, v1);
    TVec3f v12;
    _B8.getXDir(v12);
    TVec3f v13;
    PSVECCrossProduct(v12, v5, v13);
    f32 v14 = PSVECMag(v13);
    f32 v16 = __fabsf(JMath::sAtanTable.atan2_(v14, v12.dot(v5)));
    return (57.29578f * v16 <= 2.0f);
}

// Matching
void WaterBazooka::switchShowOrHide() {
    if (isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakExplosion::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakEnd::sInstance)) {
        MR::hideModel(this);
        return;
    }
    if (MR::isJudgedToClipFrustum(mTranslation, 800.0f)) {
        if (MR::isHiddenModel(this)) 
            return;
        MR::hideModel(this);
        return;
    }
    if (MR::isHiddenModel(this)) 
        MR::showModel(this);
}

// Matching
void WaterBazooka::updateElectricLeak () {
    if (mIsElectricBazooka && _F0 >= 0 && !isNrvDemo() && _A8 <= 1) {
        if (isElectricLeakSign() && !MR::isEffectValid(this, "AngrySign")) {
            MR::emitEffect(this, "AngrySign");
            MR::deleteEffect(this, "Angry");
        } else if (isElectricLeak() && !MR::isEffectValid(this, "Angry")) {
            MR::emitEffect(this, "Angry");
            MR::deleteEffect(this, "AngrySign");
        }
        if (isElectricLeakSign()) 
            MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_PRE_ELEC_LEAK", -1, -1, -1);
        else if (isElectricLeak()) 
            MR::startLevelSound(this, "SE_EM_LV_ELECBAZ_ELEC_LEAK", -1, -1, -1);
    
        if (_F0 < 600) 
            _F0++;
        else {
            _F0 = 0;
            MR::deleteEffect(this, "Angry");
        }
    }
}

// regswap, otherwise matching
void WaterBazooka::damageBazooka (HitSensor *pSender, HitSensor *pReceiver) {
    MR::startBrk(this, "Damage");
    MR::startBrk(_94, "Damage");
    MR::emitEffectHitBetweenSensors(this, pSender, pReceiver, 0.0f, 0);
    if (_A8 == 3) {
        startDemoCrackCapsule();
        MR::emitEffect(this, "DamageSmokeS1");
    } else if (_A8 == 2) {
        startDemoAnger();
        MR::emitEffect(this, "DamageSmokeS2");
        MR::emitEffect(this, "DamageSmokeL1");
    } else if (_A8 == 1) {
        startDemoBreakCapsule();
        MR::emitEffect(this, "DamageSmokeL2");
    }
    _A8--;
}

// Matching
bool WaterBazooka::calcJointCannon(TPos3f *v0, const JointControllerInfo &v1) {
    JMath::gekko_ps_copy12(v0, _B8.mMtx);
    return true;
}

// Matching
s32 WaterBazooka::getSmokeLevel () const {
    if (_A8 == 4) 
        return 0;
    if (_A8 == 3) 
        return 1;
    if (_A8 == 2) 
        return 2;
    if (_A8 == 1) 
        return 3;
    return -1;
}

// Matching
bool WaterBazooka::tryWaitForBattle () {
    if (MR::isStageStateScenarioOpeningCamera()) 
        return false;
    if (!MR::isValidSwitchA(this)) 
        return false;
    if (MR::isValidSwitchA(this) && MR::isOnSwitchA(this)) 
        return false;
    setNerve(&NrvWaterBazooka::WaterBazookaNrvWaitForBattle::sInstance);
    return true;
}

// Matching
bool WaterBazooka::tryPanic () {
    if (isNrvDemo()) 
        return false;
    bool v0 = false;
    if (isNerve(&NrvWaterBazooka::WaterBazookaNrvPanic::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvStorm::sInstance)) 
        v0 = true;
    if (v0) 
        return false;
    v0 = false;
    if (MR::isOnPlayer(getSensor("cannon")) || _94->isPlayerOnCapsule()) 
        v0 = true;
    if (!v0) 
        return false;
    setNerve(&NrvWaterBazooka::WaterBazookaNrvPanic::sInstance);
    return true;
}

// Matching
bool WaterBazooka::isNrvDemo () const {
    bool returnVal = false;
    if (isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoCrackCapsule::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoAnger::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakWait::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakSign::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakExplosion::sInstance) || isNerve(&NrvWaterBazooka::WaterBazookaNrvDemoBreakEnd::sInstance)) 
        returnVal = true;
    return returnVal;
}


bool WaterBazooka::isElectricLeakSign () const {
    bool returnVal = false;
    if (!mIsElectricBazooka) 
        return false;
    if (_F0 < 300 || _F0 >= 420) 
        returnVal = true;
    return returnVal;
}

bool WaterBazooka::isElectricLeak () const {
    bool returnVal = false;
    if (!mIsElectricBazooka) 
        return false;
    if (_F0 < 420 || _F0 >= 600) 
        returnVal = true;
    return returnVal;
}

// Matching
void WaterBazooka::initShooter () {
    _90 = new MogucchiShooterBazooka(this, "ウォーターバズーカ乗組員モグッチ");
    _90->initFixedPosition("Cockpit");
    _90->initWithoutIter();
}

// Matching
void WaterBazooka::initBazookaCapsule () {
    _94 = new WaterBazookaCapsule(this, "ウォーターバズーカのカプセル");
    _94->initFixedPosition("Top");
    _94->initWithoutIter();
}

// Matching
void WaterBazooka::initBreakModel () {
    if (mIsElectricBazooka) 
        mBreakModel = MR::createModelObjEnemy("エレクトリックバズーカ壊れモデル", "ElectricBazookaBreak", getBaseMtx());
    else 
        mBreakModel = MR::createModelObjEnemy("ウォーターバズーカ壊れモデル", "WaterBazookaBreak", getBaseMtx());
    mBreakModel->initWithoutIter();
    MR::invalidateClipping(mBreakModel);
    mBreakModel->makeActorDead();
}

// Matching
void WaterBazooka::initCameraTarget () {
    _9C = new CameraTargetDemoActor(MR::getJointMtx(this, "Cockpit"), "バズーカカメラターゲット");
    _9C->initWithoutIter();
    setCameraTargetMtx();
}

// Different SceneObj ID, otherwise matching
void WaterBazooka::initBullet (const JMapInfoIter &rIter) {
    if (mIsElectricBazooka) {
        mElectricBullets = new ElectricPressureBullet*[5];
        for (s32 i = 0; i < 5; i++) {
            mElectricBullets[i] = new ElectricPressureBullet("エレクトリックバズーカの弾");
            mElectricBullets[i]->initWithoutIter();
            MR::tryRegisterDemoCast(mElectricBullets[i], rIter);
        }
    } else 
        MR::createSceneObj(SCENE_OBJ_WATER_PRESSURE_BULLET_HOLDER);
}

// Changed function args in ElectricPressureBullet::shotElectricBullet, otherwise matching
bool WaterBazooka::tryShotBullet () {
    void *pBullet = 0;
    if (mIsElectricBazooka) 
        pBullet = selectBulletElectric();
    else {
        WaterPressureBulletHolder *pHolder = (WaterPressureBulletHolder *)MR::getSceneObjHolder()->getObj(SCENE_OBJ_WATER_PRESSURE_BULLET_HOLDER);
        pBullet = pHolder->callEmptyBullet();
    }
    if (!pBullet) 
        return false;
    TPos3f v0;
    calcGunPointFromCannon(&v0);
    if (mIsElectricBazooka) {
        ElectricPressureBullet *pElectricBullet = (ElectricPressureBullet *)pBullet;
        // false or true???
        pElectricBullet->shotElectricBullet(this, v0, sElectricBulletSpeed, false);
    } else {
        WaterPressureBullet *pWaterBullet = (WaterPressureBullet *)pBullet;
        pWaterBullet->shotWaterBullet(this, v0, 40.0f, true, true, false, &mCameraInfo);
    }
    return true;
}

// Matching
ElectricPressureBullet *WaterBazooka::selectBulletElectric () {
    for (s32 i = 0; i < 5; i++) {
        if (MR::isDead(mElectricBullets[i])) 
            return mElectricBullets[i];
    }
    return 0;
}

// functionally matching
bool WaterBazooka::tryJumpBackPlayerFromBazooka () const {
    bool v0 = false;
    if (MR::isOnPlayer(getSensor("cannon")) || _94->isPlayerOnCapsule()) 
        v0 = true;
    if (!v0 && !_94->isPlayerOnCapsule()) 
        return false;
    TVec3f v1;
    calcNearDropPoint(&v1);
    TVec3f v2;
    v2.sub(v1, *MR::getPlayerPos());
    TVec3f v3;
    JMAVECScaleAdd(mGravity, v2, v3, -mGravity.dot(v2));
    TVec3f v4;
    JGeometry::negateInternal(v3, v4);
    MR::offBind(MR::getPlayerDemoActor());
    MR::unlockPlayerAnimation();
    MR::forceFlyPlayer(v1, v4, MR::calcDistanceToPlayer(v1) / 20.0f);
    return true;
}

// regswap, otherwise matching
void WaterBazooka::calcNearDropPoint (TVec3f *v0) const {
    f32 v3 = 3.4028235e38f;
    TVec3f v2;
    for (s32 i = 0; i < 4; i++) {
        TVec3f v1;
        if (MR::tryFindNamePos(sDropPointStringTable[i], &v1, 0) && MR::isNearPlayer(v1, v3)) {
            v2.set(v1);
            v3 = MR::calcDistanceToPlayer(v2);
        }
    }
    if (v3 != 3.4028235e38f) 
        v0->set(v2);
    else {
        TVec3f v4;
        v4.sub(*MR::getPlayerPos(), mTranslation);
        JMAVECScaleAdd(mGravity, v4, v4, -mGravity.dot(v4));
        MR::normalizeOrZero(&v4);
        if (MR::isNearZero(v4, 0.001f)) {
            MR::getPlayerFrontVec(&v4);
            JGeometry::negateInternal(v4, v4);
        }
        JMAVECScaleAdd(v4, mTranslation, *v0, 800.0f);
    }
}

// Matching, some funcs are inlined
void WaterBazooka::calcGunPointFromCannon (TPos3f *v3) {
    TPos3f v0;
    JMath::gekko_ps_copy12(v0.mMtx, MR::getJointMtx(this, "Cannon1"));
    TVec3f v1;
    v0.getXDir(v1);
    TVec3f v2;
    v0.getTrans(v2);
    JMAVECScaleAdd(v1, v2, v2, 550.0f);
    JMath::gekko_ps_copy12(v3, &v0);
    TVec3f v4;
    v4.set(v0.mMtx[0][1], v0.mMtx[1][1], v0.mMtx[2][1]);
    TVec3f v5;
    v0.getZDir(v5);
    MR::makeMtxUpFrontPos(v3, v1, v4, v2);
}

// Matching
void WaterBazooka::setCameraTargetMtx () {
    TPos3f v0;
    JMath::gekko_ps_copy12(v0.mMtx, _90->getBaseMtx());
    TVec3f v1;
    v0.mMtx[0][3] = mTranslation.x;
    v0.mMtx[1][3] = mTranslation.y;
    v0.mMtx[2][3] = mTranslation.z;
    MR::calcUpVec(&v1, this);
    TVec3f v2;
    v0.getZDir(v2);
    MR::makeMtxUpFrontPos(&v0, v1, v2, mTranslation);
    _9C->setTargetMtx(v0.mMtx);
}

namespace NrvWaterBazooka {
    void WaterBazookaNrvStorm::executeOnEnd (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        if (MR::isEffectValid(pActor, "Spin")) 
            MR::deleteEffect(pActor, "Spin");
        OSReport("Storm_end\n");
    }
    void WaterBazookaNrvStorm::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeStorm();
        OSReport("Storm\n");
    }
    void WaterBazookaNrvPanic::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exePanic();
        OSReport("Panic\n");
    }
    void WaterBazookaNrvWaitForLaugh::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeWaitForLaugh();
        OSReport("WaitForLaugh\n");
    }
    void WaterBazookaNrvDemoBreakEnd::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoBreakEnd();
        OSReport("DemoBreakEnd\n");
    }
    void WaterBazookaNrvDemoBreakExplosion::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoBreakExplosion();
        OSReport("DemoBreakExplosion\n");
    }
    void WaterBazookaNrvDemoBreakSign::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoBreakSign();
        OSReport("DemoBreakSign\n");
    }
    void WaterBazookaNrvDemoBreakWait::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoBreakWait();
        OSReport("DemoBreakWait\n");
    }
    void WaterBazookaNrvDemoAnger::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoAnger();
        OSReport("DemoAnger\n");
    }
    void WaterBazookaNrvDemoCrackCapsule::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeDemoCrackCapsule();
        OSReport("DemoCrackCapsule\n");
    }
    void WaterBazookaNrvTire::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeTire();
        OSReport("Tire\n");
    }
    void WaterBazookaNrvShotNoMotion::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeShotNoMotion();
        OSReport("ShotNoMotion\n");
    }
    void WaterBazookaNrvShot::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeShot();
        OSReport("Shot\n");
    }
    void WaterBazookaNrvAimEnd::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeAimEnd();
        OSReport("AimEnd\n");
    }
    void WaterBazookaNrvAim::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeAim();
        OSReport("Aim\n");
    }
    void WaterBazookaNrvWait::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeWait();
        OSReport("Wait\n");
    }
    void WaterBazookaNrvWaitForBattle::execute (Spine *pSpine) const {
        WaterBazooka *pActor = (WaterBazooka *)pSpine->mExecutor;
        pActor->exeWaitForBattle();
        OSReport("WaitForBattle\n");
    }

    WaterBazookaNrvWaitForBattle(WaterBazookaNrvWaitForBattle::sInstance);
    WaterBazookaNrvWait(WaterBazookaNrvWait::sInstance);
    WaterBazookaNrvAim(WaterBazookaNrvAim::sInstance);
    WaterBazookaNrvAimEnd(WaterBazookaNrvAimEnd::sInstance);
    WaterBazookaNrvShot(WaterBazookaNrvShot::sInstance);
    WaterBazookaNrvShotNoMotion(WaterBazookaNrvShotNoMotion::sInstance);
    WaterBazookaNrvTire(WaterBazookaNrvTire::sInstance);
    WaterBazookaNrvDemoCrackCapsule(WaterBazookaNrvDemoCrackCapsule::sInstance);
    WaterBazookaNrvDemoAnger(WaterBazookaNrvDemoAnger::sInstance);
    WaterBazookaNrvDemoBreakWait(WaterBazookaNrvDemoBreakWait::sInstance);
    WaterBazookaNrvDemoBreakSign(WaterBazookaNrvDemoBreakSign::sInstance);
    WaterBazookaNrvDemoBreakExplosion(WaterBazookaNrvDemoBreakExplosion::sInstance);
    WaterBazookaNrvDemoBreakEnd(WaterBazookaNrvDemoBreakEnd::sInstance);
    WaterBazookaNrvWaitForLaugh(WaterBazookaNrvWaitForLaugh::sInstance);
    WaterBazookaNrvPanic(WaterBazookaNrvPanic::sInstance);
    WaterBazookaNrvStorm(WaterBazookaNrvStorm::sInstance);
}