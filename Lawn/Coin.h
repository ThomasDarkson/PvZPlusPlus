#ifndef __COIN_H__
#define __COIN_H__

#include "GameObject.h"
#include "System/PlayerInfo.h"
#include "../Sexy.TodLib/FilterEffect.h"

class HitResult;
namespace Sexy
{
    class Graphics;
}
using namespace Sexy;

class Coin : public GameObject
{
public:
    float                   mPosX;                         
    float                   mPosY;                         
    float                   mVelX;                         
    float                   mVelY;                         
    float                   mScale;                        
    bool                    mDead;                         
    int                     mFadeCount;                    
    float                   mCollectX;                     
    float                   mCollectY;                     
    int                     mGroundY;                      
    int                     mCoinAge;                      
    bool                    mIsBeingCollected;             
    int                     mDisappearCounter;             
    CoinType                mType;                         
    CoinMotion              mCoinMotion;                   
    AttachmentID            mAttachmentID;                 
    float                   mCollectionDistance;           
    SeedType                mUsableSeedType;               
    PottedPlant             mPottedPlantSpec;              
    bool                    mNeedsBouncyArrow;             
    bool                    mHasBouncyArrow;               
    bool                    mHitGround;                    
    int                     mTimesDropped;                 
    FilterEffect            mFilterEffect;
    ReanimationID           mReanimationID;

public:
    Coin();
    ~Coin();

    void                    CoinInitialize(int theX, int theY, CoinType theCoinType, CoinMotion theCoinMotion);
    void                    MouseDown(int x, int y, int theClickCount);
    bool                    MouseHitTest(int theX, int theY, HitResult* theHitResult);
    void                    Die();
    void                    StartFade();
    void                    Update();
    void                    Draw(Graphics* g);
    void                    Collect();
               int          GetSunValue();
    static            int   GetCoinValue(CoinType theCoinType);
    void                    UpdateFade();
    void                    UpdateFall();
    void                    ScoreCoin();
    void                    UpdateCollected();
    Color                   GetColor();
               bool         IsMoney();
               bool         IsSun();
    float                   GetSunScale();
    inline bool             IsOnGround() { return false; }
    SeedType                GetFinalSeedPacketType();
    bool                    IsLevelAward();
    bool                    CoinGetsBouncyArrow();
    void                    FanOutCoins(CoinType theCoinType, int theNumCoins);
    int                     GetDisappearTime();
    void                    DroppedUsableSeed();
    void                    PlayCollectSound();
    void                    TryAutoCollectAfterLevelAward();
    bool                    IsPresentWithAdvice();
    void                    PlayLaunchSound();
    void                    PlayGroundSound();

    static            bool  IsMoney(CoinType theType);
};

#endif
