#pragma once
#define OPENFL_HOST 1
#define _GOTY 1
#define GOTY 1
#define NDEBUG 1
#define _VISUALC_ 1
#define _WINDOWS 1
#define WIN32_LEAN_AND_MEAN
#define SDL_DECLSPEC __declspec(dllimport)
#define SDL_MAIN_HANDLED
#define DIRECTSOUND_VERSION 0x0800
#if defined(PVZ_DEFINE_GUIDS)
#define INITGUID
#endif
#include <windows.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <climits>
#include <cstdio>
#include <cstring>
#define _JPEGDLL_ 1
#define PNG_EXPORT(type,symbol) __declspec(dllimport) type symbol
#include <cmath>
#include <cfloat>
#include <type_traits>
template<class A, class B> std::common_type_t<A,B> pvz_min(A a, B b) { return a < b ? a : b; }
template<class A, class B> std::common_type_t<A,B> pvz_max(A a, B b) { return a > b ? a : b; }

extern HMODULE gDDrawDLL, gDSoundDLL;
extern bool gD3DInterfacePreDrawError;
void pvz_host_trace(const char* phase);

namespace Sexy { class Widget; }
void pvz_host_layout_widget(Sexy::Widget* widget);

int pvz_window_size();
void pvz_set_window_size(int index);
void pvz_host_cycle_window_size();

int pvz_sharpness();
void pvz_set_sharpness(int percent);

int pvz_fps_limit();
void pvz_set_fps_limit(int limit);
void pvz_host_cycle_fps_limit();

int pvz_display_flags();
constexpr int PVZ_CUSTOM_CONTENT_DISABLED = 4;
inline bool pvz_custom_content_enabled() { return (pvz_display_flags() & PVZ_CUSTOM_CONTENT_DISABLED) == 0; }
void pvz_set_display_flags(int flags);
void pvz_host_set_fullscreen(bool enabled);
void pvz_host_set_vsync(bool enabled);

namespace Sexy { class Graphics; class Image; }
class GameSelector;
Sexy::Image* pvz_host_dialog_image(const char* name);
void pvz_host_free_dialog_images();
void pvz_host_draw_background(Sexy::Graphics* g, const char* name);
bool pvz_host_flower_point(GameSelector* selector, int& x, int& y);

void pvz_host_update_trophy_tooltip(GameSelector* selector);

bool pvz_host_image_point(Sexy::Image* image, int x, int y, int width, int height, int cell);
void pvz_host_free_menu_geometry();

void pvz_host_fill_viewport(Sexy::Graphics* g);

class LawnApp;
void pvz_host_queue_achievement(LawnApp* app, const std::string& name);
void pvz_host_update_achievements(LawnApp* app);
void pvz_host_draw_achievements(LawnApp* app, Sexy::Graphics* g);
void pvz_host_clear_achievements();

class Board;
class LawnMower;
struct PvzAchievementDefinition
{
    const char* id;
    const char* title;
    const char* description;
    const char* icon;
};
int pvz_host_achievement_count();
const PvzAchievementDefinition& pvz_host_achievement_definition(int index);
bool pvz_host_achievement_earned(LawnApp*, const char* id);
void pvz_host_earn(LawnApp*, const char* id);
void pvz_host_achievement_progress(LawnApp*, const char* id, int amount, int target);
void pvz_host_draw_achievement_icon(LawnApp*, Sexy::Graphics*, int index, int x, int y, bool earned);
void pvz_host_achievement_plant(Board*, int seed, int column, int row);
void pvz_host_achievement_board_reset(Board*, bool known);
void pvz_host_achievement_sun(Board*);
void pvz_host_achievement_bobsled(Board*);
void pvz_host_achievement_mow(LawnMower*, bool finalZombie);
void pvz_host_achievement_win(Board*);
void pvz_host_achievement_records(LawnApp*);
void pvz_host_achievement_round(Board*);
void pvz_host_achievement_almanac(LawnApp*);
void pvz_host_achievement_yeti(LawnApp*);
void pvz_host_achievement_mower_added(LawnMower*);
void pvz_host_achievement_mower_lost(LawnMower*, bool activated);
int pvz_host_peking_best_time(LawnApp*);
void pvz_host_peking_finish(LawnApp*, int elapsedMs);
void pvz_host_achievement_garden(LawnApp*, int seed);

namespace Sexy { class Buffer; }
void pvz_host_achievement_fog(Board*);
void pvz_host_achievement_save_run(Board*, const std::string&, const Sexy::Buffer&);
void pvz_host_achievement_load_run(Board*, const std::string&, const Sexy::Buffer&);

class PvzExplosiveKillScope
{
    bool previous;
public:
    explicit PvzExplosiveKillScope(bool explosive);
    ~PvzExplosiveKillScope();
};
void pvz_host_achievement_kill(Board*);
