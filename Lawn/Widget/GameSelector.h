#ifndef __GAMESELECTOR_H__
#define __GAMESELECTOR_H__

#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "AchievementsScreen.h"
#include "ZombatarScreen.h"
#include "MoreScreen.h"
#include "QuickplayScreen.h"

class LawnApp;
class ToolTipWidget;
namespace Sexy
{
    class DialogButton;
}

using namespace Sexy;

enum SelectorAnimState
{
    SELECTOR_OPEN,
    SELECTOR_NEW_USER,
    SELECTOR_SHOW_SIGN,
    SELECTOR_IDLE
};

class GameSelector : public Widget, public ButtonListener
{
private:
    enum
    {
        GameSelector_Adventure = 100,
        GameSelector_Minigame,
        GameSelector_Puzzle,
        GameSelector_Options,
        GameSelector_Help,
        GameSelector_Quit,
        GameSelector_ChangeUser,
#ifdef _HAS_LEVELSELECTOR
        GameSelector_LevelSelect,
#endif
        GameSelector_Store,
        GameSelector_Almanac,
        GameSelector_ZenGarden,
        GameSelector_Survival,
#ifdef _HAS_ZOMBATAR
        GameSelector_Zombatar,           
#endif
#ifdef _HAS_ACHIEVEMENTS
        GameSelector_AchievementsBack,
        GameSelector_Achievements,
#endif 
#ifdef _HAS_MORESCREEN
        GameSelector_QuickPlay,
#endif 
#ifdef _HAS_UNLOCK
        GameSelector_Unlock,
#endif
    };

public:
    LawnApp*                    mApp;                              
    NewLawnButton*              mAdventureButton;                  
    NewLawnButton*              mMinigameButton;                   
    NewLawnButton*              mPuzzleButton;                     
    NewLawnButton*              mOptionsButton;                    
    NewLawnButton*              mQuitButton;                       
    NewLawnButton*              mHelpButton;                       
    NewLawnButton*              mStoreButton;                      
    NewLawnButton*              mAlmanacButton;                    
    NewLawnButton*              mZenGardenButton;                  
    NewLawnButton*              mSurvivalButton;                   
    NewLawnButton*              mChangeUserButton;                 
#ifdef _HAS_ZOMBATAR
    NewLawnButton*              mZombatarButton;                                  
#endif
#ifdef _HAS_ACHIEVEMENTS
    NewLawnButton*              mAchievementsButton;                             
#endif
    NewLawnButton*              mTrophyButton;
#ifdef _HAS_MORESCREEN
    NewLawnButton*              mQuickPlayButton;                                
#endif
#ifdef _HAS_UNLOCK
    NewLawnButton*              mUnlockButton;
#endif
    Widget*                     mOverlayWidget;                    
    bool                        mStartingGame;                     
    int                         mStartingGameCounter;              
    bool                        mMinigamesLocked;                  
    bool                        mPuzzleLocked;                     
    bool                        mSurvivalLocked;                   
    bool                        mShowStartButton;                  
    ParticleSystemID            mTrophyParticleID;                 
    ReanimationID               mSelectorReanimID;                 
    ReanimationID               mCloudReanimID[6];                 
    int                         mCloudCounter[6];                  
    ReanimationID               mFlowerReanimID[3];                 
    ReanimationID               mLeafReanimID;                      
    ReanimationID               mHandReanimID;                      
    int                         mLeafCounter;                       
    SelectorAnimState           mSelectorState;                     
    int                         mLevel;                             
    bool                        mLoading;                           
    ToolTipWidget*              mToolTip;                           
    bool                        mHasTrophy;                         
    bool                        mUnlockSelectorCheat;               
    int                         mSlideCounter;                                    
    int                         mStartX;                                          
    int                         mStartY;                                          
    int                         mDestX;                                           
    int                         mDestY;                                           
#ifdef _HAS_ZOMBATAR
    ZombatarWidget*             mZombatarWidget;                                  
#endif
#ifdef _HAS_ACHIEVEMENTS
    AchievementsWidget*         mAchievementsWidget;                              
#endif
#ifdef _HAS_MORESCREEN
    MoreWidget*                 mMoreWidget;
#endif
#ifdef _HAS_LEVELSELECTOR
    QuickplayWidget*            mLevelSelectorWidget;
    NewLawnButton* mLevelSelectButton;
#endif
    ReanimationID               mWoodSignID;            
    ReanimationID               mSpotLightID;
    int                         mScrollOffset;

public:
#ifdef OPENFL_HOST
    std::map<std::string, Image*> mHostedMenuImages;
    Image* HostedMenuImage(const std::string& name);
    void LayoutHostedMenu();
    void DrawHostedMenu(Graphics* g);
#endif
    GameSelector(LawnApp* theApp);
    virtual ~GameSelector();

    void                        SyncProfile(bool theShowLoading);
    virtual void                Draw(Graphics* g);
    virtual void                DrawOverlay(Graphics* g);
    virtual void                Update();
    virtual void                AddedToManager(WidgetManager* theWidgetManager);
    virtual void                RemovedFromManager(WidgetManager* theWidgetManager);
    virtual void                OrderInManagerChanged();
    virtual void                ButtonMouseEnter(int theId);
    virtual void                ButtonPress(int theId, int theClickCount);
    virtual void                ButtonDepress(int theId);
    virtual void                KeyDown(KeyCode theKey);
    virtual void                KeyChar(char theChar);
    virtual void                MouseDown(int x, int y, int theClickCount);
    void                        TrackButton(DialogButton* theButton, const char* theTrackName, float theOffsetX, float theOffsetY);
    void                        SyncButtons();
    void                        AddTrophySparkle();
    void                        ClickedAdventure();
    void                        UpdateTooltip();
               bool             ShouldDoZenTuturialBeforeAdventure();
    void                        AddPreviewProfiles();
                                   
               void             SlideTo(int theX, int theY);
#ifdef _HAS_ACHIEVEMENTS
    void                        ShowAchievementsScreen();                            
#endif
#ifdef _HAS_ZOMBATAR
    void                        ShowZombatarScreen();
#endif
#ifdef _HAS_MORESCREEN
    void                        ShowMoreScreen();
#endif
#ifdef _HAS_LEVELSELECTOR
    void                        ShowQuickplayScreen();
#endif
    void                        ShowGameSelectorScreen();
    void                        DisableButtons(bool isDisabled);
};

class GameSelectorOverlay : public Widget
{
public:
    GameSelector*               mParent;                           

public:
    GameSelectorOverlay(GameSelector* theGameSelector);
    virtual ~GameSelectorOverlay() { }

    virtual void Draw(Graphics* g);
};

#endif
