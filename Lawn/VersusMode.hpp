static void DrawVersusIris(Graphics& g, float irisX, float irisY, float radius)
{
    g.SetColor(Color::Black);
    const int top = ClampInt(int(irisY - radius), 0, Height);
    const int bottom = ClampInt(int(irisY + radius) + 1, 0, Height);
    if (top > 0) g.FillRect(Rect(0, 0, Width, top));
    if (bottom < Height) g.FillRect(Rect(0, bottom, Width, Height - bottom));
    for (int y = top; y < bottom; y += 3)
    {
        const float dy = y + 1.5f - irisY;
        const float half = std::sqrt(pvz_max(0.0f, radius * radius - dy * dy));
        const int left = ClampInt(int(irisX - half), 0, Width);
        const int right = ClampInt(int(irisX + half), 0, Width);
        const int height = pvz_min(3, bottom - y);
        if (left > 0) g.FillRect(Rect(0, y, left, height));
        if (right < Width) g.FillRect(Rect(right, y, Width - right, height));
    }
}

class VersusMode
{
    enum class Screen { Closed, Join, Confirm, Sides, Help, Draft, Match, Win, TrophyFlash, Result, CoopSelect, CoopPlay };
    struct Card
    {
        const char* label;
        int type;
        int cost;
        int recharge;
        int suddenRecharge;
    };
    struct Brain
    {
        int column;
        int row;
        int age;
        bool falling;
        bool collecting = false;
        float collectX = 0.0f;
        float collectY = 0.0f;
    };
    struct Pad { SDL_JoystickID id; SDL_Gamepad* handle; unsigned buttons; };

    static constexpr Card Plants[] = {
        {"Sunflower", SEED_SUNFLOWER, 50, 750, 0},
        {"Peashooter", SEED_PEASHOOTER, 100, 750, 250},
        {"Potato Mine", SEED_POTATOMINE, 25, 3000, 1000},
        {"Wall-nut", SEED_WALLNUT, 50, 3000, 3000},
        {"Jalapeno", SEED_JALAPENO, 125, 6000, 2000},
        {"Squash", SEED_SQUASH, 75, 3000, 1000},
        {"Cherry Bomb", SEED_CHERRYBOMB, 150, 6000, 2000},
        {"Snow Pea", SEED_SNOWPEA, 150, 750, 250},
        {"Repeater", SEED_REPEATER, 150, 750, 250},
        {"Threepeater", SEED_THREEPEATER, 200, 1500, 500},
        {"Spikeweed", SEED_SPIKEWEED, 100, 750, 250},
        {"Torchwood", SEED_TORCHWOOD, 125, 750, 250},
        {"Tall-nut", SEED_TALLNUT, 125, 3000, 3000},
        {"Pumpkin", SEED_PUMPKINSHELL, 125, 3000, 3000},
        {"Cabbage-pult", SEED_CABBAGEPULT, 100, 750, 250},
        {"Kernel-pult", SEED_KERNELPULT, 100, 750, 250},
        {"Melon-pult", SEED_MELONPULT, 300, 1500, 500},
        {"Grave Buster", SEED_GRAVEBUSTER, 75, 3000, 5000},
        {"Chomper", SEED_CHOMPER, 150, 750, 750},
        {"Puff-shroom", SEED_PUFFSHROOM, 0, 750, 250},
        {"Sun-shroom", SEED_SUNSHROOM, 25, 750, 0},
        {"Fume-shroom", SEED_FUMESHROOM, 75, 750, 250},
        {"Hypno-shroom", SEED_HYPNOSHROOM, 75, 3000, 1000},
        {"Scaredy-shroom", SEED_SCAREDYSHROOM, 25, 750, 250},
        {"Ice-shroom", SEED_ICESHROOM, 75, 6000, 2000},
        {"Doom-shroom", SEED_DOOMSHROOM, 125, 6000, 2000},
        {"Lily Pad", SEED_LILYPAD, 25, 750, 250},
        {"Tangle Kelp", SEED_TANGLEKELP, 25, 3000, 1000},
        {"Sea-shroom", SEED_SEASHROOM, 0, 3000, 1000},
        {"Plantern", SEED_PLANTERN, 25, 3000, 1000},
        {"Cactus", SEED_CACTUS, 100, 750, 250},
        {"Blover", SEED_BLOVER, 100, 750, 250},
        {"Split Pea", SEED_SPLITPEA, 125, 750, 250},
        {"Starfruit", SEED_STARFRUIT, 175, 1500, 500},
        {"Magnet-shroom", SEED_MAGNETSHROOM, 100, 750, 250},
        {"Flower Pot", SEED_FLOWERPOT, 25, 750, 250},
        {"Coffee Bean", SEED_INSTANT_COFFEE, 25, 750, 250},
        {"Garlic", SEED_GARLIC, 75, 750, 250},
        {"Umbrella Leaf", SEED_UMBRELLA, 100, 750, 250},
        {"Marigold", SEED_MARIGOLD, 50, 3000, 1000},
    };
    static constexpr Card Zombies[] = {
        {"Gravestone", ZOMBIE_INVALID, 50, 750, 0},
        {"Zombie", ZOMBIE_NORMAL, 25, 750, 250},
        {"Trash Can", ZOMBIE_DOOR, 50, 3000, 3000},
        {"Conehead", ZOMBIE_TRAFFIC_CONE, 75, 3000, 1000},
        {"Football", ZOMBIE_FOOTBALL, 150, 3000, 1000},
        {"Flag Zombie", ZOMBIE_FLAG, 300, 3000, 1000},
        {"Pole Vaulter", ZOMBIE_POLEVAULTER, 100, 3000, 1000},
        {"Buckethead", ZOMBIE_PAIL, 100, 3000, 1000},
        {"Newspaper", ZOMBIE_NEWSPAPER, 50, 1500, 500},
        {"Screen Door", ZOMBIE_DOOR, 100, 1500, 1500},
        {"Dancer", ZOMBIE_DANCER, 150, 3000, 1000},
        {"Zomboni", ZOMBIE_ZAMBONI, 175, 6000, 2000},
        {"Jack-in-the-Box", ZOMBIE_JACK_IN_THE_BOX, 100, 3000, 1000},
        {"Digger", ZOMBIE_DIGGER, 150, 3000, 1000},
        {"Pogo", ZOMBIE_POGO, 225, 6000, 2000},
        {"Ladder", ZOMBIE_LADDER, 150, 3000, 1000},
        {"Catapult", ZOMBIE_CATAPULT, 200, 6000, 2000},
        {"Gargantuar", ZOMBIE_GARGANTUAR, 250, 6000, 2000},
    };
    static constexpr int PlantCardCount = sizeof(Plants) / sizeof(Plants[0]);
    static constexpr int ZombieCardCount = sizeof(Zombies) / sizeof(Zombies[0]);
    static_assert(PlantCardCount == SEED_MELONPULT + 1);
    static constexpr int Rows = 5;
    static constexpr int PlantColumns = Board::VersusPlantColumns;
    static constexpr int LastColumn = 8;
    static constexpr int SuddenDeathTick = 30000;
    static constexpr int BankTop = 23;
    static constexpr int CardY = BankTop + 4;
    static constexpr int DraftTopShift = 33;
    static constexpr int CardWidth = 53;
    static constexpr int PlantBank = 312;
    static constexpr int ZombieBank = 653;
    static constexpr int IrisTicks = 115;
    static constexpr int TrophyDropTicks = 54;
    static constexpr int LosingCueTicks = 507;
    static constexpr int WinningCueTicks = 445;
    static constexpr int BrainBankX = 1008;
    static constexpr int BrainBankY = BankTop + 34;

    LawnApp& app;
    Screen screen = Screen::Closed;
    Screen resume = Screen::Closed;
    std::vector<Pad> pads;
    SDL_JoystickID playerTwo = 0;
    int nextScan = 0;
    bool ownsGamepads = false;
    bool escapeHeld = false;
    bool helpSeen = false;
    SharedImageRef helpNote, helpControllers, helpTargets, helpBrains;
    bool playStationPad = true;
    bool p1Plants = true;
    int option = 0;
    int draftTurn = 0;
    int draftCursor = 1;
    int mouseDraftCursor = 1;
    static constexpr int DraftFlightTicks = 25;
    int draftFlight = 0;
    int flyingCard = 0;
    ReanimationID readyAnimation = REANIMATIONID_NULL;
    float controllerX = Width / 2.0f;
    float controllerY = Height / 2.0f;
    int controllerAxisX = 0;
    int controllerAxisY = 0;
    int selected[2] = {1, 1};
    int recharge[2][6]{};
    int brains = 75;
    int time = 0;
    int defeated = 0;
    bool plantsWon = false;
    int pointerX = -1, pointerY = -1;
    int winTicks = 0, winHoldTicks = 0, irisX = Width / 2, irisY = Height / 2;
    int resultOption = 0;
    int trophyFlashTicks = 0;
    int lossSequenceTicks = 0;
    std::array<std::vector<int>, 2> winHistory;
    std::array<ZombieID, Rows> targets{};
    std::array<std::array<ZombieID, Rows>, 3> graves{};
    std::array<std::array<int, 6>, 2> decks{};
    std::vector<Brain> drops;
    SharedImageRef setupBack, plantBank, zombieBank, brainImage, brainGlowBig, brainGlowSmall, graveIcon;
    SharedImageRef buttonImage, buttonSelected, draftPanels[2], playerTwoGamepad, sidePortrait[2], sideSelected[2], sideLabel[2];
    SharedImageRef seedCursor[2], boardCursor[2], playerLabel[2], playerArrow[2];
    SharedImageRef zombiePacket, targetCounter, redCross, resultBackground, winLabel, zombieHead, gamerpic;
    SharedImageRef resultPanel[2], trophy[2], trophyBase, helpButtons, helpButtonsSmall, xboxButtons, xboxButtonsSmall;
    SharedImageRef damageImages[3];

    template<class Function>
    static Function Sdl(const char* name)
    {
        return reinterpret_cast<Function>(OpenPVZImports::resolve("SDL3.dll", name));
    }

    static unsigned Button(SDL_GamepadButton button) { return 1u << static_cast<unsigned>(button); }
    bool SuddenDeath() const { return time >= SuddenDeathTick; }
    int MouseSide() const { return p1Plants ? 0 : 1; }
    int ControllerSide() const { return 1 - MouseSide(); }
    int DraftSide() const { return draftTurn % 2; }
    static const Card& GetCard(int side, int id) { return side == 0 ? Plants[id] : Zombies[id]; }
    int GridScreenX(int x, int y) const { return app.mBoard->mX + app.mDDInterface->mWideScreenOffsetX + app.mBoard->GridToPixelX(x, y); }
    int GridScreenY(int x, int y) const { return app.mBoard->mY + app.mDDInterface->mWideScreenOffsetY + app.mBoard->GridToPixelY(x, y); }

    SharedImageRef ImageAsset(const char* name)
    {
        return app.GetSharedImage(std::string("../versus/") + name);
    }

    struct CoopLevel
    {
        const char* name;
        GameMode mode;
        int thumbnail;
    };

    static constexpr CoopLevel CoopLevels[] = {
        {"Co-op Day", GAMEMODE_SURVIVAL_NORMAL_STAGE_1, 0},
        {"Co-op Night", GAMEMODE_SURVIVAL_NORMAL_STAGE_2, 1},
        {"Co-op Pool", GAMEMODE_SURVIVAL_NORMAL_STAGE_3, 2},
        {"Co-op Roof", GAMEMODE_SURVIVAL_NORMAL_STAGE_5, 4},
        {"Co-op Bowling", GAMEMODE_CHALLENGE_WALLNUT_BOWLING, -7},
        {"Co-op Hard Day", GAMEMODE_SURVIVAL_HARD_STAGE_1, 5},
        {"Co-op Hard Night", GAMEMODE_SURVIVAL_HARD_STAGE_2, 6},
        {"Co-op Hard Pool", GAMEMODE_SURVIVAL_HARD_STAGE_3, 7},
        {"Co-op Hard Roof", GAMEMODE_SURVIVAL_HARD_STAGE_5, 9},
        {"Co-op Zomboss", GAMEMODE_CHALLENGE_FINAL_BOSS, -20},
        {"Co-op Endless", GAMEMODE_SURVIVAL_ENDLESS_STAGE_3, 10},
    };

    static constexpr int CoopLevelCount = sizeof(CoopLevels) / sizeof(CoopLevels[0]);
    bool coop = false;
    bool dropIn = false;
    bool triedDropInGamepads = false;
    int coopLevel = 0;
    int coopPacket = 4;
    int coopButterHold = 0;
    ZombieID coopButterTarget = ZombieID::ZOMBIEID_NULL;
    bool coopButterSpent = false;
    int versusStreak[2]{};
    int concedingPlayer = -1;
    bool concedeMouseDown = false;
    SharedImageRef xboxZombieSelected, xboxGamepad;
    bool xboxPad = false;
    bool coopShovelSelected = false;
    int coopSunHint = -1;
    bool coopCompletion[CoopLevelCount]{};
    SharedImageRef coopTrophy, coopSunCore;
    SharedImageRef coopSurvivalThumbs, coopChallengeThumbs, coopDoubleSun, coopDoubleSunGlow;

    static constexpr int CoopColumns = 5;
    static Rect CoopTile(int index)
    {
        return Rect((Width - 800) / 2 + 38 + index % CoopColumns * 155,
            (Height - 600) / 2 + 93 + index / CoopColumns * 119, 104, 115);
    }
    static Rect CoopBackRect() { return Rect(COOP_MENU_MARGIN, COOP_MENU_MARGIN, 111, 26); }

    void SelectCoopLevel(int index)
    {
        coopLevel = ClampInt(index, 0, CoopLevelCount - 1);
    }

    void StartCoopLevel()
    {
        if (!playerTwo) return;
        app.mLocalCoop = true;
        app.mCoopHasPartner = true;
        app.mCoopLevelIndex = coopLevel;
        coopPacket = 4;
        coopButterHold = 0;
        coopButterTarget = ZombieID::ZOMBIEID_NULL;
        coopButterSpent = false;
        coopShovelSelected = false;
        coopSunHint = -1;
        controllerX = Width * 0.6f;
        controllerY = Height * 0.52f;
        screen = Screen::CoopPlay;
        app.KillGameSelector();
        app.PreNewGame(CoopLevels[coopLevel].mode, false);
    }

    static bool CoopCoinHit(Coin& coin, int x, int y)
    {
        HitResult hit;
        return coin.MouseHitTest(x, y, &hit);
    }

    Point CoopBoardPoint(int x, int y) const
    {
        return Point(x - app.mBoard->mX - app.mDDInterface->mWideScreenOffsetX,
            y - app.mBoard->mY - app.mDDInterface->mWideScreenOffsetY);
    }

    bool CoopPointOnLawn(Board& board, Point point)
    {
        const int column = board.PixelToGridX(point.mX, point.mY);
        const int row = board.PixelToGridY(point.mX, point.mY);
        const int rows = board.StageHas6Rows() ? 6 : 5;
        if (column < 0 || row < 0 || row >= rows || column >= MAX_GRID_SIZE_X) return false;
        const int cellHeight = board.StageHasRoof() || rows == 6 ? 85 : 100;
        return Rect(board.GridToPixelX(column, row), board.GridToPixelY(column, row), 80, cellHeight)
            .Contains(point.mX, point.mY);
    }

    void UpdateCoop()
    {
        Board* board = app.mBoard;
        if (!board || board->mPaused || app.mGameScene != SCENE_PLAYING || app.GetDialogCount()) return;
        if (coopSunHint > 0) --coopSunHint;
        const int slots = board->HasConveyorBeltSeedBank() ? 6 : 4;
        coopPacket = ClampInt(coopPacket, slots, slots * 2 - 1);
        const auto mouse = CoopBoardPoint(pointerX, pointerY);
        const auto pad = CoopBoardPoint(int(controllerX), int(controllerY));
        Coin* coin = nullptr;
        while (board->IterateCoins(coin))
        {
            const auto entry = app.mCoopSuns.find(coin);
            if (entry == app.mCoopSuns.end() || entry->second != LawnApp::CoopSun::DoubleWaiting) continue;
            if (coopSunHint < 0) coopSunHint = 300;
            if (CoopCoinHit(*coin, mouse.mX, mouse.mY) && CoopCoinHit(*coin, pad.mX, pad.mY))
            {
                entry->second = LawnApp::CoopSun::DoubleCollected;
                coin->PlayCollectSound();
                coin->Collect();
            }
        }
    }

    void UpdateCoopButter(bool held)
    {
        if (!held)
        {
            coopButterSpent = false;
            coopButterHold = 0;
            coopButterTarget = ZombieID::ZOMBIEID_NULL;
            return;
        }

        auto* board = app.mBoard;
        if (coopButterSpent || !board || board->mPaused || board->mTimeStopCounter > 0 ||
            app.GetDialogCount() || app.mGameScene != SCENE_PLAYING)
        {
            coopButterHold = 0;
            return;
        }
        const auto position = CoopBoardPoint(int(controllerX), int(controllerY));
        auto* zombie = board->ZombieHitTest(position.mX, position.mY);
        if (!zombie || zombie->IsDeadOrDying())
        {
            coopButterTarget = ZombieID::ZOMBIEID_NULL;
            coopButterHold = 0;
            return;
        }
        const ZombieID target = board->ZombieGetID(zombie);
        if (target != coopButterTarget)
        {
            coopButterTarget = target;
            coopButterHold = 0;
        }
        if (++coopButterHold >= 100)
        {
            app.PlayFoley(FOLEY_BUTTER);
            zombie->ApplyButter();
            coopButterSpent = true;
            coopButterHold = 0;
        }
    }

    void CoopController(unsigned pressed)
    {
        const bool accept = pressed & Button(SDL_GAMEPAD_BUTTON_SOUTH);
        const bool back = pressed & Button(SDL_GAMEPAD_BUTTON_EAST);
        if (screen == Screen::CoopSelect)
        {
            if (back) Close();
            else if (accept) StartCoopLevel();
            else if (pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_UP)) SelectCoopLevel(coopLevel - CoopColumns);
            else if (pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_DOWN)) SelectCoopLevel(coopLevel + CoopColumns);
            else if (pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_LEFT)) SelectCoopLevel(coopLevel - 1);
            else if (pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) SelectCoopLevel(coopLevel + 1);
            return;
        }

        Board* board = app.mBoard;
        if (!board) return;
        const bool coopPlaying = app.mLocalCoop && app.mGameScene == SCENE_PLAYING;
        const bool pause = pressed & Button(SDL_GAMEPAD_BUTTON_START);
        if (pause || (back && (!coopPlaying || app.GetDialogCount())))
        {
            if (app.GetDialog(DIALOG_NEWOPTIONS)) app.KillNewOptionsDialog();
            else if (app.GetDialog(DIALOG_PAUSED)) app.KillDialog(DIALOG_PAUSED);
            else if (!app.GetDialogCount()) app.DoNewOptions(false);
            return;
        }
        if (app.GetDialogCount()) return;
        if (auto* chooser = app.mSeedChooserScreen; chooser && app.mGameScene == SCENE_LEVEL_INTRO && board->mCutScene->mSeedChoosing)
        {
            if (!accept || chooser->mChooseState != CHOOSE_NORMAL || chooser->mSeedsInFlight) return;
            if (chooser->mSeedsInBank == board->mSeedBank->mNumPackets)
            {
                chooser->OnStartButton();
                return;
            }
            const auto origin = chooser->GetAbsPos();
            const auto seed = chooser->SeedHitTest(int(controllerX) - origin.mX, int(controllerY) - origin.mY);
            if (seed == SEED_NONE || seed == SEED_IMITATER || chooser->SeedNotAllowedToPick(seed) ||
                chooser->SeedNotAllowedDuringTrial(seed)) return;
            auto& chosen = chooser->mChosenSeeds[seed];
            app.mCoopControllerInput = true;
            if (chosen.mSeedState == SEED_IN_BANK) chooser->ClickedSeedInBank(chosen);
            else if (chosen.mSeedState == SEED_IN_CHOOSER) chooser->ClickedSeedInChooser(chosen);
            app.mCoopControllerInput = false;
            return;
        }
        if (board->mPaused || board->mTimeStopCounter > 0 || app.mGameScene != SCENE_PLAYING) return;

        const int firstPacket = app.mLocalCoop ? (board->HasConveyorBeltSeedBank() ? 6 : 4) : 0;
        const int count = board->mSeedBank->mNumPackets - firstPacket;
        const int next = int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER))) -
            int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)));
        if (count > 0)
            coopPacket = firstPacket + (ClampInt(coopPacket - firstPacket, 0, count - 1) + count + next) % count;

        if (coopPlaying && back)
        {
            coopShovelSelected = !coopShovelSelected;
            return;
        }
        if (next) coopShovelSelected = false;

        const auto pos = CoopBoardPoint(int(controllerX), int(controllerY));
        if (!accept) return;

        if (coopPlaying && coopShovelSelected)
        {
            if (CoopPointOnLawn(*board, pos))
            {
                CursorObject cursor;
                CursorObject* mouseCursor = board->mCursorObject;
                cursor.mCursorType = CURSOR_TYPE_SHOVEL;
                board->mCursorObject = &cursor;
                board->MouseDownWithTool(pos.mX, pos.mY, 1, CURSOR_TYPE_SHOVEL);
                board->mCursorObject = mouseCursor;
                coopShovelSelected = false;
            }
            return;
        }

        app.mCoopControllerInput = true;
        Coin* coin = nullptr;
        while (board->IterateCoins(coin))
        {
            if (!CoopCoinHit(*coin, pos.mX, pos.mY) || (app.mLocalCoop && !coin->IsSun())) continue;
            if (app.mCoopSuns.find(coin) == app.mCoopSuns.end()) coin->MouseDown(pos.mX, pos.mY, 1);
            app.mCoopControllerInput = false;
            return;
        }

        CursorObject cursor;
        CursorObject* mouseCursor = board->mCursorObject;
        board->mCursorObject = &cursor;
        if (!app.mLocalCoop)
        {
            HitResult hit{};
            board->MouseHitTest(pos.mX, pos.mY, &hit);
            if (board->mChallenge->MouseDown(pos.mX, pos.mY, 1, &hit))
            {
                board->mCursorObject = mouseCursor;
                app.mCoopControllerInput = false;
                return;
            }
        }
        if (CoopPointOnLawn(*board, pos))
        {
            if (count > 0 && (mouseCursor->mCursorType != CURSOR_TYPE_PLANT_FROM_BANK || mouseCursor->mSeedBankIndex != coopPacket))
                board->mSeedBank->mSeedPackets[coopPacket].MouseDown(pos.mX, pos.mY, 1);
            if (board->IsPlantInCursor())
                board->MouseDownWithPlant(pos.mX, pos.mY, 1);
            else if (!app.mLocalCoop && app.IsWhackAZombieLevel())
            {
                cursor.mCursorType = CURSOR_TYPE_HAMMER;
                board->MouseDown(int(controllerX) - board->mX, int(controllerY) - board->mY, 1);
            }
        }
        else if (count > 0)
        {
            for (int i = firstPacket; i < board->mSeedBank->mNumPackets; ++i)
            {
                const auto& packet = board->mSeedBank->mSeedPackets[i];
                if (Rect(board->mSeedBank->mX + packet.mX + packet.mOffsetX,
                    board->mSeedBank->mY + packet.mY, packet.mWidth, packet.mHeight).Contains(pos.mX, pos.mY))
                {
                    coopPacket = i;
                    break;
                }
            }
        }
        if (cursor.mCursorType == CURSOR_TYPE_PLANT_FROM_BANK)
            board->RefreshSeedPacketFromCursor();
        board->mCursorObject = mouseCursor;
        app.mCoopControllerInput = false;
    }

    void DrawCoopMenu(Graphics& g)
    {
        ImageAt(g, Sexy::IMAGE_CHALLENGE_BACKGROUND, Rect(0, 0, Width, Height));
        Label(g, "Co-op", Width / 2, 70, Sexy::FONT_HOUSEOFTERROR28, Color(220, 220, 220));
        const int completed = int(std::count(coopCompletion, coopCompletion + CoopLevelCount - 1, true));
        ImageAt(g, coopTrophy, Rect(960, 34, 32, 32));
        Label(g, (std::to_string(completed) + "/10").c_str(), 976, 88,
            Sexy::FONT_DWARVENTODCRAFT15, Color(255, 240, 0));

        for (int index = 0; index < CoopLevelCount; ++index)
        {
            const Rect tile = CoopTile(index);
            const bool selected = index == coopLevel;
            const auto& level = CoopLevels[index];
            Image* sheet = level.thumbnail >= 0 ? (Image*)coopSurvivalThumbs : (Image*)coopChallengeThumbs;
            const int thumbnail = level.thumbnail >= 0 ? level.thumbnail : -level.thumbnail - 1;
            if (sheet)
                g.DrawImage(sheet, Rect(tile.mX + 13, tile.mY + 4, 78, 71),
                    Rect(thumbnail * 185, 0, 185, sheet->GetHeight()));
            ImageAt(g, selected ? Sexy::IMAGE_CHALLENGE_WINDOW_HIGHLIGHT : Sexy::IMAGE_CHALLENGE_WINDOW,
                Rect(tile.mX - 6, tile.mY - 2, 116, 116));
            if (coopCompletion[index])
                ImageAt(g, coopTrophy, Rect(tile.mX, tile.mY + 4, 28, 26));
            Label(g, level.name, tile.mX + 52, tile.mY + 96, Sexy::FONT_BRIANNETOD12,
                selected ? Color(250, 40, 40) : Color(42, 42, 90));
        }
        const Rect back = CoopBackRect();
        ImageAt(g, Sexy::IMAGE_SEEDCHOOSER_BUTTON2, back);
        Label(g, "Main Menu", back.mX + back.mWidth / 2, back.mY + 19, Sexy::FONT_BRIANNETOD16, Color(42, 42, 90));
    }

    void DrawCoopPlay(Graphics& g)
    {
        Board* board = app.mBoard;
        if (!board || app.GetDialogCount() || board->mPaused) return;
        const int x = int(controllerX), y = int(controllerY);
        if (auto* chooser = app.mSeedChooserScreen; chooser && board->mCutScene->mSeedChoosing)
        {
            const auto origin = chooser->GetAbsPos();
            const auto seed = chooser->SeedHitTest(x - origin.mX, y - origin.mY);
            if (seed != SEED_NONE)
            {
                const auto& card = chooser->mChosenSeeds[seed];
                DrawCursor(g, Rect(origin.mX + int(card.mX) - 4, origin.mY + int(card.mY) - 4, 57, 76), 1, false);
            }
            else DrawCursor(g, Rect(x - 20, y - 23, 40, 46), 1, true);
            const auto mouseSeed = chooser->SeedHitTest(pointerX - origin.mX, pointerY - origin.mY);
            if (mouseSeed != SEED_NONE && mouseSeed != seed)
            {
                const auto& card = chooser->mChosenSeeds[mouseSeed];
                DrawCursor(g, Rect(origin.mX + int(card.mX) - 4, origin.mY + int(card.mY) - 4, 57, 76), 0, false);
            }
            if (app.mLocalCoop) Label(g, chooser->mSeedsInBank < 4 ? "Player 1: Pick four plants" :
                chooser->mSeedsInBank < 8 ? "Player 2: Pick four plants" : "Both players ready",
                890, 654, Sexy::FONT_HOUSEOFTERROR20);
            return;
        }
        if (app.mGameScene != SCENE_PLAYING) return;
        const auto mouse = CoopBoardPoint(pointerX, pointerY);
        const auto pad = CoopBoardPoint(x, y);
        const int offsetX = x - pad.mX, offsetY = y - pad.mY;
        if (app.mLocalCoop)
        {
            const Rect shovel = board->GetShovelButtonRect();
            DrawPadButton(g, 1, offsetX + shovel.mX + shovel.mWidth - 30,
                offsetY + shovel.mY + shovel.mHeight - 27, false, playStationPad, 32);
            const Rect butter = board->GetCoopButterButtonRect();
            DrawPadButton(g, 2, offsetX + butter.mX + butter.mWidth - 30,
                offsetY + butter.mY + butter.mHeight - 27, false, playStationPad, 32);
        }
        Coin* coin = nullptr;
        while (board->IterateCoins(coin))
        {
            const auto entry = app.mCoopSuns.find(coin);
            if (entry == app.mCoopSuns.end() || entry->second != LawnApp::CoopSun::DoubleWaiting) continue;
            const bool hover = CoopCoinHit(*coin, pad.mX, pad.mY) || CoopCoinHit(*coin, mouse.mX, mouse.mY);
            const int centerX = offsetX + int(coin->mPosX) + 30;
            const int centerY = offsetY + int(coin->mPosY) + 30;
            ImageAt(g, hover ? (Image*)coopDoubleSunGlow : (Image*)coopDoubleSun,
                Rect(centerX - 48, centerY - 48, 96, 96));
            ImageAt(g, coopSunCore, Rect(centerX - 25, centerY - 16, 27, 27));
            ImageAt(g, coopSunCore, Rect(centerX - 1, centerY - 10, 27, 27));
        }
        const int firstPacket = app.mLocalCoop ? (board->HasConveyorBeltSeedBank() ? 6 : 4) : 0;
        if (board->mSeedBank->mNumPackets > firstPacket)
        {
            const auto& packet = board->mSeedBank->mSeedPackets[ClampInt(coopPacket, firstPacket, board->mSeedBank->mNumPackets - 1)];
            DrawCursor(g, Rect(offsetX + board->mSeedBank->mX + packet.mX + packet.mOffsetX - 5,
                offsetY + board->mSeedBank->mY + packet.mY - 4, 59, 78), 1, false);
            if (!coopShovelSelected && packet.mPacketType != SEED_NONE)
            {
                g.PushState();
                g.SetColorizeImages(true);
                g.SetColor(Color(255, 255, 255, 135));
                Plant::DrawSeedType(&g, packet.mPacketType, packet.mImitaterType, VARIATION_NORMAL, x - 30, y - 28);
                g.PopState();
            }
        }
        if (app.mLocalCoop && coopShovelSelected)
            ImageAt(g, Sexy::IMAGE_SHOVEL_HI_RES, Rect(x - 28, y - 32, 56, 56));
        DrawCursor(g, Rect(x - 38, y - 48, 76, 96), 1, true);
        if (CoopPointOnLawn(*board, mouse))
            DrawCursor(g, Rect(pointerX - 38, pointerY - 48, 76, 96), 0, true);
        if (coopButterHold > 0)
        {
            g.SetColor(Color(35, 25, 20));
            g.FillRect(Rect(x - 24, y + 48, 48, 6));
            g.SetColor(Color(255, 225, 105));
            g.FillRect(Rect(x - 24, y + 48, 48 * coopButterHold / 100, 6));
        }
        if (coopSunHint > 0)
        {
            g.SetColor(Color(0, 0, 0, 135));
            g.FillRect(Rect(0, 379, Width, 60));
            Label(g, "Move both cursors over double sun to pick it up!", 640, 421, Sexy::FONT_HOUSEOFTERROR20);
        }
    }

    void ScanPads()
    {
        static auto connected = Sdl<decltype(&SDL_GamepadConnected)>("SDL_GamepadConnected");
        static auto close = Sdl<decltype(&SDL_CloseGamepad)>("SDL_CloseGamepad");
        static auto list = Sdl<decltype(&SDL_GetGamepads)>("SDL_GetGamepads");
        static auto open = Sdl<decltype(&SDL_OpenGamepad)>("SDL_OpenGamepad");
        static auto freeList = Sdl<decltype(&SDL_free)>("SDL_free");
        for (auto it = pads.begin(); it != pads.end();)
        {
            if (connected(it->handle)) { ++it; continue; }
            if (playerTwo == it->id)
            {
                playerTwo = 0;
                if (screen == Screen::Closed)
                    dropIn = false;
                else if (coop && screen == Screen::CoopPlay)
                {
                    resume = screen;
                    app.mCoopHasPartner = false;
                    UpdateCoopButter(false);
                    if (app.mBoard) app.mBoard->mPaused = true;
                    screen = Screen::Join;
                }
                else if (coop && screen == Screen::CoopSelect)
                {
                    resume = screen;
                    screen = Screen::Join;
                }
                else if (screen != Screen::Result && screen != Screen::TrophyFlash && (screen != Screen::Win || !MouseWon()))
                {
                    if (screen != Screen::Confirm)
                        resume = screen == Screen::Match || screen == Screen::Draft ||
                            screen == Screen::Sides || screen == Screen::Help || screen == Screen::Win ? screen : Screen::Closed;
                    if (resume == Screen::Match || resume == Screen::Win) app.mBoard->mPaused = true;
                    screen = Screen::Join;
                }
            }
            close(it->handle);
            it = pads.erase(it);
        }
        int count = 0;
        SDL_JoystickID* ids = list(&count);
        for (int i = 0; ids && i < count; ++i)
        {
            const auto id = ids[i];
            if (std::none_of(pads.begin(), pads.end(), [id](const Pad& pad) { return pad.id == id; }))
                if (auto* handle = open(id)) pads.push_back({id, handle, 0});
        }
        freeList(ids);
    }

    void PollPads()
    {
        static auto update = Sdl<decltype(&SDL_UpdateGamepads)>("SDL_UpdateGamepads");
        static auto button = Sdl<decltype(&SDL_GetGamepadButton)>("SDL_GetGamepadButton");
        static auto axis = Sdl<decltype(&SDL_GetGamepadAxis)>("SDL_GetGamepadAxis");
        update();
        if (--nextScan <= 0)
        {
            ScanPads();
            nextScan = 25;
        }
        for (auto& pad : pads)
        {
            unsigned buttons = 0;
            for (int i = 0; i <= SDL_GAMEPAD_BUTTON_DPAD_RIGHT; ++i)
                if (button(pad.handle, static_cast<SDL_GamepadButton>(i))) buttons |= 1u << i;
            unsigned pressed = buttons & ~pad.buttons;
            pad.buttons = buttons;
            if (screen == Screen::Closed && !playerTwo)
            {
                if ((pressed & Button(SDL_GAMEPAD_BUTTON_SOUTH)) && !app.GetDialogCount() && !app.mBoard->mPaused)
                {
                    playerTwo = pad.id;
                    dropIn = true;
                    coopPacket = 0;
                    controllerX = Width * 0.6f;
                    controllerY = Height * 0.52f;
                    LoadCoopCursors();
                    return;
                }
                continue;
            }
            if (screen == Screen::Join && (pressed & Button(SDL_GAMEPAD_BUTTON_EAST)))
            {
                Close();
                return;
            }
            if (screen == Screen::Join && (pressed & Button(SDL_GAMEPAD_BUTTON_SOUTH)))
            {
                playerTwo = pad.id;
                static auto getType = Sdl<decltype(&SDL_GetGamepadType)>("SDL_GetGamepadType");
                const auto type = getType(pad.handle);
                playStationPad = type == SDL_GAMEPAD_TYPE_PS3 || type == SDL_GAMEPAD_TYPE_PS4 || type == SDL_GAMEPAD_TYPE_PS5;
                xboxPad = type == SDL_GAMEPAD_TYPE_XBOX360 || type == SDL_GAMEPAD_TYPE_XBOXONE;
                controllerAxisX = controllerAxisY = 0;
                screen = Screen::Confirm;
                return;
            }
            if (pad.id != playerTwo) continue;
            const int analogX = axis(pad.handle, SDL_GAMEPAD_AXIS_LEFTX);
            const int analogY = axis(pad.handle, SDL_GAMEPAD_AXIS_LEFTY);
            if ((screen == Screen::Match && !PauseDialogOpen()) ||
                ((screen == Screen::CoopPlay || dropIn) && !app.GetDialogCount()))
                MoveController(analogX, analogY, buttons);
            const int newX = analogX > 16000 ? 1 : analogX < -16000 ? -1 : 0;
            const int newY = analogY > 16000 ? 1 : analogY < -16000 ? -1 : 0;
            if (newX && newX != controllerAxisX)
                pressed |= Button(newX > 0 ? SDL_GAMEPAD_BUTTON_DPAD_RIGHT : SDL_GAMEPAD_BUTTON_DPAD_LEFT);
            if (newY && newY != controllerAxisY)
                pressed |= Button(newY > 0 ? SDL_GAMEPAD_BUTTON_DPAD_DOWN : SDL_GAMEPAD_BUTTON_DPAD_UP);
            controllerAxisX = newX;
            controllerAxisY = newY;
            if (screen == Screen::CoopPlay) UpdateCoopButter(buttons & Button(SDL_GAMEPAD_BUTTON_WEST));
            if (screen == Screen::Closed && dropIn) CoopController(pressed);
            else Controller(pressed);
            return;
        }
    }

    void MoveController(int analogX, int analogY, unsigned held)
    {
        float dx = analogX / 32768.0f;
        float dy = analogY / 32768.0f;
        const float magnitude = std::sqrt(dx * dx + dy * dy);
        if (magnitude > 0.17f)
        {
            const float scale = (pvz_min(magnitude, 1.0f) - 0.17f) / (0.83f * magnitude);
            dx *= scale;
            dy *= scale;
        }
        else dx = dy = 0.0f;
        const int dpadX = int(bool(held & Button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT))) -
            int(bool(held & Button(SDL_GAMEPAD_BUTTON_DPAD_LEFT)));
        const int dpadY = int(bool(held & Button(SDL_GAMEPAD_BUTTON_DPAD_DOWN))) -
            int(bool(held & Button(SDL_GAMEPAD_BUTTON_DPAD_UP)));
        if (dpadX || dpadY)
        {
            const float factor = dpadX && dpadY ? 0.7071068f : 1.0f;
            dx = dpadX * factor;
            dy = dpadY * factor;
        }
        controllerX = std::clamp(controllerX + 5.4f * dx, 16.0f, Width - 16.0f);
        controllerY = std::clamp(controllerY + 5.4f * dy, 23.0f, Height - 16.0f);
    }

    bool TileAt(int x, int y, int& col, int& lane) const
    {
        const int localX = x - GridScreenX(0, 0);
        const int localY = y - GridScreenY(0, 0);
        if (localX < 0 || localX >= (LastColumn + 1) * 80 || localY < 0 || localY >= Rows * 100) return false;
        col = localX / 80;
        lane = localY / 100;
        return true;
    }

    void Controller(unsigned pressed)
    {
        const bool accept = pressed & Button(SDL_GAMEPAD_BUTTON_SOUTH);
        const bool back = pressed & Button(SDL_GAMEPAD_BUTTON_EAST);
        const int dx = int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT))) -
            int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_LEFT)));
        const int dy = int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_DOWN))) -
            int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_DPAD_UP)));
        if (screen == Screen::Confirm && accept)
        {
            screen = resume == Screen::Closed ? (coop ? Screen::CoopSelect : Screen::Sides) : resume;
            if (screen == Screen::CoopPlay && app.mBoard)
            {
                app.mCoopHasPartner = true;
                app.mBoard->mPaused = app.GetDialogCount() > 0;
            }
            if (screen == Screen::Match)
                app.mBoard->mPaused = readyAnimation != REANIMATIONID_NULL || PauseDialogOpen();
            else if (screen == Screen::Win)
                app.mBoard->mPaused = winTicks >= IrisTicks;
            resume = Screen::Closed;
            if (screen == Screen::Sides && !helpSeen) OpenHelp();
        }
        else if (screen == Screen::CoopSelect || screen == Screen::CoopPlay)
        {
            CoopController(pressed);
        }
        else if (screen == Screen::Help)
        {
            if (accept || back) screen = Screen::Sides;
            return;
        }
        else if (screen == Screen::Sides)
        {
            option = ClampInt(option + dx, 0, 3);
            if (accept)
            {
                if (option == 3) OpenHelp();
                else SelectBattle(option);
            }
        }
        else if (screen == Screen::Draft)
        {
            const int side = ControllerSide();
            const int slot = ClampInt(DraftSlot(side, draftCursor) + dx + dy * DraftColumns(side), 0, CardCount(side) - 1);
            draftCursor = CardAtDraftSlot(side, slot);
            if (accept && !draftFlight && DraftSide() == ControllerSide()) PickCard(draftCursor);
        }
        else if (screen == Screen::Match)
        {
            if (PauseDialogOpen() && app.GetDialog(DIALOG_NEWOPTIONS))
            {
                if (back && concedingPlayer >= 0) { concedingPlayer = -1; return; }
                if (concedingPlayer == 1 && accept) { ConcedeMatch(1); return; }
                if (concedingPlayer < 0 && (pressed & Button(SDL_GAMEPAD_BUTTON_NORTH)))
                {
                    concedingPlayer = 1;
                    return;
                }
                if (concedingPlayer >= 0 && accept) return;
            }
            const bool pause = back || (pressed & Button(SDL_GAMEPAD_BUTTON_START));
            if (pause || (accept && PauseDialogOpen()))
            {
                TogglePause();
                return;
            }
            if (PauseDialogOpen()) return;
            const int side = ControllerSide();
            const int next = int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER))) -
                int(bool(pressed & Button(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)));
            selected[side] = (selected[side] + next + 6) % 6;
            if (readyAnimation != REANIMATIONID_NULL) return;
            const int x = int(controllerX), y = int(controllerY);
            if (accept && !Collect(side, x, y))
            {
                int col, lane;
                if (TileAt(x, y, col, lane)) Place(side, col, lane);
            }
            if (pressed & Button(SDL_GAMEPAD_BUTTON_WEST)) Collect(side, x, y);
        }
        else if (screen == Screen::Win && accept && !MouseWon() && TrophyLanded())
            CollectTrophy();
        else if (screen == Screen::Result)
        {
            resultOption = ClampInt(resultOption + dx, 0, 1);
            if (accept)
            {
                if (resultOption == 0) ReturnToSides();
                else Close();
            }
        }
        if (back && screen == Screen::Draft) BackFromDraft();
        else if (back && (screen == Screen::Confirm || screen == Screen::Sides || screen == Screen::Result)) Close();
    }

    void BackFromDraft()
    {
        draftFlight = 0;
        draftTurn = 0;
        screen = Screen::Sides;
    }

    void SelectBattle(int choice)
    {
        option = choice;
        for (auto& deck : decks)
            for (int i = 0; i < 6; ++i) deck[i] = choice == 1 && i ? -1 : i;
        if (choice == 1)
        {
            draftTurn = 0;
            draftCursor = mouseDraftCursor = 1;
            draftFlight = 0;
            screen = Screen::Draft;
            return;
        }
        if (choice == 2)
        {
            for (int side = 0; side < 2; ++side)
            {
                std::vector<int> pool;
                for (int id = 1; id < CardCount(side); ++id)
                    if (DraftCardAllowed(side, id)) pool.push_back(id);
                for (int i = 0; i < 5; ++i)
                {
                    std::swap(pool[i], pool[i + Rand(int(pool.size()) - i)]);
                    decks[side][i + 1] = pool[i];
                }
            }
        }
        StartMatch();
    }

    void PickCard(int card)
    {
        auto& deck = decks[DraftSide()];
        if (draftFlight || !DraftCardAllowed(DraftSide(), card) || std::find(deck.begin(), deck.end(), card) != deck.end()) return;
        deck[1 + draftTurn / 2] = card;
        flyingCard = card;
        draftFlight = DraftFlightTicks;
    }

    Zombie* SpawnProp(int x, int y, bool target)
    {
        auto* board = app.mBoard;
        auto* zombie = board->AddZombieInRow(ZOMBIE_NORMAL, y, Zombie::ZOMBIE_WAVE_DEBUG);
        if (!zombie) return nullptr;
        app.RemoveReanimation(zombie->mBodyReanimID);
        auto* anim = app.AddReanimation(0.0f, 0.0f, 0, target ? REANIM_ZOMBIE_TARGET : REANIM_VS_GRAVESTONE);
        anim->mIsAttachment = true;
        anim->PlayReanim("anim_idle", REANIM_LOOP_FULL_LAST_FRAME, 0, 12.0f);
        zombie->mBodyReanimID = app.ReanimationGetID(anim);
        zombie->mFromWave = target ? Zombie::ZOMBIE_WAVE_VERSUS_TARGET : Zombie::ZOMBIE_WAVE_VERSUS_GRAVE;
        zombie->mPosX = zombie->mX = board->GridToPixelX(x, y) + (target ? 20 : 14);
        zombie->mPosY = zombie->mY = board->GridToPixelY(x, y) - (target ? 28 : 4);
        zombie->mZombieRect = target ? Rect(0, 20, 66, 110) : Rect(0, 12, 68, 91);
        zombie->mBodyHealth = zombie->mBodyMaxHealth = target ? 200 : 400;
        return zombie;
    }

    void StartMatch()
    {
        for (int card : decks[0])
            if (Plants[card].type != SEED_GRAVEBUSTER)
                Plant::PreloadPlantResources(static_cast<SeedType>(Plants[card].type));
        for (int card : decks[1])
            if (card > 0) Zombie::PreloadZombieResources(static_cast<ZombieType>(Zombies[card].type));
        app.KillGameSelector();
        app.mGameMode = GAMEMODE_VERSUS;
        app.MakeNewBoard();
        auto* board = app.mBoard;
        board->InitLevel();
        board->mSunMoney = 75;
        board->mSodPosition = 1000;
        board->mSeedBank->mNumPackets = 0;
#ifdef _REPLANTED_SPEED_CONTROL
        board->mAllowSpeedMod = false;
#endif
        board->InitLawnMowers();
        LawnMower* mower = nullptr;
        while (board->IterateLawnMowers(mower))
        {
            mower->mVisible = true;
            mower->mPosX = -21.0f;
        }
        app.mBoardResult = BOARDRESULT_NONE;
        app.StartPlaying();
        for (int lane = 0; lane < Rows; ++lane)
        {
            auto* target = SpawnProp(LastColumn, lane, true);
            targets[lane] = target ? board->ZombieGetID(target) : ZombieID::ZOMBIEID_NULL;
        }
        for (auto& cells : graves) cells.fill(ZombieID::ZOMBIEID_NULL);
        for (int lane : {1, 3})
        {
            board->NewPlant(0, lane, SEED_SUNFLOWER);
            auto* grave = SpawnProp(LastColumn, lane, false);
            if (grave) graves[2][lane] = board->ZombieGetID(grave);
        }
        for (auto& player : recharge) std::fill(std::begin(player), std::end(player), 0);
        time = defeated = 0;
        plantsWon = false;
        brains = 75;
        drops.clear();
        selected[0] = selected[1] = 1;
        const int startColumn = ControllerSide() == 0 ? 0 : LastColumn;
        controllerX = GridScreenX(startColumn, 2) + 40.0f;
        controllerY = GridScreenY(startColumn, 2) + 50.0f;
        auto* ready = app.AddReanimation(0.0f, 0.0f, 0, REANIM_READYSETPLANT);
        ready->mIsAttachment = true;
        ready->mLoopType = REANIM_PLAY_ONCE_FULL_LAST_FRAME;
        readyAnimation = app.ReanimationGetID(ready);
        board->mPaused = true;
        app.PlaySample(Sexy::SOUND_READYSETPLANT);
        screen = Screen::Match;
    }

    bool Collect(int side, int x, int y)
    {
        auto* board = app.mBoard;
        if (side == 0)
        {
            Coin* coin = nullptr;
            while (board->IterateCoins(coin))
            {
                if (coin->IsSun() && !coin->mIsBeingCollected &&
                    Rect(GridScreenX(0, 0) - board->GridToPixelX(0, 0) + int(coin->mPosX) - 12,
                        GridScreenY(0, 0) - board->GridToPixelY(0, 0) + int(coin->mPosY) - 12, 88, 88).Contains(x, y))
                {
                    coin->PlayCollectSound();
                    coin->Collect();
                    return true;
                }
            }
        }
        else
        {
            for (auto& drop : drops)
            {
                if (drop.collecting) continue;
                const int centerX = GridScreenX(drop.column, drop.row) + 39;
                const int centerY = BrainCenterY(drop);
                if (Rect(centerX - 30, centerY - 30, 60, 60).Contains(x, y))
                {
                    drop.collecting = true;
                    drop.collectX = float(centerX);
                    drop.collectY = float(centerY);
                    app.PlaySample(Sexy::SOUND_SLURP);
                    return true;
                }
            }
        }
        return false;
    }

    void SpawnAttacker(int type, int x, int y)
    {
        auto* zombie = app.mBoard->AddZombieInRow(static_cast<ZombieType>(Zombies[type].type), y, Zombie::ZOMBIE_WAVE_DEBUG);
        if (!zombie) return;
        if (type == 2)
        {
            zombie->mFromWave = Zombie::ZOMBIE_WAVE_VERSUS_TRASH;
            zombie->mBodyHealth = zombie->mBodyMaxHealth = 200;
            zombie->mShieldHealth = zombie->mShieldMaxHealth = 800;
            zombie->mShieldType = SHIELDTYPE_TRASHCAN;
            if (Sexy::IMAGE_REANIM_ZOMBIE_TRASHCAN1)
                app.ReanimationGet(zombie->mBodyReanimID)->SetImageOverride("anim_screendoor", Sexy::IMAGE_REANIM_ZOMBIE_TRASHCAN1);
        }
        if (type <= 4 || type == 6 || type == 7 || type == 8 || type == 9 || type == 14 || type == 15)
            zombie->RiseFromGrave(x, y);
    }

    void Place(int side, int x, int y)
    {
        if (plantsWon || readyAnimation != REANIMATIONID_NULL) return;
        const int slot = selected[side];
        const int id = decks[side][slot];
        const auto& card = GetCard(side, id);
        auto* board = app.mBoard;
        int& currency = side == 0 ? board->mSunMoney : brains;
        if (currency < card.cost || recharge[side][slot] || (SuddenDeath() && !card.suddenRecharge)) return;
        if (side == 0)
        {
            if (card.type == SEED_GRAVEBUSTER)
            {
                if (x < PlantColumns) return;
                auto* grave = board->ZombieTryToGet(graves[x - PlantColumns][y]);
                if (!grave || grave->mBodyHealth <= 0) return;
                grave->TakeVersusPropDamage(grave->mBodyHealth);
                app.PlaySample(Sexy::SOUND_GRAVEBUSTERCHOMP);
            }
            else
            {
                if (x >= PlantColumns) return;
                const auto seed = static_cast<SeedType>(card.type);
                if (board->CanPlantAt(x, y, seed) != PLANTING_OK) return;
                if (!board->NewPlant(x, y, seed)) return;
                app.PlayFoley(FOLEY_PLANT);
            }
        }
        else
        {
            if (x < PlantColumns) return;
            if (id == 0)
            {
                auto& idAtCell = graves[x - PlantColumns][y];
                if (board->ZombieTryToGet(idAtCell)) return;
                auto* grave = SpawnProp(x, y, false);
                if (!grave) return;
                idAtCell = board->ZombieGetID(grave);
                app.PlaySample(Sexy::SOUND_GRAVESTONE_RUMBLE);
            }
            else
            {
                SpawnAttacker(id, x, y);
                if (id == 5)
                    for (int lane = 0; lane < Rows; ++lane)
                    {
                        SpawnAttacker(1, LastColumn, lane);
                        SpawnAttacker(3, LastColumn, lane);
                    }
            }
        }
        currency -= card.cost;
        recharge[side][slot] = SuddenDeath() ? card.suddenRecharge : card.recharge;
    }

    bool MouseWon() const { return plantsWon == p1Plants; }

    bool TrophyLanded() const { return winTicks >= 2 * IrisTicks + TrophyDropTicks; }

    Rect TrophyRect() const
    {
        const float drop = ClampInt(winTicks - 2 * IrisTicks, 0, TrophyDropTicks) / float(TrophyDropTicks);
        const int center = plantsWon ? 750 : 548;
        return Rect(center - 68, -140 + int(478 * drop * drop), 136, 136);
    }

    void RecordVersusWin(int winningSide)
    {
        const int winner = winningSide == MouseSide() ? 0 : 1;
        winHistory[winner].push_back(winningSide);
        if (winHistory[winner].size() > 9) winHistory[winner].erase(winHistory[winner].begin());
        versusStreak[1 - winner] = 0;
        if (++versusStreak[winner] == 5) pvz_host_earn(&app, "versus_vs_versus");
    }

    static Rect ConcedeRect() { return Rect(529, 629, 222, 62); }

    void ConcedeMatch(int player)
    {
        if (!PauseDialogOpen() || !app.GetDialog(DIALOG_NEWOPTIONS)) return;
        const int winnerSide = player == 0 ? ControllerSide() : MouseSide();
        app.KillNewOptionsDialog();
        if (app.mMusic) app.mMusic->StopAllMusic();
        app.PlaySample(Sexy::SOUND_WINMUSIC);
        app.mBoard->mPaused = true;
        plantsWon = winnerSide == 0;
        RecordVersusWin(winnerSide);
        concedingPlayer = -1;
        resultOption = 0;
        screen = Screen::Result;
    }

    void CollectTrophy()
    {
        app.PlaySample(Sexy::SOUND_WINMUSIC);
        RecordVersusWin(plantsWon ? 0 : 1);
        resultOption = 0;
        trophyFlashTicks = 0;
        screen = Screen::TrophyFlash;
    }

    void UpdateWin()
    {
        if (!plantsWon && lossSequenceTicks <= LosingCueTicks + 75)
        {
            ++lossSequenceTicks;
            if (lossSequenceTicks == LosingCueTicks - 90 || lossSequenceTicks == LosingCueTicks - 40)
                app.PlayFoley(FOLEY_CHOMP);
            if (lossSequenceTicks == LosingCueTicks) app.PlayFoley(FOLEY_SCREAM);
        }
        if (winTicks == IrisTicks - 1)
        {
            ++winHoldTicks;
            if (!plantsWon && lossSequenceTicks < LosingCueTicks + 75) return;
            if (plantsWon && winHoldTicks < 1050)
            {
                for (auto id : targets)
                {
                    auto* target = app.mBoard->ZombieTryToGet(id);
                    if (target && !target->mDead && target->mBodyHealth <= 0) return;
                }
            }
        }
        if (++winTicks == IrisTicks)
        {
            if (!plantsWon)
            {
                Zombie* zombie = nullptr;
                while (app.mBoard->IterateZombies(zombie))
                    if (zombie->mFromWave == Zombie::ZOMBIE_WAVE_WINNER) zombie->DieNoLoot();
            }
            app.mBoard->mPaused = true;
        }
        if (winTicks >= 2 * IrisTicks + TrophyDropTicks + 950) winTicks -= 950;
    }

    void BeginWin(bool plantVictory)
    {
        plantsWon = plantVictory;
        if (app.mMusic) app.mMusic->StopAllMusic();
        if (!plantVictory)
        {
            app.PlaySample(Sexy::SOUND_LOSEMUSIC);
            lossSequenceTicks = 0;
            irisX = GridScreenX(0, 2) - 100;
            irisY = GridScreenY(0, 2) + 45;
        }
        app.mBoardResult = plantVictory ? BOARDRESULT_WON : BOARDRESULT_LOST;
        screen = Screen::Win;
        winTicks = winHoldTicks = 0;
    }

    void UpdateMatch()
    {
        ++time;
        if (time == SuddenDeathTick)
            for (int side = 0; side < 2; ++side)
                for (int slot = 0; slot < 6; ++slot)
                {
                    const auto& card = GetCard(side, decks[side][slot]);
                    recharge[side][slot] = recharge[side][slot] * card.suddenRecharge / card.recharge;
                }
        for (auto& player : recharge)
            for (auto& timer : player) if (timer > 0) --timer;
        auto* board = app.mBoard;
        if (time % (SuddenDeath() ? 425 : 850) == 0)
        {
            for (int i = 0; i < (SuddenDeath() ? 2 : 1); ++i)
            {
                board->AddCoin(board->GridToPixelX(Rand(PlantColumns), 0) + 15, 58, COIN_SUN, COIN_MOTION_FROM_SKY);
                drops.push_back({PlantColumns + Rand(3), Rand(Rows), 0, true});
            }
        }
        for (int x = 0; x < 3; ++x)
            for (int y = 0; y < Rows; ++y)
            {
                auto* grave = board->ZombieTryToGet(graves[x][y]);
                if (grave && grave->mBodyHealth > 0 && grave->mZombieAge % 2400 == 0)
                {
                    grave->mJustGotShotCounter = 30;
                    drops.push_back({x + PlantColumns, y, 0, false});
                }
            }
        UpdateBrains();
        defeated = 0;
        for (auto id : targets)
        {
            auto* target = board->ZombieTryToGet(id);
            if (!target || target->mBodyHealth <= 0) ++defeated;
            if (target && !target->mDead && target->mBodyHealth <= 0)
            {
                irisX = board->mX + app.mDDInterface->mWideScreenOffsetX + target->mX + 30;
                irisY = board->mY + app.mDDInterface->mWideScreenOffsetY + target->mY + 55;
            }
        }
        if (defeated >= 3) BeginWin(true);
        else if (app.mBoardResult == BOARDRESULT_LOST) BeginWin(false);
    }

    static void Label(Graphics& g, const char* value, int x, int y, Font* font = nullptr, Color color = Color::White)
    {
        TodDrawString(&g, value, x, y, font ? font : Sexy::FONT_DWARVENTODCRAFT18GREENINSET, color, DS_ALIGN_CENTER);
    }

    static void ImageAt(Graphics& g, Image* image, Rect rectangle)
    {
        if (image) g.DrawImage(image, rectangle.mX, rectangle.mY, rectangle.mWidth, rectangle.mHeight);
    }

    void ConsoleButton(Graphics& g, Rect rect, const char* first, const char* second, bool active)
    {
        const bool highlighted = active || rect.Contains(pointerX, pointerY);
        ImageAt(g, highlighted ? (Image*)buttonSelected : (Image*)buttonImage, rect);
        const auto color = highlighted ? Color(255, 241, 144) : Color(104, 213, 59);
        Label(g, first, rect.mX + rect.mWidth / 2, rect.mY + rect.mHeight / 2 - 5, Sexy::FONT_DWARVENTODCRAFT24, color);
        Label(g, second, rect.mX + rect.mWidth / 2, rect.mY + rect.mHeight / 2 + 21, Sexy::FONT_DWARVENTODCRAFT24, color);
    }

    static Rect SideRect(int side) { return Rect(side == 0 ? 283 : 742, 258, 244, 266); }
    static Rect OptionRect(int index) { return Rect(324 + index * 220, 550, 201, 92); }
    static Rect ResultRect(int index) { return Rect(422 + index * 241, 532, 207, 98); }
    static int CardCount(int side) { return side == 0 ? PlantCardCount : ZombieCardCount; }
    static int DraftSlot(int side, int id) { return side == 0 ? Plants[id].type : id; }
    static int CardAtDraftSlot(int side, int slot)
    {
        if (side != 0) return slot;
        for (int id = 0; id < PlantCardCount; ++id)
            if (Plants[id].type == slot) return id;
        return 0;
    }
    static bool DraftCardAllowed(int side, int id)
    {
        return side != 0 || (Plants[id].type != SEED_BLOVER && Plants[id].type != SEED_MARIGOLD);
    }
    unsigned int DraftPlantRecommendations(int id) const
    {
        const auto seed = static_cast<SeedType>(Plants[id].type);
        const bool coffeePicked = std::any_of(std::begin(decks[0]), std::end(decks[0]), [](int card)
        {
            return card >= 0 && card < PlantCardCount && Plants[card].type == SEED_INSTANT_COFFEE;
        });
        unsigned int flags = 0;
        if (Plant::IsNocturnal(seed) && !coffeePicked) SetBit(flags, NOT_RECOMMENDED_NOCTURNAL, true);
        if (Plant::IsAquatic(seed)) SetBit(flags, NOT_RECOMMENDED_NEEDS_POOL, true);
        if (seed == SEED_PLANTERN) SetBit(flags, NOT_RECOMMENDED_NEEDS_FOG, true);
        if (seed == SEED_FLOWERPOT) SetBit(flags, NOT_RECOMMENDED_NEEDS_ROOF, true);
        return flags;
    }
    static int DraftColumns(int side) { return side == 0 ? 8 : 5; }
    static Rect DraftRect(int side, int card)
    {
        const int columns = DraftColumns(side);
        const int slot = DraftSlot(side, card);
        return Rect((side == 0 ? 274 : 730) + slot % columns * 52,
            (side == 0 ? 189 : 182) - DraftTopShift + slot / columns * 74, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
    }

    static Rect BankRect(int side, int slot)
    {
        return Rect((side == 0 ? PlantBank : ZombieBank) + slot * CardWidth, CardY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
    }

    void DrawCursor(Graphics& g, Rect rect, int player, bool lawn)
    {
        ImageAt(g, lawn ? (Image*)boardCursor[player] : (Image*)seedCursor[player], rect);
        const int center = rect.mX + rect.mWidth / 2;
        ImageAt(g, playerLabel[player], Rect(center - 16, rect.mY - 17, 34, 25));
        ImageAt(g, playerArrow[player], Rect(center - 8, rect.mY + 4, 17, 11));
    }

    void DrawZombiePortrait(Graphics& g, const Card& card, Rect rect)
    {
        if (card.type == ZOMBIE_INVALID)
        {
            ImageAt(g, graveIcon, Rect(rect.mX + 9, rect.mY + 3, 31, 45));
            return;
        }
        auto* display = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
        if (!display || display->h <= 0) return;
        const float scale = display->h / 600.0f;
        g.PushState();
        g.SetClipRect(rect.mX + 4, rect.mY + 3, rect.mWidth - 8, 49);
        const bool gargantuar = card.type == ZOMBIE_GARGANTUAR;
        g.mScaleX = g.mScaleY = (gargantuar ? 0.225f : 0.37f) / scale;
        const float portraitX = rect.mX + (gargantuar ? 2.0f : -7.0f);
        const float portraitY = rect.mY + (gargantuar ? 3.0f : -11.0f);
        if (card.type == ZOMBIE_DOOR && card.cost == 50 && Sexy::IMAGE_REANIM_ZOMBIE_TRASHCAN1)
            app.mReanimatorCache->DrawCachedTrashCanZombie(&g, portraitX, portraitY);
        else
            app.mReanimatorCache->DrawCachedZombie(&g, portraitX, portraitY, static_cast<ZombieType>(card.type));
        g.PopState();
    }

    void DrawCard(Graphics& g, int side, int id, Rect rect, int timer, bool dark, bool disabled, bool discouraged = false)
    {
        const auto& card = GetCard(side, id);
        const int duration = SuddenDeath() ? card.suddenRecharge : card.recharge;
        const float cooldown = disabled ? 0.0f : pvz_min(1.0f, timer / float(pvz_max(1, duration)));
        if (side == 0)
        {
            g.PushState();
            if (Sexy::IMAGE_SEEDPACKET_LARGER)
            {
                g.mScaleX = 2.0f * rect.mWidth / Sexy::IMAGE_SEEDPACKET_LARGER->GetWidth();
                g.mScaleY = 2.0f * rect.mHeight / Sexy::IMAGE_SEEDPACKET_LARGER->GetHeight();
            }
            DrawSeedPacket(&g, rect.mX, rect.mY, static_cast<SeedType>(card.type), SEED_NONE, 0.0f, discouraged ? 115 : 255, false, false);
            g.PopState();
        }
        else
        {
            ImageAt(g, zombiePacket, rect);
            DrawZombiePortrait(g, card, rect);
        }
        auto cost = std::to_string(card.cost);
        Label(g, cost.c_str(), rect.mX + 21, rect.mY + 62, Sexy::FONT_PICO129, Color::Black);
        if (dark || disabled)
        {
            g.SetColor(Color(0, 0, 0, 112));
            g.FillRect(rect);
        }
        if (cooldown > 0.0f)
        {
            const int covered = ClampInt(int(std::ceil(rect.mHeight * cooldown)), 0, rect.mHeight);
            g.SetColor(Color(0, 0, 0, 148));
            g.FillRect(Rect(rect.mX, rect.mY, rect.mWidth, covered));
        }
        if (disabled) ImageAt(g, redCross, Rect(rect.mX + 5, rect.mY + 3, 38, 57));
    }

    void DrawBanks(Graphics& g, bool showCards)
    {
        const bool inMatch = app.mGameMode == GAMEMODE_VERSUS;
        const int sun = inMatch ? app.mBoard->mSunMoney : 75;
        const int bra = inMatch ? brains : 75;
        ImageAt(g, plantBank, Rect(240, BankTop, 400, 80));
        ImageAt(g, zombieBank, Rect(643, BankTop, 400, 80));
        Label(g, std::to_string(sun).c_str(), 275, BankTop + 73, Sexy::FONT_CONTINUUMBOLD14, Color::Black);
        Label(g, std::to_string(bra).c_str(), 1008, BankTop + 73, Sexy::FONT_CONTINUUMBOLD14, Color::Black);
        if (!showCards) return;
        for (int side = 0; side < 2; ++side)
        {
            for (int slot = 0; slot < 6; ++slot)
            {
                const auto id = decks[side][slot];
                if (id < 0 || (draftFlight && side == DraftSide() && slot == 1 + draftTurn / 2)) continue;
                const auto& card = GetCard(side, id);
                DrawCard(g, side, id, BankRect(side, slot), inMatch ? recharge[side][slot] : 0,
                    inMatch && (side == 0 ? sun : bra) < card.cost, inMatch && SuddenDeath() && !card.suddenRecharge);
            }
            if (inMatch)
            {
                const auto rect = BankRect(side, selected[side]);
                DrawCursor(g, Rect(rect.mX - 6, rect.mY - 5, 61, 80), side == MouseSide() ? 0 : 1, false);
            }
        }
    }

    void DrawDraft(Graphics& g)
    {
        DrawBanks(g, true);
        const int active = DraftSide();
        const int lift = draftFlight ? int(7.0f * std::sin(3.14159f * draftFlight / DraftFlightTicks)) : 0;
        for (int layer = 0; layer < 2; ++layer)
        {
            const int side = layer == 1 ? active : 1 - active;
            const int movement = side == active ? lift : 0;
            g.PushState();
            g.mTransY -= movement;
            const Rect panel = side == 0 ? Rect(249, 146 - DraftTopShift, 474, 514) : Rect(713, 143 - DraftTopShift, 310, 515);
            ImageAt(g, draftPanels[side], panel);
            const auto heading = side == 0 ? Color(255, 218, 114) : Color(116, 221, 81);
            if (side != active)
            {
                g.SetColor(Color(0, 0, 0, 61));
                g.FillRect(Rect(panel.mX + 11, panel.mY + 9, panel.mWidth - 21, panel.mHeight - 19));
            }
            Label(g, side == 0 ? "Choose Your Plants!" : "Pik ur Zombeez",
                panel.mX + panel.mWidth / 2, panel.mY + 32, Sexy::FONT_DWARVENTODCRAFT24, heading);
            const auto& deck = decks[side];
            const int cursor = side == ControllerSide() ? draftCursor : mouseDraftCursor;
            for (int id = 0; id < CardCount(side); ++id)
            {
                const bool picked = std::find(deck.begin(), deck.end(), id) != deck.end();
                DrawCard(g, side, id, DraftRect(side, id), 0,
                    side != active || picked, !DraftCardAllowed(side, id),
                    side == 0 && !picked && DraftPlantRecommendations(id) != 0);
            }
            const auto rect = DraftRect(side, cursor);
            DrawCursor(g, Rect(rect.mX - 5, rect.mY - 4, 59, 78), side == MouseSide() ? 0 : 1, false);
            if (side == active && !draftFlight)
            {
                const unsigned int flags = side == 0 ? DraftPlantRecommendations(cursor) : 0;
                const SexyString warning = !DraftCardAllowed(side, cursor)
                    ? TodStringTranslate("[NOT_ALLOWED_ON_THIS_LEVEL]")
                    : flags ? TodStringTranslate(TestBit(flags, NOT_RECOMMENDED_NOCTURNAL)
                        ? "[NOCTURNAL_WARNING]" : "[NOT_RECOMMENDED_FOR_LEVEL]") : SexyString();
                const int tooltipWidth = pvz_max(148, Sexy::FONT_BRIANNETOD12->StringWidth(warning) + 16);
                const int tooltipX = ClampInt(rect.mX - 38, 255, Width - tooltipWidth - 20);
                g.SetColor(Color(255, 241, 181));
                g.FillRect(Rect(tooltipX, rect.mY + 72, tooltipWidth, warning.empty() ? 29 : 49));
                Label(g, GetCard(side, cursor).label, tooltipX + tooltipWidth / 2, rect.mY + 93,
                    Sexy::FONT_BRIANNETOD16, Color(45, 35, 24));
                if (!warning.empty()) TodDrawString(&g, warning, tooltipX + tooltipWidth / 2, rect.mY + 113,
                    Sexy::FONT_BRIANNETOD12, Color(180, 35, 25), DS_ALIGN_CENTER);
            }
            g.PopState();
        }
        if (draftFlight)
        {
            const float remaining = float(draftFlight) / DraftFlightTicks;
            const float blend = 1.0f - remaining * remaining;
            const auto from = DraftRect(active, flyingCard);
            const auto to = BankRect(active, 1 + draftTurn / 2);
            DrawCard(g, active, flyingCard, Rect(from.mX + int((to.mX - from.mX) * blend),
                from.mY + int((to.mY - from.mY) * blend), SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT), 0, false, false);
        }
    }

    void DrawPlacement(Graphics& g, int x, int y, int player)
    {
        const int side = player == 0 ? MouseSide() : ControllerSide();
        const auto& card = GetCard(side, decks[side][selected[side]]);
        const int money = side == 0 ? app.mBoard->mSunMoney : brains;
        const bool ready = money >= card.cost && !recharge[side][selected[side]] &&
            (!SuddenDeath() || card.suddenRecharge);
        g.PushState();
        g.SetColorizeImages(true);
        g.SetColor(ready ? Color(255, 255, 255, 137) : Color(52, 60, 46, 132));
        if (side == 0)
        {
            g.mScaleX = g.mScaleY = 0.85f;
            Plant::DrawSeedType(&g, static_cast<SeedType>(card.type), SEED_NONE, VARIATION_NORMAL, x + 6, y + 14);
        }
        else DrawZombiePortrait(g, card, Rect(x + 14, y + 18, 49, 69));
        g.PopState();
    }

    void DrawMatchCursor(Graphics& g, int x, int y, int player)
    {
        const int side = player == 0 ? MouseSide() : ControllerSide();
        const auto& card = GetCard(side, decks[side][selected[side]]);
        int col, lane;
        if (TileAt(x, y, col, lane) &&
            (side == 0 ? col < PlantColumns || card.type == SEED_GRAVEBUSTER : col >= PlantColumns))
            DrawPlacement(g, x - 40, y - 50, player);
        DrawCursor(g, Rect(x - 38, y - 48, 76, 96), player, true);
    }

    static constexpr int BrainStartY = 52;
    static constexpr float BrainFallSpeed = 0.67f;

    int BrainLifeRemaining(const Brain& drop) const
    {
        const int landingY = GridScreenY(drop.column, drop.row) + 44;
        const int fallTicks = drop.falling ? int(std::ceil((landingY - BrainStartY) / BrainFallSpeed)) : 0;
        return fallTicks + 1000 - drop.age;
    }

    int BrainCenterY(const Brain& drop) const
    {
        const int landingY = GridScreenY(drop.column, drop.row) + 44;
        const int fallY = BrainStartY + int(drop.age * BrainFallSpeed);
        if (drop.falling && fallY < landingY) return fallY;
        if (!drop.falling && drop.age < 48)
            return landingY - int(23.0f * std::sin(drop.age * 3.14159f / 48.0f));
        return landingY + int(3.0f * std::sin(drop.age * 0.07f));
    }

    void UpdateBrains()
    {
        for (auto it = drops.begin(); it != drops.end();)
        {
            auto& drop = *it;
            if (drop.collecting)
            {
                const float dx = BrainBankX - drop.collectX;
                const float dy = BrainBankY - drop.collectY;
                if (dx * dx + dy * dy < 64.0f)
                {
                    brains += 25;
                    it = drops.erase(it);
                    continue;
                }
                drop.collectX += dx / 21.0f;
                drop.collectY += dy / 21.0f;
            }
            else
            {
                ++drop.age;
                if (BrainLifeRemaining(drop) <= 0)
                {
                    it = drops.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void DrawBrain(Graphics& g, const Brain& drop)
    {
        const int age = drop.age;
        const int remaining = BrainLifeRemaining(drop);
        const int x = drop.collecting ? int(drop.collectX) : GridScreenX(drop.column, drop.row) + 39;
        const int y = drop.collecting ? int(drop.collectY) : BrainCenterY(drop);
        const float dx = float(x - BrainBankX), dy = float(y - BrainBankY);
        const int alpha = drop.collecting
            ? int(std::clamp(std::sqrt(dx * dx + dy * dy) * 0.035f, 0.35f, 1.0f) * 255.0f)
            : ClampInt(255 * remaining / 75, 0, 255);
        const int size = 92 + int(10.0f * std::sin(age * 0.078f));
        g.PushState();
        if (alpha < 255)
        {
            g.SetColorizeImages(true);
            g.SetColor(Color(255, 255, 255, alpha));
        }
        ImageAt(g, brainGlowBig, Rect(x - size / 2, y - size / 2, size, size));
        ImageAt(g, brainGlowSmall, Rect(x - 43, y - 43, 86, 86));
        ImageAt(g, brainImage, Rect(x - 18, y - 18, 37, 37));
        g.PopState();
    }

    void DrawSuns(Graphics& g)
    {
        auto* board = app.mBoard;
        g.PushState();
        g.mTransX += board->mX + app.mDDInterface->mWideScreenOffsetX;
        g.mTransY += board->mY + app.mDDInterface->mWideScreenOffsetY;
        Coin* coin = nullptr;
        while (board->IterateCoins(coin))
        {
            if (coin->IsSun() && coin->BeginDraw(&g))
            {
                coin->Draw(&g);
                coin->EndDraw(&g);
            }
        }
        g.PopState();
    }

    void DrawMatch(Graphics& g)
    {
        DrawBanks(g, true);
        DrawMatchCursor(g, int(controllerX), int(controllerY), 1);
        if (Rect(0, 0, Width, Height).Contains(pointerX, pointerY))
            DrawMatchCursor(g, pointerX, pointerY, 0);
        DrawSuns(g);
        for (const auto& drop : drops) DrawBrain(g, drop);
        ImageAt(g, targetCounter, Rect(856, 612, 162, 52));
        for (int i = 0; i < pvz_min(defeated, 3); ++i)
            ImageAt(g, redCross, Rect(874 + i * 45, 610, 31, 47));
        if (auto* ready = app.ReanimationTryToGet(readyAnimation))
        {
            ready->SetPosition(Width / 2, Height / 2);
            ready->Draw(&g);
        }
        if (screen == Screen::Match && SuddenDeath() && time < SuddenDeathTick + 200)
        {
            g.SetColor(Color(0, 0, 0, 144));
            g.FillRect(Rect(0, 348, Width, 72));
            Label(g, "Sudden Death!", Width / 2, 396, Sexy::FONT_HOUSEOFTERROR28, Color(255, 232, 142));
        }
    }

    void DrawWin(Graphics& g)
    {
        if (winTicks < 2 * IrisTicks)
        {
            const float progress = std::abs(winTicks - IrisTicks) / float(IrisTicks);
            DrawVersusIris(g, irisX, irisY, 128.0f + 1480.0f * progress * progress);
            return;
        }
        auto trophyRect = TrophyRect();
        if (screen == Screen::TrophyFlash)
        {
            const float progress = std::clamp(trophyFlashTicks / float(WinningCueTicks - 5), 0.0f, 1.0f);
            const float ease = 1.0f - (1.0f - progress) * (1.0f - progress);
            const int pulse = int(12.0f * std::exp(-6.0f * progress) * std::sin(25.0f * progress));
            const int size = trophyRect.mWidth + int(62.0f * ease) + pulse;
            const int startX = trophyRect.mX + trophyRect.mWidth / 2;
            const int startY = trophyRect.mY + trophyRect.mHeight / 2;
            trophyRect = Rect(startX + int((Width / 2 - startX) * ease) - size / 2,
                startY + int((Height / 2 - 32 - startY) * ease) - size / 2, size, size);
        }
        const int center = trophyRect.mX + trophyRect.mWidth / 2;
        const int top = trophyRect.mY;
        if (TrophyLanded())
        {
            for (int i = 0; i < 24; ++i)
            {
                const int age = (winTicks - 2 * IrisTicks - TrophyDropTicks + i * 8 + trophyFlashTicks) % 95;
                const float angle = i * 2.39996f;
                const int x = center + int(std::cos(angle) * (28 + age * 2));
                const int y = top + 63 + int(std::sin(angle) * (28 + age * 2));
                g.SetColor(Color(255, 239, 139, 235 - age * 2));
                g.FillRect(Rect(x - 3, y - 1, 7, 3));
                g.FillRect(Rect(x - 1, y - 3, 3, 7));
            }
        }
        ImageAt(g, trophy[plantsWon ? 0 : 1], trophyRect);
        if (screen == Screen::Win && TrophyLanded())
        {
            auto* font = Sexy::FONT_BRIANNETOD16;
            const bool mouseWon = MouseWon();
            const char* action = "Collect trophy";
            const char* buttonText = mouseWon ? "Left click" : (playStationPad ? "X" : "A");
            constexpr int iconSize = 28;
            constexpr int gap = 8;
            const int buttonWidth = mouseWon ? font->StringWidth(buttonText) + 12 : iconSize;
            const int x = center - (buttonWidth + gap + font->StringWidth(action)) / 2;
            const int baseline = top + 162;
            const int iconY = baseline - (font->GetAscent() + iconSize) / 2;
            if (mouseWon || !DrawPadButton(g, 0, x, iconY, false, playStationPad, iconSize))
            {
                const Rect hint(x, iconY, buttonWidth, iconSize);
                g.SetColor(Color(25, 25, 30, 225));
                g.FillRect(hint);
                g.SetColor(Color::White);
                g.DrawRect(hint);
                Label(g, buttonText, x + buttonWidth / 2, baseline, font);
            }
            TodDrawString(&g, action, x + buttonWidth + gap, baseline, font, Color::White, DS_ALIGN_LEFT);
        }
    }

    void DrawResults(Graphics& g)
    {
        const int winningSide = plantsWon ? 0 : 1;
        ImageAt(g, resultBackground, Rect(0, 0, Width, Height));
        Label(g, "VS Results", 640, 110, Sexy::FONT_HOUSEOFTERROR28);
        ImageAt(g, sidePortrait[winningSide], Rect(272, 132, 298, 305));
        ImageAt(g, sideLabel[winningSide], Rect(272, 132, 298, 305));
        ImageAt(g, winLabel, Rect(335, 443, 168, 85));
        for (int side = 0; side < 2; ++side)
        {
            const int player = side == MouseSide() ? 0 : 1;
            const int y = 167 + side * 174;
            g.SetColor(Color::Black);
            g.FillRect(Rect(622, y + 46, 64, 58));
            if ((Image*)gamerpic)
                g.DrawImage(gamerpic, Rect(625, y + 48, 58, 58), Rect(side * 64, 0, 64, 64));
            ImageAt(g, resultPanel[side], Rect(594, y, 422, 172));
            const auto playerName = "Player " + std::to_string(player + 1);
            Label(g, playerName.c_str(), 684, y + 41, Sexy::FONT_DWARVENTODCRAFT24);
            const auto& history = winHistory[player];
            if (side == winningSide)
            {
                ImageAt(g, trophyBase, Rect(684, y + 72, 89, 90));
                ImageAt(g, trophy[winningSide], Rect(677, y + 40, 103, 103));
            }
            const int previous = int(history.size()) - (side == winningSide ? 1 : 0);
            for (int index = 0; index < previous; ++index)
            {
                const int x = (side == winningSide ? 773 : 699) + index % 5 * 42;
                ImageAt(g, trophy[history[index]], Rect(x, y + 70 + index / 5 * 32, 42, 42));
            }
        }
        ConsoleButton(g, ResultRect(0), "Play", "Again", resultOption == 0);
        ConsoleButton(g, ResultRect(1), "Main", "Menu", resultOption == 1);
    }

    bool DrawPadButton(Graphics& g, int index, int x, int y, bool small, bool playStation, int drawSize = 0)
    {
        const int size = small ? 21 : 42;
        Image* image = playStation ? (small ? (Image*)helpButtonsSmall : (Image*)helpButtons)
            : (small ? (Image*)xboxButtonsSmall : (Image*)xboxButtons);
        const int stride = playStation ? size : size + (small ? 1 : 2);
        const int extent = drawSize > 0 ? drawSize : size;
        if (!image) return false;
        g.DrawImage(image, Rect(x, y, extent, extent), Rect(index * stride, 0, size, size));
        return true;
    }

    void DrawJoin(Graphics& g)
    {
        g.SetColor(Color(0, 0, 0, 168));
        g.FillRect(Rect(0, 0, Width, Height));
        Image* tiles[3][3] = {
            {Sexy::IMAGE_DIALOG_TOPLEFT, Sexy::IMAGE_DIALOG_TOPMIDDLE, Sexy::IMAGE_DIALOG_TOPRIGHT},
            {Sexy::IMAGE_DIALOG_CENTERLEFT, Sexy::IMAGE_DIALOG_CENTERMIDDLE, Sexy::IMAGE_DIALOG_CENTERRIGHT},
            {Sexy::IMAGE_DIALOG_BIGBOTTOMLEFT, Sexy::IMAGE_DIALOG_BIGBOTTOMMIDDLE, Sexy::IMAGE_DIALOG_BIGBOTTOMRIGHT},
        };
        const int xs[] = {385, 419, 849, 883};
        const int ys[] = {255, 287, 462, 553};
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x)
                ImageAt(g, tiles[y][x], Rect(xs[x], ys[y], xs[x + 1] - xs[x], ys[y + 1] - ys[y]));
        ImageAt(g, Sexy::IMAGE_DIALOG_HEADER, Rect(546, 204, 184, 64));
        const Color gold(250, 214, 122);
        Label(g, "Select Second Player", 640, 303, Sexy::FONT_DWARVENTODCRAFT24, gold);
        Label(g, screen == Screen::Confirm ? "Player 2 successfully joined!" : "Player 2",
            640, 348, Sexy::FONT_BRIANNETOD16, gold);
        auto* font = Sexy::FONT_BRIANNETOD16;
        const auto prompt = [&](int baseline, const char* before, int button, bool showBoth, const char* after)
        {
            constexpr int iconSize = 28;
            constexpr int gap = 6;
            const int beforeWidth = font->StringWidth(before);
            const int afterWidth = font->StringWidth(after);
            const int orWidth = font->StringWidth("or");
            const int iconsWidth = showBoth ? 2 * iconSize + 2 * gap + orWidth : iconSize;
            const int width = beforeWidth + gap + iconsWidth + (afterWidth ? gap + afterWidth : 0);
            const int start = Width / 2 - width / 2;
            const int iconY = baseline - (font->GetAscent() + iconSize) / 2;
            TodDrawString(&g, before, start, baseline, font, gold, DS_ALIGN_LEFT);
            int x = start + beforeWidth + gap;
            DrawPadButton(g, button, x, iconY, false, showBoth || playStationPad, iconSize);
            x += iconSize + gap;
            if (showBoth)
            {
                TodDrawString(&g, "or", x, baseline, font, gold, DS_ALIGN_LEFT);
                x += orWidth + gap;
                DrawPadButton(g, button, x, iconY, false, false, iconSize);
                x += iconSize + gap;
            }
            if (afterWidth) TodDrawString(&g, after, x, baseline, font, gold, DS_ALIGN_LEFT);
        };
        const bool waiting = screen != Screen::Confirm;
        prompt(392, waiting ? "Connect a controller and press" : "Press", 0, waiting,
            waiting ? "" : "to continue.");
        prompt(439, "Press", 1, waiting, "or Esc to cancel.");
    }

    static Rect HelpRect() { return Rect(1132, 635, 124, 67); }
    static Rect CloseHelpRect() { return Rect(1089, 649, 154, 45); }

    void OpenHelp()
    {
        helpSeen = true;
        app.RegistryWriteBoolean("VersusHelpSeen", true);
        screen = Screen::Help;
    }

    void DrawHelp(Graphics& g)
    {
        g.SetColor(Color(0, 0, 0, 160));
        g.FillRect(Rect(0, 0, Width, Height));
        ImageAt(g, helpNote, Rect(250, 81, 780, 558));
        Label(g, "Vs. Mode", 640, 137, Sexy::FONT_HOUSEOFTERROR28);
        ImageAt(g, helpControllers, Rect(864, 143, 98, 98));
        ImageAt(g, helpTargets, Rect(310, 296, 98, 98));
        ImageAt(g, helpBrains, Rect(864, 425, 98, 98));
        const Color ink(41, 32, 21);
        const auto line = [&](const char* words, int x, int y)
        {
            TodDrawString(&g, words, x, y, Sexy::FONT_BRIANNETOD16, ink, DS_ALIGN_LEFT);
        };
        line("In Custom Battle, take turns picking seed packets.", 314, 186);
        line("The match starts when both decks are full.", 314, 203);
        line("Player 1 uses the mouse. Player 2 uses a controller.", 314, 220);
        line("As Plants:", 442, 317);
        line("Defend the lawn to the left of the red line.", 442, 334);
        line("Collect sun to plant. Knock out three targets to win.", 442, 351);
        line("As Zombies:", 314, 453);
        line("Place gravestones to generate brains.", 314, 470);
        line("Spend brains on zombies. Get one into the house to win.", 314, 487);
        line("After five minutes, Sudden Death speeds up the battle.", 314, 547);
        const Rect close = CloseHelpRect();
        Label(g, "Close", close.mX + 35, close.mY + 30, Sexy::FONT_HOUSEOFTERROR20);
        DrawPadButton(g, 1, close.mX + 66, close.mY + 1, false, playStationPad);
        Label(g, "/ Esc", close.mX + 130, close.mY + 30, Sexy::FONT_BRIANNETOD16);
    }

    void ReturnToSides()
    {
        if (app.GetDialog(DIALOG_PAUSED)) app.KillDialog(DIALOG_PAUSED);
        readyAnimation = REANIMATIONID_NULL;
        app.mGameMode = GAMEMODE_ADVENTURE;
        app.ShowGameSelector();
        screen = playerTwo ? Screen::Sides : Screen::Join;
    }

public:
    explicit VersusMode(LawnApp& lawn) : app(lawn) {}
    ~VersusMode()
    {
        if (!ownsGamepads) return;
        auto close = Sdl<decltype(&SDL_CloseGamepad)>("SDL_CloseGamepad");
        for (const auto& pad : pads) close(pad.handle);
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    }

    bool Active() const { return screen != Screen::Closed; }
    bool CoopUsesNativeInput() const { return screen == Screen::CoopPlay; }

    void SetMousePosition(int x, int y)
    {
        pointerX = x;
        pointerY = y;
    }

    bool PauseDialogOpen() const { return screen == Screen::Match && app.GetDialogCount() > 0; }

    bool HandleConcedeMouse(int kind, int x, int y, int button)
    {
        if (button != 1 || (kind != 1 && kind != 2)) return false;
        if (!PauseDialogOpen() || !app.GetDialog(DIALOG_NEWOPTIONS))
        {
            concedeMouseDown = false;
            return false;
        }
        const bool over = ConcedeRect().Contains(x, y);
        if (kind == 1)
        {
            concedeMouseDown = over;
            return over;
        }
        const bool captured = concedeMouseDown;
        concedeMouseDown = false;
        if (captured && over)
        {
            if (concedingPlayer == 0) ConcedeMatch(0);
            else if (concedingPlayer < 0) concedingPlayer = 0;
        }
        return captured;
    }

    void TogglePause()
    {
        if (concedingPlayer >= 0)
        {
            concedingPlayer = -1;
            return;
        }
        if (app.GetDialog(DIALOG_ALMANAC)) app.KillAlmanacDialog();
        else if (app.GetDialogCount() > 1) return;
        else if (app.GetDialog(DIALOG_NEWOPTIONS)) app.KillNewOptionsDialog();
        else if (app.GetDialog(DIALOG_PAUSED)) app.KillDialog(DIALOG_PAUSED);
        else if (!PauseDialogOpen()) app.DoNewOptions(false);
        if (readyAnimation != REANIMATIONID_NULL) app.mBoard->mPaused = true;
    }

    void LostFocus()
    {
        if (((screen == Screen::Match && !PauseDialogOpen()) ||
            (screen == Screen::CoopPlay && !app.GetDialogCount())) &&
            app.mBoard && app.mGameScene == SCENE_PLAYING) app.DoPauseDialog();
    }

    void RestartMatch()
    {
        StartMatch();
    }

    void Escape(int kind)
    {
        if (kind != 1)
        {
            escapeHeld = false;
            return;
        }
        if (escapeHeld) return;
        escapeHeld = true;
        if (screen == Screen::Match) TogglePause();
        else if (screen == Screen::Draft) BackFromDraft();
        else if (screen == Screen::Help) screen = Screen::Sides;
        else if (screen != Screen::Win && screen != Screen::TrophyFlash) Close();
    }

    bool InitializeGamepads(const char* title, bool showError = true)
    {
        if (!ownsGamepads)
        {
            auto hint = Sdl<decltype(&SDL_SetHint)>("SDL_SetHint");
            hint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
            if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
            {
                if (showError) app.DoDialog(DIALOG_INFO, true, title, SDL_GetError(), "OK", Dialog::BUTTONS_FOOTER);
                return false;
            }
            ownsGamepads = true;
        }
        return true;
    }

    void LoadCoopCursors()
    {
        seedCursor[0] = ImageAsset("images/seed_selector");
        boardCursor[0] = ImageAsset("images/board_selector");
        playerLabel[0] = ImageAsset("images/p1_text");
        playerArrow[0] = ImageAsset("images/p1_arrow");
        seedCursor[1] = ImageAsset("images/seed_selector_BLUE");
        boardCursor[1] = ImageAsset("images/board_selector_BLUE");
        playerLabel[1] = ImageAsset("images/p2_text");
        playerArrow[1] = ImageAsset("images/p2_arrow");
    }

    void DrawDropIn(Graphics& g)
    {
        if (dropIn && screen == Screen::Closed)
        {
            g.ClearClipRect();
            DrawCoopPlay(g);
        }
    }

    void OpenCoop()
    {
        if (!Sexy::IMAGE_BACKGROUND1) TodLoadResources("DelayLoad_Background1");
        if (!Sexy::IMAGE_CHALLENGE_BACKGROUND) TodLoadResources("DelayLoad_ChallengeScreen");
        if (!InitializeGamepads("Co-op")) return;
        coop = true;
        dropIn = false;
        coopLevel = 0;
        coopSunCore = ImageAsset("coop/sun-core");
        coopTrophy = ImageAsset("coop/trophy");
        coopSurvivalThumbs = ImageAsset("coop/survival-thumbnails");
        coopChallengeThumbs = ImageAsset("coop/thumbnails");
        coopDoubleSun = ImageAsset("coop/double-sun");
        coopDoubleSunGlow = ImageAsset("coop/double-sun-glow");
        resultBackground = ImageAsset("images/Challenge_Background");
        helpButtons = ImageAsset("images/help_buttons");
        xboxButtons = ImageAsset("images/xbox_help_buttons");
        LoadCoopCursors();
        for (int i = 0; i < CoopLevelCount; ++i)
        {
            coopCompletion[i] = false;
            app.RegistryReadBoolean("CoopCompleted" + std::to_string(i), &coopCompletion[i]);
        }
        playerTwo = 0;
        escapeHeld = false;
        resume = Screen::Closed;
        screen = Screen::Join;
        nextScan = 0;
    }

    void Open()
    {
        if (!gReanimatorDefArray[REANIM_ZOMBIE_TARGET].mTracks.tracks ||
            !gReanimatorDefArray[REANIM_VS_GRAVESTONE].mTracks.tracks)
        {
            app.DoDialog(DIALOG_INFO, true, "Versus Mode", "Install the Versus animation assets before starting a match.", "OK", Dialog::BUTTONS_FOOTER);
            return;
        }
        if (!Sexy::IMAGE_BACKGROUND1 && !TodLoadResources("DelayLoad_Background1"))
        {
            app.DoDialog(DIALOG_INFO, true, "Versus Mode", "The day lawn could not be loaded.", "OK", Dialog::BUTTONS_FOOTER);
            return;
        }
        if (!InitializeGamepads("Versus Mode")) return;
        coop = false;
        dropIn = false;
        app.RegistryReadBoolean("VersusHelpSeen", &helpSeen);
        helpNote = ImageAsset("images/mail_background_note2");
        helpControllers = ImageAsset("images/help_menu_image_vs_controllers");
        helpTargets = ImageAsset("images/help_menu_image_vs_targets");
        helpBrains = ImageAsset("images/help_menu_image_vs_brains");
        setupBack = ImageAsset("images/vs_setup_back");
        plantBank = ImageAsset("images/SeedBank");
        zombieBank = ImageAsset("images/zombieSeedBank");
        brainImage = ImageAsset("reanim/Brain");
        brainGlowBig = ImageAsset("reanim/BrainGlowBig");
        brainGlowSmall = ImageAsset("reanim/BrainGlowSmall");
        graveIcon = ImageAsset("images/mp_tombstone");
        buttonImage = ImageAsset("images/VS Button");
        buttonSelected = ImageAsset("images/VS Button_selected");
        draftPanels[0] = ImageAsset("images/SeedChooser_Background");
        draftPanels[1] = ImageAsset("images/SeedChooser_Background2");
        playerTwoGamepad = ImageAsset("images/gamepad1");
        xboxGamepad = ImageAsset("images/xbox_gamepad0");
        xboxZombieSelected = ImageAsset("images/xbox_zombie_side_selected");
        sidePortrait[0] = ImageAsset("images/plant_side");
        sidePortrait[1] = ImageAsset("images/zombie_side");
        sideSelected[0] = ImageAsset("images/plant_side_selected");
        sideSelected[1] = ImageAsset("images/zombie_side_selected");
        sideLabel[0] = ImageAsset("images/plant_side_plants");
        sideLabel[1] = ImageAsset("images/zombie_side_zombies");
        seedCursor[0] = ImageAsset("images/seed_selector");
        seedCursor[1] = ImageAsset("images/seed_selector_BLUE");
        boardCursor[0] = ImageAsset("images/board_selector");
        boardCursor[1] = ImageAsset("images/board_selector_BLUE");
        playerLabel[0] = ImageAsset("images/p1_text");
        playerLabel[1] = ImageAsset("images/p2_text");
        playerArrow[0] = ImageAsset("images/p1_arrow");
        playerArrow[1] = ImageAsset("images/p2_arrow");
        zombiePacket = ImageAsset("images/seedpacket_Zombie");
        targetCounter = ImageAsset("images/vs_targets");
        redCross = ImageAsset("images/vs_targets_x");
        resultBackground = ImageAsset("images/Challenge_Background");
        winLabel = ImageAsset("images/win");
        zombieHead = ImageAsset("extension/reanim/Zombie_head");
        gamerpic = ImageAsset("images/no_gamerpic");
        resultPanel[0] = ImageAsset("images/vs_info_box_plants");
        resultPanel[1] = ImageAsset("images/vs_info_box_zombies");
        trophy[0] = ImageAsset("images/plant_win_trophy_hi_res");
        trophy[1] = ImageAsset("images/zombie_win_trophy_hi_res");
        trophyBase = ImageAsset("images/trophy_BaseLarge");
        helpButtons = ImageAsset("images/help_buttons");
        helpButtonsSmall = ImageAsset("images/help_buttons_small");
        xboxButtons = ImageAsset("images/xbox_help_buttons");
        xboxButtonsSmall = ImageAsset("images/xbox_help_buttons_small");
        escapeHeld = false;
        for (auto& history : winHistory) history.clear();
        versusStreak[0] = versusStreak[1] = 0;
        concedingPlayer = -1;
        for (int i = 0; i < 3; ++i)
        {
            damageImages[i] = app.GetSharedImage("../versus/reanim/zombie_target_damage0" + std::to_string(i + 1));
        }
        playerTwo = 0;
        resume = Screen::Closed;
        screen = Screen::Join;
        nextScan = 0;
    }

    void Close()
    {
        if (coop && app.mLocalCoop) app.DoBackToMain();
        if (app.mGameMode == GAMEMODE_VERSUS) ReturnToSides();
        screen = Screen::Closed;
    }

    void Update()
    {
        if (!Active())
        {
            if (app.mBoard && !app.mLocalCoop && app.mGameMode != GAMEMODE_VERSUS &&
                app.mGameMode != GAMEMODE_CHALLENGE_HEAVY_WEAPON &&
                (app.mGameScene == SCENE_PLAYING || app.mGameScene == SCENE_LEVEL_INTRO))
            {
                if (!ownsGamepads && !triedDropInGamepads)
                {
                    triedDropInGamepads = true;
                    InitializeGamepads("Player 2", false);
                }
                if (!dropIn) playerTwo = 0;
                if (ownsGamepads) PollPads();
            }
            else
            {
                dropIn = false;
                playerTwo = 0;
            }
            return;
        }
        if (coop)
        {
            if ((screen == Screen::CoopPlay || resume == Screen::CoopPlay) && (!app.mLocalCoop || !app.mBoard))
            {
                screen = Screen::Closed;
                return;
            }
            PollPads();
            if (screen == Screen::CoopPlay) UpdateCoop();
            return;
        }
        if (screen == Screen::Match && (!app.mBoard || app.mGameMode != GAMEMODE_VERSUS))
        {
            screen = Screen::Closed;
            return;
        }
        PollPads();
        if (screen == Screen::Draft && draftFlight && --draftFlight == 0)
        {
            if (++draftTurn == 10) StartMatch();
        }
        if (screen == Screen::Match)
        {
            if (PauseDialogOpen()) return;
            concedingPlayer = -1;
            concedeMouseDown = false;
            if (auto* ready = app.ReanimationTryToGet(readyAnimation))
            {
                app.mBoard->mPaused = true;
                ready->Update();
                if (!ready->mDead) return;
            }
            if (readyAnimation != REANIMATIONID_NULL)
            {
                readyAnimation = REANIMATIONID_NULL;
                app.mBoard->mPaused = false;
            }
            UpdateMatch();
        }
        else if (screen == Screen::Win) UpdateWin();
        else if (screen == Screen::TrophyFlash)
        {
            ++trophyFlashTicks;
            if (trophyFlashTicks == WinningCueTicks - 200)
                app.PlaySample(Sexy::SOUND_LIGHTFILL);
            if (trophyFlashTicks >= WinningCueTicks) screen = Screen::Result;
        }
    }

    void Mouse(int kind, int x, int y, int button)
    {
        SetMousePosition(x, y);
        if (screen == Screen::CoopSelect)
        {
            if (kind == 0 || (kind == 1 && button == 1))
            {
                if (kind == 1 && CoopBackRect().Contains(x, y))
                {
                    Close();
                    return;
                }
                for (int index = 0; index < CoopLevelCount; ++index)
                {
                    if (!CoopTile(index).Contains(x, y)) continue;
                    SelectCoopLevel(index);
                    if (kind == 1) StartCoopLevel();
                    break;
                }
            }
            return;
        }
        if (screen == Screen::Draft)
        {
            const int side = MouseSide();
            for (int id = 0; id < CardCount(side); ++id)
                if (DraftRect(side, id).Contains(x, y)) mouseDraftCursor = id;
        }
        if (kind != 1 || button != 1) return;
        if (screen == Screen::Help)
        {
            if (CloseHelpRect().Contains(x, y)) screen = Screen::Sides;
        }
        else if (screen == Screen::Sides)
        {
            if (HelpRect().Contains(x, y))
            {
                OpenHelp();
                return;
            }
            for (int side = 0; side < 2; ++side)
                if (SideRect(side).Contains(x, y)) p1Plants = side == 0;
            for (int index = 0; index < 3; ++index)
                if (OptionRect(index).Contains(x, y)) { SelectBattle(index); break; }
        }
        else if (screen == Screen::Draft && DraftSide() == MouseSide())
        {
            for (int id = 1; id < CardCount(DraftSide()); ++id)
                if (DraftRect(DraftSide(), id).Contains(x, y)) { PickCard(id); break; }
        }
        else if (screen == Screen::Match)
        {
            if (readyAnimation != REANIMATIONID_NULL) return;
            const int side = MouseSide();
            const int start = side == 0 ? PlantBank : ZombieBank;
            if (Collect(side, x, y)) return;
            if (x >= start && x < start + 6 * CardWidth && y >= CardY && y < CardY + SEED_PACKET_HEIGHT)
                selected[side] = (x - start) / CardWidth;
            else
            {
                int col, lane;
                if (TileAt(x, y, col, lane)) Place(side, col, lane);
            }
        }
        else if (screen == Screen::Win && MouseWon() && TrophyLanded() && TrophyRect().Contains(x, y))
            CollectTrophy();
        else if (screen == Screen::Result)
        {
            if (ResultRect(0).Contains(x, y)) ReturnToSides();
            else if (ResultRect(1).Contains(x, y)) Close();
        }
    }

    void Draw(Graphics& g)
    {
        if (!Active()) return;
        g.ClearClipRect();
        if (coop)
        {
            if (screen == Screen::CoopSelect) DrawCoopMenu(g);
            else if (screen == Screen::CoopPlay) DrawCoopPlay(g);
            else
            {
                if (!app.mBoard && Sexy::IMAGE_BACKGROUND1)
                    g.DrawImageF(Sexy::IMAGE_BACKGROUND1,
                        app.mDDInterface->mWideScreenOffsetX + WIDESCREEN_OFFSETX - BOARD_OFFSET,
                        app.mDDInterface->mWideScreenOffsetY + WIDESCREEN_OFFSETY);
                DrawJoin(g);
            }
            return;
        }
        if (screen == Screen::Result)
        {
            DrawResults(g);
            return;
        }
        if (PauseDialogOpen())
        {
            DrawBanks(g, true);
            if (app.GetDialog(DIALOG_NEWOPTIONS))
            {
                const Rect button = ConcedeRect();
                if (concedingPlayer == 1)
                    ConsoleButton(g, button, "P2 concede?", "Cross / A", true);
                else
                    ConsoleButton(g, button, concedingPlayer == 0 ? "Confirm" : "Concede", "Match", true);
                if (concedingPlayer < 0)
                    DrawPadButton(g, 3, button.mX + button.mWidth + 4, button.mY + 16, false, playStationPad, 30);
            }
            return;
        }
        if (app.mGameMode == GAMEMODE_VERSUS)
        {
            DrawMatch(g);
            if (screen == Screen::Win) DrawWin(g);
            if (screen == Screen::TrophyFlash)
            {
                DrawWin(g);
                const int fadeStart = WinningCueTicks - 200;
                const int alpha = ClampInt((trophyFlashTicks - fadeStart) * 255 / 195, 0, 255);
                g.SetColor(Color(255, 255, 255, alpha));
                g.FillRect(Rect(0, 0, Width, Height));
                return;
            }
            if (screen == Screen::Match || screen == Screen::Win) return;
        }
        else
        {
            if (Sexy::IMAGE_BACKGROUND1)
                g.DrawImageF(Sexy::IMAGE_BACKGROUND1,
                    app.mDDInterface->mWideScreenOffsetX + WIDESCREEN_OFFSETX - BOARD_OFFSET,
                    app.mDDInterface->mWideScreenOffsetY + WIDESCREEN_OFFSETY);
            Board::DrawVersusDivider(g, app.mDDInterface->mWideScreenOffsetX + LAWN_XMIN + 80 * PlantColumns,
                app.mDDInterface->mWideScreenOffsetY + LAWN_YMIN, 500);
        }

        if (screen == Screen::Sides || screen == Screen::Help)
        {
            DrawBanks(g, false);
            ImageAt(g, setupBack, Rect(250, 133, 780, 466));
            Label(g, "Pick Sides", 640, 173, Sexy::FONT_DWARVENTODCRAFT24);
            for (int side = 0; side < 2; ++side)
            {
                const auto rect = SideRect(side);
                const int player = side == MouseSide() ? 0 : 1;
                if (player == 1)
                {
                    const Rect portrait(rect.mX + (side == 1 ? 14 : 0), rect.mY, rect.mWidth, rect.mHeight);
                    if (side == 1 && xboxPad && (Image*)xboxZombieSelected)
                    {
                        ImageAt(g, sidePortrait[side], rect);
                        ImageAt(g, xboxZombieSelected, portrait);
                    }
                    else ImageAt(g, sideSelected[side], portrait);
                }
                ImageAt(g, sideLabel[side], rect);
                if (player == 0)
                    Label(g, "(Mouse)", rect.mX + rect.mWidth / 2, rect.mY - 25, Sexy::FONT_DWARVENTODCRAFT24);
                ImageAt(g, playerLabel[player], Rect(rect.mX + (rect.mWidth - 34) / 2, rect.mY + rect.mHeight + 1, 34, 25));
            }
            ImageAt(g, xboxPad && (Image*)xboxGamepad ? (Image*)xboxGamepad : (Image*)playerTwoGamepad,
                Rect(p1Plants ? 651 : 484, 276, 111, 80));
            ConsoleButton(g, OptionRect(0), "Quick", "Play", option == 0);
            ConsoleButton(g, OptionRect(1), "Custom", "Battle", option == 1);
            ConsoleButton(g, OptionRect(2), "Random", "Battle", option == 2);
            const Rect help = HelpRect();
            ImageAt(g, option == 3 || help.Contains(pointerX, pointerY) ? (Image*)buttonSelected : (Image*)buttonImage, help);
            Label(g, "Help", help.mX + help.mWidth / 2, help.mY + 43, Sexy::FONT_DWARVENTODCRAFT24, Color(104, 213, 59));
            if (screen == Screen::Help) DrawHelp(g);
        }
        else if (screen == Screen::Draft)
            DrawDraft(g);
        else
            DrawJoin(g);
    }
};
