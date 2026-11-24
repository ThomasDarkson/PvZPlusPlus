class VersusMode
{
    enum class Screen { Closed, Join, Confirm, Sides, Draft, Match, Win, TrophyFlash, Result };
    struct Card
    {
        const char* label;
        int type;
        int cost;
        int recharge;
        int suddenRecharge;
    };
    struct Brain { int column; int row; int age; bool falling; };
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
    static constexpr int CardCount = 18;
    static constexpr int Rows = 5;
    static constexpr int PlantColumns = 6;
    static constexpr int LastColumn = 8;
    static constexpr int SuddenDeathTick = 30000;
    static constexpr int BankTop = 23;
    static constexpr int CardY = BankTop + 4;
    static constexpr int DraftTopShift = 33;
    static constexpr int CardWidth = 53;
    static constexpr int PlantBank = 312;
    static constexpr int ZombieBank = 653;
    static constexpr int IrisTicks = 68;
    static constexpr int TrophyDropTicks = 54;

    LawnApp& app;
    Screen screen = Screen::Closed;
    Screen resume = Screen::Closed;
    std::vector<Pad> pads;
    SDL_JoystickID playerTwo = 0;
    int nextScan = 0;
    bool ownsGamepads = false;
    bool escapeHeld = false;
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
    std::array<std::vector<int>, 2> winHistory;
    std::array<ZombieID, Rows> targets{};
    std::array<std::array<ZombieID, Rows>, 3> graves{};
    std::array<std::array<int, 6>, 2> decks{};
    std::vector<Brain> drops;
    SharedImageRef setupBack, plantBank, zombieBank, brainImage, brainGlowBig, brainGlowSmall, graveIcon;
    SharedImageRef buttonImage, buttonSelected, draftPanels[2], playerTwoGamepad, sidePortrait[2], sideSelected[2], sideLabel[2];
    SharedImageRef seedCursor[2], boardCursor[2], playerLabel[2], playerArrow[2];
    SharedImageRef zombiePacket, targetCounter, redCross, resultBackground, winLabel, zombieHead;
    SharedImageRef resultPanel[2], trophy[2], trophyBase, helpButtons, helpButtonsSmall;
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
                if (screen != Screen::Result && screen != Screen::TrophyFlash && (screen != Screen::Win || !MouseWon()))
                {
                    if (screen != Screen::Confirm)
                        resume = screen == Screen::Match || screen == Screen::Draft ||
                            screen == Screen::Sides || screen == Screen::Win ? screen : Screen::Closed;
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
                controllerAxisX = controllerAxisY = 0;
                screen = Screen::Confirm;
                return;
            }
            if (pad.id != playerTwo) continue;
            const int analogX = axis(pad.handle, SDL_GAMEPAD_AXIS_LEFTX);
            const int analogY = axis(pad.handle, SDL_GAMEPAD_AXIS_LEFTY);
            if (screen == Screen::Match && !PauseDialogOpen()) MoveController(analogX, analogY, buttons);
            const int newX = analogX > 16000 ? 1 : analogX < -16000 ? -1 : 0;
            const int newY = analogY > 16000 ? 1 : analogY < -16000 ? -1 : 0;
            if (newX && newX != controllerAxisX)
                pressed |= Button(newX > 0 ? SDL_GAMEPAD_BUTTON_DPAD_RIGHT : SDL_GAMEPAD_BUTTON_DPAD_LEFT);
            if (newY && newY != controllerAxisY)
                pressed |= Button(newY > 0 ? SDL_GAMEPAD_BUTTON_DPAD_DOWN : SDL_GAMEPAD_BUTTON_DPAD_UP);
            controllerAxisX = newX;
            controllerAxisY = newY;
            Controller(pressed);
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
            screen = resume == Screen::Closed ? Screen::Sides : resume;
            if (screen == Screen::Match)
                app.mBoard->mPaused = readyAnimation != REANIMATIONID_NULL || PauseDialogOpen();
            else if (screen == Screen::Win)
                app.mBoard->mPaused = winTicks >= IrisTicks;
            resume = Screen::Closed;
        }
        else if (screen == Screen::Sides)
        {
            option = ClampInt(option + dx, 0, 2);
            if (accept) SelectBattle(option);
        }
        else if (screen == Screen::Draft)
        {
            draftCursor = ClampInt(draftCursor + dx + dy * DraftColumns(ControllerSide()), 1, CardCount - 1);
            if (accept && !draftFlight && DraftSide() == ControllerSide()) PickCard(draftCursor);
        }
        else if (screen == Screen::Match)
        {
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
            for (auto& deck : decks)
            {
                std::array<int, CardCount - 1> pool{};
                for (int i = 0; i < CardCount - 1; ++i) pool[i] = i + 1;
                for (int i = 0; i < 5; ++i)
                {
                    std::swap(pool[i], pool[i + Rand(CardCount - 1 - i)]);
                    deck[i + 1] = pool[i];
                }
            }
        }
        StartMatch();
    }

    void PickCard(int card)
    {
        auto& deck = decks[DraftSide()];
        if (draftFlight || std::find(deck.begin(), deck.end(), card) != deck.end()) return;
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
                    coin->Collect();
                    return true;
                }
            }
        }
        else
        {
            for (auto it = drops.begin(); it != drops.end(); ++it)
            {
                const int centerX = GridScreenX(it->column, it->row) + 39;
                const int centerY = BrainCenterY(*it);
                if (Rect(centerX - 30, centerY - 30, 60, 60).Contains(x, y))
                {
                    brains += 25;
                    app.PlaySample(Sexy::SOUND_CHIME);
                    drops.erase(it);
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

    void CollectTrophy()
    {
        app.PlaySample(Sexy::SOUND_SEEDLIFT);
        app.PlaySample(Sexy::SOUND_TAP2);
        const int winningSide = plantsWon ? 0 : 1;
        const int winner = winningSide == MouseSide() ? 0 : 1;
        winHistory[winner].push_back(winningSide);
        if (winHistory[winner].size() > 9) winHistory[winner].erase(winHistory[winner].begin());
        resultOption = 0;
        trophyFlashTicks = 0;
        screen = Screen::TrophyFlash;
    }

    void UpdateWin()
    {
        if (winTicks == IrisTicks - 1)
        {
            ++winHoldTicks;
            if (!plantsWon && winHoldTicks < 85) return;
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
            app.PlayFoley(FOLEY_SCREAM);
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
        for (auto& drop : drops) ++drop.age;
        drops.erase(std::remove_if(drops.begin(), drops.end(),
            [this](const Brain& drop) { return BrainLifeRemaining(drop) <= 0; }), drops.end());
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
    static int DraftColumns(int side) { return side == 0 ? 8 : 5; }
    static Rect DraftRect(int side, int card)
    {
        const int columns = DraftColumns(side);
        return Rect((side == 0 ? 274 : 730) + card % columns * 52,
            (side == 0 ? 189 : 182) - DraftTopShift + card / columns * 74, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
    }

    static Rect BankRect(int side, int slot)
    {
        return Rect((side == 0 ? PlantBank : ZombieBank) + slot * CardWidth, CardY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
    }

    void DrawDivider(Graphics& g, int x, int top, int height)
    {
        g.SetColor(Color(120, 35, 21));
        g.FillRect(Rect(x - 3, top, 7, height));
        g.SetColor(Color(195, 56, 38));
        for (int y = top; y < top + height; y += 6)
        {
            const int offset = ((y * 17) % 13) / 3 - 2;
            g.FillRect(Rect(x + offset - 2, y, 5, pvz_min(7, top + height - y)));
        }
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
        if (card.type == ZOMBIE_DOOR && card.cost == 50 && Sexy::IMAGE_REANIM_ZOMBIE_TRASHCAN1)
        {
            ImageAt(g, Sexy::IMAGE_REANIM_ZOMBIE_TRASHCAN1, Rect(rect.mX + 11, rect.mY + 18, 28, 31));
            ImageAt(g, zombieHead, Rect(rect.mX + 14, rect.mY + 4, 23, 19));
            return;
        }
        auto* display = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
        if (!display || display->h <= 0) return;
        const float scale = display->h / 600.0f;
        g.PushState();
        g.SetClipRect(rect.mX + 4, rect.mY + 3, rect.mWidth - 8, 49);
        const bool gargantuar = card.type == ZOMBIE_GARGANTUAR;
        g.mScaleX = g.mScaleY = (gargantuar ? 0.225f : 0.37f) / scale;
        app.mReanimatorCache->DrawCachedZombie(&g, rect.mX + (gargantuar ? 2.0f : -7.0f),
            rect.mY + (gargantuar ? 3.0f : -11.0f), static_cast<ZombieType>(card.type));
        g.PopState();
    }

    void DrawCard(Graphics& g, int side, int id, Rect rect, int timer, bool dark, bool disabled)
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
            DrawSeedPacket(&g, rect.mX, rect.mY, static_cast<SeedType>(card.type), SEED_NONE, 0.0f, 255, false, false);
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
            for (int id = 0; id < CardCount; ++id)
            {
                DrawCard(g, side, id, DraftRect(side, id), 0,
                    side != active || std::find(deck.begin(), deck.end(), id) != deck.end(), false);
            }
            const auto rect = DraftRect(side, cursor);
            DrawCursor(g, Rect(rect.mX - 5, rect.mY - 4, 59, 78), side == MouseSide() ? 0 : 1, false);
            if (side == active && !draftFlight)
            {
                g.SetColor(Color(255, 241, 181));
                const int tooltipX = ClampInt(rect.mX - 38, 255, 863);
                g.FillRect(Rect(tooltipX, rect.mY + 72, 148, 29));
                Label(g, GetCard(side, cursor).label, tooltipX + 74, rect.mY + 93, Sexy::FONT_BRIANNETOD16, Color(45, 35, 24));
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
        DrawCursor(g, Rect(x + 2, y, 76, 96), player, true);
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

    void DrawBrain(Graphics& g, const Brain& drop)
    {
        const int age = drop.age;
        const int remaining = BrainLifeRemaining(drop);
        const int x = GridScreenX(drop.column, drop.row) + 39;
        const int y = BrainCenterY(drop);
        const int size = 92 + int(10.0f * std::sin(age * 0.078f));
        g.PushState();
        if (remaining < 75)
        {
            g.SetColorizeImages(true);
            g.SetColor(Color(255, 255, 255, ClampInt(255 * remaining / 75, 0, 255)));
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
        DrawDivider(g, GridScreenX(PlantColumns, 0), GridScreenY(0, 0), 500);
        DrawBanks(g, true);
        int padCol, padLane;
        const int padX = int(controllerX), padY = int(controllerY);
        const int side = ControllerSide();
        const auto& controllerCard = GetCard(side, decks[side][selected[side]]);
        if (TileAt(padX, padY, padCol, padLane) &&
            (side == 0 ? padCol < PlantColumns || controllerCard.type == SEED_GRAVEBUSTER : padCol >= PlantColumns))
            DrawPlacement(g, padX - 40, padY - 50, 1);
        else DrawCursor(g, Rect(padX - 38, padY - 48, 76, 96), 1, true);
        for (int col = 0; col <= LastColumn; ++col)
        {
            for (int lane = 0; lane < Rows; ++lane)
            {
                const Rect tile(GridScreenX(col, lane), GridScreenY(col, lane), 80, 100);
                if (tile.Contains(pointerX, pointerY))
                {
                    const int side = MouseSide();
                    const auto& card = GetCard(side, decks[side][selected[side]]);
                    const bool validSide = side == 0 ? col < PlantColumns || card.type == SEED_GRAVEBUSTER
                        : col >= PlantColumns;
                    if (validSide) DrawPlacement(g, tile.mX, tile.mY, 0);
                }
            }
        }
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

    void DrawIris(Graphics& g, float radius)
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

    void DrawWin(Graphics& g)
    {
        if (winTicks < 2 * IrisTicks)
        {
            const float progress = std::abs(winTicks - IrisTicks) / float(IrisTicks);
            DrawIris(g, 25.0f + 1480.0f * progress * progress);
            return;
        }
        const auto trophyRect = TrophyRect();
        const int center = trophyRect.mX + trophyRect.mWidth / 2;
        const int top = trophyRect.mY;
        if (TrophyLanded())
        {
            for (int i = 0; i < 24; ++i)
            {
                const int age = (winTicks - 2 * IrisTicks - TrophyDropTicks + i * 8) % 95;
                const float angle = i * 2.39996f;
                const int x = center + int(std::cos(angle) * (28 + age * 2));
                const int y = top + 63 + int(std::sin(angle) * (28 + age * 2));
                g.SetColor(Color(255, 239, 139, 235 - age * 2));
                g.FillRect(Rect(x - 3, y - 1, 7, 3));
                g.FillRect(Rect(x - 1, y - 3, 3, 7));
            }
        }
        ImageAt(g, trophy[plantsWon ? 0 : 1], trophyRect);
        if (TrophyLanded())
        {
            if (!MouseWon() && playStationPad)
            {
                DrawPlayStationButton(g, 0, center - 73, top + 142, true);
                Label(g, "Collect trophy", center + 13, top + 162, Sexy::FONT_BRIANNETOD16);
            }
            else Label(g, MouseWon() ? "Click the trophy" : "Press A to collect", center, top + 162, Sexy::FONT_BRIANNETOD16);
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
            ImageAt(g, resultPanel[side], Rect(594, y, 422, 172));
            const auto playerName = "Player " + std::to_string(player + 1);
            Label(g, playerName.c_str(), 670, y + 35, Sexy::FONT_DWARVENTODCRAFT24);
            if (side == 0)
                ImageAt(g, Sexy::IMAGE_REANIM_SUNFLOWER_HEAD_SING1, Rect(615, y + 48, 54, 53));
            else
                ImageAt(g, zombieHead, Rect(615, y + 48, 53, 59));
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

    void DrawPlayStationButton(Graphics& g, int index, int x, int y, bool small)
    {
        Image* image = small ? (Image*)helpButtonsSmall : (Image*)helpButtons;
        const int size = small ? 21 : 42;
        if (image) g.DrawImage(image, x, y, Rect(index * size, 0, size, size));
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
        if (screen == Screen::Confirm && !playStationPad)
        {
            Label(g, "Press A to continue.", 640, 392, Sexy::FONT_BRIANNETOD16, gold);
            Label(g, "Press B or Esc to cancel.", 640, 437, Sexy::FONT_BRIANNETOD16, gold);
            return;
        }
        if (screen == Screen::Confirm)
        {
            Label(g, "Press", 579, 392, Sexy::FONT_BRIANNETOD16, gold);
            DrawPlayStationButton(g, 0, 614, 360, false);
            Label(g, "to continue.", 709, 392, Sexy::FONT_BRIANNETOD16, gold);
        }
        else
        {
            Label(g, "Connect a controller and press", 599, 392, Sexy::FONT_BRIANNETOD16, gold);
            DrawPlayStationButton(g, 0, 759, 360, false);
            Label(g, "or A", 829, 392, Sexy::FONT_BRIANNETOD16, gold);
        }
        Label(g, "Press", 555, 439, Sexy::FONT_BRIANNETOD16, gold);
        DrawPlayStationButton(g, 1, 590, 407, false);
        Label(g, "or B / Esc to cancel.", 721, 439, Sexy::FONT_BRIANNETOD16, gold);
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

    bool PauseDialogOpen() const { return screen == Screen::Match && app.GetDialogCount() > 0; }

    void TogglePause()
    {
        if (app.GetDialog(DIALOG_ALMANAC)) app.KillAlmanacDialog();
        else if (app.GetDialogCount() > 1) return;
        else if (app.GetDialog(DIALOG_NEWOPTIONS)) app.KillNewOptionsDialog();
        else if (app.GetDialog(DIALOG_PAUSED)) app.KillDialog(DIALOG_PAUSED);
        else if (!PauseDialogOpen()) app.DoNewOptions(false);
        if (readyAnimation != REANIMATIONID_NULL) app.mBoard->mPaused = true;
    }

    void LostFocus()
    {
        if (screen == Screen::Match && !PauseDialogOpen() && app.mBoard) app.DoPauseDialog();
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
        else if (screen != Screen::Win && screen != Screen::TrophyFlash) Close();
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
        if (!ownsGamepads)
        {
            auto hint = Sdl<decltype(&SDL_SetHint)>("SDL_SetHint");
            hint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
            if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
            {
                app.DoDialog(DIALOG_INFO, true, "Versus Mode", SDL_GetError(), "OK", Dialog::BUTTONS_FOOTER);
                return;
            }
            ownsGamepads = true;
        }
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
        resultPanel[0] = ImageAsset("images/vs_info_box_plants");
        resultPanel[1] = ImageAsset("images/vs_info_box_zombies");
        trophy[0] = ImageAsset("images/plant_win_trophy_hi_res");
        trophy[1] = ImageAsset("images/zombie_win_trophy_hi_res");
        trophyBase = ImageAsset("images/trophy_BaseLarge");
        helpButtons = ImageAsset("images/help_buttons");
        helpButtonsSmall = ImageAsset("images/help_buttons_small");
        escapeHeld = false;
        for (auto& history : winHistory) history.clear();
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
        if (app.mGameMode == GAMEMODE_VERSUS) ReturnToSides();
        screen = Screen::Closed;
    }

    void Update()
    {
        if (!Active()) return;
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
        else if (screen == Screen::TrophyFlash && ++trophyFlashTicks >= 42) screen = Screen::Result;
    }

    void Mouse(int kind, int x, int y, int button)
    {
        pointerX = x;
        pointerY = y;
        if (screen == Screen::Draft)
        {
            const int side = MouseSide();
            for (int id = 1; id < CardCount; ++id)
                if (DraftRect(side, id).Contains(x, y)) mouseDraftCursor = id;
        }
        if (kind != 1 || button != 1) return;
        if (screen == Screen::Sides)
        {
            for (int side = 0; side < 2; ++side)
                if (SideRect(side).Contains(x, y)) p1Plants = side == 0;
            for (int index = 0; index < 3; ++index)
                if (OptionRect(index).Contains(x, y)) { SelectBattle(index); break; }
        }
        else if (screen == Screen::Draft && DraftSide() == MouseSide())
        {
            for (int id = 1; id < CardCount; ++id)
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
                for (int col = 0; col <= LastColumn; ++col)
                    for (int lane = 0; lane < Rows; ++lane)
                        if (Rect(GridScreenX(col, lane), GridScreenY(col, lane), 80, 100).Contains(x, y)) Place(side, col, lane);
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
        if (screen == Screen::Result)
        {
            DrawResults(g);
            return;
        }
        if (PauseDialogOpen())
        {
            DrawBanks(g, true);
            DrawSuns(g);
            return;
        }
        if (app.mGameMode == GAMEMODE_VERSUS)
        {
            DrawMatch(g);
            if (screen == Screen::Win) DrawWin(g);
            if (screen == Screen::TrophyFlash)
            {
                DrawWin(g);
                const int alpha = ClampInt(trophyFlashTicks * 255 / 39, 0, 255);
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
            DrawDivider(g, app.mDDInterface->mWideScreenOffsetX + LAWN_XMIN + 80 * PlantColumns,
                app.mDDInterface->mWideScreenOffsetY + LAWN_YMIN, 500);
        }

        if (screen == Screen::Sides)
        {
            DrawBanks(g, false);
            ImageAt(g, setupBack, Rect(250, 133, 780, 466));
            Label(g, "Pick Sides", 640, 173, Sexy::FONT_DWARVENTODCRAFT24);
            for (int side = 0; side < 2; ++side)
            {
                const auto rect = SideRect(side);
                const int player = side == MouseSide() ? 0 : 1;
                if (player == 1) ImageAt(g, sideSelected[side],
                    Rect(rect.mX + (side == 1 ? 26 : 0), rect.mY, rect.mWidth, rect.mHeight));
                ImageAt(g, sideLabel[side], rect);
                ImageAt(g, playerLabel[player], Rect(rect.mX + (rect.mWidth - 34) / 2, rect.mY + rect.mHeight + 1, 34, 25));
            }
            ImageAt(g, playerTwoGamepad, Rect(p1Plants ? 651 : 484, 276, 111, 80));
            ConsoleButton(g, OptionRect(0), "Quick", "Play", option == 0);
            ConsoleButton(g, OptionRect(1), "Custom", "Battle", option == 1);
            ConsoleButton(g, OptionRect(2), "Random", "Battle", option == 2);
        }
        else if (screen == Screen::Draft)
            DrawDraft(g);
        else
            DrawJoin(g);
    }
};
