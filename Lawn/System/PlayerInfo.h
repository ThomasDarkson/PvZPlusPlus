#ifndef __PLAYERINFO_H__
#define __PLAYERINFO_H__

#define MAX_POTTED_PLANTS 200
#define PURCHASE_COUNT_OFFSET 1000

#include <ctime>
#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Common.h"
#include "../Widget/AchievementsScreen.h"
#include "../Plant.h"

class PottedPlant
{
public:
    enum FacingDirection
    {
        FACING_RIGHT,
        FACING_LEFT
    };

public:
    SeedType            mSeedType;                        
    GardenType          mWhichZenGarden;                  
    int                 mX;                               
    int                 mY;                               
    FacingDirection     mFacing;                           

    __time64_t          mLastWateredTime;                  
    DrawVariation       mDrawVariation;                    
    PottedPlantAge      mPlantAge;                         
    int                 mTimesFed;                         
    int                 mFeedingsPerGrow;                  
    PottedPlantNeed     mPlantNeed;                        

    __time64_t          mLastNeedFulfilledTime;            
    __time64_t          mLastFertilizedTime;               
    __time64_t          mLastChocolateTime;                
    __time64_t          mFutureAttribute[1];               

public:
    void                InitializePottedPlant(SeedType theSeedType);
};

class DataSync;
class PlayerInfo
{
public:
    SexyString          mName;                                    
    ulong               mUseSeq;                                   
    ulong               mId;                                       
    int                 mLevel;                                    
    int                 mCoins;                                    
    int                 mFinishedAdventure;                        
    int                 mChallengeRecords[100];                    
    long                mPurchases[80];                             
    int                 mPlayTimeActivePlayer;                      
    int                 mPlayTimeInactivePlayer;                    
    int                 mHasUsedCheatKeys;                          
    int                 mHasWokenStinky;                            
    int                 mDidntPurchasePacketUpgrade;                
    long                mLastStinkyChocolateTime;                   
    int                 mStinkyPosX;                                
    int                 mStinkyPosY;                                
    int                 mHasUnlockedMinigames;                      
    int                 mHasUnlockedPuzzleMode;                     
    int                 mHasNewMiniGame;                            
    int                 mHasNewScaryPotter;                         
    int                 mHasNewIZombie;                             
    int                 mHasNewSurvival;                            
    int                 mHasUnlockedSurvivalMode;                   
    int                 mNeedsMessageOnGameSelector;                
    int                 mNeedsMagicTacoReward;                      
    int                 mHasSeenStinky;                             
    int                 mHasSeenUpsell;                             
    int                 mEarnedFromZenGarden;                                                                        
    int                 mNumPottedPlants;                           
    PottedPlant         mPottedPlant[MAX_POTTED_PLANTS];            
    bool                mEarnedAchievements[(int)AchievementId::MAX_ACHIEVEMENTS];                                 
    bool                mShownAchievements[(int)AchievementId::MAX_ACHIEVEMENTS];                                      
    bool                mPlantedPlants[(int)SeedType::SEED_LEFTPEATER];                     
#ifdef _HAS_ZOMBATAR
    bool                mAckZombatarTOS;
#endif
    bool                mReservedModeSeen;
    int                 mReservedModeLevel;

public:
    PlayerInfo();

    void                Reset();
               void     AddCoins(int theAmount);
    void                SyncSummary(DataSync& theSync);
    void                SyncDetails(DataSync& theSync);
    void                DeleteUserFiles();
    void                LoadDetails();
    void                SaveDetails();
    inline int          GetLevel() const { return mLevel; }
    inline void         SetLevel(int theLevel) { mLevel = theLevel; }
               void     ResetChallengeRecord(GameMode theGameMode);
};

#endif
