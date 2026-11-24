#pragma once

#include <wtypes.h>
constexpr const double PI = 3.141592653589793;

                                                               
             
                                                               
const int			BOARD_WIDTH = 800;
const int			BOARD_HEIGHT = 600;
const int			WIDE_BOARD_WIDTH = 800;
const int			BOARD_OFFSET = 220;
const int			BOARD_EDGE = -100;
const int			BOARD_IMAGE_WIDTH_OFFSET = 1180;
const int           BOARD_ICE_START = 800;
const int           LAWN_XMIN = 40;
const int           LAWN_YMIN = 80;
const int           HIGH_GROUND_HEIGHT = 30;

constexpr int IMP_STARTING_HEALTH = 270;
constexpr int I_ZOMBIE_IMP_STARTING_HEALTH = 70;

constexpr int CHERRY_BLAST_RADIUS = 115;
constexpr int CHERRY_BLAST_ROW_RANGE = 1;
constexpr int DOOM_BLAST_RADIUS = 250;
constexpr int DOOM_BLAST_ROW_RANGE = 3;
constexpr int POTATO_MINE_BLAST_RADIUS = 60;
constexpr int POTATO_MINE_BLAST_ROW_RANGE = 0;
constexpr int COB_BLAST_RADIUS = 115;
constexpr int COB_BLAST_ROW_RANGE = 1;
constexpr int EXPLODE_O_NUT_BLAST_RADIUS = 90;
constexpr int EXPLODE_O_NUT_BLAST_ROW_RANGE = 1;
constexpr int MELON_SPLASH_WIDTH = 60;
constexpr int MELON_SPLASH_ROW_RANGE = 1;
constexpr int FORWARD_FIREBALL_SPLASH_WIDTH = 100;

constexpr int CUSTOM_PLANTS_PER_PAGE = 48;
constexpr int CUSTOM_ZOMBIES_PER_PAGE = 30;

const int           SEEDBANK_MAX = 10;
const int           SEEDBANK_STORAGE = 12;
const int           COOP_MENU_MARGIN = 14;
const int           COOP_SEED_BANK_WIDTH = 304;
const int           COOP_SEED_BANK_RIGHT_X = 454;
const int           COOP_CONVEYOR_WIDTH = 331;
const int           COOP_CONVEYOR_RIGHT_X = 480;
const int           SEED_BANK_OFFSET_X = 0;
const int           SEED_BANK_OFFSET_X_END = 10;
const int           SEED_CHOOSER_OFFSET_Y = 516;
#ifdef OPENFL_HOST
constexpr int       BOARD_HUD_MARGIN = 14;
constexpr int       BOARD_HUD_GAP = 12;
constexpr int       SEED_CHOOSER_OPEN_Y = BOARD_HUD_MARGIN - (720 - BOARD_HEIGHT) / 2;
constexpr int       SEED_BANK_OPEN_Y = SEED_CHOOSER_OPEN_Y;
constexpr int       BOARD_MENU_Y = SEED_CHOOSER_OPEN_Y;
#else
constexpr int       SEED_CHOOSER_OPEN_Y = 0;
constexpr int       SEED_BANK_OPEN_Y = 0;
constexpr int       BOARD_MENU_Y = -10;
#endif
const int           SEED_PACKET_WIDTH = 50;
const int           SEED_PACKET_HEIGHT = 70;
const int           IMITATER_DIALOG_WIDTH = 500;
const int           IMITATER_DIALOG_HEIGHT = 600;

const int			WIDESCREEN_OFFSETX = -240;
const int			WIDESCREEN_OFFSETY = -60;

const int			STREET_ZOMBIE_START_X = 1030;                            
const int			STREET_ZOMBIE_ROOF_START_X = 900;
const int			STREET_ZOMBIE_START_Y = 70;
const int			STREET_ZOMBIE_GRID_SIZE_X = 30;                          
const int			STREET_ZOMBIE_GRID_SIZE_Y = 90;
const int			STREET_ZOMBIE_ROOF_OFFSET = 30;                          

                                                               
                   
                                                               
const int			ADVENTURE_AREAS = 5;
const int			LEVELS_PER_AREA = 10;
const int           NUM_LEVELS = ADVENTURE_AREAS * LEVELS_PER_AREA;
const int			FINAL_LEVEL = NUM_LEVELS;
const int           FLAG_RAISE_TIME = 100;
const int           LAST_STAND_FLAGS = 5;
const int           ZOMBIE_COUNTDOWN_FIRST_WAVE = 1800;
const int           ZOMBIE_COUNTDOWN = 2500;
const int           ZOMBIE_COUNTDOWN_RANGE = 600;
const int           ZOMBIE_COUNTDOWN_BEFORE_FLAG = 4500;
const int           ZOMBIE_COUNTDOWN_BEFORE_REPICK = 5499;
const int           ZOMBIE_COUNTDOWN_MIN = 400;
const int           FOG_BLOW_RETURN_TIME = 2000;
const int           SUN_COUNTDOWN = 425;
const int           SUN_COUNTDOWN_RANGE = 275;
const int           SUN_COUNTDOWN_MAX = 950;
const int           SURVIVAL_NORMAL_FLAGS = 5;
const int           SURVIVAL_HARD_FLAGS = 10;

                                                               
                   
                                                               
const int           STORESCREEN_ITEMOFFSET_1_X = 422;
const int           STORESCREEN_ITEMOFFSET_1_Y = 206;
const int           STORESCREEN_ITEMOFFSET_2_X = 372;
const int           STORESCREEN_ITEMOFFSET_2_Y = 310;
const int           STORESCREEN_ITEMSIZE = 74;
const int           STORESCREEN_COINBANK_X = 650;
const int           STORESCREEN_COINBANK_Y = 559;
const int           STORESCREEN_PAGESTRING_X = 470;
const int           STORESCREEN_PAGESTRING_Y = 500;

                                                               
                                                                                               
                                                               

#ifdef _GOTY
#define _HAS_NEW_DANCERS
#define _HAS_ACHIEVEMENTS
#define _HAS_ZOMBATAR
#ifndef OPENFL_HOST
#define _HAS_UNLOCK
#endif
                     
#define _HAS_LEVELSELECTOR 
                         
#endif

#ifdef _DEBUG
#define _SHOW_OUTPUT_CONSOLE
#define _SHOW_LIMBO_PAGE
#endif

            
#define _ALLOW_RESOURCE_PACKS

                     
                                      
#define _HAS_EXTENDED_MINIGAMES
                                  

                            
#define _MOBILE_MINIGAMES
#define _DS_MINIGAMES
                            
#define _REPLANTED_SPEED_CONTROL
                               
                           

                  
#define _HAS_HEALTHBAR_TOGGLE
                      
                                     
#define _HAS_ANIMATED_WOOD_SIGN
#define _HAS_KERNELPULT_BUTTER_IDLE

               
                               

              
                          
                         

#include "ConstEnums.h"                                                                                     