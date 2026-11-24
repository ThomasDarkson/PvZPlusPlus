#ifndef __CUTSCENE_H__
#define __CUTSCENE_H__

using namespace Sexy;
class LawnApp;
class Board;
class ChallengeScreen;

class CutScene
{
public:
    LawnApp*                    mApp;                                   
    Board*                      mBoard;                                 
    int                         mCutsceneTime;                          
    int                         mSodTime;                               
    int                         mGraveStoneTime;                         
    int                         mReadySetPlantTime;                      
    int                         mFogTime;                                
    int                         mBossTime;                               
    int                         mCrazyDaveTime;                          
    int                         mLawnMowerTime;                          
    int                         mCrazyDaveDialogStart;                   
    bool                        mSeedChoosing;                           
    ReanimationID               mZombiesWonReanimID;                     
    bool                        mPreloaded;                              
    bool                        mPlacedZombies;                          
    bool                        mPlacedLawnItems;                        
    int                         mCrazyDaveCountDown;                                        
    int                         mCrazyDaveLastTalkIndex;                                        
    bool                        mUpsellHideBoard;                                             
    ChallengeScreen*            mUpsellChallengeScreen;                                           
    bool                        mPreUpdatingBoard;                                                    
public:
    CutScene();
    ~CutScene();

    void                        StartLevelIntro();
    void                        CancelIntro();
    void                        Update();
    void                        AnimateBoard();
               void             StartSeedChooser();
               void             EndSeedChooser();
               int              CalcPosition(int theTimeStart, int theTimeEnd, int thePositionStart, int thePositionEnd);
    void                        PlaceStreetZombies();
    void                        AddGraveStoneParticles();
    void                        PlaceAZombie(ZombieType theZombieType, int theGridX, int theGridY);
    bool                        CanZombieGoInGridSpot(ZombieType theZombieType, int theGridX, int theGridY, bool theZombieGrid[5][5]);
               bool             IsSurvivalRepick();
               bool             IsAfterSeedChooser();
    void                        AddFlowerPots();
    void                        UpdateZombiesWon();
    void                        StartZombiesWon();
               bool             ShowZombieWalking();
               bool             IsCutSceneOver();
    void                        ZombieWonClick();
    void                        MouseDown(int theX, int theY);
    void                        KeyDown(KeyCode theKey);
               void             AdvanceCrazyDaveDialog(bool theJustSkipping);
    void                        ShowShovel();
    bool                        CanGetPacketUpgrade();
    bool                        CanGetPacketUpgrade(int theIndex);
    void                        FindPlaceForStreetZombies(ZombieType theZombieType, bool theZombieGrid[5][5], int& thePosX, int& thePosY);
    void                        FindAndPlaceZombie(ZombieType theZombieType, bool theZombieGrid[5][5]);
    static            bool      Is2x2Zombie(ZombieType theZombieType);
    void                        PreloadResources();
               bool             IsBeforePreloading();
               bool             IsShowingCrazyDave();
    bool                        IsNonScrollingCutscene();
    bool                        IsScrolledLeftAtStart();
               bool             IsInShovelTutorial();
    void                        PlaceLawnItems();
    bool                        CanGetSecondPacketUpgrade();
    int                         ParseDelayTimeFromMessage();
    int                         ParseTalkTimeFromMessage();
    void                        ClearUpsellBoard();
    void                        LoadIntroBoard();
               void             AddUpsellZombie(ZombieType theZombieType, int thePixelX, int theGridY);
    void                        LoadUpsellBoardPool();
    void                        LoadUpsellBoardFog();
    void                        LoadUpsellChallengeScreen();
    void                        LoadUpsellBoardRoof();
    void                        UpdateUpsell();
    void                        DrawUpsell(Graphics* g);
    void                        UpdateIntro();
    void                        DrawIntro(Graphics* g);
               bool             ShouldRunUpsellBoard();
};

#endif
