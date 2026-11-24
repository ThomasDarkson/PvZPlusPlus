#pragma once

#include "../ConstEnums.h"
#include "../SexyAppFramework/KeyCodes.h"

class Board;
class Coin;
class Projectile;
class SaveGameContext;
struct SDL_Gamepad;
namespace Sexy { class Graphics; }

class HeavyWeapon
{
public:
    HeavyWeapon() = default;
    ~HeavyWeapon();
    void Attach(Board* board);
    void Init(Board* board);
    void Sync(SaveGameContext& context);
    void Start();
    void Update();
    void Draw(Sexy::Graphics* g);
    void DrawBank(Sexy::Graphics* g);
    void Aim(int x, int y);
    void MouseDown(int x, int y, int clicks);
    void MouseUp();
    int PacketAt(int x, int y) const;
    void UpdateToolTip(int x, int y);
    bool KeyDown(Sexy::KeyCode key);
    void UpdatePickup(Coin& coin);
    void UpdateProjectile(Projectile& projectile);

private:
    Board* mBoard = nullptr;
    SDL_Gamepad* mGamepad = nullptr;
    bool mOwnsGamepads = false;
    bool mMouseFiring = false;
    bool mPadAiming = false;
    bool mWasBuying = false;
    bool mWasPausing = false;
    int mPadSelection = 0;
    int mScanCountdown = 0;
    int mTier = 0;
    int mSelection = 0;
    int mFireCountdown = 0;
    int mPowerTicks = 0;
    int mPower = 0;
    int mSunCountdown = 500;
    int mBungeeCountdown = 1000;
    float mX = 400.0f;
    float mAimX = 0.0f;
    float mAimY = -1.0f;
    float mMouseX = 400.0f;
    float mMouseY = 300.0f;
    ReanimationID mBody = ReanimationID::REANIMATIONID_NULL;
    ReanimationID mTurret = ReanimationID::REANIMATIONID_NULL;
    ReanimationID mTail = ReanimationID::REANIMATIONID_NULL;
    int mTailFireCountdown = 0;

    float MuzzleX() const { return mX + 18.0f; }
    float MuzzleY() const { return 584.0f; }
    bool CanBuy(int selection) const;
    void Buy();
    void EnsureAnimations();
    void Fire();
    void FireTail();
    void UpdateFiring(bool firing);
    bool PollController(float& movement);
    void DropZombie();
};
