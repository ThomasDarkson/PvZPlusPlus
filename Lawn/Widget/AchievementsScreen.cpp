                          
#include "AchievementsScreen.h"
#include "../Board.h"
#include "GameButton.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/ProfileMgr.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../SexyAppFramework/Font.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/TodParticle.h"
#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../GameConstants.h"
#include "../../Sexy.TodLib/TodStringFile.h"

Rect aBackButtonRect = { 120, 35, 130, 80 };
bool isOnBackButton = false;

AchievementItem gAchievementList[MAX_ACHIEVEMENTS] = {
	{ _S("[HOME_LAWN_SECURITY_TITLE]"), _S("[HOME_LAWN_SECURITY_DESCRIPTION]") },
	{ _S("[NOBEL_PEAS_PRIZE_TITLE]"), _S("[NOBEL_PEAS_PRIZE_DESCRIPTION]") },
	{ _S("[BETTER_OFF_DEAD_TITLE]"), _S("[BETTER_OFF_DEAD_DESCRIPTION]") },
	{ _S("[CHINA_SHOP_TITLE]"), _S("[CHINA_SHOP_DESCRIPTION]") },
	{ _S("[SPUDOW_TITLE]"), _S("[SPUDOW_DESCRIPTION]") },
	{ _S("[EXPLODONATOR_TITLE]"), _S("[EXPLODONATOR_DESCRIPTION]") },
	{ _S("[MORTICULTURALIST_TITLE]"), _S("[MORTICULTURALIST_DESCRIPTION]") },
	{ _S("[DONT_PEA_IN_THE_POOL_TITLE]"), _S("[DONT_PEA_IN_THE_POOL_DESCRIPTION]") },
	{ _S("[ROLL_SOME_HEADS_TITLE]"), _S("[ROLL_SOME_HEADS_DESCRIPTION]") },
	{ _S("[GROUNDED_TITLE]"), _S("[GROUNDED_DESCRIPTION]") },
	{ _S("[ZOMBOLOGIST_TITLE]"), _S("[ZOMBOLOGIST_DESCRIPTION]") },
	{ _S("[PENNY_PINCHER_TITLE]"), _S("[PENNY_PINCHER_DESCRIPTION]") },
	{ _S("[SUNNY_DAYS_TITLE]"), _S("[SUNNY_DAYS_DESCRIPTION]") },
	{ _S("[POPCORN_PARTY_TITLE]"), _S("[POPCORN_PARTY_DESCRIPTION]") },
	{ _S("[GOOD_MORNING_TITLE]"), _S("[GOOD_MORNING_DESCRIPTION]") },
	{ _S("[NO_FUNGUS_AMONG_US_TITLE]"), _S("[NO_FUNGUS_AMONG_US_DESCRIPTION]") },
	{ _S("[BEYOND_THE_GRAVE_TITLE]"), _S("[BEYOND_THE_GRAVE_DESCRIPTION]") },
	{ _S("[IMMORTAL_TITLE]"), _S("[IMMORTAL_DESCRIPTION]") },
	{ _S("[TOWERING_WISDOM_TITLE]"), _S("[TOWERING_WISDOM_DESCRIPTION]") },
	{ _S("[MUSTACHE_MODE_TITLE]"), _S("[MUSTACHE_MODE_DESCRIPTION]") },
	{ _S("[DISCO_IS_UNDEAD_TITLE]"), _S("[DISCO_IS_UNDEAD_DESCRIPTION]") }
#ifdef _HAS_UNUSED_ACHIEVEMENTS
	,{ _S("[SULTAN_OF_SPIN_TITLE]"), _S("[SULTAN_OF_SPIN_DESCRIPTION]") },
	{ _S("[DISRESPECT_THE_DEAD_TITLE]"), _S("[DISRESPECT_THE_DEAD_DESCRIPTION]") },
	{ _S("[HEAVY_WEAPONS_TITLE]"), _S("[HEAVY_WEAPONS_DESCRIPTION]") },
	{ _S("[ZEN_PROFIT_TITLE]"), _S("[ZEN_PROFIT_DESCRIPTION]") },
	{ _S("[MAY_NOT_CONTAIN_NUTS_TITLE]"), _S("[MAY_NOT_CONTAIN_NUTS_DESCRIPTION]") },
	{ _S("[EVEN_MORTICULTURALIST_TITLE]"), _S("[EVEN_MORTICULTURALIST_DESCRIPTION]") },
	{ _S("[BEAT_IT_TITLE]"), _S("[BEAT_IT_DESCRIPTION]") },
	{ _S("[GOLD_FARMER_TITLE]"), _S("[GOLD_FARMER_DESCRIPTION]") },
	{ _S("[FACE_TO_FACE_TITLE]"), _S("[FACE_TO_FACE_DESCRIPTION]") }
#endif
};

                         
AchievementsWidget::AchievementsWidget(LawnApp* theApp) {
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = IMAGE_ACHEESEMENTS_CHINA->mHeight + IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->mHeight + 15700;
#ifdef OPENFL_HOST
    mHeight += 72 * ((MAX_ACHIEVEMENTS + pvz_host_achievement_count() + 1) / 2) - 600;
#endif
	mScrollDirection = -1;
	mScrollValue = 0;
	mDefaultScrollValue = 30;
	mScrollDecay = 1;
	mDidPressMoreButton = false;
	mMoreRockRect = Rect(710, 470, IMAGE_ACHEESEMENTS_MORE_ROCK->mWidth - 25, IMAGE_ACHEESEMENTS_MORE_ROCK->mHeight - 50);
}

                         
AchievementsWidget::~AchievementsWidget() {

}

                         
void AchievementsWidget::Update() {
#ifdef OPENFL_HOST
    if (mWidgetManager->mFocusWidget != this)
    {
        mPekingTiming = false;
        return;
    }
#endif
	if ((aBackButtonRect.Contains(mWidgetManager->mLastMouseX - mX, mWidgetManager->mLastMouseY - mY) || 
		mMoreRockRect.Contains(mWidgetManager->mLastMouseX - mX, mWidgetManager->mLastMouseY - mY)) && mApp->mGameSelector->mSlideCounter <= 0)
	{
		if (!isOnBackButton) {
			mApp->PlayFoley(FoleyType::FOLEY_BLEEP);
		}
		isOnBackButton = true;
	}
	else
	{
		isOnBackButton = false;
	}

	if (mScrollValue <= 0 || mApp->mGameSelector->mSlideCounter > 0)
		return;

	if (mScrollValue > mDefaultScrollValue)
		mScrollValue = mDefaultScrollValue;

	mScrollValue -= mScrollDecay;

	int aNewY = mY + mScrollValue * mScrollDirection;
	if (aNewY >= mApp->mDDInterface->mWideScreenOffsetY)
		aNewY = mApp->mDDInterface->mWideScreenOffsetY;
	                             
	                         

	int aMaxScroll = 2 * 600 + 50 - mHeight;
	if (aNewY <= aMaxScroll)
		aNewY = aMaxScroll;
#ifdef OPENFL_HOST
    const int top = mApp->mDDInterface->mWideScreenOffsetY;
    const int chinaY = mHeight - IMAGE_ACHEESEMENTS_CHINA->mHeight - 650;
    if (aNewY >= top)
        mPekingTiming = false;
    else if (mY >= top)
    {
        mPekingStart = GetTickCount();
        mPekingTiming = true;
    }
    if (mPekingTiming && aNewY + chinaY < top + BOARD_HEIGHT)
    {
        const unsigned long elapsed = GetTickCount() - mPekingStart;
        if (elapsed > 0 && elapsed <= INT_MAX)
            pvz_host_peking_finish(mApp, static_cast<int>(elapsed));
        mPekingTiming = false;
    }
#endif

	mY = aNewY;

#ifndef OPENFL_HOST
	mApp->mGameSelector->mScrollOffset = aNewY - mApp->mDDInterface->mWideScreenOffsetY;


	mApp->mGameSelector->mY = mY - 600 - mApp->mDDInterface->mWideScreenOffsetY;
#ifdef _HAS_ZOMBATAR
	mApp->mGameSelector->mZombatarWidget->mY = mY - 600;
#endif

#endif

	int aDelta = aNewY - mY;
	mMoreRockRect.mY += aDelta;
	aBackButtonRect.mY += aDelta;

	if (mScrollValue <= 0)
		mScrollValue = 0;

	const int MID_SCROLL = 218;
	const bool IS_ON_TOP = mY > MID_SCROLL;
	const bool IS_ON_BOTTOM = mY < MID_SCROLL;

	if (mScrollDirection == 1) {
		if (IS_ON_TOP) mDidPressMoreButton = true;
		else if (IS_ON_BOTTOM) mDidPressMoreButton = false;
	}
	else if (mScrollDirection == -1) {
		if (IS_ON_TOP) mDidPressMoreButton = false;
		else if (IS_ON_BOTTOM) mDidPressMoreButton = true;
	}
}

                         
void AchievementsWidget::Draw(Graphics* g) {
#ifdef OPENFL_HOST
    g->ClipRect(0, mApp->mDDInterface->mWideScreenOffsetY - mY, 800, 600);
#endif
	if (mApp->GetDialog(Dialogs::DIALOG_STORE) || mApp->GetDialog(Dialogs::DIALOG_ALMANAC))
		return;

	g->DrawImage(IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG, 0, 0);

    const int dirtStart = IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->mHeight;
#ifdef OPENFL_HOST
    const int viewTop = mApp->mDDInterface->mWideScreenOffsetY - mY;
    const int tileHeight = IMAGE_ACHEESEMENTS_HOLE_TILE->mHeight;
    if (tileHeight > 0)
    {
        const int firstTile = pvz_max(0, viewTop - dirtStart) / tileHeight;
        const int bottom = pvz_min(mHeight, viewTop + BOARD_HEIGHT);
        for (int y = dirtStart + firstTile * tileHeight; y < bottom; y += tileHeight)
            g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE, 0, y);
    }
#else
    for (int i = 1; i <= 70; ++i)
        g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE, 0, dirtStart * i);
#endif

#ifdef OPENFL_HOST
    const int extraDepth = 72 * ((MAX_ACHIEVEMENTS + pvz_host_achievement_count() + 1) / 2) - 600;
#else
    const int extraDepth = 0;
#endif
	g->DrawImage(IMAGE_ACHEESEMENTS_BOOKWORM, 0, 1125 + extraDepth);
	g->DrawImage(IMAGE_ACHEESEMENTS_BEJEWELED, 0, 2250 + extraDepth);
	g->DrawImage(IMAGE_ACHEESEMENTS_CHUZZLE, 0, 4500 + extraDepth);
	g->DrawImage(IMAGE_ACHEESEMENTS_PEGGLE, 0, 6750 + extraDepth);
	g->DrawImage(IMAGE_ACHEESEMENTS_PIPE, 0, 9000 + extraDepth);
	g->DrawImage(IMAGE_ACHEESEMENTS_ZUMA, 0, 11250 + extraDepth);

	g->DrawImage(IMAGE_ACHEESEMENTS_CHINA, 0, mHeight - IMAGE_ACHEESEMENTS_CHINA->mHeight -        650);

	bool isHand = false;
	
	if (aBackButtonRect.Contains(mWidgetManager->mLastMouseX - mX, mWidgetManager->mLastMouseY - mY) && mApp->mGameSelector->mSlideCounter <= 0)
	{
		g->DrawImage(IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT, 128, 55);
		isHand = true;
	}

#ifdef OPENFL_HOST
    const int displayCount = MAX_ACHIEVEMENTS + pvz_host_achievement_count();
    int visibleIndex = 0;
    for (int i = 0; i < displayCount; ++i)
    {
        const bool peking = i >= MAX_ACHIEVEMENTS &&
            std::strcmp(pvz_host_achievement_definition(i - MAX_ACHIEVEMENTS).id, "peking_express") == 0;
        if (peking && !pvz_host_achievement_earned(mApp, "peking_express")) continue;
        const int slot = visibleIndex++;
        const int x = slot % 2 == 0 ? 120 : 410;
        const int y = 178 + 72 * (slot / 2);
        if (y + 72 <= viewTop || y >= viewTop + 600) continue;

        SexyString title;
        SexyString description;
        bool unlocked = false;
        if (i < MAX_ACHIEVEMENTS)
        {
            title = TodStringTranslate(gAchievementList[i].name);
            description = TodStringTranslate(gAchievementList[i].description);
            unlocked = mApp->mPlayerInfo && mApp->mPlayerInfo->mEarnedAchievements[i];
        }
        else
        {
            const auto& entry = pvz_host_achievement_definition(i - MAX_ACHIEVEMENTS);
            title = StringToSexyString(entry.title);
            description = StringToSexyString(entry.description);
            unlocked = pvz_host_achievement_earned(mApp, entry.id);
            if (peking)
            {
                const int best = pvz_host_peking_best_time(mApp);
                if (best > 0) description += StrFormat(_S("\nBest time: %d.%03d s"), best / 1000, best % 1000);
            }
        }
        pvz_host_draw_achievement_icon(mApp, g, i, x, y, unlocked);
        g->SetFont(FONT_DWARVENTODCRAFT15);
        if (FONT_DWARVENTODCRAFT15->StringWidth(title) > 212) g->SetFont(FONT_DWARVENTODCRAFT12);
        g->SetColor(Color(21, 175, 0));
        g->DrawString(title, x + 63, y + 16);
        g->SetFont(FONT_DWARVENTODCRAFT12);
        g->SetColor(Color(255, 255, 255));
        g->WriteWordWrapped(Rect(x + 63, y + 20, 212, 52), description, 12);
    }
#else
    for (int i = 0; i < MAX_ACHIEVEMENTS; i++) {
		bool aHasAchievement;
		if (mApp->mPlayerInfo) aHasAchievement = mApp->mPlayerInfo->mEarnedAchievements[i];
		else aHasAchievement = false;

		int aCurrAchievementOff = 57 * int(i / 2);
		int aImageXPos = i % 2 == 0 ? 120 : 410;
		int aImageYPos = 178 + aCurrAchievementOff;
		int aTextXPos = aImageXPos + 70 ;
		int aTextYPos = aImageYPos + 16;

		                     
		                                                                           
#ifdef _HAS_UNUSED_ACHIEVEMENTS
		if (i < (int)AchievementId::SultanOfSpin)
#endif
		{
			int spaceOffset = 0;
			if (i % 7 == 1 && i / 7 < 2 || i % 7 < 2 && i / 7 == 2)
				spaceOffset = 2;
			else if (i % 7 > 0 || i / 7 == 2)
				spaceOffset = 1;
			Rect aSrcRect(70 * (i % 7) + spaceOffset, 70 * (i / 7), 70, 70);
			Rect aDestRect(aImageXPos, aImageYPos, 56, 56);

			g->SetColorizeImages(true);
			g->SetColor(aHasAchievement ? Color(255, 255, 255) : Color(255, 255, 255, 32));

			g->DrawImage(IMAGE_ACHEESEMENTS_ICONS, aDestRect, aSrcRect);
			g->SetColorizeImages(false);
		}

		
		                     
		g->SetFont(FONT_DWARVENTODCRAFT15);
		g->SetColor(Color(21, 175, 0));

		g->DrawString(TodStringTranslate(gAchievementList[i].name), aTextXPos, aTextYPos);

		                            
		Rect aPos = Rect(aTextXPos, aTextYPos + 3, 212, 60);
		
		g->SetFont(FONT_DWARVENTODCRAFT12);
		g->SetColor(Color(255, 255, 255));

		g->WriteWordWrapped(aPos, TodStringTranslate(gAchievementList[i].description), 12);
	}
#endif

	g->DrawImage(IMAGE_ACHEESEMENTS_MORE_ROCK, 700, 450);


	bool aIsHighlight = mMoreRockRect.Contains(mWidgetManager->mLastMouseX - mX, mWidgetManager->mLastMouseY - mY) && mApp->mGameSelector->mSlideCounter <= 0;
	if (mDidPressMoreButton) {
		g->DrawImage(aIsHighlight ? IMAGE_ACHEESEMENTS_TOP_BUTTON_HIGHLIGHT : IMAGE_ACHEESEMENTS_TOP_BUTTON, 700, 450);
		if (aIsHighlight) isHand = true;
	}
	else {
		g->DrawImage(aIsHighlight ? IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT : IMAGE_ACHEESEMENTS_MORE_BUTTON, 700, 450);
		if (aIsHighlight) isHand = true;
	}

	if (mApp->mWidgetManager->mFocusWidget == this && mApp->mWidgetManager->mOverWidget)
	{
		if (isHand) mApp->SetCursor(CURSOR_HAND);
		else mApp->SetCursor(CURSOR_POINTER);
	}

}

                         
void AchievementsWidget::KeyDown(KeyCode theKey) {
	if (theKey == KEYCODE_UP) {
		mScrollValue = mDefaultScrollValue;
		mScrollDirection = 1;
	}
	else if (theKey == KEYCODE_DOWN) {
		mScrollValue = mDefaultScrollValue;
		mScrollDirection = -1;
	}
}

                         
void AchievementsWidget::MouseDown(int x, int y, int theClickCount) {
	(void)theClickCount;

	if (mApp->mGameSelector->mSlideCounter > 0)
		return;

	if (aBackButtonRect.Contains(x, y))
		mApp->PlaySample(SOUND_GRAVEBUTTON);

	if (mMoreRockRect.Contains(x, y))
		mApp->PlaySample(SOUND_GRAVEBUTTON);
}

                         
void AchievementsWidget::MouseUp(int x, int y, int theClickCount) {
	(void)theClickCount;
	if (mApp->mGameSelector->mSlideCounter > 0)
		return;

	Point aPos = Point(x, y);
	if (aBackButtonRect.Contains(aPos)) {
		mApp->mGameSelector->SlideTo(0, 0);
		mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector);
		mApp->mGameSelector->DisableButtons(false);
	}

	if (mMoreRockRect.Contains(aPos)) {
		mDidPressMoreButton = !mDidPressMoreButton;
		mScrollDirection = mDidPressMoreButton ? -1 : 1;
		mScrollValue = 30;
	}
}

                         
void AchievementsWidget::MouseWheel(int theDelta) {
	if (mApp->mGameSelector->mSlideCounter > 0)
		return;

	mScrollValue = mDefaultScrollValue;

	if (theDelta > 0)
		mScrollDirection = 1;
	else if (theDelta < 0)
		mScrollDirection = -1;
}

                         
void ReportAchievement::GiveAchievement(LawnApp* theApp, int theAchievement, bool theForceGive) {
#ifndef _HAS_ACHIEVEMENTS
	return;
#endif
    if (!theApp || !theApp->mPlayerInfo || theAchievement < 0 || theAchievement >= MAX_ACHIEVEMENTS)
        return;

	if (theApp->mPlayerInfo->mEarnedAchievements[theAchievement])
		return;

	theApp->mPlayerInfo->mEarnedAchievements[theAchievement] = true;

	if (!theForceGive)
		return;

	SexyString aAchievementName = TodStringTranslate(gAchievementList[theAchievement].name);
	aAchievementName.append(_S(" Achievement!"));


#ifdef OPENFL_HOST
    pvz_host_queue_achievement(theApp, SexyStringToString(aAchievementName));
#else
    if (theApp->mBoard)
        theApp->mBoard->DisplayAdvice(aAchievementName, MESSAGE_STYLE_ACHIEVEMENT, AdviceType::ADVICE_NONE);
    theApp->PlaySample(SOUND_ACHIEVEMENT);
#endif
	
}

                         
void ReportAchievement::AchievementInitForPlayer(LawnApp* theApp) {
	if (!theApp || !theApp->mPlayerInfo)
		return;

	if (theApp->HasFinishedAdventure()) {
		GiveAchievement(theApp, AchievementId::HomeSecurity, false);
	}

	if (theApp->EarnedGoldTrophy()) {
		GiveAchievement(theApp, AchievementId::NovelPeasPrize, false);
	}

	if (theApp->CanSpawnYetis()) {
		GiveAchievement(theApp, AchievementId::Zombologist, false);
	}

	int aTreeSize = theApp->mPlayerInfo->mChallengeRecords[GAMEMODE_TREE_OF_WISDOM - GAMEMODE_SURVIVAL_NORMAL_STAGE_1];
	if (aTreeSize >= 100) {
		GiveAchievement(theApp, AchievementId::ToweringWisdom, false);
	}

	bool aGiveAchievement = true;
	for (int i = STORE_ITEM_PLANT_GATLINGPEA; i <= STORE_ITEM_PLANT_IMITATER; i++) {
		if (!theApp->SeedTypeAvailable(SeedType(SEED_GATLINGPEA + i)))
		{
			aGiveAchievement = false;
			break;
		}
	}

	if (aGiveAchievement) {
		GiveAchievement(theApp, AchievementId::Morticulturalist, false);
	}
#ifdef OPENFL_HOST
    pvz_host_achievement_records(theApp);
#endif
}