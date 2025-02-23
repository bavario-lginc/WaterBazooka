#pragma once

#include "syati.h"
#include "JSystem/JMATrigonometric.h"
#include "MiscUtil.h"
#include "MogucchiShooterBazooka.h"
#include "WaterBazookaCapsule.h"
#include "CameraTargetDemoActor.h"

class MogucchiShooterBazooka;

class WaterBazooka : public LiveActor {
public:
    WaterBazooka(const char *pName);
    virtual void init(const JMapInfoIter &rIter);
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    virtual void makeActorDead();
    virtual void kill();
    virtual void control();
    virtual void calcAndSetBaseMtx();

    void initShooter();
    void initBazookaCapsule();
    void initBreakModel();
    void initCameraTarget();
    void initBullet(const JMapInfoIter &rIter);
    bool isNrvDemo() const;
    bool isElectricLeak() const;
    bool isElectricLeakSign() const;
    bool isFirstShoot() const;
    bool isTired() const;
    bool isPanic() const;
    bool isBazookaPinch() const;
    bool isBazookaLifeOut() const;
    bool aimAtMario();
    bool tryWaitForBattle();
    bool tryShotBullet();
    bool tryJumpBackPlayerFromBazooka() const;
    bool tryPanic();
    s32 getSmokeLevel() const;
    
    void exeWaitForBattle();
    void exeWait();
    void exeAim();
    void exeAimEnd();
    void exeShot();
    void exeShotNoMotion();
    void exeTire();
    void exeDemoCrackCapsule();
    void exeDemoAnger();
    void exeDemoBreakWait();
    void exeDemoBreakSign();
    void exeDemoBreakExplosion();
    void exeDemoBreakEnd();
    void exeWaitForLaugh();
    void exePanic();
    void exeStorm();

    bool calcJointCannon(TPos3f *, const JointControllerInfo &);
    void damageBazooka(HitSensor *pSender, HitSensor *pReceiver);
    void setCameraTargetMtx();
    void switchShowOrHide();
    void updateElectricLeak();
    void startDemoCrackCapsule();
    void startDemoAnger();
    void startDemoBreakCapsule();
    ElectricPressureBullet *selectBulletElectric();
    void calcGunPointFromCannon(TPos3f *);
    void calcNearDropPoint(TVec3f *) const;

    // Array?
    MogucchiShooterBazooka *_90;
    WaterBazookaCapsule *_94;
    ModelObj *mBreakModel;                                          // _98
    CameraTargetDemoActor *_9C;
    ActorCameraInfo *mCameraInfo;                                   // _A0
    s32 _A4;
    u32 _A8;
    u32 _AC;
    CollisionParts *mCollision;                                     // _B0
    JointControlDelegator<WaterBazooka> *mJointControlDelegator;    // _B4
    TPos3f _B8;
    bool _E8;
    bool mSpawnPowerStar;
    bool mIsElectricBazooka;                                        // _EA
    ElectricPressureBullet **mElectricBullets;
    u32 _F0;
};

namespace NrvWaterBazooka {
    NERVE(WaterBazookaNrvWaitForBattle);
    NERVE(WaterBazookaNrvWait);
    NERVE(WaterBazookaNrvAim);
    NERVE(WaterBazookaNrvAimEnd);
    NERVE(WaterBazookaNrvShot);
    NERVE(WaterBazookaNrvShotNoMotion);
    NERVE(WaterBazookaNrvTire);
    NERVE(WaterBazookaNrvDemoCrackCapsule);
    NERVE(WaterBazookaNrvDemoAnger);
    NERVE(WaterBazookaNrvDemoBreakWait);
    NERVE(WaterBazookaNrvDemoBreakSign);
    NERVE(WaterBazookaNrvDemoBreakExplosion);
    NERVE(WaterBazookaNrvDemoBreakEnd);
    NERVE(WaterBazookaNrvWaitForLaugh);
    NERVE(WaterBazookaNrvPanic);
    ENDABLE_NERVE(WaterBazookaNrvStorm);
};