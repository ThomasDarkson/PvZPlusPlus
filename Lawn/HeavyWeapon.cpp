#include "HeavyWeapon.h"
#include "Board.h"
#include "Challenge.h"
#include "Coin.h"
#include "CursorObject.h"
#include "Plant.h"
#include "Projectile.h"
#include "SeedPacket.h"
#include "ToolTipWidget.h"
#include "System/SaveGame.h"
#include "Zombie.h"
#include "../LawnApp.h"
#include "../Resources.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../Sexy.TodLib/TodFoley.h"
#include "../SexyAppFramework/WidgetManager.h"
#include <algorithm>
#include <cmath>

using namespace Sexy;

namespace
{
    constexpr float HeavyPivotY = 592.0f;
    constexpr float HeavyBaseY = 544.0f;
    constexpr float HeavyTailY = 550.0f;
    constexpr float HeavyFloorY = 600.0f;
    constexpr float HeavyProjectileSpeed = 3.33f;
    constexpr float HeavyPi = 3.14159265359f;
    constexpr int HeavyCosts[] = {200, 400, 200, 125, 100, 200};
    constexpr SeedType HeavySeeds[] = {SEED_REPEATER, SEED_THREEPEATER, SEED_CATTAIL,
        SEED_TORCHWOOD, SEED_SNOWPEA, SEED_DOOMSHROOM};

    float HeavyAxis(Sint16 value)
    {
        const float normalized = value / 32768.0f;
        const float magnitude = std::abs(normalized);
        return magnitude <= 0.18f ? 0.0f :
            std::copysign((magnitude - 0.18f) / 0.82f, normalized);
    }
}

HeavyWeapon::~HeavyWeapon()
{
    if (mGamepad) SDL_CloseGamepad(mGamepad);
    if (mOwnsGamepads) SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    if (mBoard)
    {
        for (auto id : {mBody, mTurret, mTail})
            mBoard->mApp->RemoveReanimation(id);
    }
}

void HeavyWeapon::Attach(Board* board)
{
    mBoard = board;
    if (!mOwnsGamepads) mOwnsGamepads = SDL_InitSubSystem(SDL_INIT_GAMEPAD);
}

void HeavyWeapon::Init(Board* board)
{
    Attach(board);
    mBoard->mSunMoney = 50;
    mBoard->mShowShovel = false;
    mBoard->mSeedBank->mNumPackets = 6;
    for (int index = 0; index < 6; ++index)
        mBoard->mSeedBank->mSeedPackets[index].SetPacketType(HeavySeeds[index]);

    mBody = mTurret = mTail = ReanimationID::REANIMATIONID_NULL;
    EnsureAnimations();
}

void HeavyWeapon::EnsureAnimations()
{
    const ReanimationType types[] = {REANIM_HEAVY_TANK, REANIM_HEAVY_TURRET, REANIM_HEAVY_CATTAIL};
    ReanimationID* ids[] = {&mBody, &mTurret, &mTail};
    for (int index = 0; index < 3; ++index)
    {
        auto* reanim = mBoard->mApp->ReanimationTryToGet(*ids[index]);
        if (reanim && !reanim->mDead && reanim->mReanimationType == types[index]) continue;
        reanim = mBoard->mApp->AddReanimation(mX - (index == 2 ? 126 : 110), HeavyBaseY - (index == 2 ? 16 : 0), 0, types[index]);
        reanim->mIsAttachment = true;
        reanim->PlayReanim(index == 1 ? "anim_idle" : "anim_drive", REANIM_LOOP, 0, 0);
        *ids[index] = mBoard->mApp->ReanimationGetID(reanim);
    }
}

void HeavyWeapon::Sync(SaveGameContext& context)
{
    for (int* value : {&mTier, &mSelection, &mFireCountdown, &mPowerTicks, &mPower,
                      &mSunCountdown, &mBungeeCountdown})
        context.SyncInt(*value);
    for (float* value : {&mX, &mAimX, &mAimY, &mMouseX, &mMouseY})
        context.SyncBytes(value, sizeof(*value));
    for (ReanimationID* id : {&mBody, &mTurret, &mTail})
        context.SyncBytes(id, sizeof(*id));
    if (context.mReading)
    {
        if (mTier < 0 || mTier > 5 || mSelection < 0 || mSelection >= 6 ||
            mPower < 0 || mPower > 2 || mPowerTicks < 0 || mPowerTicks > 700 ||
            !std::isfinite(mX) || !std::isfinite(mAimX) || !std::isfinite(mAimY) ||
            !std::isfinite(mMouseX) || !std::isfinite(mMouseY))
            context.mFailed = true;
        mMouseFiring = false;
        mTailFireCountdown = 0;
    }
}

void HeavyWeapon::Start()
{
    mBoard->mSunMoney = 50;
    mBoard->mShowShovel = false;
    mBoard->DisplayAdvice(_S("Move your lawnmower with A and D! Aim and hold left click to fire!\nPick an upgrade with 1-6, then press Enter to power up!"),
        MESSAGE_STYLE_HINT_TALL_LONG, ADVICE_NONE);
}

void HeavyWeapon::Aim(int x, int y)
{
    mPadAiming = false;
    mMouseX = float(x);
    mMouseY = float(y);
    const float dx = mMouseX - MuzzleX();
    const float dy = std::min(0.0f, mMouseY - MuzzleY());
    const float distance = std::sqrt(dx * dx + dy * dy);
    if (distance < 0.001f) return;
    mAimX = dx / distance;
    mAimY = dy / distance;
}

void HeavyWeapon::MouseDown(int x, int y, int clicks)
{
    if (clicks <= 0) return;
    const int selection = PacketAt(x, y);
    if (selection >= 0)
    {
        mSelection = selection;
        return;
    }
    Aim(x, y);
    mMouseFiring = true;
}

void HeavyWeapon::MouseUp()
{
    mMouseFiring = false;
}

int HeavyWeapon::PacketAt(int x, int y) const
{
    const int localX = x - mBoard->mSeedBank->mX - 80;
    const int localY = y - mBoard->mSeedBank->mY - 9;
    if (localX < 0 || localY < 0 || localY >= 70) return -1;
    const int index = localX / 59;
    return index < 6 && localX % 59 < 50 ? index : -1;
}

void HeavyWeapon::UpdateToolTip(int x, int y)
{
    auto* tooltip = mBoard->mToolTip;
    const int index = PacketAt(x, y);
    tooltip->mVisible = index >= 0;
    if (index < 0) return;
    tooltip->SetLabel(Plant::GetNameString(HeavySeeds[index]));
    tooltip->SetWarningText(_S(""));
    if (!CanBuy(index))
        tooltip->SetWarningText(index < 3 ? _S("Upgrade unavailable") : _S("[WAITING_FOR_SEED]"));
    else if (mBoard->mSunMoney < HeavyCosts[index])
        tooltip->SetWarningText(_S("[NOT_ENOUGH_SUN]"));
    tooltip->mX = mBoard->mSeedBank->mX + 80 + index * 59 + (50 - tooltip->mWidth) / 2;
    tooltip->mY = mBoard->mSeedBank->mY + 81;
}

bool HeavyWeapon::KeyDown(KeyCode key)
{
    if (!mBoard || mBoard->mPaused || mBoard->mApp->GetDialogCount() ||
        mBoard->mApp->mGameScene != SCENE_PLAYING || mBoard->HasLevelAwardDropped()) return false;
    const int code = static_cast<int>(key);
    if (code >= '1' && code <= '6')
    {
        mSelection = code - '1';
        return true;
    }
    if (key == KEYCODE_RETURN)
    {
        Buy();
        return true;
    }
    return code == 'A' || code == 'D';
}

bool HeavyWeapon::CanBuy(int selection) const
{
    if (selection == 0) return mTier == 0;
    if (selection == 1) return mTier == 1;
    if (selection == 2) return mTier >= 2 && mTier < 5;
    if (selection == 3) return mPower != 1 || mPowerTicks == 0;
    if (selection == 4) return mPower != 2 || mPowerTicks == 0;
    return true;
}

void HeavyWeapon::Buy()
{
    auto* app = mBoard->mApp;
    if (!CanBuy(mSelection) || mBoard->mSunMoney < HeavyCosts[mSelection])
    {
        app->PlaySample(SOUND_BUZZER);
        return;
    }
    if (mSelection == 5)
    {
        const int column = mBoard->PixelToGridXKeepOnBoard(int(mMouseX), int(mMouseY));
        const int row = mBoard->PixelToGridYKeepOnBoard(int(mMouseX), int(mMouseY));
        if (mBoard->CanPlantAt(column, row, SEED_DOOMSHROOM) != PLANTING_OK)
        {
            app->PlaySample(SOUND_BUZZER);
            return;
        }
        auto* doom = mBoard->AddPlant(column, row, SEED_DOOMSHROOM, SEED_NONE);
        doom->SetSleeping(false);
    }
    mBoard->mSunMoney -= HeavyCosts[mSelection];
    if (mSelection < 3)
    {
        ++mTier;
        mFireCountdown = 0;
        mTailFireCountdown = 0;
    }
    else if (mSelection < 5)
    {
        mPower = mSelection == 3 ? 1 : 2;
        mPowerTicks = 700;
    }
    app->PlaySample(SOUND_SEEDLIFT);
    app->PlaySample(SOUND_TAP2);
}

bool HeavyWeapon::PollController(float& movement)
{
    if (mGamepad && !SDL_GamepadConnected(mGamepad))
    {
        SDL_CloseGamepad(mGamepad);
        mGamepad = nullptr;
        mWasBuying = mWasPausing = false;
        mPadSelection = 0;
    }
    if (mOwnsGamepads && !mGamepad && --mScanCountdown <= 0)
    {
        mScanCountdown = 100;
        int count = 0;
        auto* ids = SDL_GetGamepads(&count);
        for (int index = 0; index < count && !mGamepad; ++index)
            mGamepad = SDL_OpenGamepad(ids[index]);
        SDL_free(ids);
    }
    if (!mGamepad) return false;
    SDL_UpdateGamepads();
    movement += HeavyAxis(SDL_GetGamepadAxis(mGamepad, SDL_GAMEPAD_AXIS_LEFTX));
    const float ax = HeavyAxis(SDL_GetGamepadAxis(mGamepad, SDL_GAMEPAD_AXIS_RIGHTX));
    const float ay = HeavyAxis(SDL_GetGamepadAxis(mGamepad, SDL_GAMEPAD_AXIS_RIGHTY));
    if (ax * ax + ay * ay > 0.04f)
    {
        mPadAiming = true;
        const float up = std::min(ay, -0.05f);
        const float length = std::sqrt(ax * ax + up * up);
        mAimX = ax / length;
        mAimY = up / length;
        mMouseX = MuzzleX() + mAimX * 350.0f;
        mMouseY = MuzzleY() + mAimY * 350.0f;
    }
    const int selection = int(SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) -
        int(SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
    if (selection && selection != mPadSelection) mSelection = (mSelection + 6 + selection) % 6;
    mPadSelection = selection;
    const bool buy = SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_SOUTH);
    const bool pause = SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_START);
    auto* app = mBoard->mApp;
    if (pause && !mWasPausing)
    {
        if (app->GetDialog(DIALOG_NEWOPTIONS)) app->KillNewOptionsDialog();
        else if (!app->GetDialogCount() && app->mGameScene == SCENE_PLAYING) app->DoNewOptions(false);
    }
    if (buy && !mWasBuying && !mBoard->mPaused && !app->GetDialogCount() && app->mGameScene == SCENE_PLAYING) Buy();
    mWasBuying = buy;
    mWasPausing = pause;
    return SDL_GetGamepadAxis(mGamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 8000;
}

void HeavyWeapon::Fire()
{
    const int shots = mTier == 0 ? 1 : mTier == 1 ? 2 : 3;
    const float aim = std::atan2(mAimY, mAimX);
    for (int index = 0; index < shots; ++index)
    {
        const float spread = (index - (shots - 1) * 0.5f) * 0.17f;
        const float angle = std::clamp(aim + spread, -HeavyPi, 0.0f);
        const float dx = std::cos(angle), dy = std::sin(angle);
        const ProjectileType type = mPower == 2 ? PROJECTILE_SNOWPEA : PROJECTILE_PEA;
        auto* shot = mBoard->AddProjectile(int(MuzzleX() + dx * 24 - 8), int(MuzzleY() + dy * 24 - 8),
            Board::MakeRenderOrder(RENDER_LAYER_PROJECTILE, 4, 0), 4, type);
        shot->mMotionType = MOTION_HEAVY_WEAPON;
        shot->mDamageRangeFlags = (1 << DAMAGES_GROUND) | (1 << DAMAGES_FLYING) | (1 << DAMAGES_OFF_GROUND);
        shot->mVelX = dx * HeavyProjectileSpeed;
        shot->mVelY = dy * HeavyProjectileSpeed;
        shot->mShadowY = shot->mPosY + 28;
        if (mPower == 1) shot->ConvertToFireball(-2);
    }
    mBoard->mApp->PlayFoley(FOLEY_THROW);
    if (auto* turret = mBoard->mApp->ReanimationTryToGet(mTurret))
        turret->PlayReanim("anim_shoot", REANIM_PLAY_ONCE_AND_HOLD, 0, 60);
}

void HeavyWeapon::FireTail()
{
    auto* shot = mBoard->AddProjectile(int(mX - 34), int(HeavyTailY - 8),
        Board::MakeRenderOrder(RENDER_LAYER_PROJECTILE, 4, 0), 4, PROJECTILE_SPIKE);
    shot->mMotionType = MOTION_HEAVY_WEAPON;
    shot->mDamageRangeFlags = (1 << DAMAGES_GROUND) | (1 << DAMAGES_FLYING) | (1 << DAMAGES_OFF_GROUND);
    shot->mVelX = mAimX * HeavyProjectileSpeed;
    shot->mVelY = mAimY * HeavyProjectileSpeed;
    shot->mShadowY = shot->mPosY + 28;
    mBoard->mApp->PlayFoley(FOLEY_THROW);
    if (auto* tail = mBoard->mApp->ReanimationTryToGet(mTail))
        tail->PlayReanim("anim_shoot02", REANIM_PLAY_ONCE_AND_HOLD, 0, 60);
}

void HeavyWeapon::UpdateFiring(bool firing)
{
    if (mFireCountdown > 0) --mFireCountdown;
    if (mTailFireCountdown > 0) --mTailFireCountdown;
    if (!firing) return;
    if (mFireCountdown == 0)
    {
        Fire();
        mFireCountdown = 18;
    }
    if (mTier >= 3 && mTailFireCountdown == 0)
    {
        FireTail();
        const int intervals[] = {90, 60, 45};
        mTailFireCountdown = intervals[mTier - 3];
    }
}

void HeavyWeapon::DropZombie()
{
    const int firstRow = Rand(5);
    for (int index = 0; index < 3; ++index)
    {
        const int row = (firstRow + index) % 5;
        auto* bungee = mBoard->AddZombie(ZOMBIE_BUNGEE, mBoard->mCurrentWave);
        auto* cargo = mBoard->AddZombie(ZOMBIE_NORMAL, mBoard->mCurrentWave);
        bungee->BungeeDropZombie(cargo, RandRangeInt(3, 8), row);
    }
}

void HeavyWeapon::Update()
{
    auto* app = mBoard->mApp;
    if (!app->mHasFocus)
    {
        mMouseFiring = false;
        return;
    }
    float movement = float(app->mWidgetManager->mKeyDown['D']) -
        float(app->mWidgetManager->mKeyDown['A']);
    const bool padFiring = PollController(movement);
    if (app->mGameScene != SCENE_PLAYING || mBoard->mPaused || app->GetDialogCount() || mBoard->HasLevelAwardDropped() ||
        mBoard->mBoardFadeOutCounter >= 0)
    {
        mMouseFiring = false;
        return;
    }
    mX = std::clamp(mX + std::clamp(movement, -1.0f, 1.0f) * 3.5f, 65.0f, 740.0f);
    if (!mGamepad || !mPadAiming) Aim(int(mMouseX), int(mMouseY));
    else
    {
        mMouseX = MuzzleX() + mAimX * 350.0f;
        mMouseY = MuzzleY() + mAimY * 350.0f;
    }
    if (mPowerTicks > 0 && --mPowerTicks == 0) mPower = 0;
    if (--mSunCountdown <= 0)
    {
        auto* sun = mBoard->AddCoin(RandRangeInt(100, 675), 60, COIN_SUN, COIN_MOTION_FROM_SKY);
        sun->mGroundY = HeavyFloorY;
        mSunCountdown = 500;
    }
    if (--mBungeeCountdown <= 0)
    {
        const bool nearFlag = mBoard->mHugeWaveCountDown > 0 ||
            (mBoard->mCurrentWave < mBoard->mNumWaves &&
             mBoard->IsFlagWave(mBoard->mCurrentWave) && mBoard->mZombieCountDown <= 1000);
        if (!nearFlag && mBoard->mCurrentWave < mBoard->mNumWaves) DropZombie();
        mBungeeCountdown = 1000;
    }
    EnsureAnimations();
    UpdateFiring(mMouseFiring || padFiring);
    if (auto* body = app->ReanimationTryToGet(mBody))
    {
        body->SetPosition(mX - 110, HeavyBaseY);
        body->mAnimRate = std::abs(movement) > 0.01f ? 30.0f : 0.0f;
        body->Update();
    }
    for (auto id : {mTurret, mTail})
    {
        if (auto* animation = app->ReanimationTryToGet(id))
        {
            animation->SetPosition(mX - (id == mTail ? 126 : 110), HeavyBaseY - (id == mTail ? 16 : 0));
            animation->Update();
        }
    }
}

void HeavyWeapon::UpdatePickup(Coin& coin)
{
    if (coin.mDead || coin.mFadeCount || coin.IsLevelAward()) return;
    coin.mGroundY = HeavyFloorY;
    if (coin.mCoinMotion == COIN_MOTION_FROM_PRESENT)
    {
        coin.mCoinMotion = COIN_MOTION_COIN;
        coin.mVelY = 1.5f;
    }
    if (std::abs(coin.mPosX + coin.mWidth * 0.5f - mX) <= 48 &&
        coin.mPosY + coin.mHeight >= HeavyPivotY - 12)
    {
        coin.PlayCollectSound();
        coin.Collect();
    }
}

void HeavyWeapon::UpdateProjectile(Projectile& shot)
{
    if (shot.mProjectileType == PROJECTILE_SPIKE)
    {
        Zombie* best = nullptr;
        float bestDistance = 1000000;
        Zombie* zombie = nullptr;
        while (mBoard->IterateZombies(zombie))
        {
            if (!zombie->EffectedByDamage(shot.mDamageRangeFlags)) continue;
            const auto rect = zombie->GetZombieRect();
            const float dx = rect.mX + rect.mWidth * 0.5f - shot.mPosX;
            const float dy = rect.mY + rect.mHeight * 0.5f - shot.mPosY;
            const float distance = dx * dx + dy * dy;
            if (distance < bestDistance)
            {
                best = zombie;
                bestDistance = distance;
            }
        }
        if (best)
        {
            const auto rect = best->GetZombieRect();
            const float dx = rect.mX + rect.mWidth * 0.5f - shot.mPosX;
            const float dy = rect.mY + rect.mHeight * 0.5f - shot.mPosY;
            const float distance = std::max(1.0f, std::sqrt(dx * dx + dy * dy));
            const float vx = shot.mVelX * 0.85f + dx / distance * HeavyProjectileSpeed * 0.15f;
            const float vy = shot.mVelY * 0.85f + dy / distance * HeavyProjectileSpeed * 0.15f;
            const float speed = std::max(0.01f, std::sqrt(vx * vx + vy * vy));
            shot.mVelX = vx * HeavyProjectileSpeed / speed;
            shot.mVelY = vy * HeavyProjectileSpeed / speed;
        }
        shot.mRotation = -std::atan2(shot.mVelY, shot.mVelX);
    }
    shot.mPosX += shot.mVelX;
    shot.mPosY += shot.mVelY;
    shot.mX = int(shot.mPosX);
    shot.mY = int(shot.mPosY);
    shot.mShadowY = shot.mPosY + 28;
    shot.mRow = mBoard->PixelToGridYKeepOnBoard(shot.mX, shot.mY + 35);
    if (shot.mPosX < -40 || shot.mPosX > 850 || shot.mPosY < -90 || shot.mPosY > 650)
    {
        shot.Die();
        return;
    }
    Zombie* zombie = nullptr;
    while (mBoard->IterateZombies(zombie))
    {
        if (!zombie->EffectedByDamage(shot.mDamageRangeFlags)) continue;
        if (shot.GetProjectileRect().Intersects(zombie->GetZombieRect()))
        {
            shot.DoImpact(zombie);
            return;
        }
    }
}

void HeavyWeapon::Draw(Graphics* g)
{
    if (!mBoard || mBoard->mApp->mGameScene == SCENE_LEVEL_INTRO) return;
    EnsureAnimations();
    auto* app = mBoard->mApp;
    if (auto* body = app->ReanimationTryToGet(mBody)) body->Draw(g);
    if (mTier >= 3)
    {
        if (auto* tail = app->ReanimationTryToGet(mTail)) tail->Draw(g);
    }
    if (auto* turret = app->ReanimationTryToGet(mTurret))
    {
        const float angle = std::atan2(mAimY, mAimX) + HeavyPi * 0.5f;
        SexyTransform2D rotation;
        rotation.Translate(-128, -58);
        rotation.RotateRad(-angle);
        rotation.Translate(MuzzleX(), MuzzleY());
        turret->mOverlayMatrix = rotation;
        turret->Draw(g);
    }
}

void HeavyWeapon::DrawBank(Graphics* g)
{
    if (!mBoard) return;
    const int width = 80 + 6 * 59 + 10;
    g->DrawImage(IMAGE_SEEDBANK, 0, 0, width, 87);
    TodDrawString(g, std::to_string(mBoard->mSunMoney), 38, 78, FONT_CONTINUUMBOLD14, Color::Black, DS_ALIGN_CENTER);
    for (int index = 0; index < 6; ++index)
    {
        const int x = 80 + index * 59;
        const bool available = CanBuy(index);
        const bool affordable = mBoard->mSunMoney >= HeavyCosts[index];
        const bool recharging = mPowerTicks > 0 &&
            ((index == 3 && mPower == 1) || (index == 4 && mPower == 2));
        const float recharge = recharging ? mPowerTicks / 700.0f : 0.0f;
        DrawSeedPacket(g, x, 9, HeavySeeds[index], SEED_NONE, recharge, 255, false, false);
        TodDrawString(g, std::to_string(HeavyCosts[index]), x + 26, 63 + FONT_PICO129->GetAscent(), FONT_PICO129, Color::Black, DS_ALIGN_CENTER);
        if ((!available && !recharging) || !affordable)
        {
            g->SetColor(Color(0, 0, 0, available ? 90 : 150));
            g->FillRect(x, 9, 50, 70);
        }
        if (index == mSelection)
        {
            g->SetColor(Color(255, 226, 55));
            g->DrawRect(x - 2, 7, 54, 74);
        }
        TodDrawString(g, std::to_string(index + 1), x + 26, 85, FONT_DWARVENTODCRAFT12, Color(255, 226, 55), DS_ALIGN_CENTER);
    }
    g->SetColor(Color::White);
}
