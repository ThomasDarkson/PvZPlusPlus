#include <deque>
#include <memory>
#include <sstream>
#include <set>

namespace OpenPVZAchievements
{
    const PvzAchievementDefinition definitions[] = {
#include "achievement-list.inc"
    };
    LawnApp* owner = nullptr;
    int profile = -1;
    bool explosiveKill = false;
    std::set<std::string> earned;
    std::map<std::string, int> progress;
    std::deque<std::string> pending;
    std::unique_ptr<MessageWidget> banner;
    std::map<int, std::unique_ptr<Sexy::Image>> icons;

    struct Run
    {
        Board* board = nullptr;
        bool known = false;
        bool used[NUM_SEED_TYPES] = {};
        bool collectedSun = false;
        bool extendedHistory = false;
        bool plantedInPool = false;
        bool mowerLost = false;
        unsigned int winterMelonLanes = 0;
        int lawnMowers = 0;
        int usedLawnMowers = 0;
        bool fogCleared = false;
        bool explosiveKills = false;
        bool otherKills = false;
        bool won = false;
        int sleds = 0;
        std::map<unsigned int, int> mowerKills;
    };
    Run run;

    SexyString path()
    {
        return GetAppDataFolder() + StrFormat(_S("userdata/scriptable-achievements-user%d.txt"), profile);
    }

    void sync(LawnApp* app)
    {
        const int id = app && app->mPlayerInfo ? static_cast<int>(app->mPlayerInfo->mId) : -1;
        if (owner == app && profile == id) return;
        banner.reset();
        pending.clear();
        earned.clear();
        progress.clear();
        owner = app;
        profile = id;
        if (id < 0) return;
        Buffer data;
        if (!app->ReadBufferFromFile(path(), &data, false) || !data.GetDataLen()) return;
        std::istringstream input(std::string(reinterpret_cast<const char*>(data.GetDataPtr()), data.GetDataLen()));
        std::string line;
        if (!std::getline(input, line)) return;
        if (line == "PVZSCRIPTABLE_ACHIEVEMENTS_V2")
        {
            std::string type, key;
            while (input >> type >> key)
            {
                if (type == "earned")
                {
                    if (key != "almanac_explorer") earned.insert(key);
                }
                else if (type == "progress")
                {
                    int value;
                    if (!(input >> value)) break;
                    progress[key] = value;
                }
                else break;
            }
        }
    }

    void save()
    {
        MkDir(GetAppDataFolder() + _S("userdata"));
        std::ostringstream output;
        output << "PVZSCRIPTABLE_ACHIEVEMENTS_V2\n";
        for (const auto& key : earned) output << "earned " << key << '\n';
        for (const auto& entry : progress) output << "progress " << entry.first << ' ' << entry.second << '\n';
        const auto record = output.str();
        if (!owner->WriteBytesToFile(path(), record.data(), static_cast<unsigned long>(record.size())))
            pvz_host_trace("Could not save Plants vs Zombies++ achievements");
    }

    bool active(Board* board)
    {
        return board && board == run.board && board->mApp->mGameScene == SCENE_PLAYING;
    }

    bool has(SeedType seed)
    {
        return run.used[static_cast<int>(seed)];
    }

    int originalWins(LawnApp* app, GameMode first, GameMode last)
    {
        int wins = 0;
        for (int mode = first; mode <= last; ++mode)
            if (app->HasBeatenChallenge(static_cast<GameMode>(mode))) ++wins;
        return wins;
    }

    const char* gotyNames[] = {
        "home_lawn_security", "nobel_peas_prize", "better_off_dead", "china_shop",
        "spudow", "explodonator", "morticulturalist", "dont_pea_in_the_pool",
        "roll_some_heads", "grounded", "zombologist", "penny_pincher", "sunny_days",
        "popcorn_party", "good_morning", "no_fungus_among_us", "beyond_the_grave",
        "immortal", "towering_wisdom", "mustache_mode", "disco_is_undead"
    };

    Rect gotyCrop(int index)
    {
        const int column = index % 7;
        const int row = index / 7;
        int offset = 0;
        if ((column == 1 && row < 2) || (column < 2 && row == 2)) offset = 2;
        else if (column > 0 || row == 2) offset = 1;
        return Rect(column * 70 + offset, row * 70, 70, 70);
    }

    Sexy::Image* splitIcon(LawnApp* app, const std::string& base, Sexy::Image* source, const Rect& crop)
    {
        if (auto* existing = app->GetImage(base, false)) return existing;
        if (!source || crop.mX >= source->GetWidth() || crop.mY >= source->GetHeight()) return nullptr;
        const auto* pixels = static_cast<MemoryImage*>(source)->GetBits();
        if (!pixels) return nullptr;
        ImageLib::Image tile;
        tile.mWidth = crop.mWidth;
        tile.mHeight = crop.mHeight;
        tile.mBits = new unsigned long[tile.mWidth * tile.mHeight]{};
        const int width = pvz_min(tile.mWidth, source->GetWidth() - crop.mX);
        const int height = pvz_min(tile.mHeight, source->GetHeight() - crop.mY);
        for (int row = 0; row < height; ++row)
            std::copy_n(pixels + (crop.mY + row) * source->GetWidth() + crop.mX, width, tile.mBits + row * tile.mWidth);
        if (!ImageLib::WritePNGImage(base + ".png", &tile))
            pvz_host_trace("Could not write a separate achievement icon");
        auto* generated = new DDImage(app->mDDInterface);
        generated->SetBits(tile.mBits, tile.mWidth, tile.mHeight, true);
        return generated;
    }

    Sexy::Image* loadIcon(LawnApp* app, int index)
    {
        const auto found = icons.find(index);
        if (found != icons.end()) return found->second.get();
        Sexy::Image* icon;
        if (index < MAX_ACHIEVEMENTS)
        {
            static_assert(sizeof(gotyNames) / sizeof(gotyNames[0]) == MAX_ACHIEVEMENTS, "Update the GOTY icon names");
            MkDir(_S("../achievement-icons/goty"));
            const auto base = std::string("../achievement-icons/goty/") + gotyNames[index];
            icon = splitIcon(app, base, IMAGE_ACHEESEMENTS_ICONS, gotyCrop(index));
        }
        else
        {
            const auto& definition = definitions[index - MAX_ACHIEVEMENTS];
            const auto base = std::string("../achievement-icons/") + definition.id;
            if (definition.icon[0]) icon = app->GetImage(base, false);
            else
            {
                auto* source = IMAGE_SEEDPACKETUPGRADE_LARGER;
                icon = source ? splitIcon(app, base, source, Rect(0, 0, source->GetWidth(), source->GetHeight())) : nullptr;
            }
        }
        icons[index].reset(icon);
        return icon;
    }
}

int pvz_host_achievement_count()
{
    return static_cast<int>(sizeof(OpenPVZAchievements::definitions) / sizeof(OpenPVZAchievements::definitions[0]));
}

const PvzAchievementDefinition& pvz_host_achievement_definition(int index)
{
    return OpenPVZAchievements::definitions[index];
}

bool pvz_host_achievement_earned(LawnApp* app, const char* id)
{
    OpenPVZAchievements::sync(app);
    return OpenPVZAchievements::earned.count(id) != 0;
}

void pvz_host_earn(LawnApp* app, const char* id)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0 || earned.count(id)) return;
    for (const auto& definition : definitions)
    {
        if (std::string(definition.id) != id) continue;
        earned.insert(id);
        save();
        pvz_host_queue_achievement(app, std::string(definition.title) + " Achievement!");
        return;
    }
}

void pvz_host_achievement_progress(LawnApp* app, const char* id, int amount, int target)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0 || amount <= 0 || earned.count(id)) return;
    int& value = progress[id];
    value = pvz_min(target, value + amount);
    if (value >= target) pvz_host_earn(app, id);
    else save();
}

void pvz_host_queue_achievement(LawnApp* app, const std::string& name)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile >= 0) pending.push_back(name);
}

void pvz_host_update_achievements(LawnApp* app)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0) return;
    if (banner && banner->mDuration > 0) banner->Update();
    if ((!banner || banner->mDuration <= 0) && !pending.empty())
    {
        if (!banner) banner.reset(new MessageWidget(app));
        banner->SetLabel(StringToSexyString(pending.front()), MESSAGE_STYLE_ACHIEVEMENT);
        pending.pop_front();
        app->PlaySample(SOUND_ACHIEVEMENT);
    }
}

void pvz_host_draw_achievements(LawnApp* app, Graphics* g)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (banner && banner->mDuration > 0) banner->Draw(g);
}

void pvz_host_clear_achievements()
{
    using namespace OpenPVZAchievements;
    banner.reset();
    pending.clear();
    earned.clear();
    progress.clear();
    icons.clear();
    owner = nullptr;
    profile = -1;
    run = Run{};
}

void pvz_host_draw_achievement_icon(LawnApp* app, Graphics* g, int index, int x, int y, bool unlocked)
{
    using namespace OpenPVZAchievements;
    if (icons.empty())
    {
        for (int i = 0; i < MAX_ACHIEVEMENTS; ++i) loadIcon(app, i);
        for (int i = 0; i < pvz_host_achievement_count(); ++i)
            if (!definitions[i].icon[0]) loadIcon(app, MAX_ACHIEVEMENTS + i);
    }
    auto* icon = loadIcon(app, index);

    Graphics draw(*g);
    draw.SetColorizeImages(true);
    draw.SetColor(Color(255, 255, 255, unlocked ? 255 : 32));
    if (!icon)
    {
        if (index < MAX_ACHIEVEMENTS)
            draw.DrawImage(IMAGE_ACHEESEMENTS_ICONS, Rect(x, y, 56, 56), gotyCrop(index));
        return;
    }
    const int width = icon->GetWidth();
    const int height = icon->GetHeight();
    if (width <= 0 || height <= 0) return;
    const int longest = pvz_max(width, height);
    const int w = pvz_max(1, (width * 56 + longest / 2) / longest);
    const int h = pvz_max(1, (height * 56 + longest / 2) / longest);
    draw.DrawImage(icon, Rect(x + (56 - w) / 2, y + (56 - h) / 2, w, h), Rect(0, 0, width, height));
}

void pvz_host_achievement_board_reset(Board* board, bool known)
{
    auto& run = OpenPVZAchievements::run;
    run = OpenPVZAchievements::Run{};
    run.board = board;
    run.known = known;
    run.extendedHistory = known;
}

void pvz_host_achievement_sun(Board* board)
{
    if (OpenPVZAchievements::active(board)) OpenPVZAchievements::run.collectedSun = true;
}

void pvz_host_achievement_bobsled(Board* board)
{
    if (OpenPVZAchievements::active(board) && ++OpenPVZAchievements::run.sleds >= 3)
        pvz_host_earn(board->mApp, "chill_out");
}

void pvz_host_achievement_mow(LawnMower* mower, bool finalZombie)
{
    using namespace OpenPVZAchievements;
    auto* board = mower->mBoard;
    if (!active(board)) return;
    const auto id = board->mLawnMowers.DataArrayGetID(mower);
    if (++run.mowerKills[id] >= 10) pvz_host_earn(board->mApp, "lawn_mower_man");
    if (finalZombie) pvz_host_earn(board->mApp, "last_mown");
}

void pvz_host_achievement_plant(Board* board, int seed, int column, int row)
{
    using namespace OpenPVZAchievements;
    if (!active(board) || seed < 0 || seed >= NUM_SEED_TYPES || board->mApp->IsIZombieLevel()) return;
    run.used[seed] = true;
    if (column >= 0 && column < MAX_GRID_SIZE_X && row >= 0 && row < MAX_GRID_SIZE_Y &&
        board->IsPoolSquare(column, row))
        run.plantedInPool = true;
    auto* app = board->mApp;
    if (seed == SEED_PEASHOOTER) pvz_host_achievement_progress(app, "soil_your_plants", 1, 10);
    if (seed == SEED_COBCANNON && board->CountPlantByType(SEED_COBCANNON) >= 5)
        pvz_host_earn(app, "defcorn");
    if (seed == SEED_WINTERMELON && row >= 0 && row < MAX_GRID_SIZE_Y)
    {
        run.winterMelonLanes |= 1u << row;
        for (int lane = 0; lane < MAX_GRID_SIZE_Y; ++lane)
            if (board->RowCanHaveZombies(lane) && !(run.winterMelonLanes & (1u << lane))) return;
        pvz_host_earn(app, "melony_lane");
    }
}

void pvz_host_achievement_round(Board* board)
{
    auto* app = board->mApp;
    const int rounds = board->mChallenge->mSurvivalStage;
    if (app->IsSurvivalEndless(app->mGameMode))
    {
        if (rounds >= 40) ReportAchievement::GiveAchievement(app, AchievementId::Immortal, true);
        if (rounds * board->GetNumWavesPerSurvivalStage() / board->GetNumWavesPerFlag() >= 40) pvz_host_earn(app, "alive_planting");
    }
    if (app->IsLastStandEndless(app->mGameMode) && rounds * board->GetNumWavesPerSurvivalStage() / board->GetNumWavesPerFlag() >= 15)
        pvz_host_earn(app, "stuff_legends");
}

void pvz_host_achievement_win(Board* board)
{
    using namespace OpenPVZAchievements;
    if (!active(board) || run.won) return;
    auto* app = board->mApp;
    const auto mode = app->mGameMode;
    if (board->IsSurvivalStageWithRepick() || board->IsLastStandStageWithRepick() || app->IsSurvivalEndless(mode) ||
        app->IsLastStandEndless(mode) || app->IsEndlessScaryPotter(mode) || app->IsEndlessIZombie(mode)) return;
    run.won = true;
    if (run.extendedHistory && run.lawnMowers > 0 && run.usedLawnMowers == run.lawnMowers)
        pvz_host_earn(app, "close_shave");
    if (board->CountPlantByType(SEED_TWINSUNFLOWER) >= 10)
        pvz_host_earn(app, "flower_power");
    if (mode == GAMEMODE_CHALLENGE_SEEING_STARS && board->mCurrentWave < board->GetNumWavesPerFlag())
        pvz_host_earn(app, "shooting_star");
    if (mode == GAMEMODE_CHALLENGE_LAST_STAND && board->mSunMoney >= 2000)
        pvz_host_earn(app, "sol_invictus");
    if (mode == GAMEMODE_PUZZLE_I_ZOMBIE_1 && board->mSunMoney >= 1000)
        pvz_host_earn(app, "where_sun");
    
    if (!run.known) return;

    if (mode == GAMEMODE_CHALLENGE_POGO_PARTY && !has(SEED_TALLNUT) && !has(SEED_MAGNETSHROOM))
        pvz_host_earn(app, "sproing");
    if (mode == GAMEMODE_CHALLENGE_COLUMN && !has(SEED_JALAPENO))
        pvz_host_earn(app, "chilli_free");
    if ((mode == GAMEMODE_CHALLENGE_WAR_AND_PEAS || mode == GAMEMODE_CHALLENGE_WAR_AND_PEAS_2) && !has(SEED_WALLNUT) && !has(SEED_TALLNUT) && !has(SEED_PUMPKINSHELL))
        pvz_host_earn(app, "wall_not_attack");

    if (app->IsPuzzleMode()) return;
    if (run.explosiveKills && !run.otherKills && board->mTriggeredLawnMowers == 0 &&
        !has(SEED_BLOVER) && !has(SEED_HYPNOSHROOM) && !has(SEED_TANGLEKELP) &&
        !has(SEED_UMBRELLA) && !has(SEED_SQUASH) && !has(SEED_CHOMPER))
        pvz_host_earn(app, "pyromaniac");

    const int level = abs(board->mLevel);
    if (board->StageHasFog())
    {
        if (app->IsAdventureMode() && level >= 37 && level <= 39 && !has(SEED_PLANTERN) && !has(SEED_BLOVER))
            pvz_host_earn(app, "blind_faith");
        const bool firstFogTutorial = app->IsFirstTimeAdventureMode() && !board->mIsReplay && level == 31;
        if (!firstFogTutorial && !board->HasConveyorBeltSeedBank() && !run.fogCleared)
            pvz_host_earn(app, "nebulaphile");
    }
    if (board->mBackground == BACKGROUND_2_NIGHT && !run.collectedSun &&
        !app->IsWhackAZombieLevel() && !app->IsFinalBossLevel() && !app->IsScaryPotterLevel() &&
        mode != GAMEMODE_CHALLENGE_ZOMBIE_TRAP &&
        mode != GAMEMODE_CHALLENGE_BEGHOULED && mode != GAMEMODE_CHALLENGE_BEGHOULED_TWIST)
        pvz_host_earn(app, "photosensitive");
    if (board->StageIsDayWithPool() && run.extendedHistory && !run.plantedInPool)
        pvz_host_earn(app, "pools_closed");

    if (board->HasConveyorBeltSeedBank()) return;
    bool nomOnly = has(SEED_CHOMPER);
    for (int i = 0; i < NUM_SEED_TYPES; ++i)
    {
        if (!run.used[i]) continue;
        const auto seed = static_cast<SeedType>(i);
        if (seed != SEED_SUNFLOWER && seed != SEED_WALLNUT && seed != SEED_CHOMPER) nomOnly = false;
    }
    if (nomOnly) pvz_host_earn(app, "nom_nom_nom");
}

void pvz_host_achievement_records(LawnApp* app)
{
    using namespace OpenPVZAchievements;
    if (!app->mPlayerInfo || app->mPlayerInfo->mName == _S("[All Unlocked]")) return;
    pvz_host_achievement_almanac(app);
    const int mini = originalWins(app, GAMEMODE_CHALLENGE_WAR_AND_PEAS, GAMEMODE_CHALLENGE_FINAL_BOSS);
    const int vase = originalWins(app, GAMEMODE_SCARY_POTTER_1, GAMEMODE_SCARY_POTTER_9);
    const int izombie = originalWins(app, GAMEMODE_PUZZLE_I_ZOMBIE_1, GAMEMODE_PUZZLE_I_ZOMBIE_9);
    if (mini >= 3) pvz_host_earn(app, "future_star");
    if (mini == 20) ReportAchievement::GiveAchievement(app, AchievementId::BeyondTheGrave, true);
    if (vase == 9) pvz_host_earn(app, "smashing");
    if (izombie == 9) pvz_host_earn(app, "i_win");
    if (vase + izombie == 18) pvz_host_earn(app, "think_hard");
    if (originalWins(app, GAMEMODE_SURVIVAL_NORMAL_STAGE_1, GAMEMODE_SURVIVAL_HARD_STAGE_5) == 10)
        pvz_host_earn(app, "tough_beans");
    if (originalWins(app, GAMEMODE_LAST_STAND_STAGE_1, GAMEMODE_LAST_STAND_STAGE_5) == 5)
        pvz_host_earn(app, "still_standing");
    if (app->mPlayerInfo->mFinishedAdventure >= 2) pvz_host_earn(app, "second_life");
    int purple = 0;
    for (int seed = SEED_GATLINGPEA; seed <= SEED_COBCANNON; ++seed)
        if (app->HasSeedType(static_cast<SeedType>(seed))) ++purple;
    for (int i = 1; i <= purple; ++i)
        pvz_host_earn(app, ("purple_card_" + std::to_string(i)).c_str());
}

void pvz_host_achievement_almanac(LawnApp* app)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0 || app->mPlayerInfo->mName == _S("[All Unlocked]") ||
        earned.count("book_learner") || !app->HasFinishedAdventure() || !progress.count("yeti_encountered")) return;
    for (int seed = SEED_PEASHOOTER; seed <= SEED_IMITATER; ++seed)
        if (!app->HasSeedType(static_cast<SeedType>(seed))) return;
    pvz_host_earn(app, "book_learner");
}

void pvz_host_achievement_yeti(LawnApp* app)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0 || app->mPlayerInfo->mName == _S("[All Unlocked]")) return;
    if (progress.emplace("yeti_encountered", 1).second) save();
    pvz_host_achievement_almanac(app);
}

int pvz_host_peking_best_time(LawnApp* app)
{
    OpenPVZAchievements::sync(app);
    const auto found = OpenPVZAchievements::progress.find("peking_best_ms");
    return found == OpenPVZAchievements::progress.end() ? 0 : found->second;
}

void pvz_host_peking_finish(LawnApp* app, int elapsedMs)
{
    using namespace OpenPVZAchievements;
    const int previous = pvz_host_peking_best_time(app);
    if (profile < 0 || elapsedMs <= 0 || (previous > 0 && elapsedMs >= previous)) return;
    progress["peking_best_ms"] = elapsedMs;
    if (earned.count("peking_express"))
    {
        save();
        if (previous > 0)
            pvz_host_queue_achievement(app, StrFormat("Peking Express: New best time! %d.%03d s", elapsedMs / 1000, elapsedMs % 1000));
    }
    else pvz_host_earn(app, "peking_express");
}

void pvz_host_achievement_mower_added(LawnMower* mower)
{
    using namespace OpenPVZAchievements;
    if (mower->mBoard == run.board && mower->mMowerType == LAWNMOWER_LAWN)
        ++run.lawnMowers;
}

void pvz_host_achievement_mower_lost(LawnMower* mower, bool activated)
{
    using namespace OpenPVZAchievements;
    auto* board = mower->mBoard;
    if (!active(board) || board->HasLevelAwardDropped()) return;
    if (activated && mower->mMowerType == LAWNMOWER_LAWN) ++run.usedLawnMowers;
    run.mowerLost = true;

}

void pvz_host_achievement_garden(LawnApp* app, int seed)
{
    using namespace OpenPVZAchievements;
    sync(app);
    if (profile < 0 || earned.count("enlightened")) return;
    bool changed = false;
    auto record = [&](int value)
    {
        if (value < SEED_PEASHOOTER || value > SEED_MELONPULT || value == SEED_FLOWERPOT) return;
        changed |= progress.emplace("garden_" + std::to_string(value), 1).second;
    };
    record(seed);
    for (int i = 0; i < app->mPlayerInfo->mNumPottedPlants; ++i)
        record(app->mPlayerInfo->mPottedPlant[i].mSeedType);
    for (int value = SEED_PEASHOOTER; value <= SEED_MELONPULT; ++value)
        if (value != SEED_FLOWERPOT && !progress.count("garden_" + std::to_string(value)))
        {
            if (changed) save();
            return;
        }
    pvz_host_earn(app, "enlightened");
}



void pvz_host_achievement_fog(Board* board)
{
    if (OpenPVZAchievements::active(board)) OpenPVZAchievements::run.fogCleared = true;
}

namespace OpenPVZAchievements
{
    unsigned long long saveFingerprint(const Buffer& data)
    {
        unsigned long long hash = 14695981039346656037ULL;
        for (int i = 0; i < data.GetDataLen(); ++i)
        {
            hash ^= data.GetDataPtr()[i];
            hash *= 1099511628211ULL;
        }
        return hash;
    }
}

void pvz_host_achievement_save_run(Board* board, const SexyString& path, const Buffer& data)
{
    using namespace OpenPVZAchievements;
    if (board != run.board) return;
    std::ostringstream output;
    output << "PVZSCRIPTABLE_RUN_V2 " << saveFingerprint(data) << '\n';
    output << run.known << ' ' << run.collectedSun << ' ' << run.fogCleared << ' ' << run.explosiveKills << ' ' << run.otherKills << ' ' << run.sleds << '\n';
    output << run.extendedHistory << ' ' << run.plantedInPool << ' ' << run.mowerLost << ' '
           << run.winterMelonLanes << ' ' << run.lawnMowers << ' ' << run.usedLawnMowers << '\n';
    for (bool used : run.used) output << used;
    output << '\n';
    for (const auto& entry : run.mowerKills) output << entry.first << ' ' << entry.second << '\n';
    const auto record = output.str();
    if (!board->mApp->WriteBytesToFile(path + _S(".achievements"), record.data(), static_cast<unsigned long>(record.size())))
        pvz_host_trace("Could not save achievement history for this level");
}

void pvz_host_achievement_load_run(Board* board, const SexyString& path, const Buffer& data)
{
    using namespace OpenPVZAchievements;
    pvz_host_achievement_board_reset(board, false);
    Buffer record;
    if (!board->mApp->ReadBufferFromFile(path + _S(".achievements"), &record, false) || !record.GetDataLen()) return;
    std::istringstream input(std::string(reinterpret_cast<const char*>(record.GetDataPtr()), record.GetDataLen()));
    std::string magic, seeds;
    unsigned long long fingerprint;
    Run loaded;
    if (!(input >> magic >> fingerprint) || fingerprint != saveFingerprint(data)) return;
    const bool v2 = magic == "PVZSCRIPTABLE_RUN_V2";
    if (!v2 && magic != "PVZSCRIPTABLE_RUN_V1") return;
    if (!(input >> loaded.known >> loaded.collectedSun >> loaded.fogCleared >> loaded.explosiveKills >> loaded.otherKills >> loaded.sleds)) return;
    if (v2 && !(input >> loaded.extendedHistory >> loaded.plantedInPool >> loaded.mowerLost
                    >> loaded.winterMelonLanes >> loaded.lawnMowers >> loaded.usedLawnMowers)) return;
    if (!(input >> seeds) || seeds.size() != NUM_SEED_TYPES || seeds.find_first_not_of("01") != std::string::npos) return;
    for (int i = 0; i < NUM_SEED_TYPES; ++i) loaded.used[i] = seeds[i] == '1';
    unsigned int id;
    int kills;
    while (input >> id >> kills)
        if (kills > 0 && kills <= 1024) loaded.mowerKills[id] = kills;
    loaded.board = board;
    run = std::move(loaded);
}

PvzExplosiveKillScope::PvzExplosiveKillScope(bool explosive)
    : previous(OpenPVZAchievements::explosiveKill)
{
    OpenPVZAchievements::explosiveKill = explosive;
}

PvzExplosiveKillScope::~PvzExplosiveKillScope()
{
    OpenPVZAchievements::explosiveKill = previous;
}

void pvz_host_achievement_kill(Board* board)
{
    using namespace OpenPVZAchievements;
    if (!active(board) || board->HasLevelAwardDropped()) return;
    if (explosiveKill) run.explosiveKills = true;
    else run.otherKills = true;
}
