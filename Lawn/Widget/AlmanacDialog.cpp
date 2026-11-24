#include "../CustomContent.h"
#include "../Board.h"
#include "../Plant.h"
#include "../Projectile.h"
#include "../Zombie.h"
#include "GameButton.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/PoolEffect.h"
#include "../System/ReanimationLawn.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../SexyAppFramework/Font.h"

bool gZombieDefeated[NUM_ZOMBIE_TYPES] = { false };

static const auto& customAlmanacZombies = CustomContent::Zombies;

static bool IsCustomAlmanacZombie(ZombieType type)
{
	return CustomContent::ZombieSlot(type) >= 0;
}

static SexyString AlmanacZombieAttack(ZombieType type, int tickMs)
{
    if (Zombie::IsGargantuar(type))
        return _S("{KEYWORD}Attack:{STAT} Crushes most plants in one smash; Spikerock takes 9 at full health.\n");
    if (type == ZOMBIE_ZAMBONI)
        return _S("{KEYWORD}Attack:{STAT} Runs over plants and leaves ice.\n{KEYWORD}Counter:{STAT} Spikeweed pops its tires but is destroyed. A full-health Spikerock can pop 9 vehicles.\n");
    switch (type)
    {
    case ZOMBIE_CATAPULT:
        return StrFormat(_S("{KEYWORD}Attack:{STAT} %d damage per basketball; crushes plants when driving.\n{KEYWORD}Counter:{STAT} Umbrella Leaf blocks basketballs. Spikeweed or Spikerock can pop its tires.\n"),
            gProjectileDefinition[PROJECTILE_BASKETBALL].mDamage);
    case ZOMBIE_BUNGEE:
        return _S("{KEYWORD}Attack:{STAT} Steals plants\n");
    case ZOMBIE_BOSS:
        return _S("{KEYWORD}Attack:{STAT} Crushes plants and launches fire and ice balls\n");
    default:
        break;
    }

    const int intervalMs = TICKS_BETWEEN_EATS * pvz_max(tickMs, 1);
    const int normalDps = (1000 * DAMAGE_PER_EAT + intervalMs / 2) / intervalMs;
    const int chilledDps = (1000 * DAMAGE_PER_EAT + intervalMs) / (2 * intervalMs);
    return StrFormat(_S("{KEYWORD}Bite DPS:{STAT} %d HP/s (%d when chilled)\n"), normalDps, chilledDps);
}

static SexyString AlmanacDamage(SeedType seed)
{
	auto damage = [](ProjectileType projectile) { return gProjectileDefinition[projectile].mDamage; };
	auto perHit = [&](ProjectileType projectile, const SexyChar* unit)
	{
		return StrFormat(_S("%d %s"), damage(projectile), unit);
	};

	switch (seed)
	{
	case SEED_PEASHOOTER: case SEED_THREEPEATER: case SEED_SPLITPEA:
		return perHit(PROJECTILE_PEA, _S("per pea"));
	case SEED_REPEATER: case SEED_LEFTPEATER:
		return perHit(PROJECTILE_PEA, _S("per pea, 2 peas per volley"));
	case SEED_GATLINGPEA:
		return perHit(PROJECTILE_PEA, _S("per pea, 4 peas per volley"));
	case SEED_SNOWPEA: return perHit(PROJECTILE_SNOWPEA, _S("per pea"));
	case SEED_PUFFSHROOM: case SEED_SCAREDYSHROOM: case SEED_SEASHROOM:
		return perHit(PROJECTILE_PUFF, _S("per spore"));
	case SEED_CACTUS:
#ifdef _PIERCING_CACTUS
		return StrFormat(_S("%d first hit, 10 after piercing"), damage(PROJECTILE_PIERCE_SPIKE));
#endif
	case SEED_CATTAIL: return perHit(PROJECTILE_SPIKE, _S("per spike"));
	case SEED_STARFRUIT: return perHit(PROJECTILE_STAR, _S("per star"));
	case SEED_CABBAGEPULT: return perHit(PROJECTILE_CABBAGE, _S("per cabbage"));
	case SEED_KERNELPULT: return StrFormat(_S("%d kernel / %d butter"), damage(PROJECTILE_KERNEL), damage(PROJECTILE_BUTTER));
	case SEED_MELONPULT: case SEED_WINTERMELON:
	{
		const int hit = damage(seed == SEED_MELONPULT ? PROJECTILE_MELON : PROJECTILE_WINTERMELON);
		return StrFormat(_S("%d direct / up to %d splash"), hit, hit / 3);
	}
	case SEED_TORCHWOOD:
		return StrFormat(_S("0 direct / %d per ignited pea"), damage(PROJECTILE_FIREBALL));
	case SEED_FUMESHROOM: return _S("20 per target");
	case SEED_GLOOMSHROOM: return _S("20 per pulse, 4 pulses");
	case SEED_SPIKEWEED: return _S("20 per hit");
	case SEED_SPIKEROCK: return _S("20 per hit, 2 hits per attack");
	case SEED_ICESHROOM: return _S("20 per freezable target");
	case SEED_CHERRYBOMB: case SEED_DOOMSHROOM: case SEED_JALAPENO:
	case SEED_POTATOMINE: case SEED_COBCANNON: case SEED_EXPLODE_O_NUT:
		return _S("1800 per blast");
	case SEED_SQUASH: return _S("1800 per crush");
	case SEED_CHOMPER: return _S("40 per bite or swallows whole");
	case SEED_TANGLEKELP: return _S("Instantly drowns one target");
	case SEED_IMITATER: return _S("Copies the chosen plant");
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
	case SEED_CHERRYHOVERBOMB: return _S("1800 per blast");
	case SEED_ICEBERGLETTUCE: return perHit(PROJECTILE_LETTUCE, _S("per shot"));
	case SEED_STINGER: return perHit(PROJECTILE_SPIKE, _S("per spike"));
	case SEED_BEEHIVE: return perHit(PROJECTILE_BEE, _S("per bee"));
#endif
	default: return _S("0 direct");
	}
}

static SexyString AlmanacAttackRange(SeedType seed)
{
    switch (seed)
    {
    case SEED_CHERRYBOMB:
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
    case SEED_CHERRYHOVERBOMB:
#endif
    case SEED_COBCANNON: case SEED_EXPLODE_O_NUT:
        return _S("About a 3 x 3 tile blast area");
    case SEED_DOOMSHROOM:
        return _S("Large circular blast, about 3 tiles from its center");
    case SEED_POTATOMINE:
        return _S("Its tile and nearby zombies in the same row");
    case SEED_MELONPULT: case SEED_WINTERMELON:
        return _S("Entire row; splash reaches nearby zombies in 3 rows");
    case SEED_TORCHWOOD:
        return _S("Peas passing through its tile; fire splash stays in one row");
    case SEED_JALAPENO: return _S("Entire row");
    case SEED_ICESHROOM: return _S("Entire lawn, freezable targets");
    case SEED_GLOOMSHROOM: case SEED_UMBRELLA:
        return _S("3 x 3 tiles centered on the plant");
    case SEED_PUFFSHROOM: case SEED_SEASHROOM:
        return _S("About 3 tiles ahead in the same row");
    case SEED_FUMESHROOM:
        return _S("About 4 tiles ahead in the same row");
    case SEED_CHOMPER:
        return _S("Nearby zombies in the tile ahead");
    case SEED_SQUASH:
        return _S("Nearby zombies in its own or an adjacent tile");
    case SEED_SPIKEWEED: case SEED_SPIKEROCK:
        return _S("Zombies stepping on its tile");
    case SEED_CATTAIL: return _S("Targets across the entire lawn");
    case SEED_STARFRUIT: return _S("Five firing directions across the lawn");
    case SEED_THREEPEATER: return _S("Its row and both adjacent rows");
    case SEED_SPLITPEA: return _S("Ahead and behind in the same row");
    case SEED_LEFTPEATER: return _S("Behind it in the same row");
    case SEED_PEASHOOTER: case SEED_REPEATER: case SEED_GATLINGPEA:
    case SEED_SNOWPEA: case SEED_SCAREDYSHROOM: case SEED_CACTUS:
    case SEED_CABBAGEPULT: case SEED_KERNELPULT:
        return _S("Entire row ahead");
    default: return _S("");
    }
}

static SexyString AlmanacPlantInteractions(SeedType seed, int maxHealth)
{
    switch (seed)
    {
    case SEED_SPIKEWEED:
        return _S("Zombies cannot eat it. Pops Zomboni and Catapult tires, but is destroyed after one pop.");
    case SEED_SPIKEROCK:
        return StrFormat(_S("Zombies cannot eat it. At full health, takes %d tire pops or Gargantuar smashes to destroy."),
            (maxHealth + 49) / 50);
    case SEED_TALLNUT:
        return _S("Stops Pole Vaulting, Pogo and Dolphin Rider Zombies from jumping past it.");
    case SEED_FUMESHROOM: case SEED_GLOOMSHROOM:
        return _S("Fumes damage several targets and bypass carried screens, trash cans and ladders.");
    case SEED_TORCHWOOD:
        return _S("Turns normal peas into fire peas. Snow peas become normal peas and lose their slowing effect.");
    case SEED_WINTERMELON:
        return _S("Chills the target and nearby zombies in adjacent rows; chill-resistant zombies are unaffected by the slow.");
    case SEED_UMBRELLA:
        return _S("Protects its tile and the 8 surrounding tiles from Bungee Zombies and Catapult basketballs.");
    case SEED_DOOMSHROOM:
        return _S("Leaves a crater that temporarily blocks planting.");
    case SEED_INSTANT_COFFEE:
        return _S("Wakes a sleeping mushroom; it does not replace the mushroom.");
    case SEED_GRAVEBUSTER:
        return _S("Must be placed on a gravestone; removes it when finished.");
    default: return _S("");
    }
}

static SexyString AlmanacRecharge(SeedType seed)
{
	const int ticks = Plant::GetRefreshTime(seed, SEED_NONE);
	if (ticks % 100 == 0) return StrFormat(_S("%d s"), ticks / 100);
	if (ticks % 10 == 0) return StrFormat(_S("%d.%d s"), ticks / 100, ticks % 100 / 10);
	return StrFormat(_S("%d.%02d s"), ticks / 100, ticks % 100);
}

static SexyString AlmanacWithoutQualitativeStats(const SexyString& text, bool replaceRange = false)
{
	SexyString result;
	for (size_t start = 0; start < text.size();)
	{
		const size_t end = text.find(_S('\n'), start);
		const size_t length = (end == SexyString::npos ? text.size() : end) - start;
		const SexyString line = text.substr(start, length);
		size_t first = line.find_first_not_of(_S(" 	"));
		while (first != SexyString::npos && line[first] == _S('{'))
		{
			const size_t tagEnd = line.find(_S('}'), first);
			if (tagEnd == SexyString::npos) break;
			first = line.find_first_not_of(_S(" 	"), tagEnd + 1);
		}
		if (first == SexyString::npos ||
			(line.compare(first, 7, _S("Damage:")) != 0 && line.compare(first, 10, _S("Toughness:")) != 0 &&
                (!replaceRange || line.compare(first, 6, _S("Range:")) != 0)))
			result.append(text, start, length + (end != SexyString::npos));
		if (end == SexyString::npos) break;
		start = end + 1;
	}
	return result;
}

void AlmanacDialog::DrawEntryText(Graphics* g, const SexyString& text, const Rect& bounds, DrawStringJustification align)
{
	auto* font = FONT_BRIANNETOD12;
	const Color color(40, 50, 90);
	const int height = TodDrawStringWrappedHelper(g, text, bounds, font, color, align, false, true);
	Rect viewport = bounds;
	const bool scroll = height > bounds.mHeight;
	if (scroll) viewport.mWidth -= 10;
	const int contentHeight = scroll ? TodDrawStringWrappedHelper(g, text, viewport, font, color, align, false, true) : height;
	mScrollbar->mScrollRange = pvz_max(0, contentHeight - bounds.mHeight);
	mScrollbar->mScrollValue = ClampFloat(mScrollbar->mScrollValue, 0, mScrollbar->mScrollRange);
	mScrollbar->SetDisabled(!scroll);
	mScrollbar->Resize(bounds.mX + bounds.mWidth - 8, bounds.mY, 8, bounds.mHeight);
	mScrollbar->mViewport = viewport;
	Graphics body(*g);
	body.SetClipRect(viewport);
	body.mTransY -= mScrollbar->mScrollValue;
	TodDrawStringWrapped(&body, text, viewport, font, color, align, true);
}

          
AlmanacDialog::AlmanacDialog(LawnApp* theApp) : LawnDialog(theApp, DIALOG_ALMANAC, true, _S("Almanac"), _S(""), _S(""), BUTTONS_NONE),
	mContentPages(this, 330, 563)
{
	mApp = (LawnApp*)gSexyAppBase;
	mOpenPage = ALMANAC_PAGE_INDEX;
	mSelectedSeed = SEED_PEASHOOTER;
	mSelectedZombie = ZOMBIE_NORMAL;
	mZombie = nullptr;
	mPlant = nullptr;
	mDrawStandardBack = false;
	mScrollAmount = 0;
	TodLoadResources("DelayLoad_Almanac");
	for (int i = 0; i < LENGTH(mZombiePerfTest); i++) mZombiePerfTest[i] = nullptr;
	LawnDialog::Resize(0, 0, BOARD_WIDTH, BOARD_HEIGHT);

	mCloseButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_CLOSE);
	mCloseButton->SetLabel(_S("[CLOSE_BUTTON]"));
	mCloseButton->mButtonImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTON;
	mCloseButton->mOverImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTONHIGHLIGHT;
	mCloseButton->mDownImage = nullptr;
	mCloseButton->SetFont(Sexy::FONT_BRIANNETOD12);
	Color aColor = Color(42, 42, 90);
	mCloseButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
	mCloseButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
	mCloseButton->Resize(676, 567, 89, 26);
	mCloseButton->mParentWidget = this;
	mCloseButton->mTextOffsetX = -8;
	mCloseButton->mTextOffsetY = 1;

	mIndexButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_INDEX);
	mIndexButton->SetLabel(_S("[ALMANAC_INDEX]"));
	mIndexButton->mButtonImage = Sexy::IMAGE_ALMANAC_INDEXBUTTON;
	mIndexButton->mOverImage = Sexy::IMAGE_ALMANAC_INDEXBUTTONHIGHLIGHT;
	mIndexButton->mDownImage = nullptr;
	mIndexButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mIndexButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
	mIndexButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
	mIndexButton->Resize(32, 567, 164, 26);
	mIndexButton->mParentWidget = this;
	mIndexButton->mTextOffsetX = 8;
	mIndexButton->mTextOffsetY = 1;

	mPlantButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_PLANT);
	mPlantButton->SetLabel(_S("[VIEW_PLANTS]"));
	mPlantButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON;
	mPlantButton->mOverImage = nullptr;
	mPlantButton->mDownImage = nullptr;
	mPlantButton->mDisabledImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_DISABLED;
	mPlantButton->mOverOverlayImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_GLOW;
	mPlantButton->SetFont(Sexy::FONT_DWARVENTODCRAFT18YELLOW);
	mPlantButton->mColors[ButtonWidget::COLOR_LABEL] = Color::White;
	mPlantButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color::White;
	mPlantButton->Resize(130, 345, 156, 42);
	mPlantButton->mTextOffsetY = -1;
	mPlantButton->mParentWidget = this;

	mZombieButton = new GameButton(AlmanacDialog::ALMANAC_BUTTON_ZOMBIE);
	mZombieButton->SetLabel(_S("[VIEW_ZOMBIES]"));
	mZombieButton->Resize(487, 345, 210, 48);
	mZombieButton->mDrawStoneButton = true;
	mZombieButton->mParentWidget = this;

	mScrollbar = new LawnScrollbarWidget(mApp, ScrollbarMode::VERTICAL);
	mScrollbar->SetDisabled(true);

#ifdef OPENFL_HOST
	SetCustomEntryCounts(CustomContent::Plants.size(), customAlmanacZombies.size());
#else
	SetCustomEntryCounts(0, 0);
#endif
	SetPage(ALMANAC_PAGE_INDEX);
	if (!mApp->mBoard || !mApp->mBoard->mPaused)
		mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);
}

                     
AlmanacDialog::~AlmanacDialog()
{
	if (mCloseButton)	delete mCloseButton;
	if (mIndexButton)	delete mIndexButton;
	if (mPlantButton)	delete mPlantButton;
	if (mZombieButton)	delete mZombieButton;
	if (mScrollbar)		delete mScrollbar;
	ClearPlantsAndZombies();
	mFollowersList.clear();

}

          
void AlmanacDialog::ClearPlantsAndZombies()
{
	if (mPlant)
	{
		mPlant->Die();
		delete mPlant;
		mPlant = nullptr;
	}
	if (mZombie)
	{
		mZombie->DieNoLoot();
		delete mZombie;
		mZombie = nullptr;
	}
	for (Zombie* &aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->DieNoLoot();
			delete aZombie;
		}

		aZombie = nullptr;
	}

	for (size_t i = 0; i < mFollowersList.size();)
	{
		Zombie* aZombie = mFollowersList[i];
		if (aZombie)
		{
			aZombie->DieNoLoot();
			delete aZombie;
		}
		mFollowersList.erase(mFollowersList.begin() + i);
	}

}

          
void AlmanacDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	this->RemoveWidget(mScrollbar);
	ClearPlantsAndZombies();
}

          
void AlmanacDialog::SetupPlant()
{
	ClearPlantsAndZombies();
	mScrollbar->mScrollValue = 0;
	mScrollAmount = 0;

	float aPosX = ALMANAC_PLANT_POSITION_X;
	float aPosY = ALMANAC_PLANT_POSITION_Y;
	if (mSelectedSeed == SEED_TALLNUT)				aPosY += 18;
	else if (mSelectedSeed == SEED_COBCANNON)		aPosX -= 40;
	else if (mSelectedSeed == SEED_FLOWERPOT)		aPosY -= 20;
	else if (mSelectedSeed == SEED_INSTANT_COFFEE)	aPosY += 20;
	else if (mSelectedSeed == SEED_GRAVEBUSTER)		aPosY += 55;

	mPlant = new Plant();
	mPlant->mBoard = nullptr;
	mPlant->mIsOnBoard = false;
	mPlant->PlantInitialize(0, 0, mSelectedSeed, SEED_NONE);
	mPlant->mX = aPosX;
	mPlant->mY = aPosY;
}

          
void AlmanacDialog::SetupZombie()
{
	ClearPlantsAndZombies();

	mZombie = new Zombie();
	mZombie->mBoard = nullptr;
	mZombie->ZombieInitialize(0, mSelectedZombie, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
	mZombie->mPosX = mZombie->mX = mZombie->mPosX = ALMANAC_ZOMBIE_POSITION_X;
	mZombie->mPosY = mZombie->mY = ALMANAC_ZOMBIE_POSITION_Y;

	if (mSelectedZombie == ZombieType::ZOMBIE_BOBSLED)
	{
		for (int _i = 0; _i < 4; _i++)
		{
			Zombie* aFollower = new Zombie();
			aFollower->mBoard = nullptr;
			aFollower->ZombieInitialize(0, ZombieType::ZOMBIE_BOBSLED, false, mZombie, Zombie::ZOMBIE_WAVE_UI);
			aFollower->mX = aFollower->mPosX = mZombie->mPosX + 50 + 50 * _i;
			aFollower->mY = aFollower->mPosY = mZombie->mPosY;

			aFollower->PlayZombieReanim("anim_jump", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 20.0f);
			aFollower->mApp->ReanimationGet(aFollower->mBodyReanimID)->mAnimTime = 1.0f;
			aFollower->mAltitude = 18.0f;
			aFollower->mZombiePhase = ZombiePhase::PHASE_BOBSLED_BOARDING;
			
			mFollowersList.push_back(aFollower);
		}
	}

	mScrollbar->mScrollValue = 0;
	mScrollbar->mScrollRange = 0;
	mScrollbar->Resize(0, 0, 0, 0);
	mScrollbar->mViewport = Rect();
	mScrollAmount = 0;
}

void AlmanacDialog::SetCustomEntryCounts(std::size_t plants, std::size_t zombies)
{
	mCustomPlantCount = plants;
	mCustomZombieCount = zombies;
	const bool zombiePage = mOpenPage == ALMANAC_PAGE_ZOMBIES || mOpenPage == ALMANAC_PAGE_CUSTOM_ZOMBIES;
	mContentPages.SetCustomEntryCount(zombiePage ? zombies : plants,
		zombiePage ? CUSTOM_ZOMBIES_PER_PAGE : CUSTOM_PLANTS_PER_PAGE);
}

          
void AlmanacDialog::SetPage(AlmanacPage thePage)
{
    if (thePage == ALMANAC_PAGE_CUSTOM_PLANTS && mCustomPlantCount == 0) thePage = ALMANAC_PAGE_PLANTS;
    if (thePage == ALMANAC_PAGE_CUSTOM_ZOMBIES && mCustomZombieCount == 0) thePage = ALMANAC_PAGE_ZOMBIES;
	mOpenPage = thePage;
	ClearPlantsAndZombies();
	const bool custom = thePage == ALMANAC_PAGE_CUSTOM_PLANTS || thePage == ALMANAC_PAGE_CUSTOM_ZOMBIES;
	if (!custom) mContentPages.Reset();
	SetCustomEntryCounts(mCustomPlantCount, mCustomZombieCount);
	mContentPages.Update(thePage != ALMANAC_PAGE_INDEX);
	if (custom) mScrollbar->SetDisabled(true);

	if (mOpenPage == AlmanacPage::ALMANAC_PAGE_INDEX)
	{
		mPlant = new Plant();
		mPlant->mBoard = nullptr;
		mPlant->mIsOnBoard = false;
		mPlant->PlantInitialize(0, 0, SeedType::SEED_SUNFLOWER, SeedType::SEED_NONE);
		mPlant->mX = ALMANAC_INDEXPLANT_POSITION_X;
		mPlant->mY = ALMANAC_INDEXPLANT_POSITION_Y;

		mZombie = new Zombie();
		mZombie->mBoard = nullptr;
		mZombie->ZombieInitialize(0, ZombieType::ZOMBIE_NORMAL, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
		mZombie->mX = mZombie->mPosX = ALMANAC_INDEXZOMBIE_POSITION_X;
		mZombie->mY = mZombie->mPosY = ALMANAC_INDEXZOMBIE_POSITION_Y;

		mIndexButton->mBtnNoDraw = true;
		mPlantButton->mBtnNoDraw = false;
		mZombieButton->mBtnNoDraw = false;
		mScrollbar->SetDisabled(true);
	}
	else
	{
		if (mOpenPage == ALMANAC_PAGE_CUSTOM_PLANTS)
		{
			mSelectedSeed = CustomContent::Plants.front();
			SetupPlant();
		}
		else if (mOpenPage == ALMANAC_PAGE_PLANTS)
		{
			if (CustomContent::PlantSlot(mSelectedSeed) >= 0) mSelectedSeed = SEED_PEASHOOTER;
			SetupPlant();
		}
		else if (mOpenPage == ALMANAC_PAGE_CUSTOM_ZOMBIES)
		{
			mSelectedZombie = customAlmanacZombies.front();
			SetupZombie();
		}
		else if (mOpenPage == ALMANAC_PAGE_ZOMBIES)
		{
			if (IsCustomAlmanacZombie(mSelectedZombie)) mSelectedZombie = ZOMBIE_NORMAL;
			SetupZombie();
		}
		else if (!custom) return;

		mIndexButton->mBtnNoDraw = false;
		mPlantButton->mBtnNoDraw = true;
		mZombieButton->mBtnNoDraw = true;
	}
}

void AlmanacDialog::ShowPlant(SeedType theSeedType)
{
	mSelectedSeed = theSeedType;
	if (CustomContent::PlantSlot(theSeedType) >= 0)
	{
		SetPage(ALMANAC_PAGE_PLANTS);
		mContentPages.ShowFirstCustomPage();
		SetPage(ALMANAC_PAGE_CUSTOM_PLANTS);
	}
	else SetPage(ALMANAC_PAGE_PLANTS);
}

void AlmanacDialog::ShowZombie(ZombieType theZombieType)
{
	if (IsCustomAlmanacZombie(theZombieType))
	{
		SetPage(ALMANAC_PAGE_ZOMBIES);
		mContentPages.ShowFirstCustomPage();
		SetPage(ALMANAC_PAGE_CUSTOM_ZOMBIES);
		mSelectedZombie = theZombieType;
		SetupZombie();
	}
	else
	{
		mSelectedZombie = theZombieType == ZOMBIE_REDEYE_GARGANTUAR ? ZOMBIE_GARGANTUAR : theZombieType;
		SetPage(ALMANAC_PAGE_ZOMBIES);
	}
}

          
void AlmanacDialog::Update()
{
	mCloseButton->Update();
	mIndexButton->Update();
	mPlantButton->Update();
	mZombieButton->Update();
	mContentPages.Update(mOpenPage != ALMANAC_PAGE_INDEX);
	if (mPlant) mPlant->Update();
	if (mZombie) mZombie->Update();
	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->Update();
		}
	}

	for (Zombie* aZombie : mFollowersList)
	{
		if (aZombie) aZombie->Update();
	}
	mScrollbar->Update();

	if (mApp->mWidgetManager->mOverWidget)
	{
		int aMouseX = (mApp->mWidgetManager->mLastMouseX - mX);
		int aMouseY = (mApp->mWidgetManager->mLastMouseY - mY);
		if (SeedHitTest(aMouseX, aMouseY) != SeedType::SEED_NONE || ZombieHitTest(aMouseX, aMouseY) != ZombieType::ZOMBIE_INVALID ||
			mCloseButton->IsMouseOver() || mIndexButton->IsMouseOver() || mPlantButton->IsMouseOver() || mZombieButton->IsMouseOver() || mContentPages.IsMouseOver() || mScrollbar->isThumbOver())
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}

	Widget::Update();

	if (!mScrollbar->mDisabled)
	{
		float aScrollSpeed = 1 + abs(mScrollAmount) * 0.1f;
		mScrollbar->mScrollValue = ClampFloat(mScrollbar->mScrollValue += mScrollAmount * aScrollSpeed, 0, mScrollbar->mScrollRange);
		mScrollAmount *= 0.9f;
	}
	
	mApp->mPoolEffect->PoolEffectUpdate();
	MarkDirty();
}

ZombieType AlmanacDialog::GetZombieType(int theIndex)
{
	return theIndex >= 0 && theIndex <= ZOMBIE_BOSS ? (ZombieType)theIndex : ZOMBIE_INVALID;
}

          
void AlmanacDialog::DrawIndex(Graphics* g)
{
#ifdef OPENFL_HOST
    pvz_host_draw_background(g, "Almanac_IndexBack");
#else
	g->DrawImage(Sexy::IMAGE_ALMANAC_INDEXBACK, 0, 0);
#endif
	TodDrawString(g, _S("[SUBURBAN_ALMANAC_INDEX]"), BOARD_WIDTH / 2, 60, Sexy::FONT_HOUSEOFTERROR28, Color(220, 220, 220), DrawStringJustification::DS_ALIGN_CENTER);
	
	if (mPlant)
	{
		g->PushState();
		mPlant->BeginDraw(g);
		mPlant->Draw(g);
		g->PopState();
	}
	if (mZombie)
	{
		g->PushState();
		mZombie->BeginDraw(g);
		mZombie->DrawShadow(g);
		mZombie->Draw(g);
		g->PopState();
	}
}

          
void AlmanacDialog::DrawPlants(Graphics* g)
{
#ifdef OPENFL_HOST
    pvz_host_draw_background(g, "Almanac_PlantBack");
#else
	g->DrawImage(Sexy::IMAGE_ALMANAC_PLANTBACK, 0, 0);
#endif
	const bool custom = mOpenPage == ALMANAC_PAGE_CUSTOM_PLANTS;
	TodDrawString(g, custom ? _S("Custom Plants") : _S("[SUBURBAN_ALMANAC_PLANTS]"), BOARD_WIDTH / 2, 48, Sexy::FONT_HOUSEOFTERROR20, Color(213, 159, 43), DrawStringJustification::DS_ALIGN_CENTER);

	SeedType aSeedMouseOn = SeedHitTest((mApp->mWidgetManager->mLastMouseX - mX), (mApp->mWidgetManager->mLastMouseY - mY));
	if (mApp->SeedTypeAvailable(SEED_IMITATER))
	{
		int x, y;
		GetSeedPosition(SEED_IMITATER, x, y);
		g->DrawImage(Sexy::IMAGE_ALMANAC_IMITATER, x, y);
	}
	if (custom)
    {
        const std::size_t start = (mContentPages.GetPage() - 1) * CUSTOM_PLANTS_PER_PAGE;
        const std::size_t end = std::min(start + CUSTOM_PLANTS_PER_PAGE, CustomContent::Plants.size());
        for (std::size_t slot = start; slot < end; ++slot)
        {
            const SeedType seed = CustomContent::Plants[slot];
            if (!mApp->SeedTypeAvailable(seed)) continue;
            int x, y;
            GetSeedPosition(seed, x, y);
            DrawSeedPacket(g, x, y, seed, SEED_NONE, 0, 255, true, false);
            if (aSeedMouseOn == seed) g->DrawImage(IMAGE_SEEDPACKETFLASH, x, y);
        }
    }
	for (SeedType aSeedType = SeedType::SEED_PEASHOOTER; !custom && aSeedType < NUM_ALMANAC_SEEDS; aSeedType = (SeedType)(aSeedType + 1))
	{
		if (aSeedType == SEED_IMITATER || !mApp->SeedTypeAvailable(aSeedType)) continue;
		int aPosX, aPosY;
		GetSeedPosition(aSeedType, aPosX, aPosY);
		DrawSeedPacket(g, aPosX, aPosY, aSeedType, SeedType::SEED_NONE, 0, 255, true, false);
		if (aSeedType == aSeedMouseOn)
			g->DrawImage(Sexy::IMAGE_SEEDPACKETFLASH, aPosX, aPosY);
	}

	if (mSelectedSeed == SeedType::SEED_LILYPAD || mSelectedSeed == SeedType::SEED_TANGLEKELP || 
		mSelectedSeed == SeedType::SEED_CATTAIL || mSelectedSeed == SeedType::SEED_SEASHROOM)
	{
		bool aNight = mSelectedSeed == SeedType::SEED_SEASHROOM;
		g->DrawImage(aNight ? Sexy::IMAGE_ALMANAC_GROUNDNIGHTPOOL : Sexy::IMAGE_ALMANAC_GROUNDPOOL, 521, 107);

		if (mApp->Is3DAccelerated())
		{
			g->SetClipRect(475, 0, 397, 500);
			g->mTransY -= 145;
			mApp->mPoolEffect->PoolEffectDraw(g, aNight);
			g->mTransY += 145;
			g->ClearClipRect();
		}
	}
	else
	{
		g->DrawImage(
			Plant::IsNocturnal(mSelectedSeed) || mSelectedSeed == SeedType::SEED_GRAVEBUSTER || mSelectedSeed == SeedType::SEED_PLANTERN ? Sexy::IMAGE_ALMANAC_GROUNDNIGHT :
			mSelectedSeed == SeedType::SEED_FLOWERPOT ? Sexy::IMAGE_ALMANAC_GROUNDROOF : Sexy::IMAGE_ALMANAC_GROUNDDAY,
			521, 107
		);
	}
	
	if (mPlant)
	{
		g->PushState();
		mPlant->BeginDraw(g);
		mPlant->Draw(g);
		g->PopState();
	}

	g->DrawImage(Sexy::IMAGE_ALMANAC_PLANTCARD, 459, 86);
	PlantDefinition& aPlantDef = GetPlantDefinition(mSelectedSeed);
	SexyString aName = Plant::GetNameString(mSelectedSeed, SEED_NONE);
	SexyString description;
    const auto key = StrFormat(_S("[%s_DESCRIPTION]"), aPlantDef.mPlantName);
    const SexyString range = AlmanacAttackRange(mSelectedSeed);
    description = AlmanacWithoutQualitativeStats(TodStringTranslate(key), !range.empty());
	TodDrawString(g, aName, 617, 288, Sexy::FONT_DWARVENTODCRAFT18YELLOW, Color::White, DS_ALIGN_CENTER);
	const SexyString damage = AlmanacDamage(mSelectedSeed);
	SexyString stats = mSelectedSeed == SEED_IMITATER
		? _S("{KEYWORD}Health and damage:{STAT} Copies the chosen plant\n{KEYWORD}Recharge:{STAT} Same as the chosen plant\n")
		: StrFormat(_S("{KEYWORD}Health:{STAT} %d HP\n{KEYWORD}Damage:{STAT} %s\n"),
			mPlant->mPlantMaxHealth, damage.c_str());
	if (!range.empty()) stats += _S("{KEYWORD}Range:{STAT} ") + range + _S("\n");
    const SexyString interactions = AlmanacPlantInteractions(mSelectedSeed, mPlant->mPlantMaxHealth);
    if (!interactions.empty()) stats += _S("{KEYWORD}Special:{STAT} ") + interactions + _S("\n");
	DrawEntryText(g, stats + _S("\n{NORMAL}") + description,
		Rect(485, 309, 258, mSelectedSeed == SEED_IMITATER ? 228 : 205), DS_ALIGN_LEFT);

	if (mSelectedSeed != SeedType::SEED_IMITATER)
	{
		SexyString aCostStr = TodReplaceString(StrFormat(_S("{KEYWORD}{COST}:{STAT} %d"), aPlantDef.mSeedCost), _S("{COST}"), _S("[COST]"));
		TodDrawStringWrapped(g, aCostStr, Rect(485, 520, 134, 50), Sexy::FONT_BRIANNETOD12, Color::White, DS_ALIGN_LEFT);

		SexyString aRechargeStr = _S("{KEYWORD}") + TodStringTranslate(_S("[WAIT_TIME]")) +
			_S(": {STAT}") + AlmanacRecharge(mSelectedSeed);
		TodDrawStringWrapped(g, aRechargeStr, Rect(600, 520, 139, 50), Sexy::FONT_BRIANNETOD12, Color(40, 50, 90), DS_ALIGN_RIGHT);
	}
}

          
void AlmanacDialog::DrawZombies(Graphics* g)
{
#ifdef OPENFL_HOST
    pvz_host_draw_background(g, "Almanac_ZombieBack");
#else
	g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEBACK, 0, 0);
#endif
	const bool custom = mOpenPage == ALMANAC_PAGE_CUSTOM_ZOMBIES;
	TodDrawString(g, custom ? _S("Custom Zombies") : _S("[SUBURBAN_ALMANAC_ZOMBIES]"), BOARD_WIDTH / 2, 54, Sexy::FONT_DWARVENTODCRAFT24, Color(0, 196, 0), DS_ALIGN_CENTER);

	ZombieType aZombieMouseOn = ZombieHitTest((mApp->mWidgetManager->mLastMouseX - mX), (mApp->mWidgetManager->mLastMouseY - mY));
	const int count = custom ? static_cast<int>(customAlmanacZombies.size()) : NUM_ALMANAC_ZOMBIES;
	for (int i = 0; i < count; i++)
	{
		if (custom && static_cast<std::size_t>(i / CUSTOM_ZOMBIES_PER_PAGE + 1) != mContentPages.GetPage()) continue;
        ZombieType aZombieType = custom ? customAlmanacZombies[i] : GetZombieType(i);
		int aPosX, aPosY;
		GetZombiePosition(aZombieType, aPosX, aPosY);
		if (aZombieType != ZombieType::ZOMBIE_INVALID)
		{
			if (!ZombieIsShown(aZombieType))
			{
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEBLANK, aPosX, aPosY);
			}
			else
			{
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
				if (aZombieType == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}

				ZombieType aZombieTypeToDraw = aZombieType;
				g->PushState();
				g->SetClipRect(aPosX + 2, aPosY + 2, 72, 72);
				g->Translate(aPosX + 1, aPosY - 6);
				g->mScaleX = 0.5f;
				g->mScaleY = 0.5f;
				switch (aZombieType)
				{
				case ZombieType::ZOMBIE_POLEVAULTER:
				{
					g->TranslateF(-24.25f, -3);
					aZombieTypeToDraw = ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE;
				}
				break;
				case ZombieType::ZOMBIE_FLAG:			g->TranslateF(2, 10);		break;
				case ZombieType::ZOMBIE_TRAFFIC_CONE:	g->TranslateF(0, 12);		break;
				case ZombieType::ZOMBIE_PAIL:			g->TranslateF(0, 9);		break;
				case ZombieType::ZOMBIE_FOOTBALL:		g->TranslateF(-15, -1);	break;
				case ZombieType::ZOMBIE_ZAMBONI:		g->TranslateF(-25, 3);		break;
				case ZombieType::ZOMBIE_DOLPHIN_RIDER:	g->TranslateF(-2, -10);	break;
				case ZombieType::ZOMBIE_POGO:			g->TranslateF(0, -3);		break;
				case ZombieType::ZOMBIE_GARGANTUAR:		g->TranslateF(15, 17);		break;
				case ZombieType::ZOMBIE_IMP:			g->TranslateF(-8, -7);		break;
				case ZombieType::ZOMBIE_BUNGEE:			g->TranslateF(-4, 3);		break;
#ifdef _HAS_NEW_DANCER
				case ZombieType::ZOMBIE_BACKUP_DANCER:	g->TranslateF(-2, 5);		break;
#else
				case ZombieType::ZOMBIE_BACKUP_DANCER:	g->TranslateF(-8, 5);		break;
#endif
				case ZombieType::ZOMBIE_SNORKEL:		g->TranslateF(-10, 0);		break;
				case ZombieType::ZOMBIE_YETI:			g->TranslateF(0, 4);		break;
				case ZombieType::ZOMBIE_CATAPULT:		g->TranslateF(-24, -1);	break;
				case ZombieType::ZOMBIE_BOBSLED:		g->TranslateF(0, -8);		break;
				case ZombieType::ZOMBIE_LADDER:			g->TranslateF(0, -3);		break;
				default: break;
				}
				if (ZombieHasSilhouette(aZombieType))
				{
					g->SetColor(Color(0, 0, 0, 40));
					g->SetColorizeImages(true);
				}
				if (aZombieType == ZombieType::ZOMBIE_BUNGEE)
				{
					g->mTransY -= 300;
				}

				SDL_DisplayID display = SDL_GetPrimaryDisplay();
				float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;

				g->mScaleX /= scale;
				g->mScaleY /= scale;

				mApp->mReanimatorCache->DrawCachedZombie(g, 0, 0, aZombieTypeToDraw);
				g->PopState();
				g->PushState();
				g->SetLinearBlend(false);
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
				g->PopState();
				if (aZombieType == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}
			}
		}
	}

	g->DrawImage(mZombie->mZombieType == ZombieType::ZOMBIE_ZAMBONI || mZombie->mZombieType == ZombieType::ZOMBIE_BOBSLED ?
		Sexy::IMAGE_ALMANAC_GROUNDICE : Sexy::IMAGE_ALMANAC_GROUNDDAY, 518, 110);
	if (mZombie && !ZombieHasSilhouette(mZombie->mZombieType))
	{
		int followerID = 0;
		for (Zombie* aFolllower : mFollowersList) {
			if (aFolllower)
			{
				followerID++;
				
				g->PushState();
				aFolllower->BeginDraw(g);
				g->SetClipRect(-42 - 50 * followerID, -51, 197, 187);
				if (aFolllower->mZombieType != ZombieType::ZOMBIE_BUNGEE && aFolllower->mZombieType != ZombieType::ZOMBIE_BOSS &&
					aFolllower->mZombieType != ZombieType::ZOMBIE_ZAMBONI && aFolllower->mZombieType != ZombieType::ZOMBIE_CATAPULT)
					aFolllower->DrawShadow(g);
				if (followerID == 1) {
					ZombieDrawPosition aDrawPos;
					aFolllower->GetDrawPos(aDrawPos);
					g->DrawImage(IMAGE_ZOMBIE_BOBSLED_INSIDE, aFolllower->mPosX + aDrawPos.mImageOffsetX - aFolllower->mPosX - 76.0f - 50, 15.0f);
				}
				aFolllower->Draw(g);
				g->PopState();
			}
		}

		g->PushState();
		mZombie->BeginDraw(g);
		g->SetClipRect(-42, -51, 197, 187);
		switch (mZombie->mZombieType)
		{
		case ZombieType::ZOMBIE_ZAMBONI:		g->TranslateF(-30, 5);		break;
		case ZombieType::ZOMBIE_GARGANTUAR:		g->TranslateF(0, 40);		break;
		case ZombieType::ZOMBIE_FOOTBALL:		g->TranslateF(-10, 0);		break;
		case ZombieType::ZOMBIE_BALLOON:		g->TranslateF(0, -20);		break;
		case ZombieType::ZOMBIE_BUNGEE:			g->TranslateF(15, 0);		break;
		case ZombieType::ZOMBIE_CATAPULT:		g->TranslateF(-10, 0);		break;
		case ZombieType::ZOMBIE_BOSS:			g->TranslateF(-540, -175);	break;
		default: break;
		}
		if (mZombie->mZombieType != ZombieType::ZOMBIE_BUNGEE && mZombie->mZombieType != ZombieType::ZOMBIE_BOSS &&
			mZombie->mZombieType != ZombieType::ZOMBIE_ZAMBONI && mZombie->mZombieType != ZombieType::ZOMBIE_CATAPULT)
			mZombie->DrawShadow(g);
		mZombie->Draw(g);

		if (mZombie->mZombieType == ZombieType::ZOMBIE_BOBSLED) {
			ZombieDrawPosition aDrawPos;
			mZombie->GetDrawPos(aDrawPos);
			g->SetClipRect(-42, -51, 197, 187);
			g->DrawImage(IMAGE_ZOMBIE_BOBSLED1, mZombie->mPosX + aDrawPos.mImageOffsetX - mZombie->mPosX - 76.0f, 15.0f);
		}
		g->PopState();
	}
	g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIECARD, 455, 78);

	ZombieDefinition& aZombieDef = GetZombieDefinition(mSelectedZombie);
    SexyString aName = ZombieHasSilhouette(mSelectedZombie) ? _S("???") :
        StrFormat(_S("[%s]"), aZombieDef.mZombieName);
	TodDrawString(g, aName, 613, 362, Sexy::FONT_DWARVENTODCRAFT18GREENINSET, Color(190, 255, 235, 255), DS_ALIGN_CENTER);

	SexyString aHeader;
	SexyString aDescription;

	DrawStringJustification aAlign;
	if (ZombieHasDescription(mSelectedZombie))
	{
		const int body = mZombie->mBodyMaxHealth;
		const int helmet = mZombie->mHelmMaxHealth;
		const int shield = mZombie->mShieldMaxHealth;
		const int flight = mZombie->mFlyingMaxHealth;
		if (mSelectedZombie == ZOMBIE_BOSS)
			aHeader = _S("{KEYWORD}Health:{STAT} 40000 HP (Adventure), 60000 HP (other modes)\n");
		else if (mSelectedZombie == ZOMBIE_IMP)
			aHeader = StrFormat(_S("{KEYWORD}Health:{STAT} %d HP (%d HP in I, Zombie)\n"),
				IMP_STARTING_HEALTH, I_ZOMBIE_IMP_STARTING_HEALTH);
		else if (mSelectedZombie == ZOMBIE_GARGANTUAR)
			aHeader = StrFormat(_S("{KEYWORD}Health:{STAT} %d HP (Giga-Gargantuar: 6000 HP)\n"), body);
		else
			aHeader = StrFormat(_S("{KEYWORD}Health:{STAT} %d HP\n"), body + helmet + shield + flight);
		if (helmet || shield || flight)
		{
			aHeader += StrFormat(_S("Body: %d"), body);
			if (helmet) aHeader += StrFormat(_S(" / Helmet: %d"), helmet);
			if (shield) aHeader += StrFormat(_S(" / Shield: %d"), shield);
			if (flight) aHeader += StrFormat(_S(" / Flight gear: %d"), flight);
			aHeader += _S("\n");
		}
		aHeader += AlmanacZombieAttack(mSelectedZombie, mApp->mFrameTime);
        aHeader += AlmanacWithoutQualitativeStats(TodStringTranslate(StrFormat(_S("[%s_HEADER]"), aZombieDef.mZombieName)));
        aDescription = AlmanacWithoutQualitativeStats(TodStringTranslate(StrFormat(_S("[%s_DESCRIPTION]"), aZombieDef.mZombieName)));
		aAlign = DS_ALIGN_LEFT;
	}
	else
	{
		aHeader = _S("");
		aDescription = _S("[NOT_ENCOUNTERED_YET]");
		aAlign = DS_ALIGN_CENTER_VERTICAL_MIDDLE;
	}
	for (TodStringListFormat& aFormat : gLawnStringFormats)
	{
		if (TestBit(aFormat.mFormatFlags, TodStringFormatFlag::TOD_FORMAT_HIDE_UNTIL_MAGNETSHROOM))
		{
			if (mApp->HasSeedType(SeedType::SEED_MAGNETSHROOM))
			{
				aFormat.mNewColor.mAlpha = 255;
				aFormat.mLineSpacingOffset = 0;
			}
			else
			{
				aFormat.mNewColor.mAlpha = 0;
				aFormat.mLineSpacingOffset = -17;
			}
		}
	}

	DrawEntryText(g, aHeader.empty() ? TodStringTranslate(aDescription) : aHeader + _S("\n\n{NORMAL}") + aDescription,
		Rect(484, 377, 258, 155), aAlign);
}
          
void AlmanacDialog::Draw(Graphics* g)
{
	g->SetLinearBlend(true);
	switch (mOpenPage)
	{
	case AlmanacPage::ALMANAC_PAGE_INDEX:	DrawIndex(g);	break;
	case AlmanacPage::ALMANAC_PAGE_PLANTS:	DrawPlants(g);	break;
	case AlmanacPage::ALMANAC_PAGE_ZOMBIES:	DrawZombies(g);	break;
	case ALMANAC_PAGE_CUSTOM_PLANTS: DrawPlants(g); break;
	case ALMANAC_PAGE_CUSTOM_ZOMBIES: DrawZombies(g); break;
	}

	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			g->PushState();
			aZombie->Draw(g);
			g->PopState();
		}
	}

	mCloseButton->Draw(g);
	mIndexButton->Draw(g);
	mPlantButton->Draw(g);
	mZombieButton->Draw(g);
	mContentPages.Draw(g);

	if (!mScrollbar->mDisabled)
	{
		mScrollbar->mButtonNoDraw = false;
		g->PushState();
		g->mTransX = g->mTransX + mScrollbar->mX;
		g->mTransY = g->mTransY + mScrollbar->mY;
		mScrollbar->Draw(g);
		mScrollbar->mButtonNoDraw = true;
		g->PopState();
	}
}

void AlmanacDialog::GetSeedPosition(SeedType theSeedType, int& x, int& y)
{
	if (const int slot = CustomContent::PlantSlot(theSeedType); slot >= 0)
    {
        const int local = slot % CUSTOM_PLANTS_PER_PAGE;
        x = local % 8 * 52 + 26;
        y = local / 8 * 78 + 92;
    }
	else if (theSeedType == SeedType::SEED_IMITATER)
		x = 20, y = 23;
	else
	{
		x = theSeedType % 8 * 52 + 26;
		y = theSeedType / 8 * 78 + 92;

		if (theSeedType > SeedType::SEED_IMITATER)
			x -= 52;
	}
}

          
SeedType AlmanacDialog::SeedHitTest(int x, int y)
{
	if (!mMouseVisible) return SEED_NONE;
	if ((mOpenPage == ALMANAC_PAGE_PLANTS || mOpenPage == ALMANAC_PAGE_CUSTOM_PLANTS) &&
		mApp->SeedTypeAvailable(SEED_IMITATER))
	{
		int seedX, seedY;
		GetSeedPosition(SEED_IMITATER, seedX, seedY);
		if (Rect(seedX, seedY, 34, 46).Contains(x, y)) return SEED_IMITATER;
	}
	if (mOpenPage == ALMANAC_PAGE_CUSTOM_PLANTS)
    {
        const std::size_t start = (mContentPages.GetPage() - 1) * CUSTOM_PLANTS_PER_PAGE;
        const std::size_t end = std::min(start + CUSTOM_PLANTS_PER_PAGE, CustomContent::Plants.size());
        for (std::size_t slot = start; slot < end; ++slot)
        {
            const SeedType seed = CustomContent::Plants[slot];
            if (!mApp->SeedTypeAvailable(seed)) continue;
            int seedX, seedY;
            GetSeedPosition(seed, seedX, seedY);
            if (Rect(seedX, seedY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT).Contains(x, y)) return seed;
        }
    }
	if (mOpenPage == ALMANAC_PAGE_PLANTS)
	{
		for (SeedType aSeedType = SeedType::SEED_PEASHOOTER; aSeedType < NUM_ALMANAC_SEEDS; aSeedType = (SeedType)(aSeedType + 1))
		{
			if (aSeedType != SEED_IMITATER && mApp->SeedTypeAvailable(aSeedType))
			{
				int aSeedX, aSeedY;
				GetSeedPosition(aSeedType, aSeedX, aSeedY);
				Rect aSeedRect(aSeedX, aSeedY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
				if (aSeedRect.Contains(x, y)) return aSeedType;
			}
		}
	}
	return SeedType::SEED_NONE;
}

bool AlmanacDialog::ZombieHasSilhouette(ZombieType theZombieType)
{
	                                                                                                                                                                     
	if (theZombieType != ZombieType::ZOMBIE_YETI || mApp->CanSpawnYetis())
		return false;

	                                                                                                                                          
	return mApp->HasFinishedAdventure() || mApp->mPlayerInfo->GetLevel() > GetZombieDefinition(ZombieType::ZOMBIE_YETI).mStartingLevel;
}

          
bool AlmanacDialog::ZombieIsShown(ZombieType theZombieType)
{
	                                                                     
	if (mApp->IsTrialStageLocked() && theZombieType > ZombieType::ZOMBIE_SNORKEL)
		return false;

	if (IsCustomAlmanacZombie(theZombieType))
		return true;

	                                                                                                                        
	                                                                                                                                    
	if (theZombieType == ZombieType::ZOMBIE_YETI)
		return mApp->CanSpawnYetis() || ZombieHasSilhouette(ZombieType::ZOMBIE_YETI);

	                                       
	                                               
	{
		                                                            
		if (mApp->HasFinishedAdventure())
			return true;

		int aLevel = mApp->mPlayerInfo->GetLevel();
		int aStart = GetZombieDefinition(theZombieType).mStartingLevel;
		                                                
		                                                                                                                                                                              
		return aStart <= aLevel && (aStart != aLevel || !Board::IsZombieTypeSpawnedOnly(theZombieType) || gZombieDefeated[theZombieType]);
	}

	return false;
}

          
bool AlmanacDialog::ZombieHasDescription(ZombieType theZombieType)
{
	if (IsCustomAlmanacZombie(theZombieType)) return true;
	int aLevel = mApp->mPlayerInfo->GetLevel();
	int aStart = GetZombieDefinition(theZombieType).mStartingLevel;

	                     
	if (theZombieType == ZombieType::ZOMBIE_YETI)
	{
		                                                                                                                  
		if (!mApp->CanSpawnYetis())
			return false;
		                                                            
		if (mApp->mPlayerInfo->mFinishedAdventure >= 2)
			return true;
	}
	                                                                                                   
	else if (mApp->HasFinishedAdventure())
		return true;

	                                                                                                                  
	                                                                                                               
	return aStart <= aLevel && (aStart != aLevel || gZombieDefeated[theZombieType]);
}

void AlmanacDialog::GetZombiePosition(ZombieType theZombieType, int& x, int& y)
{
	if (theZombieType == ZombieType::ZOMBIE_BOSS)
		x = 192, y = 486;
	else if (IsCustomAlmanacZombie(theZombieType))
	{
		const int slot = CustomContent::ZombieSlot(theZombieType) % CUSTOM_ZOMBIES_PER_PAGE;
        x = slot % 5 * 85 + 22;
        y = slot / 5 * 80 + 86;
	}
	else
	{
		int theIndex = (int) theZombieType;

		if (theZombieType >= ZombieType::ZOMBIE_BOSS)
			--theIndex;

		x = theIndex % 5 * 85 + 22;
		y = theIndex / 5 * 80 + 86;
	}
}

          
ZombieType AlmanacDialog::ZombieHitTest(int x, int y)
{
	if (mMouseVisible && mOpenPage == ALMANAC_PAGE_CUSTOM_ZOMBIES)
	{
		for (ZombieType type : customAlmanacZombies)
        {
            if (static_cast<std::size_t>(CustomContent::ZombieSlot(type) / CUSTOM_ZOMBIES_PER_PAGE + 1) != mContentPages.GetPage()) continue;
			int entryX, entryY;
			GetZombiePosition(type, entryX, entryY);
			if (Rect(entryX, entryY, 76, 76).Contains(x, y)) return type;
		}
	}
	if (mMouseVisible && mOpenPage == AlmanacPage::ALMANAC_PAGE_ZOMBIES)
	{
		for (int i = 0; i < NUM_ALMANAC_ZOMBIES; i++)
		{
			ZombieType aZombieType = GetZombieType(i);
			if (aZombieType != ZombieType::ZOMBIE_INVALID && ZombieIsShown(aZombieType))
			{
				int aZombieX, aZombieY;
				GetZombiePosition(aZombieType, aZombieX, aZombieY);
				if (Rect(aZombieX, aZombieY, 76, 76).Contains(x, y))
					return aZombieType;
			}
		}
	}
	return ZombieType::ZOMBIE_INVALID;
}

          
void AlmanacDialog::MouseUp(int x, int y, int theClickCount)
{
	if (mPlantButton->IsMouseOver())		SetPage(ALMANAC_PAGE_PLANTS);
	else if (mZombieButton->IsMouseOver())	SetPage(ALMANAC_PAGE_ZOMBIES);
	else if (mCloseButton->IsMouseOver())	mApp->KillAlmanacDialog();
	else if (mIndexButton->IsMouseOver())	SetPage(ALMANAC_PAGE_INDEX);
	else if (theClickCount == 1 && mContentPages.Click())
	{
		const bool plants = mOpenPage == ALMANAC_PAGE_PLANTS || mOpenPage == ALMANAC_PAGE_CUSTOM_PLANTS;
		const AlmanacPage page = plants
			? (mContentPages.IsCustomPage() ? ALMANAC_PAGE_CUSTOM_PLANTS : ALMANAC_PAGE_PLANTS)
			: (mContentPages.IsCustomPage() ? ALMANAC_PAGE_CUSTOM_ZOMBIES : ALMANAC_PAGE_ZOMBIES);
		if (page != mOpenPage) SetPage(page);
	}
}

          
void AlmanacDialog::MouseDown(int x, int y, int theClickCount)
{
	if (mPlantButton->IsMouseOver() || mCloseButton->IsMouseOver() || mIndexButton->IsMouseOver() || mContentPages.IsMouseOver())
		mApp->PlaySample(Sexy::SOUND_TAP);
	if (mZombieButton->IsMouseOver())
		mApp->PlaySample(Sexy::SOUND_GRAVEBUTTON);

	SeedType aSeedType = SeedHitTest(x, y);
	if (aSeedType != SeedType::SEED_NONE && aSeedType != mSelectedSeed)
	{
		mSelectedSeed = aSeedType;
		SetupPlant();
		mApp->PlaySample(Sexy::SOUND_TAP);
	}
	ZombieType aZombieType = ZombieHitTest(x, y);
	if (aZombieType != ZombieType::ZOMBIE_INVALID && aZombieType != mSelectedZombie)
	{
		mSelectedZombie = aZombieType;
		SetupZombie();
		mApp->PlaySample(Sexy::SOUND_TAP);
	}

	if(mScrollbar->isThumbOver())
	{
		mScrollbar->MouseDown(x, y, theClickCount);
	}
}

void AlmanacInitForPlayer()
{
	for (int i = 0; i < ZombieType::NUM_ZOMBIE_TYPES; i++)
		gZombieDefeated[i] = false;
}

void AlmanacPlayerDefeatedZombie(ZombieType theZombieType)
{
	gZombieDefeated[(int)theZombieType] = true;
}

void AlmanacDialog::MouseWheel(int theDelta)
{
	Widget::MouseWheel(theDelta);

	if (mScrollbar->mDisabled) return;

	mScrollAmount -= theDelta;
	mScrollAmount -= mScrollAmount * 0.1f;
}

void AlmanacDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	this->AddWidget(mScrollbar);
}

void AlmanacDialog::OrderInManagerChanged()
{
}
