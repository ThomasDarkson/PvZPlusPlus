#include "ZombieTrap.h"
#include "Board.h"
#include "Challenge.h"
#include "Zombie.h"
#include "Coin.h"
#include "Widget/GameButton.h"
#include "System/SaveGame.h"
#include "System/PlayerInfo.h"
#include "../LawnApp.h"
#include "../Resources.h"
#include "../SexyAppFramework/SoundManager.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../Sexy.TodLib/TodFoley.h"
#include <algorithm>
#include <cmath>

using namespace Sexy;
namespace
{
    constexpr int TrapX = 198, TrapY = 214, TrapWidth = 390, TrapHeight = 203;
    constexpr int Goal = 35;
    constexpr int PickupDelay = 40, PickupSpacing = 15, PickupTravel = 30;
    constexpr int PickupLiftStart = PickupDelay + 3 * PickupSpacing + PickupTravel;
    constexpr int PickupEnd = PickupLiftStart + 110;
    const char* Tutorials[] = {
        "Slide brains to attract zombies!",
        "Get them to the center, and press the \"Cage\" icon in the upper right!",
        "Catch 35 zombies to win! Don't let zombies get to your house!"
    };
    float FeetX(const Zombie& z) { return z.mPosX + 50; }
    float FeetY(const Zombie& z) { return z.mPosY + 110; }
    bool InTrap(const Zombie& z)
    {
        return FeetX(z) >= TrapX && FeetX(z) <= TrapX + TrapWidth &&
            FeetY(z) >= TrapY && FeetY(z) <= TrapY + TrapHeight;
    }
}

std::string ZombieTrap::RecordKey() const
{
    return "ZombieTrapBestCage_" + std::to_string(mBoard->mApp->mPlayerInfo->mId);
}

void ZombieTrap::Init(Board* board)
{
    mBoard = board;
    mBoard->mApp->RegistryReadInteger(RecordKey(), &mBest);
    mBoard->mSunMoney = 0;
}

void ZombieTrap::Start()
{
    mIntro = true;
    mBoard->mApp->CrazyDaveEnter();
    mBoard->mApp->CrazyDaveTalkMessage("You caught " + std::to_string(mBest) + " zombies at one time!");
}

void ZombieTrap::PlayCrow(bool flap)
{
    auto* app = mBoard->mApp;
    if (!app->mSoundManager) return;
    int& id = flap ? mFlap : mCaw;
    if (id < 0) id = app->mSoundManager->LoadSound(flap ?
        "../widescreen-assets/zombie-trap/flap" : "../widescreen-assets/zombie-trap/caw");
    if (id >= 0) app->PlaySample(id);
}

void ZombieTrap::MouseDown(int x, int y)
{
    if (mBoard->mPaused || mBoard->mApp->GetDialogCount() || mDone) return;
    if (mIntro)
    {
        if (!mDaveLeaving)
        {
            mDaveLeaving = true;
            mBoard->mApp->CrazyDaveLeave();
        }
        return;
    }
    auto* cageButton = mBoard->mStoreButton;
    if (cageButton && Rect(cageButton->mX - mBoard->mApp->mDDInterface->mWideScreenOffsetX,
        cageButton->mY - mBoard->mApp->mDDInterface->mWideScreenOffsetY,
        cageButton->mWidth, cageButton->mHeight).Contains(x, y))
    {
        DropCage();
        return;
    }
    if (mBrains > 0 && mBrainAltitude == 0 &&
        std::hypot(x - mBrainX, y - mBrainY) < 55)
    {
        mDragging = true;
        MouseMove(x, y);
    }
}

void ZombieTrap::MouseMove(int x, int y)
{
    if (!mDragging || mIntro || mDone || mBoard->mPaused || mBoard->mApp->GetDialogCount()) return;
    mBrainX = std::clamp(float(x), 0.0f, 735.0f);
    mBrainY = std::clamp(float(y), 125.0f, 535.0f);
}

void ZombieTrap::DropCage()
{
    if (mCagePhase || mCages <= 0) return;
    --mCages;
    mCagePhase = 1;
    mCageTicks = 0;
    mTrapped.clear();
    mCaught = 0;
}

void ZombieTrap::FinishCapture()
{
    mCaught = 0;
    for (ZombieID id : mTrapped)
        if (auto* zombie = mBoard->ZombieTryToGet(id))
            if (!zombie->IsDeadOrDying())
            {
                zombie->mDroppedLoot = true;
                zombie->mBodyHealth = 0;
                zombie->PlayDeathAnim(0U);
                ++mCaught;
            }
    mScore += mCaught;
    mBoard->mChallenge->mChallengeScore = mScore;
    if (mCaught >= 5)
    {
        ++mBrains;
        ++mCages;
        mBoard->DisplayAdvice("Great catch! +1 brain and +1 cage!", MESSAGE_STYLE_HINT_LONG, ADVICE_NONE);
        mBoard->mApp->PlaySample(SOUND_POINTS);
    }
    if (mCaught > mBest)
    {
        mBest = mCaught;
        mBoard->mApp->RegistryWriteInteger(RecordKey(), mBest);
    }
    mTrapped.clear();
}

void ZombieTrap::Fail()
{
    if (mDone) return;
    mDone = true;
    mDragging = false;
    mBoard->ZombiesWon();
}

void ZombieTrap::PrepareZombie(Zombie& zombie)
{
    if (zombie.mZombieType == ZOMBIE_GARGANTUAR)
    {
        zombie.mHasObject = false;
        zombie.ReanimShowPrefix("Zombie_imp", RENDER_GROUP_HIDDEN);
        zombie.ReanimShowTrack("Zombie_gargantuar_whiterope", RENDER_GROUP_HIDDEN);
    }
}

void ZombieTrap::SpawnBungee()
{
    const int row = Rand(5);
    auto* zombie = mBoard->AddZombieInRow(ZOMBIE_NORMAL, row, mBoard->mCurrentWave);
    auto* carrier = mBoard->AddZombieInRow(ZOMBIE_BUNGEE, row, mBoard->mCurrentWave);
    if (zombie && carrier) carrier->BungeeDropZombie(zombie, RandRangeInt(0, 2), row);
}

void ZombieTrap::Update()
{
    auto* app = mBoard->mApp;
    if (mIntro && !mBoard->mPaused && !app->GetDialogCount())
    {
        app->UpdateCrazyDave();
        if (mDaveLeaving && app->mCrazyDaveState == CRAZY_DAVE_OFF)
        {
            mIntro = mDaveLeaving = false;
            mBoard->DisplayAdvice(Tutorials[0], MESSAGE_STYLE_HINT_LONG, ADVICE_NONE);
        }
    }
    if (mDone || mIntro || mBoard->mPaused || app->GetDialogCount() ||
        app->mGameScene != SCENE_PLAYING || mBoard->HasLevelAwardDropped()) return;
    ++mTicks;
    if (mTicks == 400 || mTicks == 800)
        mBoard->DisplayAdvice(Tutorials[mTicks / 400], MESSAGE_STYLE_HINT_LONG, ADVICE_NONE);
    mBrainAltitude = std::max(0.0f, mBrainAltitude - 2.4f);
    if (mCagePhase)
    {
        ++mCageTicks;
        if (mCagePhase == 1 && mCageTicks >= 70)
        {
            Zombie* zombie = nullptr;
            while (mBoard->IterateZombies(zombie))
                if (!zombie->IsDeadOrDying() && zombie->mZombieType != ZOMBIE_BUNGEE &&
                    zombie->mZombieHeight == HEIGHT_ZOMBIE_NORMAL && InTrap(*zombie))
                    mTrapped.push_back(mBoard->ZombieGetID(zombie));
            mCagePhase = 2;
            mCageTicks = 0;
            mBoard->ShakeBoard(2, 2);
            mBoard->mApp->PlayFoley(FOLEY_THUMP);
            if (!mTrapped.empty()) PlayCrow(false);
        }
        else if (mCagePhase == 2)
        {
            if (!mTrapped.empty() && mCageTicks % 40 == 0)
            {
                mBoard->mApp->PlayFoley(FOLEY_CHOMP);
                PlayCrow(false);
            }
            if (mCageTicks >= 220)
            {
                FinishCapture();
                mCagePhase = 3;
                mCageTicks = 0;
            }
        }
        else if (mCagePhase == 3)
        {
            if (mCaught > 0 && mCageTicks >= PickupDelay &&
                mCageTicks <= PickupDelay + 3*PickupSpacing &&
                (mCageTicks-PickupDelay)%PickupSpacing == 0) PlayCrow(true);
            if (mCageTicks >= PickupEnd)
            {
                mCagePhase = 0;
                mCageTicks = 0;
                if (mScore >= Goal)
                {
                    mDone = true;
                    Zombie* zombie = nullptr;
                    while (mBoard->IterateZombies(zombie))
                        if (!zombie->IsDeadOrDying()) zombie->DieNoLoot();
                    mBoard->mChallenge->SpawnLevelAward(4, 2);
                }
                else if (mCages == 0) Fail();
            }
        }
    }
}

bool ZombieTrap::IsAttracted(ZombieID id) const
{
    return std::any_of(mAttracted.begin(), mAttracted.end(),
        [id](const Attraction& state) { return state.id == id && state.interested; });
}

bool ZombieTrap::FacingRight(Zombie& zombie) const
{
    return mBrains > 0 && mBrainAltitude == 0 && IsAttracted(mBoard->ZombieGetID(&zombie)) &&
        mBrainX > FeetX(zombie);
}

bool ZombieTrap::UpdateZombie(Zombie& zombie)
{
    if (zombie.IsDeadOrDying() || zombie.mZombieType == ZOMBIE_BUNGEE ||
        zombie.mZombieHeight == HEIGHT_GETTING_BUNGEE_DROPPED) return false;
    if (mDone || mIntro || mBoard->HasLevelAwardDropped()) return true;
    const ZombieID id = mBoard->ZombieGetID(&zombie);
    if (std::find(mTrapped.begin(), mTrapped.end(), id) != mTrapped.end())
    {
        zombie.UpdateReanim();
        return true;
    }
    if (zombie.mPhaseCounter > 0 && !zombie.IsImmobilizied()) --zombie.mPhaseCounter;
    if (zombie.mZombieType == ZOMBIE_DANCER) zombie.UpdateZombieDancer();
    else if (zombie.mZombieType == ZOMBIE_BACKUP_DANCER) zombie.UpdateZombieBackupDancer();
    if (zombie.mZombiePhase == PHASE_DANCER_RISING)
    {
        zombie.UpdateReanim();
        return true;
    }
    const bool groundedBrain = mBrains > 0 && mBrainAltitude == 0;
    const float sightX = mBrainX - FeetX(zombie);
    const float sightY = mBrainY - FeetY(zombie);
    auto interest = std::find_if(mAttracted.begin(), mAttracted.end(),
        [id](const Attraction& state) { return state.id == id; });
    if (groundedBrain && interest != mAttracted.end() && interest->interested &&
        std::hypot(sightX, sightY) > 380.0f)
    {
        interest->interested = false;
        interest->noticedAt = mTicks;
    }
    if (groundedBrain && !IsAttracted(id) && sightX >= -280 && sightX <= 20 && std::abs(sightY) <= 70)
    {
        if (interest == mAttracted.end()) mAttracted.push_back({id, mTicks, true});
        else
        {
            interest->interested = true;
            interest->noticedAt = mTicks;
        }
    }
    const bool bait = groundedBrain && IsAttracted(id);
    const float dx = bait ? sightX : -1000;
    const float dy = bait ? mBrainY - FeetY(zombie) : 0;
    const float distance = std::hypot(dx, dy);
    const bool eating = bait && distance < 30;
    const bool ridingPogo = zombie.mZombieType == ZOMBIE_POGO && zombie.IsBouncingPogo();
    auto stopEating = [&]()
    {
        if (ridingPogo)
        {
            zombie.mIsEating = false;
            zombie.PlayZombieReanim("anim_pogo", REANIM_PLAY_ONCE_AND_HOLD, 0, 40.0f);
        }
        else zombie.StopEating();
    };
    if (eating)
    {
        if (!zombie.mIsEating)
        {
            if (ridingPogo) zombie.mIsEating = true;
            else zombie.StartEating();
        }
        if (mBrainHealth % 60 == 0) mBoard->mApp->PlayFoley(FOLEY_CHOMP);
        if (--mBrainHealth <= 0)
        {
            --mBrains;
            mAttracted.clear();
            mDragging = false;
            stopEating();
            if (mBrains <= 0) Fail();
            else
            {
                mBrainHealth = 300;
                mBrainX = 400;
                mBrainY = 300;
                mBrainAltitude = 250;
            }
        }
    }
    else
    {
        if (zombie.mIsEating) stopEating();
        const float speed = zombie.mZombieType == ZOMBIE_FOOTBALL ? 0.9f :
            zombie.mZombieType == ZOMBIE_POGO ? 0.72f : zombie.mZombieType == ZOMBIE_GARGANTUAR ? 0.32f : 0.48f;
        if (distance > 0.01f && !zombie.ZombieNotWalking())
        {
            zombie.mPosX += dx / distance * speed;
            zombie.mPosY += dy / distance * speed;
        }
    }
    zombie.mX = int(zombie.mPosX);
    zombie.mY = int(zombie.mPosY);
    zombie.mRow = std::clamp(mBoard->PixelToGridY(400, int(FeetY(zombie))), 0, 4);
    zombie.mRenderOrder = Board::MakeRenderOrder(RENDER_LAYER_ZOMBIE, zombie.mRow, 4);
    if (zombie.mZombieType == ZOMBIE_POGO) zombie.UpdateZombiePogo();
    zombie.UpdateReanim();
    zombie.CheckForBoardEdge();
    return true;
}

void ZombieTrap::DrawGround(Graphics* g)
{
    g->DrawImageF(IMAGE_STRIPECROWS, 148, 178);
}

void ZombieTrap::Draw(Graphics* g)
{
    if (mDone || mBoard->HasLevelAwardDropped()) return;
    g->PushState();
    if (mBrains > 0)
        g->DrawImage(IMAGE_BRAIN, int(mBrainX)-25, int(mBrainY-mBrainAltitude)-20, 50, 40);
    float cageY = TrapY - 72;
    if (mCagePhase == 1) cageY = TrapY - 72 - 600 * (1.0f - mCageTicks / 70.0f);
    if (mCagePhase == 3) cageY = TrapY - 72 - std::max(0, mCageTicks-PickupLiftStart) * 5.5f;
    if (mCagePhase)
    {
        if (mCagePhase == 2 && !mTrapped.empty())
        {
            for (int i = 0; i < 20; ++i)
            {
                const int x = TrapX + 25 + (i*61+mTicks*2)%(TrapWidth-50);
                const int y = TrapY + 20 + (i*47)%(TrapHeight-40);
                g->SetColor(Color(120+i%3*15, 120+i%3*15, 120+i%3*15, 235));
                for (int scan = -20; scan <= 20; scan += 4)
                {
                    const int radius = int(std::sqrt(float(400-scan*scan))*1.4f);
                    g->FillRect(x-radius, y+scan, radius*2+1, 4);
                }
            }
        }
    }
    auto drawCage = [&]()
    {
        if (!mCagePhase) return;
        g->SetColorizeImages(false);
        Image* cage = pvz_host_dialog_image("../zombie-trap/cage");
        if (cage) g->DrawImage(cage, TrapX, int(cageY), TrapWidth, 275);
    };
    if (mCagePhase != 2) drawCage();
    if (mCagePhase == 2)
    {
        g->PushState();
        g->ClipRect(TrapX+8, TrapY, TrapWidth-16, TrapHeight-6);
    }
    for (int bird = 0; ((mCagePhase == 2 && !mTrapped.empty()) || (mCagePhase == 3 && mCaught > 0)) && bird < 4; ++bird)
    {
        const std::string name = "../zombie-trap/crow-fly" + std::to_string((mTicks/8+bird)%4+1);
        Image* crow = pvz_host_dialog_image(name.c_str());
        float x = 265 + bird*80, y = 65;
        if (mCagePhase == 2)
        {
            x = TrapX+65+(bird%2)*260+std::sin(mTicks*.1f+bird)*20;
            const float attackY = TrapY+45+(bird/2)*110+std::cos(mTicks*.13f+bird)*22;
            y = -80 + (attackY+80)*std::min(1.0f,mCageTicks/35.0f);
        }
        if (mCagePhase == 3)
        {
            const int arrival = PickupDelay + bird*PickupSpacing;
            if (mCageTicks < arrival) continue;
            const float progress = std::clamp((mCageTicks-arrival)/float(PickupTravel), 0.0f, 1.0f);
            x = TrapX+(bird%2)*TrapWidth;
            y = -130 + (cageY+(bird/2)*62+130)*progress;
        }
        if (crow) g->DrawImage(crow, int(x)-70, int(y)-115, 140, 164);
    }
    if (mCagePhase == 2)
    {
        g->PopState();
        drawCage();
    }
    for (const auto& notice : mAttracted)
    {
        auto* zombie = mBoard->ZombieTryToGet(notice.id);
        if (!zombie || zombie->IsDeadOrDying() || mTicks-notice.noticedAt >= 90) continue;
        const int x = int(FeetX(*zombie));
        const int y = int(zombie->mPosY)-12;
        const char* mark = notice.interested ? "!" : "?";
        TodDrawString(g, mark, x+1, y+1, FONT_DWARVENTODCRAFT24, Color(0,0,0), DS_ALIGN_CENTER);
        TodDrawString(g, mark, x, y, FONT_DWARVENTODCRAFT24, Color(255,255,255), DS_ALIGN_CENTER);
    }
    g->PopState();
}

void ZombieTrap::Sync(SaveGameContext& context)
{
    if (!context.mReading) mSpawnTicks = -20261123;
    for (int* v : {&mBrains,&mCages,&mScore,&mBrainHealth,&mTicks,&mSpawnTicks,&mBungeeTicks,
        &mCagePhase,&mCageTicks,&mCaught,&mBest}) context.SyncInt(*v);
    for (float* v : {&mBrainX,&mBrainY,&mBrainAltitude}) context.SyncBytes(v,sizeof(*v));
    int count = int(mTrapped.size());
    context.SyncInt(count);
    if (count < 0 || count > 1024) { context.mFailed = true; return; }
    if (context.mReading) mTrapped.resize(count);
    for (auto& id : mTrapped) context.SyncBytes(&id,sizeof(id));
    if (mSpawnTicks == -20261122 || mSpawnTicks == -20261123)
    {
        int attractedCount = int(mAttracted.size());
        context.SyncInt(attractedCount);
        if (attractedCount < 0 || attractedCount > 1024) { context.mFailed = true; return; }
        if (context.mReading) mAttracted.resize(attractedCount);
        for (auto& notice : mAttracted)
        {
            context.SyncBytes(&notice.id,sizeof(notice.id));
            context.SyncInt(notice.noticedAt);
            if (mSpawnTicks == -20261123)
            {
                int interested = notice.interested ? 1 : 0;
                context.SyncInt(interested);
                if (interested < 0 || interested > 1) context.mFailed = true;
                notice.interested = interested != 0;
            }
            else if (context.mReading) notice.interested = true;
        }
    }
    else if (context.mReading) mAttracted.clear();
    if (context.mReading)
    {
        mDragging = mIntro = mDone = mDaveLeaving = false;
        if (mBrains < 0 || mCages < 0 || mScore < 0 || mCagePhase < 0 || mCagePhase > 3 ||
            !std::isfinite(mBrainX) || !std::isfinite(mBrainY)) context.mFailed = true;
    }
}
