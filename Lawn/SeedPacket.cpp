#include "Board.h"
#include "Cutscene.h"
#include "Challenge.h"
#include "SeedPacket.h"
#include "../LawnApp.h"
#include "CursorObject.h"
#include "../Resources.h"
#include "MessageWidget.h"
#include "../SexyAppFramework/Font.h"
#include "../Sexy.TodLib/FilterEffect.h"
#include "../SexyAppFramework/SexyMatrix.h"

SeedPacket::SeedPacket()
{
	mSlotMachiningPosition = 0.0f;
	mPacketType = SeedType::SEED_NONE;
	mImitaterType = SeedType::SEED_NONE;
	mIndex = -1;
	mSlotMachiningNextSeed = SeedType::SEED_NONE;
	mWidth = 50;
	mHeight = 70;
	mRefreshCounter = 0;
	mRefreshTime = 0;
	mRefreshing = false;
	mActive = true;
	mOffsetX = 0;
	mSlotMachineCountDown = 0;
	mSlotMachiningNextSeed = SeedType::SEED_NONE;
	mTimesUsed = 0;
}

          
void SeedPacket::PickNextSlotMachineSeed()
{
	int aPeasCount = mBoard->CountPlantByType(SeedType::SEED_PEASHOOTER);

	SeedType SLOT_SEED_TYPES[] = {
		SeedType::SEED_SUNFLOWER,
		SeedType::SEED_PEASHOOTER,
		SeedType::SEED_SNOWPEA,
		SeedType::SEED_WALLNUT,
		SeedType::SEED_SLOT_MACHINE_SUN,
		SeedType::SEED_SLOT_MACHINE_DIAMOND
	};

	int aSeedsCount = 0;
	TodWeightedArray<SeedType> aSeedWeightArray[(int)SeedType::NUM_SEED_TYPES];
	for (int i = 0; i < LENGTH(SLOT_SEED_TYPES); i++)
	{
		SeedType aSeedType = SLOT_SEED_TYPES[i];

		int aWeight = 100;
		if (aSeedType == SeedType::SEED_PEASHOOTER)
		{
			aWeight = TodAnimateCurve(0, 5, aPeasCount, 200, 100, TodCurves::CURVE_LINEAR);
		}
		else if (aSeedType == SeedType::SEED_SLOT_MACHINE_DIAMOND)
		{
			aWeight = 30;
		}

		if (mIndex == 2 && aSeedType != SeedType::SEED_SLOT_MACHINE_DIAMOND)
		{
			if (aSeedType == mBoard->mSeedBank->mSeedPackets[0].mSlotMachiningNextSeed || aSeedType == mBoard->mSeedBank->mSeedPackets[1].mSlotMachiningNextSeed)
			{
				aWeight += aWeight / 2;
			}
		}

		aSeedWeightArray[aSeedsCount].mItem = aSeedType;
		aSeedWeightArray[aSeedsCount].mWeight = aWeight;
		aSeedsCount++;
	}

	mSlotMachiningNextSeed = (SeedType)TodPickFromWeightedArray(aSeedWeightArray, aSeedsCount);
}

void SeedPacket::SlotMachineStart()
{
	mSlotMachineCountDown = 300;
	mSlotMachiningPosition = 0.0f;
	PickNextSlotMachineSeed();
}

          
void SeedPacket::FlashIfReady()
{
	if (!CanPickUp() || mApp->mEasyPlantingCheat)
		return;

	if (!mBoard->HasConveyorBeltSeedBank())
	{
		int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_UI_BOTTOM, 0, 2);
		mApp->AddTodParticle(mX + mBoard->mSeedBank->mX, mY + mBoard->mSeedBank->mY, aRenderPosition, ParticleEffect::PARTICLE_SEED_PACKET_FLASH);
	}
	
	if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_1_REFRESH_PEASHOOTER)
	{
		mBoard->SetTutorialState(TutorialState::TUTORIAL_LEVEL_1_PICK_UP_PEASHOOTER);
	}
	else if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_2_REFRESH_SUNFLOWER && mPacketType == SeedType::SEED_SUNFLOWER)
	{
		mBoard->SetTutorialState(TutorialState::TUTORIAL_LEVEL_2_PICK_UP_SUNFLOWER);
	}
	else if (mBoard->mTutorialState == TutorialState::TUTORIAL_MORESUN_REFRESH_SUNFLOWER && mPacketType == SeedType::SEED_SUNFLOWER)
	{
		mBoard->SetTutorialState(TutorialState::TUTORIAL_MORESUN_PICK_UP_SUNFLOWER);
	}
}

void SeedPacket::Activate()
{
	TOD_ASSERT(mPacketType != SeedType::SEED_NONE);
	mActive = true;
}

void SeedPacket::Deactivate()
{
	mActive = false;
	mRefreshCounter = 0;
	mRefreshTime = 0;
	mRefreshing = false;
}

void SeedPacket::SetActivate(bool theActive)
{
	if (theActive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

          
void SeedPacket::Update()
{
	if (mApp && mApp->mGameScene != GameScenes::SCENE_PLAYING || mPacketType == SeedType::SEED_NONE)
	{
		return;
	}

	if (mBoard->mMainCounter == 0)
	{
		FlashIfReady();
	}

	if (!mActive && mRefreshing)
	{
		mRefreshCounter++;
		if (mRefreshCounter > mRefreshTime)
		{
			mRefreshCounter = 0;
			mRefreshing = false;
			Activate();
			FlashIfReady();
		}
	}

	if (mSlotMachineCountDown > 0)
	{
		mSlotMachineCountDown--;
		float aFlipsPerSecont = TodAnimateCurveFloat(SLOT_MACHINE_TIME, 0, mSlotMachineCountDown, 6.0f, 2.0f, TodCurves::CURVE_LINEAR);
		mSlotMachiningPosition += aFlipsPerSecont * 0.01f;

		if (mSlotMachiningPosition >= 1.0f)
		{
			mPacketType = mSlotMachiningNextSeed;
			if (mSlotMachineCountDown == 0)
			{
				Activate();
				mSlotMachiningPosition = 0.0f;
			}
			else
			{
				mSlotMachiningPosition -= 1.0f;
				PickNextSlotMachineSeed();
			}
		}
		else if (mSlotMachineCountDown == 0)
		{
			mSlotMachineCountDown = 1;
		}
	}
}

          
void SeedPacketDrawSeed(Graphics* g, float x, float y, SeedType theSeedType, SeedType theImitaterType, float theOffsetX, float theOffsetY, float theScale)
{
	Image* aImage = IMAGE_PACKET_PLANTS;
	SeedType aSeedType = theSeedType;
	if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
	{
		aSeedType = theImitaterType;
		FilterEffect aFilterEffect = FilterEffect::FILTER_EFFECT_WASHED_OUT;
		if (theImitaterType == SeedType::SEED_HYPNOSHROOM || theImitaterType == SeedType::SEED_SQUASH || theImitaterType == SeedType::SEED_POTATOMINE ||
			theImitaterType == SeedType::SEED_GARLIC || theImitaterType == SeedType::SEED_LILYPAD)
			aFilterEffect = FilterEffect::FILTER_EFFECT_LESS_WASHED_OUT;
		aImage = FilterEffectGetImage(aImage, aFilterEffect);
	}

	                                                                   
  
                                                                        
  
                                                                    
  
                                                                        
  
                                                                        
  
                                                                        
  
                                                                    
  
                                                                        
  
                                                                   
  
                                                                        
  
                                                                         
  
                                                                        
  
                                                                          
  
                                                                        
  
                                                                      
  
                                                                        
  
                                                                        
  
                                                                        
  
                                                                       
  
                                                                        
  
                                                                      
  
                                                                         
  
                                                                        
  
                                                                         
  
                                                                      
  
                                                                         
  
       
	{
		g->PushState();
		
		g->mScaleX = theScale * g->mScaleX;
		g->mScaleY = theScale * g->mScaleY;
		if (theSeedType == SeedType::SEED_ZOMBIE_BUNGEE)
		{
			g->mClipRect.mY = y + theOffsetY + 10 + gSexyAppBase->mDDInterface->mWideScreenOffsetY;
			y -= 180;
		}
		if (theSeedType == SeedType::SEED_ZOMBIE_POLEVAULTER)
		{
			g->SetClipRect(x + theOffsetX + 25, g->mClipRect.mY, 40, g->mClipRect.mHeight);
		}
		Plant::DrawSeedType(g, theSeedType, theImitaterType, DrawVariation::VARIATION_NORMAL, x + theOffsetX, y + theOffsetY);
		g->PopState();
	}
}

          
void DrawSeedPacket(Graphics* g, float x, float y, SeedType theSeedType, SeedType theImitaterType, float thePercentDark, int theGrayness, bool theDrawCost, bool theUseCurrentCost)
{
	g->PushState();
	g->SetLinearBlend(true);

	SeedType aSeedType = theSeedType;
	if (aSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
	{
		aSeedType = theImitaterType;
	}

	if (theGrayness != 255)
	{
		g->SetColor(Color(theGrayness, theGrayness, theGrayness));
		g->SetColorizeImages(true);
	}
	else if (thePercentDark > 0)
	{
		g->SetColor(Color(128, 128, 128, 255));
		g->SetColorizeImages(true);
	}

	int aPacketBackground =
		theSeedType == SeedType::SEED_IMITATER ? 0 :
		Plant::IsUpgrade(aSeedType) ? 1 :
		theSeedType == SeedType::SEED_BEGHOULED_BUTTON_CRATER ? 3 :
		theSeedType == SeedType::SEED_BEGHOULED_BUTTON_SHUFFLE ? 4 :
		theSeedType == SeedType::SEED_SLOT_MACHINE_SUN ? 5 :
		theSeedType == SeedType::SEED_SLOT_MACHINE_DIAMOND ? 6 :
		theSeedType == SeedType::SEED_ZOMBIQUARIUM_SNORKLE ? 7 :
		theSeedType == SeedType::SEED_ZOMBIQUARIUM_TROPHY ? 8 : 2;

	                                       
	{
		if (aPacketBackground == 0)	TodDrawImageScaledF(g, Sexy::IMAGE_SEEDPACKETIMITATER_LARGER, x, y, g->mScaleX * 0.5f, g->mScaleY * 0.5f);
		else if (aPacketBackground == 1) TodDrawImageScaledF(g, Sexy::IMAGE_SEEDPACKETUPGRADE_LARGER, x, y, g->mScaleX * 0.5f, g->mScaleY * 0.5f);
		else if (aPacketBackground == 2) TodDrawImageScaledF(g, Sexy::IMAGE_SEEDPACKET_LARGER, x, y, g->mScaleX * 0.5f, g->mScaleY * 0.5f);
		else
		{
			TodDrawImageCelScaledF(g, Sexy::IMAGE_SEEDS, x, y, aPacketBackground, 0, g->mScaleX, g->mScaleY);
		}
	}
	                                              
  
                                                                 
    
	      
  
                                                                                                   
    

	float aScale = 0.5f;
	bool aDrawSeedInMiddle = true;
	float aOffsetX = 5.0f;
	float aOffsetY = 8.0f;
	switch (aSeedType)
	{
	case SeedType::SEED_TALLNUT:
		aScale = 0.3f;
		aOffsetX = 12.0f;
		aOffsetY = 22.0f;
		break;

	case SeedType::SEED_INSTANT_COFFEE:
		aScale = 0.55f;
		aOffsetX = 0;
		aOffsetY = 9.0f;
		break;

	case SeedType::SEED_COBCANNON:
		aScale = 0.26f;
		aOffsetX = 6.5f;
		aOffsetY = 23.0f;
		break;

	case SeedType::SEED_CACTUS:
		aOffsetX = 9.0f;
		aOffsetY = 13.0f;
		break;

	case SeedType::SEED_POTATOMINE:
		aScale = 0.4f;
		aOffsetX = 8.0f;
		aOffsetY = 12.0f;
		break;

	case SeedType::SEED_MAGNETSHROOM:
		aOffsetY = 12.0f;
		break;

	case SeedType::SEED_PUMPKINSHELL:
	case SeedType::SEED_CHOMPER:
	case SeedType::SEED_HYPNOSHROOM:
	case SeedType::SEED_SPIKEROCK:
		aScale = 0.4f;
		aOffsetX = 8.0f;
		aOffsetY = 12.0f;
		break;

	case SeedType::SEED_FUMESHROOM:
	case SeedType::SEED_DOOMSHROOM:
	case SeedType::SEED_SQUASH:
	case SeedType::SEED_SPIKEWEED:
	case SeedType::SEED_PLANTERN:
	case SeedType::SEED_TORCHWOOD:
	case SeedType::SEED_TANGLEKELP:
		aScale = 0.4f;
		aOffsetX = 8.0f;
		aOffsetY = 12.0f;
		break;

	case SeedType::SEED_TWINSUNFLOWER:
		aScale = 0.45f;
		aOffsetX = 7.0f;
		aOffsetY = 14.0f;
		break;
	case SeedType::SEED_GLOOMSHROOM:
		aScale = 0.45f;
		aOffsetX = 7.0f;
		aOffsetY = 14.0f;
		break;

	case SeedType::SEED_CATTAIL:
		aScale = 0.45f;
		aOffsetX = 8.0f;
		aOffsetY = 13.0f;
		break;

	case SeedType::SEED_UMBRELLA:
		aScale = 0.5f;
		aOffsetX = 5.0f;
		aOffsetY = 10.0f;
		break;

	case SeedType::SEED_KERNELPULT:
		aScale = 0.4f;
		aOffsetX = 13.0f;
		aOffsetY = 14.0f;
		break;

	case SeedType::SEED_CABBAGEPULT:
		aScale = 0.4f;
		aOffsetX = 15.0f;
		aOffsetY = 14.0f;
		break;

	case SeedType::SEED_MELONPULT:
	case SeedType::SEED_WINTERMELON:
		aScale = 0.35f;
		aOffsetX = 18.0f;
		aOffsetY = 19.0f;
		break;

	case SeedType::SEED_GRAVEBUSTER:
		aScale = 0.4f;
		aOffsetX = 10.0f;
		aOffsetY = 15.0f;
		break;

	case SeedType::SEED_SPLITPEA:
		aScale = 0.45f;
		aOffsetX = 12.0f;
		aOffsetY = 12.0f;
		break;

	case SeedType::SEED_BLOVER:
		aScale = 0.4f;
		aOffsetX = 8.0f;
		aOffsetY = 17.0f;
		break;

	case SeedType::SEED_STARFRUIT:
		aScale = 0.5f;
		aOffsetX = 6.0f;
		aOffsetY = 8.0f;
		break;

	case SeedType::SEED_THREEPEATER:
		aScale = 0.5f;
		aOffsetX = 5.0f;
		aOffsetY = 10.0f;
		break;

	case SeedType::SEED_GATLINGPEA:
		aScale = 0.5f;
		aOffsetX = 2.0f;
		aOffsetY = 8.0f;
		break;

	case SeedType::SEED_IMITATER:
		aOffsetY = 10.0f;
		break;

	case SeedType::SEED_ZOMBIE_NORMAL:
	case SeedType::SEED_ZOMBIE_TRAFFIC_CONE:
	case SeedType::SEED_ZOMBIE_PAIL:
	case SeedType::SEED_ZOMBIE_DANCER:
		aScale = 0.35f;
		aOffsetX = -3.0f;
		aOffsetY = -7.0f;
		break;

	case SeedType::SEED_ZOMBIE_POLEVAULTER:
		aScale = 0.35f;
		aOffsetX = -8.0f - 16.25f;
		aOffsetY = -12.0f;
		break;

	case SeedType::SEED_ZOMBIE_LADDER:
	case SeedType::SEED_ZOMBIE_DIGGER:
	case SeedType::SEED_ZOMBIE_SCREEN_DOOR:
	case SeedType::SEED_ZOMBIE_POGO:
		aScale = 0.35f;
		aOffsetX = -3.0f;
		aOffsetY = -10.0f;
		break;

	case SeedType::SEED_ZOMBIE_BUNGEE:
		aScale = 0.3f;
		aOffsetX = 1.0f;
		aOffsetY = -1.0f;
		break;

	case SeedType::SEED_ZOMBIE_FOOTBALL:
		aScale = 0.33f;
		aOffsetX = -7.0f;
		aOffsetY = -9.0f;
		break;

	case SeedType::SEED_ZOMBIE_BALLOON:
		aScale = 0.35f;
		aOffsetX = -3.0f;
		aOffsetY = -5.0f;
		break;

	case SeedType::SEED_ZOMBIE_IMP:
		aScale = 0.4f;
		aOffsetX = -12.0f;
		aOffsetY = -17.0f;
		break;

	case SeedType::SEED_ZOMBONI:
		aScale = 0.23f;
		aOffsetX = 12.0f;
		aOffsetY = 3.0f;
		break;

	case SeedType::SEED_ZOMBIE_GARGANTUAR:
		aScale = 0.23f;
		aOffsetX = 4.0f;
		aOffsetY = 3.0f;
		break;

	case SeedType::SEED_BEGHOULED_BUTTON_SHUFFLE:
	case SeedType::SEED_BEGHOULED_BUTTON_CRATER:
	case SeedType::SEED_SLOT_MACHINE_SUN:
	case SeedType::SEED_SLOT_MACHINE_DIAMOND:
	case SeedType::SEED_ZOMBIQUARIUM_SNORKLE:
	case SeedType::SEED_ZOMBIQUARIUM_TROPHY:
		aDrawSeedInMiddle = false;
		break;
	}
	if (!((LawnApp*)gSexyAppBase)->GetDialog(Dialogs::DIALOG_ALMANAC) &&
		((LawnApp*)gSexyAppBase)->mGameMode == GameMode::GAMEMODE_CHALLENGE_BIG_TIME)
	{
		if (aSeedType == SeedType::SEED_WALLNUT || aSeedType == SeedType::SEED_SUNFLOWER || aSeedType == SeedType::SEED_MARIGOLD)
		{
			aOffsetX = 16.0f;
			aOffsetY = 34.0f;
		}
	}
	if (aSeedType == SeedType::SEED_GIANT_WALLNUT)
	{
		aScale *= 0.75f;
		aOffsetX = 52.0f;
		aOffsetY = 58.0f;
	}
	aOffsetX = g->mScaleX * aOffsetX;
	aOffsetY = g->mScaleY * (aOffsetY + 1.0f);
	if (aDrawSeedInMiddle)
	{
		SeedPacketDrawSeed(g, x, y, theSeedType, theImitaterType, aOffsetX, aOffsetY, aScale);
	}

	if (thePercentDark > 0.0f)
	{
		int aDarknessHeight = FloatRoundToInt(68.0f * thePercentDark) + 2;
		g->SetColor(Color(64, 64, 64, 255));
		g->SetColorizeImages(true);
		g->ClipRect(x, y, SEED_PACKET_WIDTH * g->mScaleX, aDarknessHeight * g->mScaleY);
		TodDrawImageCelScaledF(g, Sexy::IMAGE_SEEDS, x, y, aPacketBackground, 0, g->mScaleX, g->mScaleY);
		if (aDrawSeedInMiddle)
		{
			SeedPacketDrawSeed(g, x, y, theSeedType, theImitaterType, aOffsetX, aOffsetY, aScale);
		}
		g->SetColor(Color::White);
		g->SetColorizeImages(false);
		g->ClearClipRect();
	}

	if (theDrawCost)
	{
		SexyString aCostStr;
		if (gLawnApp->mBoard && gLawnApp->mBoard->PlantUsesAcceleratedPricing(aSeedType) && !gLawnApp->GetDialog(Dialogs::DIALOG_ALMANAC)
			)
		{
			if (theUseCurrentCost)
			{
				aCostStr = StrFormat(_S("%d"), gLawnApp->mBoard->GetCurrentPlantCost(theSeedType, theImitaterType));
			}
			else
			{
				aCostStr = StrFormat(_S("%d+"), Plant::GetCost(theSeedType, theImitaterType));
			}
		}
		else
		{
			aCostStr = StrFormat(_S("%d"), Plant::GetCost(theSeedType, theImitaterType));
		}


		g->SetLinearBlend(false);
		g->SetPixelArtBlend(true);

		Font* aTextFont = Sexy::FONT_PICO129;
		int aTextOffsetX = 32 - aTextFont->StringWidth(aCostStr);
		int aTextOffsetY = aTextFont->GetAscent() + 54;
		if (g->mScaleX == 1.0f && g->mScaleY == 1.0f)
		{
			TodDrawString(g, aCostStr, x + aTextOffsetX, y + aTextOffsetY, aTextFont, Color::Black, DS_ALIGN_LEFT);
		}
		else
		{
			SexyMatrix3 aMatrix;
			TodScaleTransformMatrix(aMatrix, aTextOffsetX * g->mScaleX + x + gSexyAppBase->mDDInterface->mWideScreenOffsetX, aTextOffsetY * g->mScaleY + y + gSexyAppBase->mDDInterface->mWideScreenOffsetY, g->mScaleX, g->mScaleY);
			                         
			    
			                                 
			    
			TodDrawStringMatrix(g, aTextFont, aMatrix, aCostStr, Color::Black);
		}
	}
	g->PopState();
}

          
void SeedPacket::Draw(Graphics* g)
{
	float aPercentDark = 0.0f;
	if (!mActive)
	{
		if (mRefreshTime == 0)
		{
			aPercentDark = 1.0f;
		}
		else
		{
			aPercentDark = (float)(mRefreshTime - mRefreshCounter) / (float)mRefreshTime;
		}
	}

	if (mSlotMachineCountDown > 0)
	{
		int aOffsetY = FloatRoundToInt(-mHeight * mSlotMachiningPosition);

		g->PushState();
		g->ClipRect(0, 0, mWidth * g->mScaleX, mHeight * g->mScaleY);
		DrawSeedPacket(g, 0.0f, aOffsetY, mPacketType, SeedType::SEED_NONE, 0.0f, 128, false, false);
		DrawSeedPacket(g, 0.0f, mHeight + aOffsetY, mSlotMachiningNextSeed, SeedType::SEED_NONE, 0.0f, 128, false, false);
		g->PopState();
	}
	else
	{
		SeedType aUseSeedType = mPacketType;
		if (mPacketType == SeedType::SEED_IMITATER && mImitaterType != SeedType::SEED_NONE)
		{
			aUseSeedType = mImitaterType;
		}

		bool aDrawCost = true;
		if (mBoard->HasConveyorBeltSeedBank() || mApp->IsSlotMachineLevel())
		{
			aDrawCost = false;
		}
		int aCost = mBoard->GetCurrentPlantCost(mPacketType, mImitaterType);

		int aGrayness = 255;
		if ((mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED && !mActive) ||
			(mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST && !mActive))
		{
			aGrayness = 64;
		}
		else if (mApp->mGameScene != GameScenes::SCENE_PLAYING)
		{
			aGrayness = mBoard->mSeedBank->mCutSceneDarken;
			aPercentDark = 0.0f;
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_1_PICK_UP_PEASHOOTER && mBoard->mTutorialTimer == -1 && mPacketType == SeedType::SEED_PEASHOOTER)
		{
			aGrayness = GetFlashingColor(mBoard->mMainCounter, 75).mRed;
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_2_PICK_UP_SUNFLOWER && mPacketType == SeedType::SEED_SUNFLOWER)
		{
			aGrayness = GetFlashingColor(mBoard->mMainCounter, 75).mRed;
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_MORESUN_PICK_UP_SUNFLOWER && mPacketType == SeedType::SEED_SUNFLOWER)
		{
			aGrayness = GetFlashingColor(mBoard->mMainCounter, 75).mRed;
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_WHACK_A_ZOMBIE_PICK_SEED)
		{
			aGrayness = GetFlashingColor(mBoard->mMainCounter, 75).mRed;
		}
		else if (mApp->mEasyPlantingCheat)
		{
			aPercentDark = 0.0f;
		}
		else if ((aDrawCost && !(mApp->mLocalCoop && mIndex >= 4 ? aCost <= mApp->mCoopSun : mBoard->CanTakeSunMoney(aCost))) ||
			aPercentDark > 1.0f || !mBoard->PlantingRequirementsMet(aUseSeedType))
		{
			aGrayness = 128;
		}

		g->PushState();

		DrawSeedPacket(g, mOffsetX, 0.0f, mPacketType, mImitaterType, aPercentDark, aGrayness, false, true);
		
		if (aDrawCost)
		{
			SeedType aSeedType = mPacketType;
			if (aSeedType == SeedType::SEED_IMITATER && mImitaterType != SeedType::SEED_NONE)
			{
				aSeedType = mImitaterType;
			}
			if (aGrayness != 255)
			{
				g->SetColor(Color(aGrayness, aGrayness, aGrayness));
				g->SetColorizeImages(true);
			}
			else if (aPercentDark > 0)
			{
				g->SetColor(Color(128, 128, 128, 255));
				g->SetColorizeImages(true);
			}

			SexyString aCostStr;
			if (gLawnApp->mBoard && gLawnApp->mBoard->PlantUsesAcceleratedPricing(aSeedType) && !gLawnApp->GetDialog(Dialogs::DIALOG_ALMANAC))
			{
				aCostStr = StrFormat(_S("%d"), gLawnApp->mBoard->GetCurrentPlantCost(mPacketType, mImitaterType));
			}
			else
			{
				aCostStr = StrFormat(_S("%d"), Plant::GetCost(mPacketType, mImitaterType));
			}

			Font* aTextFont = Sexy::FONT_PICO129;
			int aTextOffsetX = 32 - aTextFont->StringWidth(aCostStr);
			int aTextOffsetY = aTextFont->GetAscent() + 54;
			if (g->mScaleX == 1.0f && g->mScaleY == 1.0f)
			{
				TodDrawString(g, aCostStr, mOffsetX + aTextOffsetX, aTextOffsetY, aTextFont, Color::Black, DS_ALIGN_LEFT);
			}
			else
			{
				SexyMatrix3 aMatrix;
				TodScaleTransformMatrix(aMatrix, g->mTransX + aTextOffsetX * g->mScaleX + mOffsetX, g->mTransY + aTextOffsetY * g->mScaleY, g->mScaleX, g->mScaleY);
				                        
				{
					g->SetLinearBlend(false);
				}
				TodDrawStringMatrix(g, aTextFont, aMatrix, aCostStr, Color::Black);
				g->SetLinearBlend(true);
			}
		}
		
		g->PopState();
	}
}

          
bool SeedPacket::CanPickUp()
{
	if (mBoard->mPaused || mApp->mGameScene != GameScenes::SCENE_PLAYING || mPacketType == SeedType::SEED_NONE)
	{
		return false;
	}

	SeedType aUseSeedType = mPacketType;
	if (mPacketType == SeedType::SEED_IMITATER && mImitaterType != SeedType::SEED_NONE)
	{
		aUseSeedType = mImitaterType;
	}

	if (mApp->IsSlotMachineLevel())
	{
		return false;
	}

	if (!mApp->mEasyPlantingCheat)
	{
		if (!mActive)
		{
			return false;
		}

		int aCost = mBoard->GetCurrentPlantCost(mPacketType, mImitaterType);
		if (!mBoard->CanTakeSunMoney(aCost) && !mBoard->HasConveyorBeltSeedBank())
		{
			return false;
		}

		if (!mBoard->PlantingRequirementsMet(aUseSeedType))
		{
			return false;
		}
	}

	return true;
}

          
void SeedPacket::MouseDown(int x, int y, int theClickCount)
{
    if (mApp->mLocalCoop)
    {
        const int slots = mBoard->HasConveyorBeltSeedBank() ? 6 : 4;
        if ((mIndex >= slots) != mApp->mCoopControllerInput) return;
    }
	if (mBoard->mPaused || mApp->mGameScene != GameScenes::SCENE_PLAYING || mPacketType == SeedType::SEED_NONE)
	{
		return;
	}

	if (mApp->IsSlotMachineLevel())
	{
		if (!mBoard->mAdvice->IsBeingDisplayed())
		{
			mBoard->DisplayAdvice(_S("[ADVICE_SLOT_MACHINE_PULL]"), MessageStyle::MESSAGE_STYLE_HINT_TALL_FAST, AdviceType::ADVICE_NONE);
		}
		mBoard->mChallenge->mSlotMachineRollCount = pvz_min(mBoard->mChallenge->mSlotMachineRollCount, 2);
		return;
	}

	SeedType aUseSeedType = mPacketType;
	if (mPacketType == SeedType::SEED_IMITATER && mImitaterType != SeedType::SEED_NONE)
	{
		aUseSeedType = mImitaterType;
	}

	if (!mApp->mEasyPlantingCheat)
	{
		if (!mActive)
		{
			mApp->PlaySample(SOUND_BUZZER);
			if (mApp->IsFirstTimeAdventureMode() && mBoard->mLevel == 1 && mBoard->mHelpDisplayed[(int)AdviceType::ADVICE_CLICK_ON_SUN])
			{
				mBoard->DisplayAdvice(_S("[ADVICE_SEED_REFRESH]"), MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1, AdviceType::ADVICE_SEED_REFRESH);
			}
			return;
		}

		int aCost = mBoard->GetCurrentPlantCost(mPacketType, mImitaterType);
		if (!mBoard->CanTakeSunMoney(aCost) && !mBoard->HasConveyorBeltSeedBank())
		{
			mApp->PlaySample(SOUND_BUZZER);
			mBoard->mOutOfMoneyCounter = 70;
			if (mApp->IsFirstTimeAdventureMode() && mBoard->mLevel == 1 && mBoard->mHelpDisplayed[(int)AdviceType::ADVICE_CLICK_ON_SUN])
			{
				mBoard->DisplayAdvice(_S("[ADVICE_CANT_AFFORD_PLANT]"), MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1, AdviceType::ADVICE_CANT_AFFORD_PLANT);
			}
			return;
		}

		if (!mBoard->PlantingRequirementsMet(aUseSeedType))
		{
			mApp->PlaySample(SOUND_BUZZER);
			if (aUseSeedType == SeedType::SEED_GATLINGPEA)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_REPEATER]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_REPEATER);
			}
			else if (aUseSeedType == SeedType::SEED_WINTERMELON)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_MELONPULT]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_MELONPULT);
			}
			else if (aUseSeedType == SeedType::SEED_TWINSUNFLOWER)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_SUNFLOWER]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_SUNFLOWER);
			}
			else if (aUseSeedType == SeedType::SEED_SPIKEROCK)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_SPIKEWEED]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_SPIKEWEED);
			}
			else if (aUseSeedType == SeedType::SEED_COBCANNON)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_KERNELPULT]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_KERNELPULT);
			}
			else if (aUseSeedType == SeedType::SEED_GOLD_MAGNET)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_MAGNETSHROOM]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_MAGNETSHROOM);
			}
			else if (aUseSeedType == SeedType::SEED_GLOOMSHROOM)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_FUMESHROOM]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_FUMESHROOM);
			}
			else if (aUseSeedType == SeedType::SEED_CATTAIL)
			{
				mBoard->DisplayAdvice(_S("[ADVICE_PLANT_NEEDS_LILYPAD]"), MessageStyle::MESSAGE_STYLE_HINT_LONG, AdviceType::ADVICE_PLANT_NEEDS_LILYPAD);
			}
			else
			{
				TOD_ASSERT();
			}

			return;
		}
	}

	mBoard->ClearAdvice(AdviceType::ADVICE_CANT_AFFORD_PLANT);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_REPEATER);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_MELONPULT);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_SUNFLOWER);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_KERNELPULT);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_SPIKEWEED);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_MAGNETSHROOM);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_FUMESHROOM);
	mBoard->ClearAdvice(AdviceType::ADVICE_PLANT_NEEDS_LILYPAD);

	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED || mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST)
	{
		mBoard->mChallenge->BeghouledPacketClicked(this);
	}
	else if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIQUARIUM)
	{
		mBoard->mChallenge->ZombiquariumPacketClicked(this);
	}
	else
	{
		mBoard->mCursorObject->mType = mPacketType;
		mBoard->mCursorObject->mImitaterType = mImitaterType;
		mBoard->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_PLANT_FROM_BANK;
		mBoard->mCursorObject->mSeedBankIndex = mIndex;
		mApp->PlaySample(SOUND_SEEDLIFT);

		if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_1_PICK_UP_PEASHOOTER)
		{
			mBoard->SetTutorialState(TutorialState::TUTORIAL_LEVEL_1_PLANT_PEASHOOTER);
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_LEVEL_2_PICK_UP_SUNFLOWER)
		{
			if (mPacketType == SeedType::SEED_SUNFLOWER)
			{
				mBoard->SetTutorialState(TutorialState::TUTORIAL_LEVEL_2_PLANT_SUNFLOWER);
			}
			else
			{
				mBoard->SetTutorialState(TutorialState::TUTORIAL_LEVEL_2_REFRESH_SUNFLOWER);
			}
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_MORESUN_PICK_UP_SUNFLOWER)
		{
			if (mPacketType == SeedType::SEED_SUNFLOWER)
			{
				mBoard->SetTutorialState(TutorialState::TUTORIAL_MORESUN_PLANT_SUNFLOWER);
			}
			else
			{
				mBoard->SetTutorialState(TutorialState::TUTORIAL_MORESUN_REFRESH_SUNFLOWER);
			}
		}
		else if (mBoard->mTutorialState == TutorialState::TUTORIAL_WHACK_A_ZOMBIE_PICK_SEED || mBoard->mTutorialState == TutorialState::TUTORIAL_WHACK_A_ZOMBIE_BEFORE_PICK_SEED)
		{
			mBoard->SetTutorialState(TutorialState::TUTORIAL_WHACK_A_ZOMBIE_COMPLETED);
		}

		Deactivate();
	}
}

          
void SeedPacket::WasPlanted()
{
	TOD_ASSERT(mPacketType != SeedType::SEED_NONE);

	if (mBoard->HasConveyorBeltSeedBank())
	{
		mBoard->mSeedBank->RemoveSeed(mIndex);
	}
	else if (mApp->IsSlotMachineLevel())
	{
		Deactivate();
	}
	else if (mApp->IsLastStand() && mBoard->mChallenge->mChallengeState != ChallengeState::STATECHALLENGE_LAST_STAND_ONSLAUGHT)
	{
		mTimesUsed++;
		mActive = true;
		FlashIfReady();
	}
	else
	{
		mTimesUsed++;
		mRefreshing = true;
		mRefreshTime = Plant::GetRefreshTime(mPacketType, mImitaterType);
	}
}

          
bool SeedPacket::MouseHitTest(int theX, int theY, HitResult* theHitResult)
{
	if (mSlotMachineCountDown > 0 || mPacketType == SeedType::SEED_NONE)
	{
		theHitResult->mObject = nullptr;
		theHitResult->mObjectType = GameObjectType::OBJECT_TYPE_NONE;
		return false;
	}

	if (theX >= mX + mOffsetX && theX < mX + mOffsetX + mWidth && theY >= mY && theY < mY + mHeight)
	{
		theHitResult->mObject = this;
		theHitResult->mObjectType = GameObjectType::OBJECT_TYPE_SEEDPACKET;
		return true;
	}

	theHitResult->mObject = nullptr;
	theHitResult->mObjectType = GameObjectType::OBJECT_TYPE_NONE;
	return false;
}

          
SeedBank::SeedBank()
{
	mWidth = IMAGE_SEEDBANK->GetWidth();
	mHeight = IMAGE_SEEDBANK->GetHeight();

	mNumPackets = 0;
	mConveyorBeltCounter = 0;
	mCutSceneDarken = 255;
}

          
void SeedBank::Draw(Graphics* g)
{
    if (mApp->mGameMode == GAMEMODE_CHALLENGE_HEAVY_WEAPON)
    {
        mBoard->mChallenge->mHeavyWeapon.DrawBank(g);
        return;
    }
	if (mBoard->mCutScene && mBoard->mCutScene->IsBeforePreloading())
	{
		return;
	}

	if (mApp->mGameScene != GameScenes::SCENE_PLAYING)
	{
		g->mTransX -= mBoard->mX;
		g->mTransY -= mBoard->mY;
	}

    if (mApp->mLocalCoop)
    {
        const bool conveyor = mBoard->HasConveyorBeltSeedBank();
        const int width = conveyor ? COOP_CONVEYOR_WIDTH : COOP_SEED_BANK_WIDTH;
        const int right = conveyor ? COOP_CONVEYOR_RIGHT_X : COOP_SEED_BANK_RIGHT_X;
        auto bank = mApp->GetSharedImage(conveyor ? "../versus/coop/conveyor-back" : "../versus/coop/seed-bank");
        SharedImageRef belt;
        if (conveyor) belt = mApp->GetSharedImage("../versus/coop/conveyor");
        for (int player = 0; player < 2; ++player)
        {
            if (Image* image = bank)
                g->DrawImage(image, player * right, 0, width, 87);
            else
                g->DrawImage(IMAGE_SEEDBANK, player * right, 0, width, 87);
            if (Image* image = belt)
                g->DrawImage(image, Rect(player * right + 7, 63, width - 14, 16),
                    Rect(0, (mConveyorBeltCounter / 4 % 6) * 16, image->GetWidth(), 16));
        }
        if (mApp->mGameScene == SCENE_PLAYING)
        {
            auto butter = mApp->GetSharedImage("../versus/coop/butter");
            const Rect tool = mBoard->GetCoopButterButtonRect();
            const int x = tool.mX - mX;
            const int y = tool.mY - mY;
            g->DrawImage(IMAGE_SHOVELBANK, x, y, tool.mWidth, tool.mHeight);
            if (Image* image = butter) g->DrawImage(image, x + 7, y + 7, 55, 57);
        }
    }
    else if (mApp->IsSlotMachineLevel())
    {
        g->DrawImage(IMAGE_SUNBANK, 0, 0);
	}
	else if (mBoard->HasConveyorBeltSeedBank())
	{
		g->DrawImage(IMAGE_CONVEYORBELT_BACKDROP, 83, 0);
		g->DrawImageCel(IMAGE_CONVEYORBELT, 90, 63, mConveyorBeltCounter / 4 % 6);
		g->SetClipRect(90, 0, 501, BOARD_HEIGHT);
	}
	else
	{
		int aExtraWidth = mBoard->GetSeedBankExtraWidth();
		Rect theSrcRect(IMAGE_SEEDBANK->mWidth - aExtraWidth - 12, 0, aExtraWidth + 12, IMAGE_SEEDBANK->mHeight);
		g->DrawImage(IMAGE_SEEDBANK, 0, 0);
		g->DrawImage(IMAGE_SEEDBANK, IMAGE_SEEDBANK->mWidth - 12, 0, theSrcRect);
	}

	for (int i = 0; i < mNumPackets; i++)
	{
		SeedPacket* aSeedPacket = &mSeedPackets[i];
		if (aSeedPacket->mPacketType != SeedType::SEED_NONE && aSeedPacket->BeginDraw(g))
		{
			aSeedPacket->Draw(g);
			aSeedPacket->EndDraw(g);
		}
	}

	g->ClearClipRect();
	if (mApp->IsSlotMachineLevel() && mY > -IMAGE_SEEDBANK->GetHeight())
	{
		g->DrawImage(IMAGE_SLOTMACHINE_OVERLAY, 189, -2);
	}

	if (!mBoard->HasConveyorBeltSeedBank())
	{
		SexyString aMoneyLabel = StrFormat(_S("%d"), pvz_max(mBoard->mSunMoney, 0));
		Color aMoneyColor(0, 0, 0);
		if (mBoard->mOutOfMoneyCounter > 0 && mBoard->mOutOfMoneyCounter % 20 < 10)
		{
			aMoneyColor = Color(255, 0, 0);
		}

        TodDrawString(g, aMoneyLabel, 34, 78, FONT_CONTINUUMBOLD14, aMoneyColor, DS_ALIGN_CENTER);
        if (mApp->mLocalCoop)
            TodDrawString(g, std::to_string(mApp->mCoopSun), COOP_SEED_BANK_RIGHT_X + 34, 78, FONT_CONTINUUMBOLD14, Color::Black, DS_ALIGN_CENTER);
	}

	if (mApp->mGameScene != GameScenes::SCENE_PLAYING)
	{
		g->mTransX += mBoard->mX;
		g->mTransY += mBoard->mY;
	}
}

          
bool SeedBank::MouseHitTest(int x, int y, HitResult* theHitResult)
{
    if (mApp->mGameMode == GAMEMODE_CHALLENGE_HEAVY_WEAPON)
    {
        const int index = mBoard->mChallenge->mHeavyWeapon.PacketAt(x, y);
        theHitResult->mObject = index >= 0 ? &mSeedPackets[index] : nullptr;
        theHitResult->mObjectType = index >= 0 ? OBJECT_TYPE_SEEDPACKET : OBJECT_TYPE_NONE;
        return index >= 0;
    }
	if (x - mX <= mWidth - 5 && mNumPackets > 0)
	{
		for (SeedPacket& aSeedPacket : mSeedPackets)
		{
			if (aSeedPacket.MouseHitTest(x - mX, y - mY, theHitResult))
			{
				return true;
			}
		}
	}

	theHitResult->mObject = nullptr;
	theHitResult->mObjectType = GameObjectType::OBJECT_TYPE_NONE;
	return false;
}

          
bool SeedBank::ContainsPoint(int theX, int theY)
{
	return theX >= mX && theX < mX + mWidth && theY >= mY && theY < mY + mHeight;
}

          
void SeedBank::AddSeed(SeedType theSeedType, bool thePlaceOnLeft)
{
	TOD_ASSERT(mBoard->HasConveyorBeltSeedBank());
	TOD_ASSERT(theSeedType != SeedType::SEED_NONE);

    if (mApp->mLocalCoop)
    {
        for (int attempt = 0; attempt < 2; ++attempt)
        {
            const int player = mApp->mCoopNextConveyor;
            mApp->mCoopNextConveyor = 1 - player;
            for (int index = player * 6; index < player * 6 + 6; ++index)
            {
                auto& packet = mSeedPackets[index];
                if (packet.mPacketType != SEED_NONE) continue;
                packet.mPacketType = theSeedType;
                packet.mImitaterType = SEED_NONE;
                packet.mActive = true;
                packet.mRefreshing = false;
                packet.mRefreshCounter = packet.mRefreshTime = 0;
                packet.mOffsetX = thePlaceOnLeft ? 0 : pvz_min(38, (5 - index % 6) * 12);
                return;
            }
        }
        return;
    }

    int aNumSeeds = GetNumSeedsOnConveyorBelt();
	if (aNumSeeds == mNumPackets)
	{
		return;
	}

	SeedPacket* aSeedPacket = &mSeedPackets[aNumSeeds];
	aSeedPacket->mPacketType = theSeedType;
	aSeedPacket->mRefreshCounter = 0;
	aSeedPacket->mRefreshTime = 0;
	aSeedPacket->mRefreshing = false;
	aSeedPacket->mActive = true;
	aSeedPacket->mOffsetX = 515 - (SEED_PACKET_WIDTH + 1) * aNumSeeds;
	if (thePlaceOnLeft)
	{
		aSeedPacket->mOffsetX = 0;
	}
	if (aNumSeeds > 0)
	{
		SeedPacket* aPrevPacket = &mSeedPackets[aNumSeeds - 1];
		if (aSeedPacket->mOffsetX < aPrevPacket->mOffsetX)
		{
			aSeedPacket->mOffsetX = aPrevPacket->mOffsetX + 40;
		}
	}
}

          
void SeedBank::RemoveSeed(int theIndex)
{
	TOD_ASSERT(mBoard->HasConveyorBeltSeedBank());
    if (mApp->mLocalCoop)
    {
        const int end = (theIndex / 6 + 1) * 6;
        for (int index = theIndex; index < end; ++index)
        {
            auto& packet = mSeedPackets[index];
            const bool last = index + 1 == end;
            packet.mPacketType = last ? SEED_NONE : mSeedPackets[index + 1].mPacketType;
            packet.mImitaterType = SEED_NONE;
            packet.mOffsetX = last ? 0 : mSeedPackets[index + 1].mOffsetX;
            packet.mActive = true;
            packet.mRefreshing = false;
            packet.mRefreshTime = packet.mRefreshCounter = 0;
        }
        return;
    }
    TOD_ASSERT(theIndex >= 0 && theIndex < GetNumSeedsOnConveyorBelt());

	for (int i = theIndex; i < mNumPackets; i++)
	{
		SeedPacket* aSeedPacket = &mSeedPackets[i];
		if (aSeedPacket->mPacketType == SeedType::SEED_NONE)
		{
			break;
		}

		if (i == mNumPackets - 1)
		{
			aSeedPacket->mPacketType = SeedType::SEED_NONE;
			aSeedPacket->mOffsetX = 0;
		}
		else
		{
			SeedPacket* aNextPacket = &mSeedPackets[i + 1];
			aSeedPacket->mPacketType = aNextPacket->mPacketType;
			aSeedPacket->mOffsetX = aNextPacket->mOffsetX + SEED_PACKET_WIDTH + 1;
		}

		aSeedPacket->mRefreshCounter = 0;
		aSeedPacket->mRefreshTime = 0;
		aSeedPacket->mRefreshing = false;
		aSeedPacket->mActive = true;
	}
}

          
int SeedBank::GetNumSeedsOnConveyorBelt()
{
    if (mApp->mLocalCoop)
    {
        int count = 0;
        for (int i = 0; i < mNumPackets; ++i)
            if (mSeedPackets[i].mPacketType != SEED_NONE) ++count;
        return count;
    }
	for (int i = 0; i < mNumPackets; i++)
	{
		if (mSeedPackets[i].mPacketType == SeedType::SEED_NONE)
		{
			return i;
		}
	}
	return mNumPackets;
}

int SeedBank::CountOfTypeOnConveyorBelt(SeedType theSeedType)
{
	int aCount = 0;
	for (int i = 0; i < mNumPackets; i++)
	{
		if (mSeedPackets[i].mPacketType == theSeedType)
		{
			aCount++;
		}
	}
	return aCount;
}

          
void SeedPacket::SetPacketType(SeedType theSeedType, SeedType theImitaterType)
{
	mPacketType = theSeedType;
	mImitaterType = theImitaterType;
	mRefreshCounter = 0;
	mRefreshTime = 0;
	mRefreshing = false;
	mActive = true;

	SeedType aUseSeedType = theSeedType;
	if (theSeedType == SeedType::SEED_IMITATER && theImitaterType != SeedType::SEED_NONE)
	{
		aUseSeedType = theImitaterType;
	}

	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED || mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST || 
		mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIQUARIUM || mApp->IsLastStand() ||
		mApp->IsIZombieLevel() || mApp->IsScaryPotterLevel() || mApp->IsWhackAZombieLevel() || (mApp->IsSurvivalMode() && mBoard->mChallenge->mSurvivalStage > 0))
		return;

	if ((Plant::IsUpgrade(aUseSeedType) && !gLawnApp->IsSurvivalMode()) || Plant::GetRefreshTime(mPacketType, mImitaterType) == 5000)
	{
		mRefreshTime = 3500;
		mRefreshing = true;
		mActive = false;
	}
	else if (Plant::IsUpgrade(aUseSeedType) && gLawnApp->IsSurvivalMode())
	{
		mRefreshTime = 8000;
		mRefreshing = true;
		mActive = false;
	}
	else if (Plant::GetRefreshTime(mPacketType, mImitaterType) == 3000)
	{
		mRefreshTime = 2000;
		mRefreshing = true;
		mActive = false;
	}
}

          
void SeedBank::UpdateConveyorBelt()
{
	mConveyorBeltCounter++;

	int speed = CONVEYOR_SPEED;
	const bool spamSkill = IsSpamSkill(mApp->mGameMode);
	if (spamSkill) speed = 1;

#ifdef _DS_MINIGAMES
	if (mApp->mGameMode == GAMEMODE_CHALLENGE_BOMB_ALL_TOGETHER)
		speed = 0;
#endif

	if (speed != 0 && mConveyorBeltCounter % speed == 0)
	{
		for (int i = 0; i < mNumPackets; i++)
		{
			SeedPacket* aSeedPacket = &mSeedPackets[i];
			if (aSeedPacket->mOffsetX > 0)
			{
				aSeedPacket->mOffsetX = pvz_max(aSeedPacket->mOffsetX - (spamSkill ? 4 : 1), 0);
			}
		}

		mBoard->UpdateToolTip();
	}
	else if (speed == 0) {
		for (int i = 0; i < mNumPackets; i++) {
			SeedPacket* aSeedPacket = &mSeedPackets[i];
			aSeedPacket->mOffsetX = 0;
		}
		mBoard->UpdateToolTip();
	}
}

          
void SeedBank::UpdateWidth()
{
	mNumPackets = mBoard->GetNumSeedsInBank();
	mWidth = IMAGE_SEEDBANK->GetWidth() + mBoard->GetSeedBankExtraWidth();
	for (int i = 0; i < mNumPackets; i++)
	{
		mSeedPackets[i].mX = mBoard->GetSeedPacketPositionX(i);
	}
}

          
void SeedBank::RefreshAllPackets()
{
	for (int i = 0; i < mNumPackets; i++)
	{
		SeedPacket* aSeedPacket = &mSeedPackets[i];
		if (aSeedPacket->mPacketType == SeedType::SEED_NONE)
		{
			break;
		}

		if (aSeedPacket->mRefreshing)
		{
			aSeedPacket->mRefreshCounter = 0;
			aSeedPacket->mRefreshing = false;
			aSeedPacket->mActive = true;
			aSeedPacket->FlashIfReady();
		}
	}
}
