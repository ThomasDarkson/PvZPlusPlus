#ifndef __CHALLENGE_H__
#define __CHALLENGE_H__

#include "../ConstEnums.h"
#include "HeavyWeapon.h"
#include "ZombieTrap.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/FilterEffect.h"
#include "../SexyAppFramework/Graphics.h"

#define BEGHOULED_MAX_GRIDSIZEX 8
#define BEGHOULED_MAX_GRIDSIZEY 5
#define ART_CHALLEGE_SIZE_X 9
#define MAX_PICK_GRID_SIZE 50

constexpr const int BEGHOULED_WINNING_SCORE = 75;
constexpr const int SLOT_MACHINE_WINNING_SCORE = 2000;
constexpr const int ZOMBIQUARIUM_WINNING_SCORE = 1000;
constexpr const int I_ZOMBIE_WINNING_SCORE = 5;
constexpr const int MAX_PORTALS = 4;
constexpr const int MAX_SQUIRRELS = 7;
constexpr const int MAX_SCARY_POTS = 54;
constexpr const int STORM_FLASH_TIME = 150;

using namespace Sexy;

class LawnApp;
class Board;
class Plant;
class Zombie;
class GridItem;
class SeedPacket;
class HitResult;
struct TodWeightedGridArray;

enum BeghouledUpgrade
{
    BEGHOULED_UPGRADE_REPEATER,
    BEGHOULED_UPGRADE_FUMESHROOM,
    BEGHOULED_UPGRADE_TALLNUT,
    NUM_BEGHOULED_UPGRADES
};

struct BeghouledBoardState
{
	SeedType		        mSeedType[9][6];
};

class Challenge
{
public:
	LawnApp*				mApp;                                                     
	Board*					mBoard;                                                   
	bool					mBeghouledMouseCapture;                                   
    int                     mBeghouledMouseDownX;                                     
    int                     mBeghouledMouseDownY;                                      
    bool                    mBeghouledEated[9][6];                                     
    bool                    mBeghouledPurcasedUpgrade[NUM_BEGHOULED_UPGRADES];         
    int                     mBeghouledMatchesThisMove;                                 
    ChallengeState          mChallengeState;                                           
    int                     mChallengeStateCounter;                                    
    int                     mConveyorBeltCounter;                                      
    int                     mChallengeScore;                                           
    bool                    mShowBowlingLine;                                          
    SeedType                mLastConveyorSeedType;                                     
    int                     mSurvivalStage;                                            
    int                     mSlotMachineRollCount;                                     
    ReanimationID           mReanimChallenge;                                          
    ReanimationID           mReanimClouds[6];                                          
    int                     mCloudsCounter[6];                                         
    int                     mChallengeGridX;                                           
    int                     mChallengeGridY;                                           
    int                     mScaryPotterPots;                                          
    int                     mRainCounter;                                              
    int                     mTreeOfWisdomTalkIndex;                                    
    int					    mShoutingCounter;                                                        
    int                     mChallengePoints;
    bool                    mHadInitBombs;
    int                     mBombsLeft;
    int                     mBBLeft;                        
    int                     mCageLeft;
    bool                    mDoesBrainAttracts;
    int                     mBrainHealth;
    int                     mBrainX;
    int                     mBrainY;
    float                   mBrainAltitude;
    int                     mBrainRow;
    int                     mBrainRenderOrder;
    HeavyWeapon             mHeavyWeapon;
    ZombieTrap              mZombieTrap;

public:
    Challenge();

    void                    StartLevel();
    void                    BeghouledPopulateBoard();
    void                    LoadBeghouledBoardState(BeghouledBoardState* theState);
    SeedType                BeghouledPickSeed(int theGridX, int theGridY, BeghouledBoardState* theBoardState, bool theAllowMatches);
    bool                    BeghouledBoardHasMatch(BeghouledBoardState* theBoardState);
               SeedType     BeghouledGetPlantAt(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    int                     BeghouledVerticalMatchLength(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    int                     BeghouledHorizontalMatchLength(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
               void         BeghouledDragStart(int x, int y);
    void                    BeghouledDragUpdate(int x, int y);
    inline void             BeghouledDragCancel() { mBeghouledMouseCapture = false; }
    bool                    MouseMove(int x, int y);
    bool                    MouseDown(int x, int y, int theClickCount, HitResult* theHitResult);
    bool                    MouseUp(int x, int y);
    void                    ClearCursor();
    void                    BeghouledRemoveHorizontalMatch(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    void                    BeghouledRemoveVerticalMatch(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    void                    BeghouledRemoveMatches(BeghouledBoardState* theBoardState);
    void                    Update();
    void                    UpdateBeghouled();
    bool                    UpdateBeghouledPlant(Plant* thePlant);
    void                    BeghouledFallIntoSquare(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    void                    BeghouledMakePlantsFall(BeghouledBoardState* theBoardState);
    void                    ZombieAtePlant(Zombie* theZombie, Plant* thePlant);
    void                    DrawBackdrop(Graphics* g);
    void                    DrawArtChallenge(Graphics* g);
    void                    CheckForCompleteArtChallenge(int theGridX, int theGridY);
               SeedType     GetArtChallengeSeed(int theGridX, int theGridY);
    void                    PlantAdded(Plant* thePlant);
    PlantingReason          CanPlantAt(int theGridX, int theGridY, SeedType theSeedType);
    void                    DrawBeghouled(Graphics* g);
    bool                    BeghouledIsValidMove(int theFromX, int theFromY, int theToX, int theToY, BeghouledBoardState* theBoardState);
    bool                    BeghouledCheckForPossibleMoves(BeghouledBoardState* theBoardState);
    void                    BeghouledCheckStuckState();
    void                    InitZombieWavesSurvival();
               void         InitZombieWavesFromList(ZombieType* theZombieList, int theListLength);
    void                    InitZombieWaves();
               Rect         SlotMachineGetHandleRect();
    void                    UpdateSlotMachine();
    void                    DrawSlotMachine(Graphics* g);
    bool                    UpdateToolTip(int theX, int theY);
    void                    WhackAZombieSpawning();
    bool                    UpdateZombieSpawning();
    void                    BeghouledClearCrater(int theCount);
    void                    MouseDownWhackAZombie(int theX, int theY);
    void                    DrawStormNight(Graphics* g);
    void                    UpdateStormyNight();
    void                    InitLevel();
    void                    SpawnZombieWave();
    void                    GraveDangerSpawnRandomGrave();
    void                    GraveDangerSpawnGraveAt(int theGridX, int theGridY);
    void                    SpawnLevelAward(int theGridX, int theGridY);
    void                    BeghouledScore(int theGridX, int theGridY, int theNumPlants, bool theIsHorizontal);
    void                    DrawStormFlash(Graphics* g, int theTime, int theMaxAmount);
    void                    UpdateRainingSeeds();
               void         PlayBossEnter();
    void                    UpdateConveyorBelt();
    void                    PortalStart();
    void                    UpdatePortalCombat();
    GridItem*               GetOtherPortal(GridItem* thePortal);
    void                    UpdatePortal(GridItem* thePortal);
    float                   PortalCombatRowSpawnWeight(int theGridY);
    bool                    CanTargetZombieWithPortals(Plant* thePlant, Zombie* theZombie);
    GridItem*               GetPortalToRight(int theGridX, int theGridY);
    GridItem*               GetPortalAt(int theGridX, int theGridY);
    void                    MoveAPortal();
    int                     GetPortalDistanceToMower(int theGridY);
    GridItem*               GetPortalToLeft(int theGridX, int theGridY);
    void                    BeghouledPacketClicked(SeedPacket* theSeedPacket);
    void                    BeghouledShuffle();
               bool         BeghouledCanClearCrater();
    void                    BeghouledUpdateCraters();
    Zombie*                 ZombiquariumSpawnSnorkle();
    void                    ZombiquariumPacketClicked(SeedPacket* theSeedPacket);
    void                    ZombiquariumMouseDown(int x, int y);
    void                    ZombiquariumDropBrain(int x, int y);
    void                    ZombiquariumUpdate();
               void         ShovelAddWallnuts();
    void                    ScaryPotterPlacePot(ScaryPotType theScaryPotType, ZombieType theZombieType, SeedType theSeedType, int theCount, TodWeightedGridArray* theGridArray, int theGridArrayCount);
    void                    ScaryPotterStart();
    void                    ScaryPotterUpdate();
    void                    ScaryPotterOpenPot(GridItem* theScaryPot);
    void                    ScaryPotterJackExplode(int thePosX, int thePosY);
    bool                    ScaryPotterIsCompleted();
    void                    ScaryPotterChangePotType(GridItemState thePotType, int theCount);
    void                    ScaryPotterPopulate();
               void         ScaryPotterDontPlaceInCol(int theCol, TodWeightedGridArray* theGridArray, int theGridArrayCount);
    void                    ScaryPotterFillColumnWithPlant(int theCol, SeedType theSeedType, TodWeightedGridArray* theGridArray, int theGridArrayCount);
    void                    PuzzleNextStageClear();
    void                    ScaryPotterMalletPot(GridItem* theScaryPot);
    static ZombieType       IZombieSeedTypeToZombieType(SeedType theSeedType);
    static            bool  IsZombieSeedType(SeedType theSeedType);
    void                    IZombieMouseDownWithZombie(int theX, int theY, int theClickCount);
    void                    IZombieStart();
    void                    IZombiePlacePlants(SeedType theSeedType, int theCount, int theGridY = -1);
    void                    IZombieUpdate();
    void                    IZombieDrawPlant(Graphics* g, Plant* thePlant);
    void                    IZombieSetPlantFilterEffect(Plant* thePlant, FilterEffect theFilterEffect);
               int          ScaryPotterCountSunInPot(GridItem* theScaryPot);
    int                     ScaryPotterCountPots();
    void                    IZombieInitLevel();
    void                    DrawRain(Graphics* g);
    void                    DrawWeather(Graphics* g);
    void                    SquirrelUpdate();
               int          SquirrelCountUncaught();
    void                    SquirrelStart();
    void                    SquirrelFound(GridItem* theSquirrel);
    void                    SquirrelPeek(GridItem* theSquirrel);
    void                    SquirrelChew(GridItem* theSquirrel);
    void                    SquirrelUpdateOne(GridItem* theSquirrel);
    void                    IZombieSetupPlant(Plant* thePlant);
    void                    UpdateRain();
    bool                    IZombieEatBrain(Zombie* theZombie);
    GridItem*               IZombieGetBrainTarget(Zombie* theZombie);
               void         IZombiePlacePlantInSquare(SeedType theSeedType, int theGridX, int theGridY = -1);
    void                    AdvanceCrazyDaveDialog();
    void                    BeghouledFlashPlant(int theFlashX, int theFlashY, int theFromX, int theFromY, int theToX, int theToY);
    void                    BeghouledFlashAMatch();
    bool                    BeghouledFlashFromBoardState(BeghouledBoardState* theBoardState, int theFromX, int theFromY, int theToX, int theToY);
    void                    IZombiePlantDropRemainingSun(Plant* thePlant);
    void                    IZombieSquishBrain(GridItem* theBrain);
    void                    IZombieScoreBrain(GridItem* theBrain);
    void                    LastStandUpdate();
    void                    WhackAZombiePlaceGraves(int theGraveCount);
    bool                    BeghouledTwistSquareFromMouse(int theX, int theY, int& theGridX, int& theGridY);
    bool                    BeghouledTwistValidMove(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    void                    BeghouledTwistMouseDown(int x, int y);
    bool                    BeghouledTwistMoveCausesMatch(int theGridX, int theGridY, BeghouledBoardState* theBoardState);
    bool                    BeghouledTwistFlashMatch(BeghouledBoardState* theBoardState, int theGridX, int theGridY);
               void         BeghouledCancelMatchFlashing();
    void                    BeghouledStartFalling(ChallengeState theState);
    void                    BeghouledFillHoles(BeghouledBoardState* theBoardState, bool theAllowMatches);
               void         BeghouledMakeStartBoard();
    void                    BeghouledCreatePlants(BeghouledBoardState* theOldBoardState, BeghouledBoardState* theNewBoardState);
    void                    PuzzlePhaseComplete(int theGridX, int theGridY);
               bool         PuzzleIsAwardStage();
    void                    IZombiePlaceZombie(ZombieType theZombieType, int theGridX, int theGridY);
    void                    WhackAZombieUpdate();
    void                    LastStandCompletedStage();
    void                    TreeOfWisdomUpdate();
    void                    TreeOfWisdomFertilize();
    void                    TreeOfWisdomInit();
               bool         TreeOfWisdomMouseOn(int theX, int theY);
               int          TreeOfWisdomGetSize();
    void                    TreeOfWisdomDraw(Graphics* g);
               void         TreeOfWisdomNextGarden();
               void         TreeOfWisdomToolUpdate(GridItem* theZenTool);
    void                    TreeOfWisdomOpenStore();
    void                    TreeOfWisdomLeave();
    void                    TreeOfWisdomGrow();
               void         TreeOfWisdomTool(int theMouseX, int theMouseY);
    bool                    TreeOfWisdomHitTest(int theX, int theY, HitResult* theHitResult);
    void                    TreeOfWisdomBabble();
    void                    TreeOfWisdomGiveWisdom();
    void                    TreeOfWisdomSayRepeat();
    bool                    TreeOfWisdomCanFeed();
#ifdef _MOBILE_MINIGAMES
    void                    HeatWaveUpdate();
#endif

    GridItem*               GetPortalLeftRight(int theGridX, int theGridY, bool theToLeft = true);

    void                    MouseDownButterAZombie(int theX, int theY);
};

extern SeedType gArtChallengeWallnut[6][9];
extern SeedType gArtChallengeSunFlower[6][9];
extern SeedType gArtChallengeStarFruit[6][9];

class ZombieAllowedLevels
{
public:
    ZombieType                      mZombieType;
    int                             mAllowedOnLevel[50];
};
extern int gZombieWaves[NUM_LEVELS];           
extern ZombieAllowedLevels gZombieAllowedLevels[NUM_ZOMBIE_TYPES];            

#endif