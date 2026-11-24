#ifndef __LAWNAPP_H__
#define __LAWNAPP_H__

#include "ConstEnums.h"
#include <map>
#include "SexyAppFramework/SexyApp.h"

#include "portaudio.h"
#include "SexyAppFramework/DDImage.h"
#include "SexyAppFramework/SexyMatrix.h"
#include "Sexy.TodLib/FilterEffect.h"

#include "SexyAppFramework/Common.h"

#include <SDL3/SDL.h>
#include "SexyAppFramework/SDL3Image.h"

#define SAMPLE_RATE  44100
#define FRAMES_PER_BUFFER 256
#define SHOUT_THRESHOLD 0.2f 

class Board;
class GameSelector;
class ChallengeDefinition;
class SeedChooserScreen;
class AwardScreen;
class CreditScreen;
class LanguageWidget;
class ParticleScreen;
class TodFoley;
class PoolEffect;
class ZenGarden;
class PottedPlant;
class EffectSystem;
class TodParticleSystem;
class Reanimation;
class ReanimatorCache;
class ProfileMgr;
class PlayerInfo;
class Music;
class TitleScreen;
class PopDRMComm;
class ChallengeScreen;
class StoreScreen;
class AlmanacDialog;
class TypingCheck;

namespace Sexy
{
	class Dialog;
	class Graphics;
	class ButtonWidget;
};

enum FoleyType : int;

using namespace Sexy;

typedef std::list<ButtonWidget*> ButtonList;
typedef std::list<Image*> ImageList;

class LevelStats
{
public:
	int								mUnusedLawnMowers;

public:
	LevelStats() { Reset(); }
	inline void						Reset() { mUnusedLawnMowers = 0; }
};

class Coin;

class LawnApp : public SexyApp
{
public:
	static SDL_Window*				mSDLWindow;
	static SDL_Renderer*			mSDLRenderer;
	static SDL_Cursor*				mSDLPointerCursor;
	static SDL_Cursor*				mSDLHandCursor;
	static SDL_Cursor*				mSDLDraggingCursor;
	static SDL_Cursor*				mSDLTextCursor;
	static SDL_Cursor*				mSDLWaitCursor;
	static SDL_Cursor*				mSDLNoCursor;
	static bool						mIsPlayingVideo;

public:
	Board*							mBoard;											        
	TitleScreen*					mTitleScreen;									        
	GameSelector*					mGameSelector;									        
	SeedChooserScreen*				mSeedChooserScreen;								        
	AwardScreen*					mAwardScreen;									        
	CreditScreen*					mCreditScreen;									        
	ChallengeScreen*				mChallengeScreen;								        
	LanguageWidget*					mLanguageScreen;
	ParticleScreen*					mParticleScreen;
	TodFoley*						mSoundSystem;									        
	ButtonList						mControlButtonList;								        
	ImageList						mCreatedImageList;								        
	std::string						mReferId;										        
	std::string						mRegisterLink;									        
	std::string						mMod;											        
	bool							mRegisterResourcesLoaded;						        
	bool							mTodCheatKeys;									        
	GameMode						mGameMode;										        
	GameScenes						mGameScene;										        
	bool							mLoadingZombiesThreadCompleted;					        
	bool							mFirstTimeGameSelector;							        
	int								mGamesPlayed;									        
	int								mMaxExecutions;									        
	int								mMaxPlays;										        
	int								mMaxTime;										        
	bool							mEasyPlantingCheat;								        
	PoolEffect*						mPoolEffect;									        
	ZenGarden*						mZenGarden;										        
	EffectSystem*					mEffectSystem;									        
	ReanimatorCache*				mReanimatorCache;								        
	ProfileMgr*						mProfileMgr;									        
	PlayerInfo*						mPlayerInfo;									        
	LevelStats*						mLastLevelStats;								        
	bool							mCloseRequest;									        
	int								mAppCounter;									        
	Music*							mMusic;											        
	ReanimationID					mCrazyDaveReanimID;								        
	CrazyDaveState					mCrazyDaveState;								        
	int								mCrazyDaveBlinkCounter;							        
	ReanimationID					mCrazyDaveBlinkReanimID;						        
	int								mCrazyDaveMessageIndex;							        
	SexyString						mCrazyDaveMessageText;							        
	int								mAppRandSeed;									        
	HICON							mBigArrowCursor;								        
	PopDRMComm*						mDRM;											        
	int								mSessionID;										        
	int								mPlayTimeActiveSession;							        
	int								mPlayTimeInactiveSession;						        
	BoardResult						mBoardResult;									        
	bool							mSawYeti;										        
	TypingCheck*					mKonamiCheck;									        
	TypingCheck*					mMustacheCheck;									        
	TypingCheck*					mMoustacheCheck;								        
	TypingCheck*					mSuperMowerCheck;								        
	TypingCheck*					mSuperMowerCheck2;								        
	TypingCheck*					mFutureCheck;									        
	TypingCheck*					mPinataCheck;									        
	TypingCheck*					mDanceCheck;									        
	TypingCheck*					mDaisyCheck;									        
	TypingCheck*					mSukhbirCheck;									        
	bool							mMustacheMode;									        
	bool							mSuperMowerMode;								        
	bool							mFutureMode;									        
	bool							mPinataMode;									        
	bool							mDanceMode;										        
	bool							mDaisyMode;										        
	bool							mSukhbirMode;									        
	TrialType						mTrialType;										        
	bool							mDebugTrialLocked;								        
	bool							mMuteSoundsForCutscene;							        
	bool							mShowHealthBar;
	bool							mFlowersPlucked[3];
	PaStream*						mPortAudioStream;
	float							mVoiceVolume;

	Rect							gBoardBounds;


	int								mPlayerLevelRef;
    int mPendingAdventureLevel = 0;
    bool mAdventureReplay = false;
    bool mLocalCoop = false;
    bool mCoopHasPartner = false;
    bool mCoopControllerInput = false;
    int mCoopSun = 50;
    int mCoopLevelIndex = -1;
    int mCoopNextConveyor = 0;
    enum class CoopSun { DoubleWaiting, DoubleCollected, Controller };
    std::map<const Coin*, CoopSun> mCoopSuns;
    int GetAdventureLevel() const;
	bool                            mEnableFPS;

public:
	LawnApp();
	virtual ~LawnApp();

	bool							KillNewOptionsDialog();
	virtual void					GotFocus();
	virtual void					LostFocus();
	virtual void					InitHook();
	virtual void					WriteToRegistry();
	virtual void					ReadFromRegistry();
	virtual void					LoadingThreadProc();
	virtual void					LoadingCompleted();
	virtual void					LoadingThreadCompleted();
	virtual void					URLOpenFailed(const std::string& theURL);
	virtual void					URLOpenSucceeded(const std::string& theURL);
	virtual bool					OpenURL(const std::string& theURL, bool shutdownOnOpen);
	virtual bool					DebugKeyDown(int theKey);
	virtual void					HandleCmdLineParam(const std::string& theParamName, const std::string& theParamValue);
	void							ConfirmQuit();
	void							ConfirmCheckForUpdates() { ; }
	void							CheckForUpdates() { ; }
	void							DoUserDialog();
	void							FinishUserDialog(bool isYes);
	void							DoCreateUserDialog();
	void							DoCheatDialog();
	void							FinishCheatDialog(bool isYes);
	void							FinishCreateUserDialog(bool isYes);
	void							DoConfirmDeleteUserDialog(const SexyString& theName);
	void							FinishConfirmDeleteUserDialog(bool isYes);
	void							DoRenameUserDialog(const SexyString& theName);
	void							FinishRenameUserDialog(bool isYes);
	void							FinishNameError(int theId);
	void							FinishRestartConfirmDialog();
	void							DoConfirmSellDialog(const SexyString& theMessage);
	void							DoConfirmPurchaseDialog(const SexyString& theMessage);
	void							FinishTimesUpDialog();
	void							KillBoard();
	void							MakeNewBoard();
	void							StartPlaying();
	bool							TryLoadGame();
	bool							TryLoadGame(int theLevel);
	void							NewGame();
	void							PreNewGame(GameMode theGameMode, bool theLookForSavedGame);
	void							PreNewGame(GameMode theGameMode, bool theLookForSavedGame, int theLevel, bool theReplay = false);
	void							ShowGameSelector();
	void							KillGameSelector();
	void							ShowAwardScreen(AwardType theAwardType, bool theShowAchievements);
	void							KillAwardScreen();
	void							ShowSeedChooserScreen();
	void							KillSeedChooserScreen();
	void							DoHighScoreDialog();
	void							DoBackToMain();
	void							DoConfirmBackToMain();
	void							DoNewOptions(bool theFromGameSelector);
	void							DoRegister();
	void							DoRegisterError();
	bool							CanDoRegisterDialog();
	           bool					WriteCurrentUserConfig();
	void							DoNeedRegisterDialog();
	void							DoContinueDialog();
	void							DoPauseDialog();
	void							FinishModelessDialogs();
	virtual Dialog*					DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
	virtual Dialog*					DoDialogDelay(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
	virtual void					Shutdown();
	virtual void					Init();
	virtual void					Start();
	virtual Dialog*					NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
	virtual bool					KillDialog(int theDialogId);
	virtual void					ModalOpen();
	virtual void					ModalClose();
	virtual void					PreDisplayHook();
	virtual bool					ChangeDirHook(const char* theIntendedPath);
	virtual bool					NeedRegister();
	virtual void					UpdateRegisterInfo();
	virtual void					ButtonPress(int theId);
	virtual void					ButtonDepress(int theId);
	virtual void					UpdateFrames();
	virtual bool					UpdateApp();
	           bool					IsAdventureMode();
	           bool					IsSurvivalMode();
	bool							IsContinuousChallenge();
	           bool					IsArtChallenge();
	bool							NeedPauseGame();
	virtual void					ShowResourceError(bool doExit = false);
	void							ToggleSlowMo();
	void							ToggleFastMo();
	void							PlayFoley(FoleyType theFoleyType);
	void							PlayFoleyPitch(FoleyType theFoleyType, float thePitch);
	void							PlaySample(int theSoundNum);
	void							FastLoad(GameMode theGameMode);
	static SexyString				GetStageString(int theLevel);
	           void					KillChallengeScreen();
	void							ShowChallengeScreen(ChallengePage thePage);
	ChallengeDefinition&			GetCurrentChallengeDef();
	void							CheckForGameEnd();
	virtual void					CloseRequestAsync();
	           bool					IsChallengeWithoutSeedBank();
	AlmanacDialog*					DoAlmanacDialog(SeedType theSeedType = SeedType::SEED_NONE, ZombieType theZombieType = ZombieType::ZOMBIE_INVALID);
	bool							KillAlmanacDialog();
	int								GetSeedsAvailable();
	Reanimation*					AddReanimation(float theX, float theY, int theRenderOrder, ReanimationType theReanimationType);
	TodParticleSystem*				AddTodParticle(float theX, float theY, int theRenderOrder, ParticleEffect theEffect);
	           ParticleSystemID		ParticleGetID(TodParticleSystem* theParticle);
	           TodParticleSystem*	ParticleGet(ParticleSystemID theParticleID);
	           TodParticleSystem*	ParticleTryToGet(ParticleSystemID theParticleID);
	           ReanimationID		ReanimationGetID(Reanimation* theReanimation);
	           Reanimation*			ReanimationGet(ReanimationID theReanimationID);
	           Reanimation*			ReanimationTryToGet(ReanimationID theReanimationID);
	void							RemoveReanimation(ReanimationID theReanimationID);
	void							RemoveParticle(ParticleSystemID theParticleID);
	StoreScreen*					ShowStoreScreen();
	void							KillStoreScreen();
	bool							HasSeedType(SeedType theSeedType);
	           bool					SeedTypeAvailable(SeedType theSeedType);
	           void					EndLevel();
	inline bool						IsIceDemo() { return false; }
	           bool					IsShovelLevel();
	           bool					IsWallnutBowlingLevel();
	           bool					IsMiniBossLevel();
	           bool					IsSlotMachineLevel();
	           bool					IsLittleTroubleLevel();
	           bool					IsStormyNightLevel();
	           bool					IsFinalBossLevel();
	           bool					IsBungeeBlitzLevel();
	static            SeedType		GetAwardSeedForLevel(int theLevel);
	SexyString						GetCrazyDaveText(int theMessageIndex);
	           bool					CanShowAlmanac();
	           bool					IsNight();
	           bool					CanShowStore();
	           bool					HasBeatenChallenge(GameMode theGameMode);
	PottedPlant*					GetPottedPlantByIndex(int thePottedPlantIndex);
	static            bool			IsSurvivalNormal(GameMode theGameMode);
	static            bool			IsSurvivalHard(GameMode theGameMode);
	static            bool			IsSurvivalEndless(GameMode theGameMode);
	           bool					HasFinishedAdventure();
	           bool					IsFirstTimeAdventureMode();
	           bool					CanSpawnYetis();
	void							CrazyDaveEnter();
	void							UpdateCrazyDave();
	void							CrazyDaveTalkIndex(int theMessageIndex);
	void							CrazyDaveTalkMessage(const SexyString& theMessage);
	void							CrazyDaveLeave();
	void							DrawCrazyDave(Graphics* g);
	void							CrazyDaveDie();
	void							CrazyDaveStopTalking();
	void							PreloadForUser();
	int								GetNumPreloadingTasks();
	int								LawnMessageBox(int theDialogId, const SexyChar* theHeaderName, const SexyChar* theLinesName, const SexyChar* theButton1Name, const SexyChar* theButton2Name, int theButtonMode);
	virtual void					EnforceCursor();
	void							ShowCreditScreen();
	void							KillCreditScreen();
	static SexyString				Pluralize(int theCount, const SexyChar* theSingular, const SexyChar* thePlural);
	int								GetNumTrophies(ChallengePage thePage);
	           bool					EarnedGoldTrophy();
	inline bool						IsRegistered() { return false; }
	inline bool						IsExpired() { return false; }
	inline bool						IsDRMConnected() { return false; }
	           bool					IsScaryPotterLevel();
	static            bool			IsEndlessScaryPotter(GameMode theGameMode);
	           bool					IsSquirrelLevel();
	           bool					IsIZombieLevel();
	           bool					CanShowZenGarden();
	static SexyString				GetMoneyString(int theAmount);
	bool							AdvanceCrazyDaveText();
	           bool					IsWhackAZombieLevel();
	void							BetaSubmit(bool theAskForComments);
	void							BetaRecordLevelStats();
	void							UpdatePlayTimeStats();
	void							BetaAddFile(std::list<std::string>& theUploadFileList, std::string theFileName, std::string theShortName);
	bool							CanPauseNow();
	           bool					IsPuzzleMode();
	           bool					IsChallengeMode();
	static            bool			IsEndlessIZombie(GameMode theGameMode);
	void							CrazyDaveDoneHanding();
	inline SexyString				GetCurrentLevelName() { return _S("Unknown"); }
	           int					TrophiesNeedForGoldSunflower();
	           int					GetCurrentChallengeIndex();
	void							LoadGroup(const char* theGroupName, int theGroupAveMsToLoad);
	void							TraceLoadGroup(const char* theGroupName, int theGroupTime, int theTotalGroupWeigth, int theTaskWeight);
	void							CrazyDaveStopSound();
	           bool					IsTrialStageLocked();
	           void					FinishZenGardenToturial();
	bool							UpdatePlayerProfileForFinishingLevel();
	bool							SaveFileExists();
	           bool					CanDoPinataMode();
	           bool					CanDoDanceMode();
	           bool					CanDoDaisyMode();
	virtual void					SwitchScreenMode(bool wantWindowed, bool is3d, bool force = false);
	static            void			CenterDialog(Dialog* theDialog, int theWidth, int theHeight);
#ifdef _HAS_ZOMBATAR
	void							ShowZombatarTOS();
#endif
	void							ShowLanagugeScreen();
	void							KillLanguageScreen();
	static bool						ChallengeUsesMicrophone(GameMode theGameMode);
	static bool						ChallengeHasScores(GameMode theGameMode);
	bool							IsLastStand();
	static            bool			IsLastStandEndless(GameMode theGameMode);

	static int						AudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
	
	void							ShowParticleEditor();
	bool							TryToInitializePA();

	void							DoMoreSettingsDialog();
	void							KillMoreSettingsDialog();
	void							MakeWindow();
	bool							DrawDirtyStuff();
	void							Redraw(Rect* theClipRect);
	bool							UpdateAppStep(bool* updated);

	bool							PlayVideo(std::string url, bool isSkipable = false, Color bgColor = Color::White);
};

SexyString							LawnGetCurrentLevelName();
bool								LawnGetCloseRequest();
bool								LawnHasUsedCheatKeys();
void								BetaSubmitFunc();

extern bool (*gAppCloseRequest)();				            
extern bool (*gAppHasUsedCheatKeys)();			            
extern SexyString (*gGetCurrentLevelName)();

extern bool gIsPartnerBuild;
extern bool gFastMo;            
extern bool gSlowMo;            
extern LawnApp* gLawnApp;            
extern int gSlowMoCounter;            
#endif	                