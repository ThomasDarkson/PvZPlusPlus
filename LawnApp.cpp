#include <time.h>
#include "LawnApp.h"
#include "Lawn/Board.h"
#include "Lawn/Plant.h"
#include "Lawn/Zombie.h"
#include "Lawn/Cutscene.h"
#include "GameConstants.h"
#include "Lawn/Challenge.h"
#include "Lawn/ZenGarden.h"
#include "Sexy.TodLib/Trail.h"
#include "Lawn/System/Music.h"
#include "Lawn/System/SaveGame.h"
#include "Sexy.TodLib/TodDebug.h"
#include "Sexy.TodLib/TodFoley.h"
#include "Sexy.TodLib/Attachment.h"
#include "Lawn/System/PlayerInfo.h"
#include "Lawn/System/PoolEffect.h"
#include "Lawn/System/ProfileMgr.h"
#include "Lawn/System/PopDRMComm.h"
#include "Lawn/Widget/GameButton.h"
#include "Sexy.TodLib/Reanimator.h"
#include "Lawn/Widget/UserDialog.h"
#include "Lawn/System/TypingCheck.h"
#include "Sexy.TodLib/TodParticle.h"
#include "Lawn/Widget/AwardScreen.h"
#include "Lawn/Widget/TitleScreen.h"
#include "Lawn/Widget/StoreScreen.h"
#include "Lawn/Widget/CheatDialog.h"
#include "Lawn/Widget/GameSelector.h"
#if defined(OPENFL_HOST) && defined(_HAS_LEVELSELECTOR)
#include "Lawn/Widget/QuickplayScreen.h"
#endif
#include "Lawn/Widget/CreditScreen.h"
#include "Sexy.TodLib/EffectSystem.h"
#include "Sexy.TodLib/FilterEffect.h"
#include "SexyAppFramework/Graphics.h"
#include "Sexy.TodLib/TodStringFile.h"
#include "Lawn/Widget/AlmanacDialog.h"
#include "Lawn/Widget/NewUserDialog.h"
#include "Lawn/Widget/ContinueDialog.h"
#include "Lawn/Widget/ZombatarTOS.h"
#include "Lawn/Widget/LanguageScreen.h"
#include "Lawn/System/ReanimationLawn.h"
#include "Lawn/Widget/ChallengeScreen.h"
#include "Lawn/Widget/NewOptionsDialog.h"
#include "Lawn/Widget/SeedChooserScreen.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ResourceManager.h"

#include "SexyAppFramework/Checkbox.h"
#include "SexyAppFramework/BassMusicInterface.h"
#include "SexyAppFramework/Dialog.h"
#include "SexyAppFramework/resource.h"

#include "Lawn/SeedPacket.h"

#include "Sexy.TodLib/Definition.h"

#include "portaudio.h"

#include "Editor/ParticleScreen.h"
#include "SexyAppFramework/D3DInterface.h"

#include "Lawn/Widget/MoreSettingsDialog.h"

#include <windows.h>
#include <windowsx.h>

bool gIsPartnerBuild = false;
bool gSlowMo = false;            
bool gFastMo = false;            
LawnApp* gLawnApp = nullptr;            
int gSlowMoCounter = 0;            

#include "SexyAppFramework/Debug.h"

#include <SDL3/SDL.h>
#include "SexyAppFramework/SDL3Image.h"
SDL_Window* LawnApp::mSDLWindow = nullptr;
SDL_Renderer* LawnApp::mSDLRenderer = nullptr;
SDL_Cursor* LawnApp::mSDLPointerCursor = nullptr;
SDL_Cursor* LawnApp::mSDLHandCursor = nullptr;
SDL_Cursor* LawnApp::mSDLDraggingCursor = nullptr;
SDL_Cursor* LawnApp::mSDLTextCursor = nullptr;
SDL_Cursor* LawnApp::mSDLWaitCursor = nullptr;
SDL_Cursor* LawnApp::mSDLNoCursor = nullptr;




#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

#include <thread>

bool LawnApp::mIsPlayingVideo = false;

                                                                    
bool LawnApp::PlayVideo(std::string url, bool isSkipable, Color bgColor)
{
	return false;                                                                     
}

void LawnApp::MakeWindow()
{
	throw std::runtime_error("OpenFL host must supply MakeWindow");
}

bool LawnApp::DrawDirtyStuff()
{
	return SexyAppBase::DrawDirtyStuff();
}

void LawnApp::Redraw(Rect* theClipRect)
{
	                                                       
}

static int mouseMoveCount = 0;
static int lastMouseX = -1;
static int lastMouseY = -1;

bool IsRealMouseMove(const SDL_Event& e)
{
	if (lastMouseX == -1)
	{
		lastMouseX = e.motion.x;
		lastMouseY = e.motion.y;
		return false;
	}

	if (e.motion.x != lastMouseX || e.motion.y != lastMouseY)
	{
		lastMouseX = e.motion.x;
		lastMouseY = e.motion.y;
		mouseMoveCount++;
	}

	return mouseMoveCount >= 4;
}

bool LawnApp::UpdateAppStep(bool* updated)
{
	return SexyAppBase::UpdateAppStep(updated);                                          
}

          
bool LawnGetCloseRequest()
{
	if (gLawnApp == nullptr)
		return false;

	return gLawnApp->mCloseRequest;
}

          
bool LawnHasUsedCheatKeys()
{
	return gLawnApp && gLawnApp->mPlayerInfo && gLawnApp->mPlayerInfo->mHasUsedCheatKeys;
}

          
LawnApp::LawnApp()
{
	mBoard = nullptr;
	mGameSelector = nullptr;
	mChallengeScreen = nullptr;
	mSeedChooserScreen = nullptr;
	mAwardScreen = nullptr;
	mCreditScreen = nullptr;
	mLanguageScreen = nullptr;
	mParticleScreen = nullptr;
	mTitleScreen = nullptr;
	mSoundSystem = nullptr;
	mKonamiCheck = nullptr;
	mMustacheCheck = nullptr;
	mMoustacheCheck = nullptr;
	mSuperMowerCheck = nullptr;
	mSuperMowerCheck2 = nullptr;
	mFutureCheck = nullptr;
	mPinataCheck = nullptr;
	mDanceCheck = nullptr;
	mDaisyCheck = nullptr;
	mSukhbirCheck = nullptr;
	mMustacheMode = false;
	mSuperMowerMode = false;
	mFutureMode = false;
	mPinataMode = false;
	mDanceMode = false;
	mDaisyMode = false;
	mSukhbirMode = false;
	mGameScene = GameScenes::SCENE_LOADING;
	mPoolEffect = nullptr;
	mZenGarden = nullptr;
	mEffectSystem = nullptr;
	mReanimatorCache = nullptr;
	mCloseRequest = false;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	mFullscreenBits = 32;
	mAppCounter = 0;
	mAppRandSeed = _time64(nullptr);
	mTrialType = TrialType::TRIALTYPE_NONE;
	mDebugTrialLocked = false;
	mMuteSoundsForCutscene = false;
	mMusicVolume = 0.85;
	mSfxVolume = 0.5525;
	mAutoStartLoadingThread = false;
	mDebugKeysEnabled = false;
	mProdName = "PlantsVsZombies";
	SexyString aTitleName = _S("Plants vs. Zombies");
#ifdef _DEBUG
	aTitleName += _S(" ") + StringToSexyString(mProductVersion);
	aTitleName += _S(" DEBUG");
#endif
	mTitle = aTitleName;
	mCustomCursorsEnabled = false;
	mPlayerInfo = nullptr;
	mLastLevelStats = new LevelStats();
	mFirstTimeGameSelector = true;
	mGameMode = GameMode::GAMEMODE_ADVENTURE;
	mEasyPlantingCheat = false;
	mAutoEnable3D = true;
	Tod_SWTri_AddAllDrawTriFuncs();
	mLoadingZombiesThreadCompleted = true;
	mGamesPlayed = 0;
	mMaxExecutions = 0;
	mMaxPlays = 0;
	mMaxTime = 0;
	mCompletedLoadingThreadTasks = 0;
	mProfileMgr = new ProfileMgr();
	mRegisterResourcesLoaded = false;
	mTodCheatKeys = false;
	mCrazyDaveReanimID = ReanimationID::REANIMATIONID_NULL;
	mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_OFF;
	mCrazyDaveBlinkCounter = 0;
	mCrazyDaveBlinkReanimID = ReanimationID::REANIMATIONID_NULL;
	mCrazyDaveMessageIndex = -1;
	mBigArrowCursor = LoadCursor(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDC_CURSOR1));
	mDRM = nullptr;
	mShowHealthBar = false;
	mVoiceVolume = 0.0f;
	memset(&mFlowersPlucked, false, sizeof(mFlowersPlucked));
	mPlayerLevelRef = -1;
}

                     
LawnApp::~LawnApp()
{
	if (mBoard)
	{
		WriteCurrentUserConfig();
	}

	if (mBoard)
	{
		mBoardResult = BoardResult::BOARDRESULT_QUIT_APP;
		mBoard->TryToSaveGame();
		mWidgetManager->RemoveWidget(mBoard);
		delete mBoard;
		mBoard = nullptr;
	}

	if (mTitleScreen)
	{
		mWidgetManager->RemoveWidget(mTitleScreen);
		delete mTitleScreen;
	}

	delete mSoundSystem;
	delete mMusic;

	if (mKonamiCheck)
	{
		delete mKonamiCheck;
	}
	if (mMustacheCheck)
	{
		delete mMustacheCheck;
	}
	if (mMoustacheCheck)
	{
		delete mMoustacheCheck;
	}
	if (mSuperMowerCheck)
	{
		delete mSuperMowerCheck;
	}
	if (mSuperMowerCheck2)
	{
		delete mSuperMowerCheck2;
	}
	if (mFutureCheck)
	{
		delete mFutureCheck;
	}
	if (mPinataCheck)
	{
		delete mPinataCheck;
	}
	if (mDanceCheck)
	{
		delete mDanceCheck;
	}
	if (mDaisyCheck)
	{
		delete mDaisyCheck;
	}
	if (mSukhbirCheck)
	{
		delete mSukhbirCheck;
	}

	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		delete mGameSelector;
	}
	if (mChallengeScreen)
	{
		mWidgetManager->RemoveWidget(mChallengeScreen);
		delete mChallengeScreen;
	}
	if (mSeedChooserScreen)
	{
		mWidgetManager->RemoveWidget(mSeedChooserScreen);
		delete mSeedChooserScreen;
	}
	if (mAwardScreen)
	{
		mWidgetManager->RemoveWidget(mAwardScreen);
		delete mAwardScreen;
	}
	if (mCreditScreen)
	{
		mWidgetManager->RemoveWidget(mCreditScreen);
		delete mCreditScreen;
	}
	if (mLanguageScreen)
	{
		mWidgetManager->RemoveWidget(mLanguageScreen);
		delete mLanguageScreen;
	}
	if (mParticleScreen)
	{
		mWidgetManager->RemoveWidget(mParticleScreen);
		delete mParticleScreen;
	}

	delete mProfileMgr;
	delete mLastLevelStats;

	mResourceManager->DeleteResources("");
#ifdef _DEBUG
	BetaSubmit(true);
#endif
}

          
void LawnApp::Shutdown()
{
	if (!mLoadingThreadCompleted)
	{
		mLoadingFailed = true;
		return;
	}

	if (!mShutdown)
	{
		for (int i = 0; i < Dialogs::NUM_DIALOGS; i++)
		{
			KillDialog(i);
		}

		if (mBoard)
		{
			mBoardResult = BoardResult::BOARDRESULT_QUIT_APP;
			mBoard->TryToSaveGame();
			KillBoard();
			WriteCurrentUserConfig();
		}

#ifdef _HAS_ZOMBATAR
		if (mGameSelector && mGameSelector->mZombatarWidget && mGameSelector->mZombatarWidget->mZombie) {
			mGameSelector->mZombatarWidget->mZombie->DieNoLoot();
			delete mGameSelector->mZombatarWidget->mZombie;
			mGameSelector->mZombatarWidget->mZombie = nullptr;
		}
#endif

		ProcessSafeDeleteList();

		if (mPoolEffect)
		{
			mPoolEffect->PoolEffectDispose();
			delete mPoolEffect;
			mPoolEffect = nullptr;
		}

		if (mZenGarden)
		{
			delete mZenGarden;
			mZenGarden = nullptr;
		}

		if (mEffectSystem)
		{
			mEffectSystem->EffectSystemDispose();
			delete mEffectSystem;
			mEffectSystem = nullptr;
		}

		if (mReanimatorCache)
		{
			mReanimatorCache->ReanimatorCacheDispose();
			delete mReanimatorCache;
			mReanimatorCache = nullptr;
		}

		DisposeZombatarClothesCache();
		FilterEffectDisposeForApp();
		TodParticleFreeDefinitions();
		ReanimatorFreeDefinitions();
		TrailFreeDefinitions();
		FreeGlobalAllocators();
		UpdateRegisterInfo();

		if (mPortAudioStream)
		{
			Pa_StopStream(mPortAudioStream);
			Pa_CloseStream(mPortAudioStream);
			Pa_Terminate();

			mPortAudioStream = nullptr;
		}

		SexyAppBase::Shutdown();





		                                                                       
		                                          
		                                                       

		if (mDRM)
		{
			delete mDRM;
		}
		mDRM = nullptr;
	}
}

          
void LawnApp::KillBoard()
{
	if (mPortAudioStream)
	{
		Pa_StopStream(mPortAudioStream);
		Pa_CloseStream(mPortAudioStream);
		Pa_Terminate();

		mPortAudioStream = nullptr;
	}

	FinishModelessDialogs();
	KillSeedChooserScreen();
	if (mBoard)
	{
#ifdef _DEBUG
		BetaRecordLevelStats();
#endif
		mBoard->DisposeBoard();
		mWidgetManager->RemoveWidget(mBoard);
		SafeDeleteWidget(mBoard);
		mBoard = nullptr;
	}

	SetCursor(CURSOR_POINTER);
}

          
bool LawnApp::CanPauseNow()
{
	if (mBoard == nullptr)                    
		return false;

	if (mSeedChooserScreen && mSeedChooserScreen->mMouseVisible)                       
		return false;

	if (mBoard->mBoardFadeOutCounter >= 0)                          
		return false;

	if (mCrazyDaveState != CrazyDaveState::CRAZY_DAVE_OFF)                 
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)                                   
		return false;

	return GetDialogCount() <= 0;                    
}

void LawnApp::GotFocus()
{
}

          
void LawnApp::LostFocus()
{
	if (!mTodCheatKeys && CanPauseNow())
	{
		DoPauseDialog();
	}
}

          
void LawnApp::WriteToRegistry()
{
	if (mPlayerInfo)
	{
		RegistryWriteString("CurUser", SexyStringToString(mPlayerInfo->mName));
		mPlayerInfo->SaveDetails();
	}

	SexyAppBase::WriteToRegistry();
}

          
void LawnApp::ReadFromRegistry()
{
	SexyApp::ReadFromRegistry();
}

          
bool LawnApp::WriteCurrentUserConfig()
{
	if (mPlayerInfo)
		mPlayerInfo->SaveDetails();

	return true;
}

          
void LawnApp::PreNewGame(GameMode theGameMode, bool theLookForSavedGame)
{
    mPendingAdventureLevel = 0;
    mAdventureReplay = false;
	                     
	   
	                      
	          
	   

    mGameMode = theGameMode;
    if (mLocalCoop)
    {
        NewGame();
        return;
    }
	if (theLookForSavedGame && TryLoadGame())
		return;

	SexyString aFileName = GetSavedGameName(mGameMode, mPlayerInfo->mId);
	EraseFile(aFileName);
	NewGame();
}

int LawnApp::GetAdventureLevel() const
{
    if (mPendingAdventureLevel > 0) return mPendingAdventureLevel;
    if (mBoard && mBoard->mLevel > 0) return mBoard->mLevel;
    return mPlayerInfo ? mPlayerInfo->mLevel : 1;
}

void LawnApp::PreNewGame(GameMode theGameMode, bool theLookForSavedGame, int theLevel, bool theReplay)
{
    if (theLevel < 1 || theLevel > FINAL_LEVEL)
        throw std::runtime_error("Adventure level must be between 1 and 50");
    mGameMode = theGameMode;
    mPendingAdventureLevel = theLevel;
    mAdventureReplay = theReplay;
    try
    {
        if (!theReplay && theLookForSavedGame && TryLoadGame(theLevel))
        {
            mPendingAdventureLevel = 0;
            return;
        }
                                                                                  
        if (!theReplay) EraseFile(GetSavedGameName(mGameMode, mPlayerInfo->mId, theLevel));
        NewGame();
        mPendingAdventureLevel = 0;
    }
    catch (...)
    {
        mPendingAdventureLevel = 0;
        throw;
    }
}

          
void LawnApp::MakeNewBoard()
{
	KillBoard();
	mBoard = new Board(this);
	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);
	mWidgetManager->BringToBack(mBoard);
	mWidgetManager->SetFocus(mBoard);
	if (!mPortAudioStream && ChallengeUsesMicrophone(mGameMode))
	{
		if (!TryToInitializePA())
		{
			DoDialog(Dialogs::DIALOG_INFO, true, "Error", "Something went wrong when starting Port Audio.\n\n Please check your Audio driver or whether your system meets the requirements to support PortAudio. Please contact the devoleper", _S("[OK_LABEL]"), Dialog::BUTTONS_FOOTER);
			mPortAudioStream = nullptr;
			DoBackToMain();
			return;
		}
	}
}

          
void LawnApp::StartPlaying()
{
	KillSeedChooserScreen();
	mBoard->StartLevel();
	mGameScene = GameScenes::SCENE_PLAYING;
}

          
bool LawnApp::SaveFileExists()
{
	SexyString aFileName = GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mPlayerInfo->mId);
	return this->FileExists(aFileName);
}

          
bool LawnApp::TryLoadGame()
{
	return TryLoadGame(mPlayerInfo->mLevel);
}

bool LawnApp::TryLoadGame(int theLevel)
{
	SexyString aSaveName = GetSavedGameName(mGameMode, mPlayerInfo->mId, theLevel);
	mMusic->StopAllMusic();

	if (this->FileExists(aSaveName))
	{
		MakeNewBoard();
		if (mBoard->LoadGame(aSaveName))
		{
			mFirstTimeGameSelector = false;
			DoContinueDialog();
			return true;
		}

		KillBoard();
	}

	return false;
}


          
void LawnApp::NewGame()
{
    if (mLocalCoop)
    {
        mCoopSun = 50;
        mCoopNextConveyor = 0;
        mCoopSuns.clear();
    }
	mFirstTimeGameSelector = false;

	MakeNewBoard();
	if (!mBoard) return;
	mBoard->InitLevel();
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mGameScene = GameScenes::SCENE_LEVEL_INTRO;

	ShowSeedChooserScreen();
	mBoard->mCutScene->StartLevelIntro();
}

          
void LawnApp::ShowGameSelector()
{
    mLocalCoop = false;
    mCoopHasPartner = false;
    mCoopSuns.clear();
	KillBoard();
    mAdventureReplay = false;
    mPendingAdventureLevel = 0;
	                       
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
	}
	mGameMode = GameMode::GAMEMODE_ADVENTURE;
	mGameScene = GameScenes::SCENE_MENU;
	mGameSelector = new GameSelector(this);
	mGameSelector->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mGameSelector);
	mWidgetManager->BringToBack(mGameSelector);
	mWidgetManager->SetFocus(mGameSelector);

	                     
	   
	                          
	   
}

          
void LawnApp::KillGameSelector()
{
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
		mGameSelector = nullptr;
	}
}

          
void LawnApp::ShowAwardScreen(AwardType theAwardType, bool theShowAchievements)
{
	mGameScene = GameScenes::SCENE_AWARD;
	mAwardScreen = new AwardScreen(this, theAwardType, theShowAchievements);
	mAwardScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mAwardScreen);
	mWidgetManager->BringToBack(mAwardScreen);
	mWidgetManager->SetFocus(mAwardScreen);
}

          
void LawnApp::KillAwardScreen()
{
	if (mAwardScreen)
	{
		mWidgetManager->RemoveWidget(mAwardScreen);
		SafeDeleteWidget(mAwardScreen);
		mAwardScreen = nullptr;
	}
}

          
void LawnApp::ShowCreditScreen()
{
	mCreditScreen = new CreditScreen(this);
	mCreditScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mCreditScreen);
	mWidgetManager->BringToBack(mCreditScreen);
	mWidgetManager->SetFocus(mCreditScreen);
}

          
void LawnApp::KillCreditScreen()
{
	if (mCreditScreen)
	{
		mWidgetManager->RemoveWidget(mCreditScreen);
		SafeDeleteWidget(mCreditScreen);
		mCreditScreen = nullptr;
	}
}

          
void LawnApp::ShowChallengeScreen(ChallengePage thePage)
{
	mGameScene = GameScenes::SCENE_CHALLENGE;
	mChallengeScreen = new ChallengeScreen(this, thePage);
	mChallengeScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mChallengeScreen);
	mWidgetManager->BringToBack(mChallengeScreen);
	mWidgetManager->SetFocus(mChallengeScreen);
}

          
void LawnApp::KillChallengeScreen()
{
	if (mChallengeScreen)
	{
		mWidgetManager->RemoveWidget(mChallengeScreen);
		SafeDeleteWidget(mChallengeScreen);
		mChallengeScreen = nullptr;
	}
}

          
StoreScreen* LawnApp::ShowStoreScreen()
{
	                          
	TOD_ASSERT(!GetDialog((int)Dialogs::DIALOG_STORE));

	StoreScreen* aStoreScreen = new StoreScreen(this);
	AddDialog(aStoreScreen);
	mWidgetManager->SetFocus(aStoreScreen);

	return aStoreScreen;
}

void LawnApp::KillStoreScreen()
{
	if (GetDialog(Dialogs::DIALOG_STORE))
	{
		KillDialog(Dialogs::DIALOG_STORE);
		ClearUpdateBacklog(false);
	}
}

          
void LawnApp::ShowSeedChooserScreen()
{
	TOD_ASSERT(mSeedChooserScreen == nullptr);

	mSeedChooserScreen = new SeedChooserScreen();
	mSeedChooserScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mSeedChooserScreen);
	mWidgetManager->BringToBack(mSeedChooserScreen);
}

          
void LawnApp::KillSeedChooserScreen()
{
	if (mSeedChooserScreen)
	{
		mWidgetManager->RemoveWidget(mSeedChooserScreen);
		SafeDeleteWidget(mSeedChooserScreen);
		mSeedChooserScreen = nullptr;
	}
}

void LawnApp::EndLevel()
{
	KillBoard();
	if (IsAdventureMode())
	{
		NewGame();
	}

	mFirstTimeGameSelector = true;

	MakeNewBoard();
	mBoard->InitLevel();
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mGameScene = GameScenes::SCENE_LEVEL_INTRO;
	ShowSeedChooserScreen();
	mBoard->mCutScene->StartLevelIntro();
}

          
void LawnApp::DoBackToMain()
{
	mMusic->StopAllMusic();
	mSoundSystem->CancelPausedFoley();
	WriteCurrentUserConfig();
	KillNewOptionsDialog();
	KillBoard();
	ShowGameSelector();
}

          
void LawnApp::DoConfirmBackToMain()
{
	LawnDialog* aDialog = (LawnDialog*)DoDialog(
		Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN, 
		true, 
		_S("Leave Game?"                  ),
		mLocalCoop ? _S("Do you want to return to the main menu?") : _S("Do you want to return\nto the main menu?\n\nYour game will be saved."), 
		_S(""), 
		Dialog::BUTTONS_YES_NO
	);

	aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[LEAVE_BUTTON]")); 
	aDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_CANCEL]"));
	                          
}

          
void LawnApp::DoNewOptions(bool theFromGameSelector)
{
	                          

	NewOptionsDialog* aDialog = new NewOptionsDialog(this, theFromGameSelector);
#ifdef OPENFL_HOST
    CenterDialog(aDialog, IMAGE_OPTIONS_MENUBACK->mWidth, IMAGE_OPTIONS_MENUBACK->mHeight + 119);
#else
	CenterDialog(aDialog, IMAGE_OPTIONS_MENUBACK->mWidth, IMAGE_OPTIONS_MENUBACK->mHeight);
#endif
	AddDialog(Dialogs::DIALOG_NEWOPTIONS, aDialog);
	mWidgetManager->SetFocus(aDialog);
}

          
AlmanacDialog* LawnApp::DoAlmanacDialog(SeedType theSeedType, ZombieType theZombieType)
{
	PerfTimer mTimer;
	mTimer.Start();

	                          

	AlmanacDialog* aDialog = new AlmanacDialog(this);
	AddDialog(Dialogs::DIALOG_ALMANAC, aDialog);
	mWidgetManager->SetFocus(aDialog);

	if (theSeedType != SeedType::SEED_NONE)
	{
		aDialog->ShowPlant(theSeedType);
	}
	else if (theZombieType != ZombieType::ZOMBIE_INVALID)
	{
		aDialog->ShowZombie(theZombieType);
	}

	int aDuration = mTimer.GetDuration();
	TodTrace("almanac load time: %d ms", aDuration);

#ifdef OPENFL_HOST
    pvz_host_achievement_almanac(this);
#endif

	return aDialog;
}

          
void LawnApp::DoContinueDialog()
{
	ContinueDialog* aDialog = new ContinueDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CONTINUE, aDialog);
}

          
void LawnApp::DoPauseDialog()
{
	mBoard->Pause(true);
	                          

	LawnDialog* aDialog = (LawnDialog*)DoDialog(
		Dialogs::DIALOG_PAUSED,
		true,
		_S("GAME PAUSED"                   ),
		_S("Click to resume game"), 
		_S("Resume Game"                   ),
		Dialog::BUTTONS_FOOTER
	);

	aDialog->mReanimation->AddReanimation(72.0f, 42.0f, ReanimationType::REANIM_ZOMBIE_NEWSPAPER);
	aDialog->mSpaceAfterHeader = 155;
	aDialog->CalcSize(0, 10);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
}

          
int LawnApp::LawnMessageBox(int theDialogId, const SexyChar* theHeaderName, const SexyChar* theLinesName, const SexyChar* theButton1Name, const SexyChar* theButton2Name, int theButtonMode)
{
	Widget* aOldFocus = mWidgetManager->mFocusWidget;

	LawnDialog* aDialog = (LawnDialog*)DoDialog(theDialogId, true, theHeaderName, theLinesName, theButton1Name, theButtonMode);
	if (aDialog->mYesButton)
	{
		aDialog->mYesButton->mLabel = TodStringTranslate(theButton1Name);
	}
	if (aDialog->mNoButton)
	{
		aDialog->mNoButton->mLabel = TodStringTranslate(theButton2Name);
	}
	                          

	mWidgetManager->SetFocus(aDialog);
	int aResult = aDialog->WaitForResult(true);
	mWidgetManager->SetFocus(aOldFocus);

	return aResult;
}

          
Dialog* LawnApp::DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	SexyString aHeader = TodStringTranslate(theDialogHeader);
	SexyString aLines = TodStringTranslate(theDialogLines);
	SexyString aFooter = TodStringTranslate(theDialogFooter);

	Dialog* aDialog = SexyAppBase::DoDialog(theDialogId, isModal, aHeader, aLines, aFooter, theButtonMode);
	if (mWidgetManager->mFocusWidget == nullptr)
	{
		mWidgetManager->mFocusWidget = aDialog;
	}

	return aDialog;
}

Dialog* LawnApp::DoDialogDelay(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	LawnDialog* aDialog = (LawnDialog*)SexyAppBase::DoDialog(theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);
	aDialog->SetButtonDelay(30);
	return aDialog;
}

          
void LawnApp::DoUserDialog()
{
	KillDialog(Dialogs::DIALOG_USERDIALOG);

	UserDialog* aDialog = new UserDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_USERDIALOG, aDialog);
	mWidgetManager->SetFocus(aDialog);
}

          
void LawnApp::FinishUserDialog(bool isYes)
{
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (aUserDialog)
	{
		if (isYes)
		{
			PlayerInfo* aProfile = mProfileMgr->GetProfile(aUserDialog->GetSelName());
			if (aProfile)
			{
				mPlayerInfo = aProfile;
				mWidgetManager->MarkAllDirty();

				if (mGameSelector)
				{
					mGameSelector->SyncProfile(true);
				}
			}
			mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
		}

		KillDialog(Dialogs::DIALOG_USERDIALOG);
	}
}

          
void LawnApp::DoCreateUserDialog()
{
	KillDialog(Dialogs::DIALOG_CREATEUSER);

	NewUserDialog* aDialog = new NewUserDialog(this, false);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CREATEUSER, aDialog);
}

          
void LawnApp::FinishCreateUserDialog(bool isYes)
{
	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(Dialogs::DIALOG_CREATEUSER);
	if (aNewUserDialog == nullptr)
		return;

	SexyString aName = aNewUserDialog->GetName();

	if (isYes && aName.empty())
	{
		DoDialog(
			Dialogs::DIALOG_CREATEUSERERROR,
			true,
			_S("Enter Your Name"),
			_S("Please enter your name to create a new user profile for storing high score data and game progress"),
			_S("OK"),
			Dialog::BUTTONS_FOOTER
		);
	}
	else if (mPlayerInfo == nullptr && (!isYes || aName.empty()))
	{
		DoDialog(
			Dialogs::DIALOG_CREATEUSERERROR,
			true,
			_S("Enter Your Name"                       ),
			_S("Please enter your name to create a new user profile for storing high score data and game progress"                      ),
			_S("OK"                        ),
			Dialog::BUTTONS_FOOTER
		);
	}
	else if (!isYes)
	{
		KillDialog(Dialogs::DIALOG_CREATEUSER);
	}
	else
	{
		PlayerInfo* aProfile = mProfileMgr->AddProfile(aName);
		if (aProfile == nullptr)
		{
			DoDialog(
				Dialogs::DIALOG_CREATEUSERERROR,
				true,
				_S("Name Conflict"                     ),
				_S("The name you entered is already being used.  Please enter a unique player name"                                ),
				_S("OK"                        ),
				Dialog::BUTTONS_FOOTER
			);
		}
		else
		{
			mProfileMgr->Save();
			mPlayerInfo = aProfile;

			                                   

                                                                
   
                         
    
                                  
                                                
                            
          
                    
                                          
                    
                           
      

                                                                                            
                                                                                                               
                                     
                             
                                                             
      

			KillDialog(Dialogs::DIALOG_USERDIALOG);
			KillDialog(Dialogs::DIALOG_CREATEUSER);
			mWidgetManager->MarkAllDirty();

			if (mGameSelector)
			{
				mGameSelector->SyncProfile(true);
			}
		}
	}
}

          
void LawnApp::DoConfirmDeleteUserDialog(const SexyString& theName)
{
	KillDialog(Dialogs::DIALOG_CONFIRMDELETEUSER);
	DoDialog(
		Dialogs::DIALOG_CONFIRMDELETEUSER, 
		true, 
		_S("Are You Sure"                    ), 
		                                                                                                      
		StrFormat(_S("This will permanently remove '%s' from the player roster!"), theName.c_str()),
		_S(""), 
		Dialog::BUTTONS_YES_NO
	);
}

          
void LawnApp::FinishConfirmDeleteUserDialog(bool isYes)
{
	KillDialog(Dialogs::DIALOG_CONFIRMDELETEUSER);
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (aUserDialog == nullptr)
		return;

	mWidgetManager->SetFocus(aUserDialog);

	if (!isYes)
		return;

	SexyString aCurName = mPlayerInfo ? mPlayerInfo->mName : _S("");
	SexyString aName = aUserDialog->GetSelName();
	if (aName == aCurName)
	{
		mPlayerInfo = nullptr;
	}

	mProfileMgr->DeleteProfile(aName);
	aUserDialog->FinishDeleteUser();
	if (mPlayerInfo == nullptr)
	{
		mPlayerInfo = mProfileMgr->GetProfile(aUserDialog->GetSelName());
		if (mPlayerInfo == nullptr)
		{
			mPlayerInfo = mProfileMgr->GetAnyProfile();
		}
		mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
	}

	mProfileMgr->Save();
	if (mPlayerInfo == nullptr)
	{
		DoCreateUserDialog();
	}

	mWidgetManager->MarkAllDirty();
	if (mGameSelector != nullptr)
	{
		mGameSelector->SyncProfile(true);
	}
}

          
void LawnApp::DoRenameUserDialog(const SexyString& theName)
{
	KillDialog(Dialogs::DIALOG_RENAMEUSER);

	NewUserDialog* aDialog = new NewUserDialog(this, true);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	aDialog->SetName(theName);
	AddDialog(Dialogs::DIALOG_RENAMEUSER, aDialog);
}

          
void LawnApp::FinishRenameUserDialog(bool isYes)
{
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (!isYes)
	{
		KillDialog(Dialogs::DIALOG_RENAMEUSER);
		mWidgetManager->SetFocus(aUserDialog);
		return;
	}

	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(Dialogs::DIALOG_RENAMEUSER);
	if (aUserDialog == nullptr || aNewUserDialog == nullptr)
		return;

	SexyString anOldName = aUserDialog->GetSelName();
	SexyString aNewName = aNewUserDialog->GetName();
	if (aNewName.empty())
		return;
	
	bool isCurrentUser = mProfileMgr->GetProfile(anOldName) == mPlayerInfo;
	if (!mProfileMgr->RenameProfile(anOldName, aNewName))
	{
		DoDialog(
			Dialogs::DIALOG_RENAMEUSERERROR,
			true,
			_S("Name Conflict"                     ),
			_S("The name you entered is already being used.  Please enter a unique player name"                                ),
			_S("OK"                        ),
			Dialog::BUTTONS_FOOTER
		);
		return;
	}

	mProfileMgr->Save();
	if (isCurrentUser)
	{
		mPlayerInfo = mProfileMgr->GetProfile(aNewName);
	}
	mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;

	aUserDialog->FinishRenameUser(aNewName);
	mWidgetManager->MarkAllDirty();
	KillDialog(Dialogs::DIALOG_RENAMEUSER);
	mWidgetManager->SetFocus(aUserDialog);
}

          
void LawnApp::FinishNameError(int theId)
{
	KillDialog(theId);

	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(theId == Dialogs::DIALOG_CREATEUSERERROR ? Dialogs::DIALOG_CREATEUSER : Dialogs::DIALOG_RENAMEUSER);
	if (aNewUserDialog)
	{
		mWidgetManager->SetFocus(aNewUserDialog->mNameEditWidget);
	}
}

          
void LawnApp::FinishRestartConfirmDialog()
{
    if (!mBoard) return;
	mSawYeti = mBoard->mKilledYeti;
	int theLevel = -1;
    bool aReplay = mBoard && mBoard->mIsReplay;

	KillDialog(Dialogs::DIALOG_CONTINUE);
	KillDialog(Dialogs::DIALOG_RESTARTCONFIRM);
	if (mBoard && IsAdventureMode())
		theLevel = mBoard->mLevel;
	KillBoard();

	if (theLevel > 0)
		PreNewGame(mGameMode, false, theLevel, aReplay);
	else
		PreNewGame(mGameMode, false);
}

void LawnApp::DoCheatDialog()
{
	KillDialog(Dialogs::DIALOG_CHEAT);

	CheatDialog* aDialog = new CheatDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CHEAT, aDialog);
}

void LawnApp::FinishCheatDialog(bool isYes)
{
	CheatDialog* aCheatDialog = (CheatDialog*)GetDialog(Dialogs::DIALOG_CHEAT);
	if (aCheatDialog == nullptr)
		return;

	if (isYes && !aCheatDialog->ApplyCheat())
		return;

	KillDialog(Dialogs::DIALOG_CHEAT);
	if (isYes)
	{
		mMusic->StopAllMusic();
		mBoardResult = BoardResult::BOARDRESULT_CHEAT;
		PreNewGame(mGameMode, false);
	}
}

void LawnApp::FinishTimesUpDialog()
{
	KillDialog(Dialogs::DIALOG_TIMESUP);
}

void LawnApp::DoConfirmSellDialog(const SexyString& theMessage)
{
	Dialog* aConfirmDialog = DoDialog(Dialogs::DIALOG_ZEN_SELL, true, _S("[ZEN_SELL_HEADER]"), theMessage, _S(""), Dialog::BUTTONS_YES_NO);
	aConfirmDialog->mYesButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_YES]"));
	aConfirmDialog->mNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_NO]"));
}

void LawnApp::DoConfirmPurchaseDialog(const SexyString& theMessage)
{
	LawnDialog* aComfirmDialog = (LawnDialog*)DoDialog(Dialogs::DIALOG_STORE_PURCHASE, true, _S("买下这个物品？"), theMessage, _S(""), Dialog::BUTTONS_YES_NO);
	aComfirmDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_YES]"));
	aComfirmDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_NO]"));
}

          
Dialog* LawnApp::NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	LawnDialog* aDialog = new LawnDialog(
		this, 
		theDialogId, 
		isModal, 
		theDialogHeader, 
		theDialogLines, 
		theDialogFooter, 
		theButtonMode
	);

	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	return aDialog;
}

          
bool LawnApp::KillNewOptionsDialog()
{
	NewOptionsDialog* aNewOptionsDialog = (NewOptionsDialog*)GetDialog(Dialogs::DIALOG_NEWOPTIONS);
	if (aNewOptionsDialog == nullptr)
		return false;

	bool wantWindowed = !aNewOptionsDialog->mFullscreenCheckbox->IsChecked();
	                                                                              
	mEnableVsync = aNewOptionsDialog->mHardwareAccelerationCheckbox->IsChecked();
	RegistryWriteBoolean(_S("EnableVsync"), mEnableVsync);
#ifdef OPENFL_HOST
    pvz_host_set_fullscreen(!wantWindowed);
    pvz_host_set_vsync(mEnableVsync);
#else
	SDL_SetRenderVSync(mSDLRenderer, mEnableVsync);
	SwitchScreenMode(wantWindowed, true, false);
#endif

	KillDialog(Dialogs::DIALOG_NEWOPTIONS);
	ClearUpdateBacklog();
	return true;
}

          
bool LawnApp::KillAlmanacDialog()
{
	if (GetDialog(Dialogs::DIALOG_ALMANAC))
	{
		KillDialog(Dialogs::DIALOG_ALMANAC);
#ifdef OPENFL_HOST
		if (mSeedChooserScreen && mGameScene == GameScenes::SCENE_LEVEL_INTRO && !NeedPauseGame())
			mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);
#endif
		ClearUpdateBacklog(false);
		return true;
	}

	return false;
}

          
bool LawnApp::NeedPauseGame()
{
	if (mDialogList.size() == 0)
		return false;

	if (mDialogList.size() == 1 && mDialogList.front()->mId != Dialogs::DIALOG_NEW_GAME)
	{
		int anId = mDialogList.front()->mId;
		if (anId == Dialogs::DIALOG_CHOOSER_WARNING || anId == Dialogs::DIALOG_PURCHASE_PACKET_SLOT || anId == Dialogs::DIALOG_IMITATER)
		{
			return false;
		}
	}

	return (mBoard == nullptr || mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN) && (mBoard == nullptr || mGameMode != GameMode::GAMEMODE_TREE_OF_WISDOM);
}

          
void LawnApp::ModalOpen()
{
	if (mBoard && NeedPauseGame())
	{
		mBoard->Pause(true);
	}
}

void LawnApp::ModalClose()
{
}

          
bool LawnApp::KillDialog(int theDialogId)
{
	if (SexyAppBase::KillDialog(theDialogId))
	{
		if (mDialogMap.size() == 0)
		{
			if (mBoard)
			{
				mWidgetManager->SetFocus(mBoard);
			}
			else if (mGameSelector)
			{
				mWidgetManager->SetFocus(mGameSelector);
			}
		}

		if (mBoard && !NeedPauseGame())
		{
			mBoard->Pause(false);
		}

		return true;
	}

	return false;
}

          
void LawnApp::ShowResourceError(bool doExit)
{
	SexyAppBase::ShowResourceError(doExit);
}

void BetaSubmitFunc()
{
	if (gLawnApp)
	{
		gLawnApp->BetaSubmit(false);
	}
}

SDL_Cursor* CreateCursorFromResource(HINSTANCE hInstance, int resourceID, int hotX, int hotY)
{
	HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resourceID), RT_RCDATA);
	HGLOBAL hResData = LoadResource(hInstance, hRes);
	void* pData = LockResource(hResData);
	DWORD size = SizeofResource(hInstance, hRes);
	SDL_IOStream* io = SDL_IOFromConstMem(pData, size);
	SDL_Surface* surface = SDL_LoadBMP_IO(io, true);
	SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotX, hotY);
	SDL_DestroySurface(surface);
	return cursor;
}

               
             
unsigned char mFingerCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3,
	0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff,
	0xc0, 0x07, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xfc, 0x40, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff,
	0xfc, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0,
	0x03, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
	0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x18, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1b, 0x60, 0x00, 0x00, 0x1b, 0x68, 0x00,
	0x00, 0x1b, 0x6c, 0x00, 0x01, 0x9f, 0xec, 0x00, 0x01, 0xdf, 0xfc, 0x00, 0x00, 0xdf, 0xfc,
	0x00, 0x00, 0x5f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f,
	0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
	0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

                
             
unsigned char mDraggingCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xe0,
	0x01, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff,
	0xe0, 0x00, 0xff, 0xfe, 0x60, 0x00, 0xff, 0xfc, 0x20, 0x00, 0xff, 0xfc, 0x00, 0x00, 0xff,
	0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0,
	0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x80, 0x00, 0x00, 0x01, 0xb0, 0x00, 0x00, 0x0d, 0xb0, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00,
	0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00,
	0x01, 0x8d, 0xb6, 0x00, 0x01, 0xcf, 0xfe, 0x00, 0x00, 0xef, 0xfe, 0x00, 0x00, 0xff, 0xfe,
	0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x1f,
	0xfc, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00,
	0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

SDL_Cursor* CreateCursorFromRaw(const unsigned char* src, int w, int h, int hotX, int hotY)
{
	size_t maskSize = (w * h) / 8;
	SDL_Surface* surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB8888);

	Uint32* pixels = (Uint32*)surface->pixels;
	int pitch = surface->pitch / sizeof(Uint32);  
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			pixels[y * pitch + x] = 0x00000000;  
		}
	}
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int byteIndex = y * (w / 8) + (x / 8);
			int bitIndex = 7 - (x % 8); 

			Uint8 andBit = (src[byteIndex] >> bitIndex) & 1;
			Uint8 xorBit = (src[byteIndex + maskSize] >> bitIndex) & 1;

			if (andBit == 0) {  
				if (xorBit == 0) {
					pixels[y * pitch + x] = 0xFF000000;
				}
				else {
					pixels[y * pitch + x] = 0xFFFFFFFF;  
				}
			}
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotX, hotY);
	SDL_DestroySurface(surface);
	return cursor;
}

          
void LawnApp::Init()
{
	DoParseCmdLine();
	if (!mTodCheatKeys)
	{
		mOnlyAllowOneCopyToRun = false;
	}

	                                 
	                             
	                                                                                                 
	                                               
	   
	                            
	   
	                                                          
	   
	                            
	   

	mSessionID = _time32(nullptr);
	mPlayTimeActiveSession = 0;
	mPlayTimeInactiveSession = 0;
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mSawYeti = false;

	SexyApp::Init();
#ifdef OPENFL_HOST
	pvz_host_trace("BEGIN main resource manifest: properties/resources.xml");
#endif

#ifdef _DEBUG
	TodAssertInitForApp();
	gBetaSubmitFunc = BetaSubmitFunc;
	TodLog("session id: %u", mSessionID);
#endif

	
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	bool IsXMLPackLoaded = mResourceManager->ParseResourcesFile(_S("properties\\resources.xml"));
#ifdef OPENFL_HOST
	pvz_host_trace(IsXMLPackLoaded ? "END main resource manifest: OK" : "END main resource manifest: FAILED");
#endif

	if (GetFileAttributesExA("dependency\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("dependency\\properties\\resources.xml"))
	{
#ifdef OPENFL_HOST
		pvz_host_trace("BEGIN dependency resource manifest");
#endif
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("dependency\\properties\\resources.xml"));
#ifdef OPENFL_HOST
		pvz_host_trace("END dependency resource manifest");
#endif
	}
	else
	{
		MsgBox("dependency files missing! Please get the dependency.pak from the Stable Decompile Github Repo. Contact the developers.", "Error");
	}

	if (GetFileAttributesExA("extension\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("extension\\properties\\resources.xml"))
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("extension\\properties\\resources.xml"));

#ifdef _ALLOW_RESOURCE_PACKS
	if (GetFileAttributesExA("resourcepack\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("resourcepack\\properties\\resources.xml"))
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("resourcepack\\properties\\resources.xml"));
#endif

	if (!IsXMLPackLoaded)
	{
		ShowResourceError(true);
		return;
	}

#ifdef OPENFL_HOST
	pvz_host_trace("BEGIN Init resource group");
#endif
	if (!TodLoadResources("Init"))
	{
		return;
	}
#ifdef OPENFL_HOST
	pvz_host_trace("END Init resource group; BEGIN profile loading");
#endif

	PerfTimer mTimer;
	mTimer.Start();

	mProfileMgr->Load();
#ifdef OPENFL_HOST
	pvz_host_trace("END profile loading");
#endif

	std::string theUser;
	SexyString aCurUser;
	if (mPlayerInfo == nullptr && RegistryReadString("CurUser", &theUser))
	{
		aCurUser = StringToSexyString(theUser);
		mPlayerInfo = mProfileMgr->GetProfile(aCurUser);
		mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
	}
	if (mPlayerInfo == nullptr)
	{
		mPlayerInfo = mProfileMgr->GetAnyProfile();
	}

	mMaxExecutions = GetInteger("MaxExecutions", 0);
	mMaxPlays = GetInteger("MaxPlays", 0);
	mMaxTime = GetInteger("MaxTime", 60);

	mTitleScreen = new TitleScreen(this);
	mTitleScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mTitleScreen);
	mWidgetManager->SetFocus(mTitleScreen);
	
#ifdef _DEBUG
	int aDuration = mTimer.GetDuration();
	TodTrace("loading: 'profiles' %d ms", aDuration);
#endif
	mTimer.Start();

	                                                                       
	                                                              
	HINSTANCE hInstance = GetModuleHandle(NULL);
	mSDLPointerCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	mSDLHandCursor = CreateCursorFromRaw(mFingerCursorData, 32, 32, 11, 4);
	mSDLDraggingCursor = CreateCursorFromRaw(mDraggingCursorData, 32, 32, 15, 10);
	mSDLTextCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
	mSDLWaitCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	mSDLNoCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);

	mMusic = new Music();
	mSoundSystem = new TodFoley();
	mEffectSystem = new EffectSystem();
	mEffectSystem->EffectSystemInitialize();

	mKonamiCheck = new TypingCheck();
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_UP);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_UP);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_DOWN);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_DOWN);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_LEFT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_RIGHT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_LEFT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_RIGHT);
	mKonamiCheck->AddChar('b');
	mKonamiCheck->AddChar('a');
	mMustacheCheck = new TypingCheck("mustache");
	mMoustacheCheck = new TypingCheck("moustache");
	mSuperMowerCheck = new TypingCheck("trickedout");
	mSuperMowerCheck2 = new TypingCheck("tricked out");
	mFutureCheck = new TypingCheck("future");
	mPinataCheck = new TypingCheck("pinata");
	mDanceCheck = new TypingCheck("dance");
	mDaisyCheck = new TypingCheck("daisies");
	mSukhbirCheck = new TypingCheck("sukhbir");

#ifdef _DEBUG
	aDuration = mTimer.GetDuration();
	TodTrace("loading: 'system' %d ms", aDuration);
#endif
	mTimer.Start();

#ifdef OPENFL_HOST
	pvz_host_trace("BEGIN loading-bar reanimation setup");
#endif
	ReanimatorLoadDefinitions(gLawnReanimationArray, ReanimationType::NUM_REANIMS);
	ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LOADBAR_SPROUT, true);
	ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LOADBAR_ZOMBIEHEAD, true);
#ifdef OPENFL_HOST
	pvz_host_trace("END loading-bar reanimation setup");
#endif

#ifdef _DEBUG
	aDuration = mTimer.GetDuration();
	TodTrace("loading: 'loaderbar' %d ms", aDuration);
#endif
	mTimer.Start();

	                                           
	if (!IsScreenSaver() && !IsParticleEditor())
		PlayVideo(StrFormat("%svideos/intro.mp4", SDL_GetBasePath()).c_str(), true);
}

          
bool LawnApp::ChangeDirHook(const char* theIntendedPath)
{
	return false;
}

          
void LawnApp::Start()
{
	if (mLoadingFailed)
		return;

	SexyAppBase::Start();
}

int LawnApp::AudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	if (!inputBuffer) {
		*(float*)userData = 0.0f;
		return paContinue;
	}

	const float* input = (const float*)inputBuffer;
	float rms = 0.0f;
	for (unsigned long i = 0; i < framesPerBuffer; ++i) {
		rms += input[i] * input[i];     
	}

	rms = sqrt(rms / framesPerBuffer);

	float* volume = (float*)userData;
	*volume = rms;

	return paContinue;
}
          
bool LawnApp::DebugKeyDown(int theKey)
{
	return SexyAppBase::DebugKeyDown(theKey);
}

          
void LawnApp::HandleCmdLineParam(const std::string& theParamName, const std::string& theParamValue)
{
	if (theParamName == "-tod")
	{
#ifdef _DEBUG
		mTodCheatKeys = true;
		mDebugKeysEnabled = true;
#endif
	}
	else
	{
		SexyApp::HandleCmdLineParam(theParamName, theParamValue);
	}
}

          
bool LawnApp::UpdatePlayerProfileForFinishingLevel()
{
    if (mLocalCoop) return false;
#ifdef OPENFL_HOST
    if (mBoard) pvz_host_achievement_win(mBoard);
#endif
    if (IsAdventureMode() && mBoard && mBoard->mIsReplay) return false;
	bool aUnlockedNewChallenge = false;

	if (IsAdventureMode())
	{
		if (abs(mBoard->mLevel) == FINAL_LEVEL)
		{
			if (mBoard->mLevel >= 0)
			{
				mPlayerInfo->SetLevel(1);
				mPlayerInfo->mFinishedAdventure++;
			}
			else
			{
				mPlayerInfo->SetLevel(-(FINAL_LEVEL - 1));
			}

			if (mPlayerInfo->mFinishedAdventure == 1)
			{
				mPlayerInfo->mNeedsMessageOnGameSelector = 1;
			}
			ReportAchievement::GiveAchievement(this, AchievementId::HomeSecurity, false);                            
		}
		else
		{
			if (!mBoard->mIsReplay)
				mPlayerInfo->SetLevel(mBoard->mLevel + 1);                          
		}

		if (!HasFinishedAdventure() && abs(mPlayerInfo->mLevel) == 34)
		{
			mPlayerInfo->mNeedsMagicTacoReward = 1;
		}

		                       
		if (mBoard->StageIsDayWithPool() && !mBoard->mPeaShooterUsed) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::DontPea, false);
		}
		if (mBoard->StageHasRoof() && !mBoard->HasConveyorBeltSeedBank() && !mBoard->mCatapultPlantsUsed) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::Grounded, false);
		}
		if (mBoard->StageIsNight() && !mBoard->mMushroomsUsed && mPlayerInfo->mLevel != 35 && mPlayerInfo->mLevel != 40)
		{
			ReportAchievement::GiveAchievement(this, AchievementId::NoFungusAmongUs, false);
		}
		if (!mBoard->StageIsNight() && mBoard->mMushroomAndCoffeeBeansOnly) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::GoodMorning, false);
		}
#ifdef _HAS_UNUSED_ACHIEVEMENTS
		if (mBoard->mSpawnedDiggerZombie && !mBoard->mDiggerHasReachLeftSide) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::FaceToFace, false);
		}
		if (!mBoard->mHadPlantedNuts && mPlayerInfo->mLevel > 3)
		{
			ReportAchievement::GiveAchievement(this, AchievementId::MayNotContainNuts, true);
		}
#endif
	}
	else if (IsSurvivalMode())
	{
		if (mBoard->IsFinalSurvivalStage())
		{
			aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
			mBoard->SurvivalSaveScore();

			if (aUnlockedNewChallenge && HasFinishedAdventure())
			{
				int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_SURVIVAL);
				if (aNumTrophies != 8 && aNumTrophies != 9)
				{
					mPlayerInfo->mHasNewSurvival = true;
				}
			}
		}
	}
	else if (IsPuzzleMode())
	{
		aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
		mPlayerInfo->mChallengeRecords[GetCurrentChallengeIndex()]++;

		if (!HasFinishedAdventure() && (mGameMode == GameMode::GAMEMODE_SCARY_POTTER_3 || mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_3))
		{
			aUnlockedNewChallenge = false;
		}

		if (aUnlockedNewChallenge)
		{
			if (IsScaryPotterLevel())
			{
				mPlayerInfo->mHasNewScaryPotter = 1;
			}
			else
			{
				mPlayerInfo->mHasNewIZombie = 1;
			}
		}
	}
	else
	{
		aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
		mPlayerInfo->mChallengeRecords[GetCurrentChallengeIndex()]++;

		if (aUnlockedNewChallenge && HasFinishedAdventure() && mGameMode != GAMEMODE_CHALLENGE_SPAM_SKILL)
		{
			int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
			if (aNumTrophies <= 17)
			{
				mPlayerInfo->mHasNewMiniGame = 1;
			}
		}

		                       
#ifndef OPENFL_HOST
        int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
        if (aNumTrophies == 20)
            ReportAchievement::GiveAchievement(this, AchievementId::BeyondTheGrave, false);
#endif
	}

#ifdef OPENFL_HOST
    pvz_host_achievement_records(this);
#endif
	WriteCurrentUserConfig();

	return aUnlockedNewChallenge;
}


          
          
                         
void LawnApp::CheckForGameEnd()
{
	if (mBoard == nullptr || !mBoard->mLevelComplete)
		return;

    

    if (mLocalCoop && (!IsSurvivalMode() || mBoard->IsFinalSurvivalStage()))
    {
#ifdef OPENFL_HOST
        if (mCoopHasPartner && mGameMode == GAMEMODE_CHALLENGE_WALLNUT_BOWLING)
        {
            extern void pvz_host_earn(LawnApp*, const char*);
            pvz_host_earn(this, "zff_4_evr");
        }
#endif
        if (!IsSurvivalEndless(mGameMode))
            RegistryWriteBoolean("CoopCompleted" + std::to_string(mCoopLevelIndex), true);
        DoBackToMain();
        return;
    }

    bool aUnlockedNewChallenge = UpdatePlayerProfileForFinishingLevel();

	if (IsAdventureMode())
	{
		int aLevel = mBoard->mLevel;
		bool isReplaying = mBoard->mIsReplay;
		KillBoard();

		if (!isReplaying)
		{
			if (IsFirstTimeAdventureMode() && aLevel < 50)
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else if (aLevel == FINAL_LEVEL && mPlayerInfo->mFinishedAdventure == 1)
			{
				if (mPlayerInfo->mFinishedAdventure > 1)
				{
					ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
				}
				else
				{
					ShowAwardScreen(AwardType::AWARD_CREDITS_ZOMBIENOTE, true);
				}
			}
			else if (aLevel == 9 || aLevel == 19 || aLevel == 29 || aLevel == 39 || aLevel == 49)
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else
			{
				PreNewGame(mGameMode, false);
			}
		}
		else
		{
			DoBackToMain();
		}
	}
	else if (IsLastStandEndless(mGameMode))
	{
		mBoard->mNukeCounter = 0;
		mBoard->mChallenge->mSurvivalStage++;
#ifdef OPENFL_HOST
        pvz_host_achievement_round(mBoard);
#endif
		KillGameSelector();
		mBoard->InitSurvivalStage();
	}
	else if (IsSurvivalMode())
	{
		if (mBoard->IsFinalSurvivalStage())
		{
			KillBoard();

			if (aUnlockedNewChallenge && HasFinishedAdventure())
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else
			{
				ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_SURVIVAL);
			}
		}
		else
		{
			mBoard->mNukeCounter = 0;
			mBoard->mChallenge->mSurvivalStage++;
#ifdef OPENFL_HOST
            pvz_host_achievement_round(mBoard);
#endif
			KillGameSelector();
			mBoard->InitSurvivalStage();
		}
	}
	else if (IsPuzzleMode())
	{
		KillBoard();

		if (aUnlockedNewChallenge)
		{
			ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
		}
		else
		{
			ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_PUZZLE);
		}
	}
	else
	{
		KillBoard();
        if (mGameMode == GAMEMODE_CHALLENGE_ZOMBIE_TRAP)
            ShowChallengeScreen(CHALLENGE_PAGE_DS_MINIGAMES);
        else if (mGameMode == GAMEMODE_CHALLENGE_HEAVY_WEAPON)
            ShowChallengeScreen(CHALLENGE_PAGE_CONSOLE_MINIGAMES);
        else
            ShowChallengeScreen(mGameMode == GAMEMODE_CHALLENGE_SPAM_SKILL
                ? CHALLENGE_PAGE_CUSTOM_MINIGAMES : CHALLENGE_PAGE_CHALLENGE);
	}
}


void LawnApp::UpdatePlayTimeStats()
{
	static int aLastTime = -1;

	int aTickCount = GetTickCount();
	int aSession = (aTickCount - aLastTime) / 1000;

	if (mPlayerInfo && !mPlayerInfo->mHasUsedCheatKeys && !mDebugKeysEnabled && mTodCheatKeys)
	{
		mPlayerInfo->mHasUsedCheatKeys = 1;
	}

	if (aLastTime == -1)
	{
		aLastTime = aTickCount;
		return;
	}

	if (aSession > 0)
	{
		aLastTime = aTickCount;

		if ((mBoard == nullptr || !mBoard->mPaused) && mHasFocus && mLastTimerTime - mLastUserInputTick <= 10000)
		{
			mPlayTimeActiveSession += aSession;

			if (mBoard)
			{
				mBoard->mPlayTimeActiveLevel += aSession;
			}

			if (mPlayerInfo)
			{
				mPlayerInfo->mPlayTimeActivePlayer += aSession;
			}
		}
		else
		{
			mPlayTimeInactiveSession += aSession;

			if (mBoard)
			{
				mBoard->mPlayTimeInactiveLevel += aSession;
			}

			if (mPlayerInfo)
			{
				mPlayerInfo->mPlayTimeInactivePlayer += aSession;
			}
		}
	}
}

          
void LawnApp::UpdateFrames()
{
	if ((!mActive || mMinimized) && mBoard)
	{
		mBoard->ResetFPSStats();
	}

#ifdef _DEBUG
	UpdatePlayTimeStats();
#endif

	int aUpdateCount = 1;
	if (gSlowMo)
	{
		++gSlowMoCounter;
		if (gSlowMoCounter < 4)
		{
			aUpdateCount = 0;
		}
		else
		{
			gSlowMoCounter = 0;
		}
	}
	else if (gFastMo)
	{
		aUpdateCount = 20;
	}

	for (int i = 0; i < aUpdateCount; i++)
	{
		mAppCounter++;
		
		if (mBoard)
		{
			mBoard->ProcessDeleteQueue();

			if (mBoard->mTimeStopCounter > 0)	
			{
				mBoard->mTimeStopCounter--;

				for (int j = 0; j <= 5; j++)
				{
					for (int k = 0; k < mBoard->mSeedBank->mNumPackets; k++)
					{
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
						if (mBoard->mSeedBank->mSeedPackets[k].mPacketType == SeedType::SEED_TIMESTOPPER)
							continue;
#endif

						mBoard->mSeedBank->mSeedPackets[k].Update();
					}
				}
			}

			if (mGameMode == GameMode::GAMEMODE_CHALLENGE_SPEED && !mBoard->HasLevelAwardDropped()) {
				if (mBoard->mTimeStopCounter > 0)
				{
					mBoard->mTimeStopCounter--;

					for (int j = 0; j <= 5; j++)
					{
						for (int k = 0; k < mBoard->mSeedBank->mNumPackets; k++)
						{
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
							if (mBoard->mSeedBank->mSeedPackets[k].mPacketType == SeedType::SEED_TIMESTOPPER)
								continue;
#endif

							mBoard->mSeedBank->mSeedPackets[k].Update();
						}
					}
				}
			}
		}

		SexyApp::UpdateFrames();
		if (mMusic && mMusic->mMusicInterface)
			mMusic->MusicUpdate();
		if (mLoadingThreadCompleted && mEffectSystem)
		{
			mEffectSystem->ProcessDeleteQueue();
		}

		CheckForGameEnd();
	}
}

void LawnApp::ToggleSlowMo()
{
	gSlowMoCounter = 0;
	gSlowMo = !gSlowMo;
	gFastMo = false;
}

void LawnApp::ToggleFastMo()
{
	gSlowMo = false;
	gFastMo = !gFastMo;
}

          
void LawnApp::LoadGroup(const char* theGroupName, int theGroupAveMsToLoad)
{
	PerfTimer aTimer;
	aTimer.Start();

	mResourceManager->StartLoadResources(theGroupName);
	while (!mShutdown && !mCloseRequest && !mLoadingFailed && TodLoadNextResource())
	{
		mCompletedLoadingThreadTasks += theGroupAveMsToLoad;
	}

	if (mShutdown || mCloseRequest)
		return;

	if (mResourceManager->HadError() || !ExtractResourcesByName(mResourceManager, theGroupName))
	{
		ShowResourceError();
		mLoadingFailed = true;
	}

	int aTotalGroupWeight = mResourceManager->GetNumResources(theGroupName) * theGroupAveMsToLoad;
	int aGroupTime = pvz_max(aTimer.GetDuration(), 0);
	TraceLoadGroup(theGroupName, aGroupTime, aTotalGroupWeight, theGroupAveMsToLoad);
}

          
void LawnApp::LoadingThreadProc()
{
	if (!TodLoadResources("LoaderBar"))
		return;

	TodStringListLoad(_S("Properties\\LawnStrings.txt"));

	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (GetFileAttributesExA("dependency\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("dependency\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("dependency\\properties\\LawnStrings.txt"));

	if (GetFileAttributesExA("extension\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("extension\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("extension\\properties\\LawnStrings.txt"));

#ifdef _ALLOW_RESOURCE_PACKS
	if (GetFileAttributesExA("resourcepack\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("resourcepack\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("resourcepack\\properties\\LawnStrings.txt"));
#endif

#ifdef _HAS_ZOMBATAR
	if (!IsScreenSaver() && !IsParticleEditor())
		TodStringListLoad(_S("Properties\\ZombatarTOS.txt"));
#endif
	
	if (mTitleScreen)
	{
		mTitleScreen->mLoaderScreenIsLoaded = true;
	}

	const char* groups[] = { "LoadingFonts", "LoadingImages", "LoadingSounds" };
	int group_ave_ms_to_load[] = { 54, 9, 54 };
	for (int i = 0; i < 3; i++)
	{
		if (IsParticleEditor()) continue;

		mNumLoadingThreadTasks += mResourceManager->GetNumResources(groups[i]) * group_ave_ms_to_load[i];
	}
	mNumLoadingThreadTasks += 636;
	if (!IsParticleEditor())
		mNumLoadingThreadTasks += GetNumPreloadingTasks();
	mNumLoadingThreadTasks += mMusic->GetNumLoadingTasks();

	PerfTimer aTimer;
	aTimer.Start();

	TodHesitationTrace("start loading");
	TodHesitationBracket aHesitationResources("Resources");
	TodHesitationTrace("loading thread start");

	LoadGroup("LoadingFonts", 54);
	LoadGroup("LoadingImages", 9);

	if (mLoadingFailed || mShutdown || mCloseRequest)
		return;

	aHesitationResources.EndBracket();
	TodTrace("loading '%s' %d ms", "resources", (int)aTimer.GetDuration());

	mMusic->MusicInit();
	int aDuration = pvz_max(aTimer.GetDuration(), 0);
	aTimer.Start();

	if (                        !IsParticleEditor())
	{
		mPoolEffect = new PoolEffect();
		mPoolEffect->PoolEffectInitialize();
		mZenGarden = new ZenGarden();
	}

	if (!IsParticleEditor())
	{
		mReanimatorCache = new ReanimatorCache();
		mReanimatorCache->ReanimatorCacheInitialize();
	}

	TodFoleyInitialize(gLawnFoleyParamArray, LENGTH(gLawnFoleyParamArray));

	TodTrace("loading '%s' %d ms", "stuff", (int)aTimer.GetDuration());
	aTimer.Start();

	TrailLoadDefinitions(gLawnTrailArray, LENGTH(gLawnTrailArray));
	TodTrace("loading '%s' %d ms", "trail", (int)aTimer.GetDuration());
	aTimer.Start();
	TodHesitationTrace("trail");

	TodParticleLoadDefinitions(gLawnParticleArray, LENGTH(gLawnParticleArray));
	                                               
	aTimer.Start();

	if (!IsParticleEditor())
		PreloadForUser();
	
	if (mLoadingFailed || mShutdown || mCloseRequest)
		return;

	                                               
	aTimer.Start();

	                          
	if (!IsParticleEditor())
		LoadGroup("LoadingSounds", 54);
	TodHesitationTrace("finished loading");
}

          
void LawnApp::FastLoad(GameMode theGameMode)
{
	if (!mShutdown)
	{
		mWidgetManager->RemoveWidget(mTitleScreen);
		SafeDeleteWidget(mTitleScreen);
		mTitleScreen = nullptr;

		PreNewGame(theGameMode, false);
	}
}

void LawnApp::LoadingThreadCompleted()
{
}

          
void LawnApp::LoadingCompleted()
{
	mWidgetManager->RemoveWidget(mTitleScreen);
	SafeDeleteWidget(mTitleScreen);
	mTitleScreen = nullptr;

	mResourceManager->DeleteImage("IMAGE_TITLESCREEN");
	if (IsScreenSaver()) 
	{
		KillBoard();
		mZenGarden->UpdatePlantNeeds();
		PreNewGame(GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN, false);
	}
	else if (IsParticleEditor())
	{
		KillBoard();
		ShowParticleEditor();
	}
	else
	{
		ShowGameSelector();
	}
}

          
void LawnApp::URLOpenFailed(const std::string& theURL)
{
	SexyAppBase::URLOpenFailed(theURL);
	KillDialog(Dialogs::DIALOG_OPENURL_WAIT);
	CopyToClipboard(theURL);

	std::string aString = 
		"Please open the following URL in your browser\n\n" + 
		theURL + 
		"\n\nFor your convenience, this URL has already been copied to your clipboard.";

	DoDialog(Dialogs::DIALOG_OPENURL_WAIT, true, _S("Open Browser"), _S("OK"), StringToSexyStringFast(aString), Dialog::BUTTONS_FOOTER);
}

          
void LawnApp::URLOpenSucceeded(const std::string& theURL)
{
	SexyAppBase::URLOpenSucceeded(theURL);
	KillDialog(Dialogs::DIALOG_OPENURL_WAIT);
}

          
bool LawnApp::OpenURL(const std::string& theURL, bool shutdownOnOpen)
{
	DoDialog(
		Dialogs::DIALOG_OPENURL_WAIT, 
		true, 
		_S("Opening Browser"), 
		StringToSexyString(theURL),
		_S(""), 
		Dialog::BUTTONS_NONE
	);

	DrawDirtyStuff();

	return SexyAppBase::OpenURL(theURL, shutdownOnOpen);
}

          
void LawnApp::ConfirmQuit()
{
	SexyString aBody = TodStringTranslate(_S("[QUIT_MESSAGE]"));
	SexyString aHeader = TodStringTranslate(_S("[QUIT_HEADER]"));
	LawnDialog* aDialog = (LawnDialog*)DoDialog(Dialogs::DIALOG_QUIT, true, aHeader, aBody, _S(""), Dialog::BUTTONS_OK_CANCEL);
	aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[QUIT_BUTTON]"));
#ifdef OPENFL_HOST
	aDialog->Resize((mDDInterface->mWidth - aDialog->mWidth) / 2,
		(mDDInterface->mHeight - aDialog->mHeight) / 2, aDialog->mWidth, aDialog->mHeight);
#else
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
#endif
}

          
void LawnApp::PreDisplayHook()
{
	SexyApp::PreDisplayHook();
}

void LawnApp::ButtonPress(int theId)
{
}

          
void LawnApp::ButtonDepress(int theId)
{
#ifdef OPENFL_HOST
    const int hostedDialogId = theId >= 2000 && theId < 3000 ? theId - 2000 :
        theId >= 3000 && theId < 4000 ? theId - 3000 : -1;
    if (hostedDialogId >= 0)
    {
        auto* store = static_cast<StoreScreen*>(GetDialog(Dialogs::DIALOG_STORE));
                                                                              
                                                                            
        if (store && (store->mHostedDialog == hostedDialogId || hostedDialogId == Dialogs::DIALOG_STORE)) return;
        if (mCreditScreen && mCreditScreen->mHostedPauseDialog == hostedDialogId) return;
        if (hostedDialogId == Dialogs::DIALOG_CONFIRM_RESTART) return;
        if (hostedDialogId == Dialogs::DIALOG_CHOOSER_WARNING && mSeedChooserScreen) return;
        if (hostedDialogId == Dialogs::DIALOG_ZEN_SELL && mZenGarden && mZenGarden->mHostedSaleBoard) return;
    }
#endif
	if (theId % 10000 >= 2000 && theId % 10000 < 3000)                                                                                                             
	{
		switch (theId - 2000)
		{
		case Dialogs::DIALOG_NEW_GAME:
			KillDialog(Dialogs::DIALOG_NEW_GAME);
			ShowGameSelector();
			return;

		case Dialogs::DIALOG_NEWOPTIONS:
			KillNewOptionsDialog();
			return;

		case Dialogs::DIALOG_PREGAME_NAG:
			DoRegister();
			return;

		case Dialogs::DIALOG_LOAD_GAME:
			return;

		case Dialogs::DIALOG_CONFIRM_UPDATE_CHECK:
			KillDialog(Dialogs::DIALOG_CONFIRM_UPDATE_CHECK);
			CheckForUpdates();
			return;

		case Dialogs::DIALOG_QUIT:
			KillDialog(Dialogs::DIALOG_QUIT);
#ifdef OPENFL_HOST
            mCloseRequest = true;                                                           
#else
			SendMessage(mHWnd, WM_CLOSE, NULL, NULL);
#endif
			return;

		case Dialogs::DIALOG_NAG:
			KillDialog(Dialogs::DIALOG_NAG);
			DoRegister();
			return;

		case Dialogs::DIALOG_SCRIPTABLE_CREDITS:
			KillDialog(Dialogs::DIALOG_SCRIPTABLE_CREDITS);
			return;

		case Dialogs::DIALOG_INFO:
			KillDialog(Dialogs::DIALOG_INFO);
			return;

		case Dialogs::DIALOG_PAUSED:
			KillDialog(Dialogs::DIALOG_PAUSED);
			return;

		case Dialogs::DIALOG_NO_MORE_MONEY:
			KillDialog(Dialogs::DIALOG_NO_MORE_MONEY);
			mBoard->AddSunMoney(100);
			return;

		case Dialogs::DIALOG_BONUS:
			KillDialog(Dialogs::DIALOG_BONUS);
			return;

		case Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN:
			KillDialog(Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN);
			mBoardResult = BoardResult::BOARDRESULT_QUIT;
			mBoard->TryToSaveGame();
			DoBackToMain();
			return;

		case Dialogs::DIALOG_USERDIALOG:
			FinishUserDialog(true);
			return;

		case Dialogs::DIALOG_CREATEUSER:
			FinishCreateUserDialog(true);
			return;

		case Dialogs::DIALOG_CONFIRMDELETEUSER:
			FinishConfirmDeleteUserDialog(true);
			return;

		case Dialogs::DIALOG_RENAMEUSER:
			FinishRenameUserDialog(true);
			return;

		case Dialogs::DIALOG_CREATEUSERERROR:
		case Dialogs::DIALOG_RENAMEUSERERROR:
			FinishNameError(theId - 2000);
			return;

		case Dialogs::DIALOG_CHEAT:
			FinishCheatDialog(true);
			return;

		case Dialogs::DIALOG_RESTARTCONFIRM:
			FinishRestartConfirmDialog();
			return;

		case Dialogs::DIALOG_TIMESUP:
			FinishTimesUpDialog();
			return;

		case 20008:
			KillDialog(20008);
			KillDialog(Dialogs::DIALOG_CHECKING_UPDATES);
			return;

		case Dialogs::DIALOG_UNLOCK:
		{
			KillDialog(Dialogs::DIALOG_UNLOCK);
			if (mPlayerInfo)
			{
				mPlayerInfo->SetLevel(1);
				mPlayerInfo->mFinishedAdventure = 2;
				mPlayerInfo->AddCoins(50000);
				mPlayerInfo->mHasUsedCheatKeys = true;
				mPlayerInfo->mHasUnlockedMinigames = true;
				mPlayerInfo->mHasUnlockedPuzzleMode = true;
				mPlayerInfo->mHasUnlockedSurvivalMode = true;

				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FERTILIZER] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_BUG_SPRAY] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_CHOCOLATE] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_FOOD] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GATLINGPEA] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_CATTAIL] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_WINTERMELON] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_SPIKEROCK] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_COBCANNON] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_IMITATER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 4;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_ROOF_CLEANER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PHONOGRAPH] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_GARDENING_GLOVE] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_MUSHROOM_GARDEN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_WHEEL_BARROW] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_AQUARIUM_GARDEN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_OF_WISDOM] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FIRSTAID] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_GOLD_WATERINGCAN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_STINKY_THE_SNAIL] = 1;

				if (mGameSelector) mGameSelector->SyncProfile(true);

				EraseFile(GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mPlayerInfo->mId));
			}
			return;
		}

		                                    
		   
		                                                                                              
		               
		    
		                                                                
		                                                        
		                    
		                               
		                                                          
		                                                    
		                           
		                                        
		     
		                      
		                             
		                           
		                           
		                                
		     
		                                                            
		                                                      
		                                                                 
		                                                              
		                                                                         
		                                                                 
		                                                                     
		                                                              
		                                                            
		                                                  
		                                                                     
		                                                              
		                                 
		                                                            
		                                                      
		                                                              
		                                                    

		                                                                     
		                                                 

		                                     
		                                                                                                                      
		                                                                                                                      
		       

		                                             
		                         
		    
		          
		   

		default:
			KillDialog(theId - 2000);
			return;
		}
	}

	if (theId % 10000 >= 3000 && theId < 4000)                                                                                                             
	{
		switch (theId - 3000)
		{
		case Dialogs::DIALOG_PREGAME_NAG:
			KillDialog(Dialogs::DIALOG_PREGAME_NAG);
			Shutdown();
			return;

		case Dialogs::DIALOG_LOAD_GAME:
			KillDialog(Dialogs::DIALOG_LOAD_GAME);
			return;

		case Dialogs::DIALOG_USERDIALOG:
			FinishUserDialog(false);
			return;

		case Dialogs::DIALOG_CREATEUSER:
			FinishCreateUserDialog(false);
			return;

		case Dialogs::DIALOG_CONFIRMDELETEUSER:
			FinishConfirmDeleteUserDialog(false);
			return;

		case Dialogs::DIALOG_RENAMEUSER:
			FinishRenameUserDialog(false);
			return;

		case Dialogs::DIALOG_CHEAT:
			FinishCheatDialog(false);
			return;

		case Dialogs::DIALOG_TIMESUP:
			FinishTimesUpDialog();
			return;

		case 10008:
			KillDialog(10008);
			KillDialog(Dialogs::DIALOG_CHECKING_UPDATES);
			return;

		default:
			KillDialog(theId - 3000);
			return;
		}
	}
}

void LawnApp::CenterDialog(Dialog* theDialog, int theWidth, int theHeight)
{
	theDialog->Resize((gLawnApp->mWidth - theWidth) / 2, (gLawnApp->mHeight - theHeight) / 2, theWidth, theHeight);
}

          
void LawnApp::PlayFoley(FoleyType theFoleyType)
{
	if (!mMuteSoundsForCutscene)
	{
		mSoundSystem->PlayFoley(theFoleyType);
	}
}

          
void LawnApp::PlayFoleyPitch(FoleyType theFoleyType, float thePitch)
{
	if (!mMuteSoundsForCutscene)
	{
		mSoundSystem->PlayFoleyPitch(theFoleyType, thePitch);
	}
}

          
SexyString LawnApp::GetStageString(int theLevel)
{
	int aArea = ClampInt((abs(theLevel) - 1) / LEVELS_PER_AREA + 1, 1, ADVENTURE_AREAS + 1);
	int aSub = abs(theLevel) - (aArea - 1) * LEVELS_PER_AREA;
	SexyString aFormat;
	if (theLevel < 0) aFormat = StrFormat(_S(" -%d-%d"), aArea, aSub);
	else aFormat = StrFormat(_S(" %d-%d"), aArea, aSub);
	return aFormat;
} 

bool LawnApp::IsAdventureMode()
{
	return mGameMode == GameMode::GAMEMODE_ADVENTURE;
}

          
bool LawnApp::IsSurvivalMode()
{
	if (mGameMode == GAMEMODE_SURVIVAL_SPAM_SKILL_ENDLESS) return true;
	return mGameMode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1 && mGameMode <= GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_5 || mGameMode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_6 && mGameMode <= GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_HIGHGROUND;
}

          
bool LawnApp::IsPuzzleMode()
{
	return
		(mGameMode >= GameMode::GAMEMODE_SCARY_POTTER_1 && mGameMode <= GameMode::GAMEMODE_SCARY_POTTER_ENDLESS) ||
		(mGameMode >= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 && mGameMode <= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS);
}

          
bool LawnApp::IsChallengeMode()
{
	return !IsAdventureMode() && !IsPuzzleMode() && !IsSurvivalMode();
}

bool LawnApp::IsSurvivalNormal(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsSurvivalHard(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsSurvivalEndless(GameMode theGameMode)
{
	if (theGameMode == GAMEMODE_SURVIVAL_SPAM_SKILL_ENDLESS) return true;
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsEndlessScaryPotter(GameMode theGameMode)
{
	return theGameMode == GameMode::GAMEMODE_SCARY_POTTER_ENDLESS;
}

bool LawnApp::IsEndlessIZombie(GameMode theGameMode)
{
	return theGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS;
}

          
bool LawnApp::IsContinuousChallenge()
{
	return 
		IsArtChallenge() || 
		IsSlotMachineLevel() || 
		IsFinalBossLevel() || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED || 
		mGameMode == GameMode::GAMEMODE_UPSELL || 
		mGameMode == GameMode::GAMEMODE_INTRO || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST
		;
}

bool LawnApp::IsArtChallenge()
{
	if (mBoard == nullptr)
		return false;

	return 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_WALLNUT || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_SUNFLOWER || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_SEEING_STARS;
}

          
bool LawnApp::IsSquirrelLevel()
{
	return mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SQUIRREL;
}

          
bool LawnApp::IsIZombieLevel()
{
	if (mBoard == nullptr)
		return false;

	return
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_2 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_3 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_4 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_5 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_6 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_7 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_8 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_9 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS;
}

          
bool LawnApp::IsShovelLevel()
{
	return mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SHOVEL;
}

          
bool LawnApp::IsWallnutBowlingLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING || mGameMode == GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING_2)
		return true;

	return IsAdventureMode() && GetAdventureLevel() == 5;
}

          
bool LawnApp::IsSlotMachineLevel()
{
	return (mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SLOT_MACHINE);
}

          
bool LawnApp::IsWhackAZombieLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_WHACK_A_ZOMBIE)
		return true;

	return IsAdventureMode() && GetAdventureLevel() == 15;
}

          
bool LawnApp::IsLittleTroubleLevel()
{
	if (mBoard == nullptr)
		return false;

	return mGameMode == GameMode::GAMEMODE_CHALLENGE_LITTLE_TROUBLE || (mGameMode == GameMode::GAMEMODE_ADVENTURE && GetAdventureLevel() == 25);
}

          
bool LawnApp::IsScaryPotterLevel()
{
	if (mGameMode >= GameMode::GAMEMODE_SCARY_POTTER_1 && mGameMode <= GameMode::GAMEMODE_SCARY_POTTER_ENDLESS)
		return true;

#ifdef _MOBILE_MINIGAMES
	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_VASEBREAKER)
		return true;
#endif

	if (mBoard == nullptr)
		return false;

	return IsAdventureMode() && GetAdventureLevel() == 35;
}

          
bool LawnApp::IsStormyNightLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_STORMY_NIGHT)
		return true;

	return IsAdventureMode() && GetAdventureLevel() == 40;
}

          
bool LawnApp::IsBungeeBlitzLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_BUNGEE_BLITZ)
		return true;

	return IsAdventureMode() && GetAdventureLevel() == 45;
}

          
bool LawnApp::IsMiniBossLevel()
{
	if (mBoard == nullptr)
		return false;

	return
		(IsAdventureMode() && GetAdventureLevel() == 10) ||
		(IsAdventureMode() && GetAdventureLevel() == 20) ||
		(IsAdventureMode() && GetAdventureLevel() == 30) ||
		(IsAdventureMode() && GetAdventureLevel() == 40);
}

          
bool LawnApp::IsFinalBossLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_FINAL_BOSS)
		return true;

	return IsAdventureMode() && GetAdventureLevel() == 50;
}

          
bool LawnApp::IsChallengeWithoutSeedBank()
{
	return 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_RAINING_SEEDS || 
		mGameMode == GameMode::GAMEMODE_UPSELL || 
		mGameMode == GameMode::GAMEMODE_INTRO || 
		IsWhackAZombieLevel() || 
		IsSquirrelLevel() || 
		IsScaryPotterLevel() || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || 
		mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM 
#ifdef _MOBILE_MINIGAMES
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_BUTTERED_POPCORN
#endif
#ifdef _DS_MINIGAMES
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP 
#endif
		;
}

bool LawnApp::IsNight()
{
	if (IsIceDemo() || mPlayerInfo == nullptr                         )
		return false;

	if (mBoard)
		return (mBoard->mLevel >= 11 && mBoard->mLevel <= 20) || (mBoard->mLevel >= 31 && mBoard->mLevel <= 40) || mBoard->mLevel == 50;

	return (mPlayerInfo->mLevel >= 11 && mPlayerInfo->mLevel <= 20) || (mPlayerInfo->mLevel >= 31 && mPlayerInfo->mLevel <= 40) || mPlayerInfo->mLevel == 50;
}

int LawnApp::GetCurrentChallengeIndex()
{
	return (int)mGameMode - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
}

ChallengeDefinition& LawnApp::GetCurrentChallengeDef()
{
	return GetChallengeDefinition(GetCurrentChallengeIndex());
}

PottedPlant* LawnApp::GetPottedPlantByIndex(int thePottedPlantIndex)
{
	TOD_ASSERT(thePottedPlantIndex >= 0 && thePottedPlantIndex < mPlayerInfo->mNumPottedPlants);
	return &mPlayerInfo->mPottedPlant[thePottedPlantIndex];
}

          
bool LawnApp::UpdateApp()
{
	if (mCloseRequest)
	{
		Shutdown();
		return false;
	}

	                              
	   
	                            
	   

	bool updated = SexyAppBase::UpdateApp();

	                                             
	   
	                     
	   

	return updated;
}

          
void LawnApp::CloseRequestAsync()
{
	mDeferredMessages.clear();
	mExitToTop = true;
	mCloseRequest = true;
}

          
SeedType LawnApp::GetAwardSeedForLevel(int theLevel)
{
	int aArea = (theLevel - 1) / LEVELS_PER_AREA + 1;
	int aSub = (theLevel - 1) % LEVELS_PER_AREA + 1;
	int aSeedsHasGot = (aArea - 1) * 8 + aSub;                                                                                         
	if (aSub >= 10)
	{
		aSeedsHasGot -= 2;                                                                              
	}
	else if (aSub >= 5)
	{
		aSeedsHasGot -= 1;                                                                             
	}
	if (aSeedsHasGot > 40)
	{
		aSeedsHasGot = 40;
	}
	
	return (SeedType)aSeedsHasGot;
}

          
int LawnApp::GetSeedsAvailable()
{
    if (mLocalCoop) return mPlayerInfo->mFinishedAdventure ? 49 : 40;
	int aLevel = mBoard && mBoard->mIsReplay && mPlayerLevelRef > 4 ? mPlayerLevelRef : mPlayerInfo->GetLevel();
	int maxPlants = 49;

	if (HasFinishedAdventure() || aLevel > 50 && mPlayerInfo && mPlayerInfo->mHasUsedCheatKeys)
	{
		if (mTodCheatKeys || mDebugKeysEnabled || mPlayerInfo && mPlayerInfo->mHasUsedCheatKeys) maxPlants += NUM_SEEDS_IN_CHOOSER - SEED_IMITATER - 1;
		return maxPlants;
	}

	SeedType aSeedTypeMax = GetAwardSeedForLevel(aLevel);
	return pvz_min(maxPlants, aSeedTypeMax);
}

          
bool LawnApp::HasSeedType(SeedType theSeedType)
{
	if (IsTrialStageLocked() && theSeedType >= SeedType::SEED_JALAPENO)
		return false;

	          
                                                                                           
                                                                          
   

    // Retired bundled additions; retain their IDs for existing saves.

	if (theSeedType == SeedType::SEED_GATLINGPEA)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GATLINGPEA] > 0;
	}

	if (theSeedType == SeedType::SEED_TWINSUNFLOWER)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] > 0;
	}
	if (theSeedType == SeedType::SEED_GLOOMSHROOM)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] > 0;
	}
	if (theSeedType == SeedType::SEED_CATTAIL)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_CATTAIL] > 0;
	}
	if (theSeedType == SeedType::SEED_WINTERMELON)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_WINTERMELON] > 0;
	}
	if (theSeedType == SeedType::SEED_GOLD_MAGNET)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] > 0;
	}
	if (theSeedType == SeedType::SEED_SPIKEROCK)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_SPIKEROCK] > 0;
	}
	if (theSeedType == SeedType::SEED_COBCANNON)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_COBCANNON] > 0;
	}
	if (theSeedType == SeedType::SEED_IMITATER)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_IMITATER] > 0;
	}

	return theSeedType < GetSeedsAvailable();
}

bool LawnApp::SeedTypeAvailable(SeedType theSeedType)
{
    return (mLocalCoop && theSeedType >= SEED_PEASHOOTER && theSeedType < SEED_GATLINGPEA) || HasSeedType(theSeedType);
}

          
Reanimation* LawnApp::AddReanimation(float theX, float theY, int theRenderOrder, ReanimationType theReanimationType)
{
	return mEffectSystem->mReanimationHolder->AllocReanimation(theX, theY, theRenderOrder, theReanimationType);
}

          
TodParticleSystem* LawnApp::AddTodParticle(float theX, float theY, int theRenderOrder, ParticleEffect theEffect)
{
	return mEffectSystem->mParticleHolder->AllocParticleSystem(theX, theY, theRenderOrder, theEffect);
}

ParticleSystemID LawnApp::ParticleGetID(TodParticleSystem* theParticle)
{
	return (ParticleSystemID)mEffectSystem->mParticleHolder->mParticleSystems.DataArrayGetID(theParticle);
}

ReanimationID LawnApp::ReanimationGetID(Reanimation* theReanimation)
{
	return (ReanimationID)mEffectSystem->mReanimationHolder->mReanimations.DataArrayGetID(theReanimation);
}

TodParticleSystem* LawnApp::ParticleGet(ParticleSystemID theParticleID)
{
	return mEffectSystem->mParticleHolder->mParticleSystems.DataArrayGet((unsigned int)theParticleID);
}

TodParticleSystem* LawnApp::ParticleTryToGet(ParticleSystemID theParticleID)
{
	return mEffectSystem->mParticleHolder->mParticleSystems.DataArrayTryToGet((unsigned int)theParticleID);
}

Reanimation* LawnApp::ReanimationGet(ReanimationID theReanimationID)
{
	return mEffectSystem->mReanimationHolder->mReanimations.DataArrayGet((unsigned int)theReanimationID);
}

          
Reanimation* LawnApp::ReanimationTryToGet(ReanimationID theReanimationID)
{
	return mEffectSystem->mReanimationHolder->mReanimations.DataArrayTryToGet((unsigned int)theReanimationID);
}

          
void LawnApp::RemoveReanimation(ReanimationID theReanimationID)
{
	Reanimation* aReanim = ReanimationTryToGet(theReanimationID);
	if (aReanim)
	{
		aReanim->ReanimationDie();
	}
}

void LawnApp::RemoveParticle(ParticleSystemID theParticleID)
{
	TodParticleSystem* aParticle = ParticleTryToGet(theParticleID);
	if (aParticle)
	{
		aParticle->ParticleSystemDie();
	}
}

          
bool LawnApp::AdvanceCrazyDaveText()
{
	std::string aMessageName = StrFormat("[CRAZY_DAVE_%d]", mCrazyDaveMessageIndex + 1).c_str();
	if (!TodStringListExists(aMessageName))
	{
		return false;
	}

	CrazyDaveTalkIndex(mCrazyDaveMessageIndex + 1);
	return true;
}

          
SexyString LawnApp::GetCrazyDaveText(int theMessageIndex)
{
	SexyString aMessage = StrFormat(_S("[CRAZY_DAVE_%d]"), theMessageIndex);
	aMessage = TodReplaceString(aMessage, _S("{PLAYER_NAME}"), mPlayerInfo->mName);
	aMessage = TodReplaceString(aMessage, _S("{MONEY}"), GetMoneyString(mPlayerInfo->mCoins));
	int aCost = StoreScreen::GetItemCost(StoreItem::STORE_ITEM_PACKET_UPGRADE);
	aMessage = TodReplaceString(aMessage, _S("{UPGRADE_COST}"), GetMoneyString(aCost));
	return aMessage;
}

          
bool LawnApp::CanShowAlmanac()
{
	if (IsIceDemo())
		return false;

	if (mPlayerInfo == nullptr)
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mLevel >= 15;
}

          
bool LawnApp::CanShowStore()
{
	if (IsIceDemo())
		return false;

	if (mPlayerInfo == nullptr)
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mHasSeenUpsell || mPlayerInfo->mLevel >= 25;
}

          
bool LawnApp::CanShowZenGarden()
{
	if (mPlayerInfo == nullptr)
		return false;

	if (IsTrialStageLocked())
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mLevel >= 45;
}

bool LawnApp::CanSpawnYetis()
{
	const ZombieDefinition& aZombieDef = GetZombieDefinition(ZombieType::ZOMBIE_YETI);
	return HasFinishedAdventure() && (mPlayerInfo->mFinishedAdventure >= 2 || mPlayerInfo->mLevel >= aZombieDef.mStartingLevel);
}

          
bool LawnApp::HasBeatenChallenge(GameMode theGameMode)
{
	if (mPlayerInfo == nullptr)
		return false;

	int aChallengeIndex = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	TOD_ASSERT(aChallengeIndex >= 0 && aChallengeIndex < NUM_CHALLENGE_MODES);
	if (IsSurvivalNormal(theGameMode))
	{
		return mPlayerInfo->mChallengeRecords[aChallengeIndex] >= SURVIVAL_NORMAL_FLAGS;
	}
	if (IsSurvivalHard(theGameMode))
	{
		return mPlayerInfo->mChallengeRecords[aChallengeIndex] >= SURVIVAL_HARD_FLAGS;
	}
	if (IsSurvivalEndless(theGameMode) || IsEndlessScaryPotter(theGameMode) || IsEndlessIZombie(theGameMode) || IsLastStandEndless(theGameMode))
	{
		return false;
	}

	if (mGameMode == GameMode::GAMEMODE_UPSELL && theGameMode > GameMode::GAMEMODE_CHALLENGE_SLOT_MACHINE) {
		return false;
	}
	return mPlayerInfo->mChallengeRecords[aChallengeIndex] > 0;
}

          
bool LawnApp::HasFinishedAdventure()
{
	return mPlayerInfo && mPlayerInfo->mFinishedAdventure > 0;
}

          
bool LawnApp::IsFirstTimeAdventureMode()
{
	return mGameMode == GAMEMODE_ADVENTURE && !HasFinishedAdventure();
}

          
void LawnApp::CrazyDaveEnter()
{
	TOD_ASSERT(mCrazyDaveState == CRAZY_DAVE_OFF);
	TOD_ASSERT(!ReanimationTryToGet(mCrazyDaveReanimID));

	Reanimation* aCrazyDaveReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_CRAZY_DAVE);
	aCrazyDaveReanim->mIsAttachment = true;
	aCrazyDaveReanim->SetBasePoseFromAnim("anim_idle_handing");
	mCrazyDaveReanimID = ReanimationGetID(aCrazyDaveReanim);
	aCrazyDaveReanim->PlayReanim("anim_enter", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);

	mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_ENTERING;
	mCrazyDaveMessageIndex = -1;
	mCrazyDaveMessageText.clear();
	mCrazyDaveBlinkCounter = RandRangeInt(400, 800);

	if (mGameScene == GameScenes::SCENE_LEVEL_INTRO && IsStormyNightLevel())
	{
		aCrazyDaveReanim->mColorOverride = Color(64, 64, 64);
	}
}

          
void LawnApp::CrazyDaveDie()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim)
	{
		aCrazyDaveReanim->ReanimationDie();

		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_OFF;
		mCrazyDaveReanimID = ReanimationID::REANIMATIONID_NULL;
		mCrazyDaveMessageIndex = -1;
		mCrazyDaveMessageText.clear();

		CrazyDaveStopSound();
	}
}

          
void LawnApp::CrazyDaveLeave()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim)
	{
		if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
		{
			CrazyDaveDoneHanding();
		}

		aCrazyDaveReanim->PlayReanim("anim_leave", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 24.0f);
		aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);

		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_LEAVING;
		mCrazyDaveMessageIndex = -1;
		mCrazyDaveMessageText.clear();

		CrazyDaveStopSound();
	}
}

          
void LawnApp::CrazyDaveTalkIndex(int theMessageIndex)
{
	mCrazyDaveMessageIndex = theMessageIndex;
	SexyString aMessageText = GetCrazyDaveText(theMessageIndex);
	CrazyDaveTalkMessage(aMessageText);
}

          
void LawnApp::CrazyDaveDoneHanding()
{
	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);
	ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
	AttachmentDie(aHandTrackInstance->mAttachmentID);

	TodTrace("DoneHanding");
}

          
void LawnApp::CrazyDaveStopSound()
{
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_EXTRA_LONG);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_CRAZY);
}

          
void LawnApp::CrazyDaveTalkMessage(const SexyString& theMessage)
{
	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);

	bool doHanding = false;
	if (theMessage.find(_S("{HANDING}")) != SexyString::npos)
	{
		doHanding = true;
	}
	if ((mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING) && !doHanding)
	{
		CrazyDaveDoneHanding();
	}

	bool doSound = true;
	if (theMessage.find(_S("{NO_SOUND}")) != SexyString::npos)
	{
		doSound = false;
	}
	else
	{
		CrazyDaveStopSound();
	}

	int aWordsCount = 0;
	bool isControlWord = false;
	for (int i = 0; i < theMessage.size(); i++)
	{
		if (theMessage[i] == _S('{'))
		{
			isControlWord = true;
		}
		else if (theMessage[i] == _S('}'))
		{
			isControlWord = false;
		}
		else if (!isControlWord)
		{
			aWordsCount++;
		}
	}

	aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);

	if (mCrazyDaveState != CrazyDaveState::CRAZY_DAVE_TALKING || doSound)
	{
		if (doHanding)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			if (doSound)
			{
				if (theMessage.find(_S("{SHORT_SOUND}")) != SexyString::npos)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
				}
				else if (theMessage.find(_S("{SCREAM}")) != SexyString::npos)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM);
				}
				else
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
				}
			}
			
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHAKE}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_crazy", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_CRAZY);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SCREAM}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_smalltalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SCREAM2}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_mediumtalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM_2);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_WALLNUT}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aWallnutReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_WALLNUT);
			aWallnutReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 0, 12.0f);
			TodTrace("Handed");

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachEffect* aAttachEffect = AttachReanim(aHandTrackInstance->mAttachmentID, aWallnutReanim, 100.0f, 393.0f);
			aAttachEffect->mOffset.m00 = 1.2f;
			aAttachEffect->mOffset.m11 = 1.2f;

			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM_2);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_HAMMER}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aHammerReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_HAMMER);
			aHammerReanim->PlayReanim("anim_whack_zombie", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aHammerReanim->mAnimTime = 1.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachEffect* aAttachEffect = AttachReanim(aHandTrackInstance->mAttachmentID, aHammerReanim, 62.0f, 445.0f);
			aAttachEffect->mOffset.m00 = 1.5f;
			aAttachEffect->mOffset.m11 = 1.5f;

			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_FERTILIZER}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aFertilizerReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_ZENGARDEN_FERTILIZER);
			aFertilizerReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aFertilizerReanim->mAnimRate = 0.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aFertilizerReanim, 102.0f, 412.0f);
			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_TREE_FOOD}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aTreeFoodReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_TREEOFWISDOM_TREEFOOD);
			aTreeFoodReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aTreeFoodReanim->mAnimRate = 0.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aTreeFoodReanim, 102.0f, 412.0f);
			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_MONEYBAG}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aMoneyBagReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_ZENGARDEN_FERTILIZER);
			aMoneyBagReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aMoneyBagReanim->mAnimRate = 0.0f;
			aMoneyBagReanim->SetImageOverride("bag", IMAGE_MONEYBAG);

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aMoneyBagReanim, 90.0f, 405.0f);
			aCrazyDaveReanim->Update();
			  
                                                                     
                       
              
     
			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else
		{
			if (aWordsCount < 23)
			{
				aCrazyDaveReanim->PlayReanim("anim_smalltalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
			else if (aWordsCount < 52)
			{
				aCrazyDaveReanim->PlayReanim("anim_mediumtalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
			else
			{
				aCrazyDaveReanim->PlayReanim("anim_blahblah", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_EXTRA_LONG);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
		}
	}

	mCrazyDaveMessageText = theMessage;
}

          
void LawnApp::CrazyDaveStopTalking()
{
	bool aDoneHanding = true;
	if (mGameMode == GameMode::GAMEMODE_UPSELL)
	{
		aDoneHanding = false;
	}
	if (aDoneHanding && mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		CrazyDaveDoneHanding();
	}

	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);
	aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);
	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING && !aDoneHanding)
	{
		aCrazyDaveReanim->PlayReanim("anim_idle_handing", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_IDLING;
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		aCrazyDaveReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_IDLING;
	}

	mCrazyDaveMessageIndex = -1;
	mCrazyDaveMessageText.clear();
	CrazyDaveStopSound();
}

          
void LawnApp::UpdateCrazyDave()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim == nullptr)
		return;

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_ENTERING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING)
	{
		if (aCrazyDaveReanim->mLoopCount > 0)
		{
			aCrazyDaveReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_IDLING;
		}
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		if (aCrazyDaveReanim->mLoopCount > 0)
		{
			aCrazyDaveReanim->PlayReanim("anim_idle_handing", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_IDLING;
		}
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_LEAVING && aCrazyDaveReanim->mLoopCount > 0)
	{
		CrazyDaveDie();
	}

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_IDLING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
	{
		if (mCrazyDaveMessageText.find(_S("{MOUTH_BIG_SMILE}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH1);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_SMALL_SMILE}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH5);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_BIG_OH}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH4);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_SMALL_OH}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH6);
		}
	}

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_IDLING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING || 
		mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
	{
		mCrazyDaveBlinkCounter--;
		if (mCrazyDaveBlinkCounter <= 0)
		{
			mCrazyDaveBlinkCounter = RandRangeInt(400, 800);
			Reanimation* aBlinkReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_CRAZY_DAVE);
			aBlinkReanim->SetFramesForLayer("anim_blink");
			aBlinkReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_FULL_LAST_FRAME_AND_HOLD;
			aBlinkReanim->mAnimRate = 15.0f;
			aBlinkReanim->AttachToAnotherReanimation(aCrazyDaveReanim, "Dave_head");
			aBlinkReanim->mColorOverride = aCrazyDaveReanim->mColorOverride;
			aCrazyDaveReanim->AssignRenderGroupToTrack("Dave_eye", RENDER_GROUP_HIDDEN);
			mCrazyDaveBlinkReanimID = ReanimationGetID(aBlinkReanim);
		}
	}

	Reanimation* aBlinkReanim = ReanimationTryToGet(mCrazyDaveBlinkReanimID);
	if (aBlinkReanim && aBlinkReanim->mLoopCount > 0)
	{
		aCrazyDaveReanim->AssignRenderGroupToTrack("Dave_eye", RENDER_GROUP_NORMAL);
		RemoveReanimation(mCrazyDaveBlinkReanimID);
		mCrazyDaveBlinkReanimID = ReanimationID::REANIMATIONID_NULL;
	}

	aCrazyDaveReanim->Update();
}

          
void LawnApp::DrawCrazyDave(Graphics* g)
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim == nullptr)
		return;

	if (mCrazyDaveMessageText.size())
	{
		Image* aBubbleImage = IMAGE_STORE_SPEECHBUBBLE2;
		int aPosX = 285;
		int aPosY = 20;
		if (GetDialog(Dialogs::DIALOG_STORE))
		{
			aBubbleImage = IMAGE_STORE_SPEECHBUBBLE;
			aPosX -= 180;
			aPosY -= 78;
		}
		else if (mGameMode == GameMode::GAMEMODE_UPSELL)
		{
			aPosX += 130;
			aPosY += 70;
		}
		g->DrawImage(aBubbleImage, aPosX, aPosY);

		SexyString aBubbleText = mCrazyDaveMessageText;
		Rect aRect(aPosX + 25, aPosY + 6, 233, 144);
		if (aBubbleText.find(_S("{SHAKE}")) != SexyString::npos)
		{
			aBubbleText = TodReplaceString(aBubbleText, _S("{SHAKE}"), _S(""));
			aRect.mX += rand() % 2;
			aRect.mY += rand() % 2;
		}

		bool clickToContinue = true;
		if (mGameMode == GameMode::GAMEMODE_UPSELL)
		{
			clickToContinue = false;
		}
		else if (aBubbleText.find(_S("{NO_CLICK}")) != SexyString::npos)
		{
			aBubbleText = TodReplaceString(aBubbleText, _S("{NO_CLICK}"), _S(""));
			clickToContinue = false;
		}

		TodDrawStringWrapped(g, aBubbleText, aRect, FONT_BRIANNETOD16, Color::Black, DrawStringJustification::DS_ALIGN_CENTER_VERTICAL_MIDDLE);
		if (clickToContinue)
		{
			TodDrawString(g, _S("click to continue"), aPosX + 139, aPosY + 140, FONT_PICO129, Color::Black, DrawStringJustification::DS_ALIGN_CENTER);
		}
	}

	aCrazyDaveReanim->Draw(g);
}

          
int LawnApp::GetNumPreloadingTasks()
{
	int aTaskCount = IsScreenSaver() ? 0 : 13;
	if (mPlayerInfo)
	{
		for (SeedType i = SeedType::SEED_PEASHOOTER; i < SeedType::NUM_SEED_TYPES; i = (SeedType)((int)i + 1))
		{
			if (SeedTypeAvailable(i) || HasFinishedAdventure())
			{
				aTaskCount++;
			}

		}

		for (ZombieType i = ZombieType::ZOMBIE_NORMAL; i < ZombieType::NUM_ZOMBIE_TYPES;i = (ZombieType)((int)i + 1))
		{
			if (IsScreenSaver()) break;

			if (HasFinishedAdventure() || mPlayerInfo->mLevel >= GetZombieDefinition(i).mStartingLevel)
			{
				                                     
                                        
                                          
                                      
                                       
				{
					aTaskCount++;
				}
			}
		}
	}
	return aTaskCount * 68;
}

          
void LawnApp::PreloadForUser()
{
	int aNumTasks = mCompletedLoadingThreadTasks + GetNumPreloadingTasks();
	if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
	{
		TodTrace("preload canceled\n");
		mNumLoadingThreadTasks = aNumTasks;
		return;
	}

	if (!IsScreenSaver())
	{
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_PUFF, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LAWN_MOWERED_ZOMBIE, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_READYSETPLANT, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;       
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_FINAL_WAVE, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SUN, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_FADE_ON, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;       
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE_NEWSPAPER, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;       
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SELECTOR_SCREEN, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 340;       
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE_HAND, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_SLIDE_ON, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_SLIDE_DOWN, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SELECTORSCREEN_SPOTLIGHT, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES3, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES4, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES5, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES3, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES4, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES5, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 408;
	}

	if (mPlayerInfo)
	{
		for (SeedType i = SeedType::SEED_PEASHOOTER; i < SeedType::NUM_SEED_TYPES; i = (SeedType)((int)i + 1))
		{
			if (SeedTypeAvailable(i) || HasFinishedAdventure())
			{
				Plant::PreloadPlantResources(i);
				if (mCompletedLoadingThreadTasks < aNumTasks)
				{
					mCompletedLoadingThreadTasks += 68;
				}

				if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
				{
					TodTrace("preload canceled\n");
					mNumLoadingThreadTasks = aNumTasks;
					return;
				}

				if (mShutdown || mCloseRequest)
				{
					return;
				}
			}
		}

		for (ZombieType i = ZombieType::ZOMBIE_NORMAL; i < ZombieType::NUM_ZOMBIE_TYPES;i = (ZombieType)((int)i + 1))
		{
			if (IsScreenSaver()) break;

			if (mPlayerInfo->mLevel >= GetZombieDefinition(i).mStartingLevel || HasFinishedAdventure())
			{
				if (i == ZombieType::ZOMBIE_BOSS && i == ZombieType::ZOMBIE_CATAPULT && i == ZombieType::ZOMBIE_GARGANTUAR ||
					i == ZombieType::ZOMBIE_DIGGER && i == ZombieType::ZOMBIE_ZAMBONI)
				{
					continue;
				}

				Zombie::PreloadZombieResources(i);
				if (mCompletedLoadingThreadTasks < aNumTasks)
				{
					mCompletedLoadingThreadTasks += 68;
				}

				if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
				{
					TodTrace("preload canceled\n");
					mNumLoadingThreadTasks = aNumTasks;
					return;
				}

				if (mShutdown || mCloseRequest)
				{
					return;
				}
			}
		}
	}

	if (mCompletedLoadingThreadTasks != aNumTasks)
	{
		TodTrace("num preload tasks wasn't calculated correctly");
		mCompletedLoadingThreadTasks = aNumTasks;
	}
}


          
void LawnApp::EnforceCursor()
{
	if (mSEHOccured || !mMouseIn)
	{
		SDL_SetCursor(NULL);
		return;
	}

    LRESULT hitTest = SendMessage(mHWnd, WM_NCHITTEST, 0, MAKELPARAM(GET_X_LPARAM(GetMessagePos()), GET_Y_LPARAM(GetMessagePos())));
	if (hitTest != HTCLIENT)
		return;

	SDL_Cursor* newCursor = nullptr;

	if (mIsPlayingVideo)
	{
		SDL_HideCursor();
	}
	else if (mOverrideCursor)
	{
		SDL_SetCursor(newCursor);
		SDL_ShowCursor();
	}
	else
	{
		switch (mCursorNum)
		{
			                                                            
			case CURSOR_HAND: newCursor = mSDLHandCursor; break;
			case CURSOR_DRAGGING: newCursor = mSDLDraggingCursor; break;
			case CURSOR_TEXT: newCursor = mSDLTextCursor; break;
			case CURSOR_CIRCLE_SLASH: newCursor = mSDLNoCursor; break;
			case CURSOR_WAIT: newCursor = mSDLWaitCursor; break;
			case CURSOR_CUSTOM:
			case CURSOR_NONE: SDL_HideCursor(); break;
			default: newCursor = IsParticleEditor() ? SDL_GetDefaultCursor() : mSDLPointerCursor; break;
		}
	}

	if (newCursor || IsParticleEditor())
	{
		SDL_ShowCursor();
		SDL_SetCursor(newCursor);
	}
}

          
SexyString LawnApp::Pluralize(int theCount, const SexyChar* theSingular, const SexyChar* thePlural)
{
	if (theCount == 1)
	{
		return TodReplaceNumberString(theSingular, _S("{COUNT}"), theCount);
	}

	return TodReplaceNumberString(thePlural, _S("{COUNT}"), theCount);
}

          
int LawnApp::GetNumTrophies(ChallengePage thePage)
{
	int aNumTrophies = 0;

	for (int i = 0; i < NUM_CHALLENGE_MODES; i++)
	{
		const ChallengeDefinition& aDef = GetChallengeDefinition(i);
		if (aDef.mPage == thePage && HasBeatenChallenge(aDef.mChallengeMode))
		{
			aNumTrophies++;
		}
	}

	return aNumTrophies;
}

          
int LawnApp::TrophiesNeedForGoldSunflower()
{
    if (mLocalCoop) return 2;
	return 48 - GetNumTrophies(CHALLENGE_PAGE_SURVIVAL) - GetNumTrophies(CHALLENGE_PAGE_CHALLENGE) - GetNumTrophies(CHALLENGE_PAGE_PUZZLE);
}

          
bool LawnApp::EarnedGoldTrophy()
{
	return HasFinishedAdventure() && TrophiesNeedForGoldSunflower() <= 0;
}

void LawnApp::FinishZenGardenToturial()
{
	mBoardResult = BoardResult::BOARDRESULT_WON;
	KillBoard();
	PreNewGame(GameMode::GAMEMODE_ADVENTURE, false);
}

          
bool LawnApp::IsTrialStageLocked()
{
	if (mDebugTrialLocked)
		return true;

	if (mDRM && mDRM->QueryData())
		return false;

	return mTrialType == TrialType::TRIALTYPE_STAGELOCKED;
}

          
void LawnApp::InitHook()
{
#ifdef _DEBUG
	mDRM = nullptr;
#else
	mDRM = new PopDRMComm();
	mDRM->DoIPC();
	if (sexystricmp(GetString("MarketingMode", _S("")).c_str(), _S("StageLocked")) == 0)
	{
		mTrialType = TrialType::TRIALTYPE_STAGELOCKED;
		mDRM->EnableLocking();
	}
	else
	{
		mTrialType = TrialType::TRIALTYPE_NONE;
	}
#endif
}

          
SexyString LawnApp::GetMoneyString(int theAmount)
{
	int aValue = theAmount * 10;
	if (aValue > 999999)
	{
		return StrFormat(_S("$%d,%03d,%03d"), aValue / 1000000, (aValue - aValue / 1000000 * 1000000) / 1000, aValue - aValue / 1000 * 1000);
	}
	else if (aValue > 9999)
	{
		return StrFormat(_S("$%d,%03d"), aValue / 1000, aValue - aValue / 1000 * 1000);
	}
	else
	{
		return StrFormat(_S("$%d"), aValue);
	}
}

          
SexyString LawnGetCurrentLevelName()
{
	if (gLawnApp && gLawnApp->mGameMode == GAMEMODE_VERSUS) return _S("Versus Mode");
	if (gLawnApp == nullptr)
	{
		return _S("Before App");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_LOADING)
	{
		return _S("Game Loading");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_MENU)
	{
		return _S("Game Selector");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_AWARD)
	{
		return _S("Award Screen");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_CHALLENGE)
	{
		return _S("Challenge Screen");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_CREDIT)
	{
		return _S("Credits");
	}
	if (gLawnApp->mBoard == nullptr)
	{
		return _S("Not Playing");
	}

	if (gLawnApp->IsFirstTimeAdventureMode())
	{
		return gLawnApp->GetStageString(gLawnApp->mBoard->mLevel);
	}
	if (gLawnApp->IsAdventureMode())
	{
		return _S("F") + gLawnApp->GetStageString(gLawnApp->mBoard->mLevel);
	}

	return gLawnApp->GetCurrentChallengeDef().mChallengeName;
}

          
bool LawnApp::CanDoPinataMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 1000;
}

          
bool LawnApp::CanDoDanceMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 500;
}

          
bool LawnApp::CanDoDaisyMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 100;
}

          
void LawnApp::PlaySample(int theSoundNum)
{
	if (!mMuteSoundsForCutscene)
	{
		SexyAppBase::PlaySample(theSoundNum);
	}
}

          
void LawnApp::SwitchScreenMode(bool wantWindowed, bool is3d, bool force)
{
	                                                           
	mIsWindowed = wantWindowed;
	SDL_SetWindowFullscreen(mSDLWindow, !wantWindowed);

	NewOptionsDialog* aNewOptionsDialog = (NewOptionsDialog*)GetDialog(Dialogs::DIALOG_NEWOPTIONS);
	if (aNewOptionsDialog)
	{
		aNewOptionsDialog->mFullscreenCheckbox->SetChecked(!mIsWindowed);
	}
}

                                                                                                          

void LawnApp::BetaSubmit(bool theAskForComments)
{

}

void LawnApp::BetaRecordLevelStats()
{

}

void LawnApp::BetaAddFile(std::list<std::string>& theUploadFileList, std::string theFileName, std::string theShortName)
{

}

void LawnApp::TraceLoadGroup(const char* theGroupName, int theGroupTime, int theTotalGroupWeigth, int theTaskWeight)
{

}

                                                                                                          

void LawnApp::DoHighScoreDialog()
{

}

void LawnApp::DoRegister()
{

}

void LawnApp::DoRegisterError()
{

}

bool LawnApp::CanDoRegisterDialog()
{
	return false;
}

void LawnApp::DoNeedRegisterDialog()
{

}

void LawnApp::FinishModelessDialogs()
{

}

bool LawnApp::NeedRegister()
{
	return false;
}

void LawnApp::UpdateRegisterInfo()
{

}

#ifdef _HAS_ZOMBATAR
void LawnApp::ShowZombatarTOS()
{
	KillDialog(Dialogs::DIALOG_ZOMBATARTOS);

	ZombatarTOS* aDialog = new ZombatarTOS(this);
	aDialog->mApp = this;
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	aDialog->mY -= 16.5f;
	AddDialog(Dialogs::DIALOG_ZOMBATARTOS, aDialog);
	mWidgetManager->SetFocus(aDialog);
}
#endif

void LawnApp::ShowLanagugeScreen()
{
	mLanguageScreen = new LanguageWidget(this);
	mWidgetManager->AddWidget(mLanguageScreen);
	mWidgetManager->BringToBack(mLanguageScreen);
	mWidgetManager->SetFocus(mLanguageScreen);
}

void LawnApp::KillLanguageScreen()
{
	if (mLanguageScreen)
	{
		mWidgetManager->RemoveWidget(mLanguageScreen);
		SafeDeleteWidget(mLanguageScreen);
		mLanguageScreen = nullptr;
	}
}

bool LawnApp::ChallengeUsesMicrophone(GameMode theGameMode)
{
	return
#ifdef _DS_MINIGAMES
		theGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE;      
#endif
		                                                                                      
}

bool LawnApp::ChallengeHasScores(GameMode theGameMode)
{
#ifndef _HAS_SCORE_SYSTEM
	return false;
#endif
	return IsEndlessIZombie(theGameMode) || IsEndlessScaryPotter(theGameMode) || IsSurvivalEndless(theGameMode) || IsLastStandEndless(theGameMode);
}

bool LawnApp::IsLastStand() {
	bool aIsLastStand = mGameMode == GameMode::GAMEMODE_CHALLENGE_LAST_STAND;
	aIsLastStand |= (mGameMode >= GameMode::GAMEMODE_LAST_STAND_STAGE_1 && mGameMode <= GameMode::GAMEMODE_LAST_STAND_STAGE_5);
	aIsLastStand |= IsLastStandEndless(mGameMode);
	return aIsLastStand;
}

bool LawnApp::IsLastStandEndless(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_1;
	return aLevel >= 0 && aLevel <= 4;
}

void LawnApp::ShowParticleEditor()
{
	throw std::runtime_error("Particle editor is not available in the initial OpenFL host");
}

bool LawnApp::TryToInitializePA()
{
	PaError err = Pa_Initialize();
	if (err != paNoError)
		return false;

	err = Pa_OpenDefaultStream(&mPortAudioStream, 1, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, AudioCallback, &mVoiceVolume);
	if (err != paNoError)
		return false;

	err = Pa_StartStream(mPortAudioStream);
	if (err != paNoError)
		return false;

	return true;
}




void LawnApp::DoMoreSettingsDialog()
{
	MoreSettingsDialog* aDialog = new MoreSettingsDialog(this);
	AddDialog(Dialogs::DIALOG_MORESETTINGS, aDialog);
}
