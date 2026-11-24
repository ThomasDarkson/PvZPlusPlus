#pragma once
#include "../ConstEnums.h"
#include <vector>
#include <string>
class Board;
class Zombie;
class SaveGameContext;
namespace Sexy { class Graphics; }
class ZombieTrap
{
public:
    void Init(Board* board);
    void Attach(Board* board) { mBoard = board; }
    void Start();
    void Update();
    void Draw(Sexy::Graphics* g);
    void DrawGround(Sexy::Graphics* g);
    void MouseDown(int x, int y);
    void MouseMove(int x, int y);
    void MouseUp() { mDragging = false; }
    bool UpdateZombie(Zombie& zombie);
    bool FacingRight(Zombie& zombie) const;
    void Sync(SaveGameContext& context);
    bool Intro() const { return mIntro; }
    int Score() const { return mScore; }
    int Brains() const { return mBrains; }
    int Cages() const { return mCages; }
    void PrepareZombie(Zombie& zombie);
    void SpawnBungee();
private:
    Board* mBoard = nullptr;
    int mBrains = 3, mCages = 7, mScore = 0, mBrainHealth = 300;
    int mTicks = 0, mSpawnTicks = 300, mBungeeTicks = 1800;
    int mCagePhase = 0, mCageTicks = 0, mCaught = 0, mBest = 0;
    float mBrainX = 400, mBrainY = 300, mBrainAltitude = 180;
    bool mDragging = false, mIntro = false, mDone = false, mDaveLeaving = false;
    int mCaw = -1, mFlap = -1;
    std::vector<ZombieID> mTrapped;
    struct Attraction { ZombieID id; int noticedAt; bool interested = true; };
    std::vector<Attraction> mAttracted;
    bool IsAttracted(ZombieID id) const;
    void DropCage();
    void FinishCapture();
    void PlayCrow(bool flap);
    void Fail();
    std::string RecordKey() const;
};
