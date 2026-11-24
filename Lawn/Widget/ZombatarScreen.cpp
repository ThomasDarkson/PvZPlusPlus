#include "../../GameConstants.h"
#ifdef _HAS_ZOMBATAR
#include "ZombatarScreen.h"
#include "../../Resources.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "GameSelector.h"
#include "GameButton.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../SexyAppFramework/MemoryImage.h"
#include "../../GameConstants.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../Zombie.h"
#include "../../Sexy.TodLib/Attachment.h"
#include "../../SexyAppFramework/Font.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../System/PlayerInfo.h"
#include "../../SexyAppFramework/Buffer.h"

ZombatarDefinition gZombatarDefinitions[] = {
	{ZombatarItem::ZOMBATAR_BACKGROUND_CRAZYDAVE,	nullptr,							&Sexy::IMAGE_ZOMBATAR_BACKGROUND_CRAZYDAVE,	ZombatarCategory::ZombatarCategory_Backdrops,	  0, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_BACKGROUND_MENU,	    nullptr,							&Sexy::IMAGE_ZOMBATAR_BACKGROUND_MENU,		ZombatarCategory::ZombatarCategory_Backdrops,	  1, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_BACKGROUND_MENU_DOS,    nullptr,							&Sexy::IMAGE_ZOMBATAR_BACKGROUND_MENU_DOS,  ZombatarCategory::ZombatarCategory_Backdrops,	  2, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_BACKGROUND_ROOF,	    nullptr,							&Sexy::IMAGE_ZOMBATAR_BACKGROUND_ROOF,		ZombatarCategory::ZombatarCategory_Backdrops,	  3, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_BACKGROUND_BLANK,		nullptr,							&Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK,		ZombatarCategory::ZombatarCategory_Backdrops,	  4, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_1,					&Sexy::IMAGE_ZOMBATAR_HAIR_1,		&Sexy::IMAGE_ZOMBATAR_HAIR_1_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  0, 0,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_2,					&Sexy::IMAGE_ZOMBATAR_HAIR_2,		&Sexy::IMAGE_ZOMBATAR_HAIR_2_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  1, 0,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_3,					&Sexy::IMAGE_ZOMBATAR_HAIR_3,		nullptr,									ZombatarCategory::ZombatarCategory_Hairs,		  2, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_HAIR_4,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_4,				ZombatarCategory::ZombatarCategory_Hairs,		  3, 0,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_5,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_5,				ZombatarCategory::ZombatarCategory_Hairs,		  4, 0,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_6,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_6,				ZombatarCategory::ZombatarCategory_Hairs,		  5, 0,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_7,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_7,				ZombatarCategory::ZombatarCategory_Hairs,		  0, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_8,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_8,				ZombatarCategory::ZombatarCategory_Hairs,		  1, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_9,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_9,				ZombatarCategory::ZombatarCategory_Hairs,		  2, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_10,				nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_10,				ZombatarCategory::ZombatarCategory_Hairs,		  3, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_11,				&Sexy::IMAGE_ZOMBATAR_HAIR_11,		&Sexy::IMAGE_ZOMBATAR_HAIR_11_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  4, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_12,				&Sexy::IMAGE_ZOMBATAR_HAIR_12,		&Sexy::IMAGE_ZOMBATAR_HAIR_12_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  5, 1,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_13,				&Sexy::IMAGE_ZOMBATAR_HAIR_13,		&Sexy::IMAGE_ZOMBATAR_HAIR_13_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  0, 2,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_14,				&Sexy::IMAGE_ZOMBATAR_HAIR_14,		&Sexy::IMAGE_ZOMBATAR_HAIR_14_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  1, 2,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_15,				&Sexy::IMAGE_ZOMBATAR_HAIR_15,		&Sexy::IMAGE_ZOMBATAR_HAIR_15_MASK,			ZombatarCategory::ZombatarCategory_Hairs,		  2, 2,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_HAIR_16,				nullptr,							&Sexy::IMAGE_ZOMBATAR_HAIR_16,				ZombatarCategory::ZombatarCategory_Hairs,		  3, 2,		gZombatarDimPalletes},
	{ZombatarItem::ZOMBATAR_TIDBIT_1,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_1,			ZombatarCategory::ZombatarCategory_Tidbits,		  0, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_TIDBIT_2,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_2,			ZombatarCategory::ZombatarCategory_Tidbits,		  1, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_TIDBIT_3,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_3,			ZombatarCategory::ZombatarCategory_Tidbits,		  2, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_TIDBIT_4,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_4,			ZombatarCategory::ZombatarCategory_Tidbits,		  3, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_5,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_5,			ZombatarCategory::ZombatarCategory_Tidbits,		  4, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_6,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_6,			ZombatarCategory::ZombatarCategory_Tidbits,		  5, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_7,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_7,			ZombatarCategory::ZombatarCategory_Tidbits,		  0, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_8,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_8,			ZombatarCategory::ZombatarCategory_Tidbits,		  1, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_9,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_9,			ZombatarCategory::ZombatarCategory_Tidbits,		  2, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_10,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_10,			ZombatarCategory::ZombatarCategory_Tidbits,		  3, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_11,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_11,			ZombatarCategory::ZombatarCategory_Tidbits,		  4, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_12,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_12,			ZombatarCategory::ZombatarCategory_Tidbits,		  5, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_13,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_13,			ZombatarCategory::ZombatarCategory_Tidbits,		  0, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_TIDBIT_14,				nullptr,							&Sexy::IMAGE_ZOMBATAR_TIDBITS_14,			ZombatarCategory::ZombatarCategory_Tidbits,		  1, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_EYEWEAR_1,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_1,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_1_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  0, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_2,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_2,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_2_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  1, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_3,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_3,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_3_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  2, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_4,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_4,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_4_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  3, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_5,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_5,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_5_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  4, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_6,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_6,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_6_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  5, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_7,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_7,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_7_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  0, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_8,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_8,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_8_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  1, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_9,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_9,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_9_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  2, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_10,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_10,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_10_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  3, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_11,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_11,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_11_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  4, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_12,				&Sexy::IMAGE_ZOMBATAR_EYEWEAR_12,	&Sexy::IMAGE_ZOMBATAR_EYEWEAR_12_MASK,		ZombatarCategory::ZombatarCategory_EyeWears,	  5, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_EYEWEAR_13,				nullptr,							&Sexy::IMAGE_ZOMBATAR_EYEWEAR_13,			ZombatarCategory::ZombatarCategory_EyeWears,	  0, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_EYEWEAR_14,				nullptr,							&Sexy::IMAGE_ZOMBATAR_EYEWEAR_14,			ZombatarCategory::ZombatarCategory_EyeWears,	  1, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_EYEWEAR_15,				nullptr,							&Sexy::IMAGE_ZOMBATAR_EYEWEAR_15,			ZombatarCategory::ZombatarCategory_EyeWears,	  2, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_EYEWEAR_16,				nullptr,							&Sexy::IMAGE_ZOMBATAR_EYEWEAR_16,			ZombatarCategory::ZombatarCategory_EyeWears,	  3, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_1,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_1,			ZombatarCategory::ZombatarCategory_Clothes,		  0, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_2,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_2,			ZombatarCategory::ZombatarCategory_Clothes,		  1, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_3,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_3,			ZombatarCategory::ZombatarCategory_Clothes,		  2, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_4,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_4,			ZombatarCategory::ZombatarCategory_Clothes,		  3, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_5,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_5,			ZombatarCategory::ZombatarCategory_Clothes,		  4, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_6,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_6,			ZombatarCategory::ZombatarCategory_Clothes,		  5, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_7,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_7,			ZombatarCategory::ZombatarCategory_Clothes,		  0, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_8,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_8,			ZombatarCategory::ZombatarCategory_Clothes,		  1, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_9,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_9,			ZombatarCategory::ZombatarCategory_Clothes,		  2, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_10,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_10,			ZombatarCategory::ZombatarCategory_Clothes,		  3, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_11,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_11,			ZombatarCategory::ZombatarCategory_Clothes,		  4, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_CLOTHE_12,				nullptr,							&Sexy::IMAGE_ZOMBATAR_CLOTHES_12,			ZombatarCategory::ZombatarCategory_Clothes,		  5, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_1,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_1,			ZombatarCategory::ZombatarCategory_Accessories,	  0, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_2,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_2,			ZombatarCategory::ZombatarCategory_Accessories,	  1, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_3,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_3,			ZombatarCategory::ZombatarCategory_Accessories,	  2, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_4,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_4,			ZombatarCategory::ZombatarCategory_Accessories,	  3, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_5,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_5,			ZombatarCategory::ZombatarCategory_Accessories,	  4, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_6,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_6,			ZombatarCategory::ZombatarCategory_Accessories,	  5, 0,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_7,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_8,			ZombatarCategory::ZombatarCategory_Accessories,	  0, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_8,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_9,			ZombatarCategory::ZombatarCategory_Accessories,	  1, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_ACCESSORY_9,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_10,			ZombatarCategory::ZombatarCategory_Accessories,	  2, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_10,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_11,			ZombatarCategory::ZombatarCategory_Accessories,	  3, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_ACCESSORY_11,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_12,			ZombatarCategory::ZombatarCategory_Accessories,	  4, 1,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_12,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_13,			ZombatarCategory::ZombatarCategory_Accessories,	  5, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_ACCESSORY_13,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_14,			ZombatarCategory::ZombatarCategory_Accessories,	  0, 2,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_ACCESSORY_14,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_15,			ZombatarCategory::ZombatarCategory_Accessories,	  1, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_ACCESSORY_15,			nullptr,							&Sexy::IMAGE_ZOMBATAR_ACCESSORY_16,			ZombatarCategory::ZombatarCategory_Accessories,	  2, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_HAT_1,					&Sexy::IMAGE_ZOMBATAR_HATS_1,		&Sexy::IMAGE_ZOMBATAR_HATS_1_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  0, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_2,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_2,				ZombatarCategory::ZombatarCategory_Hats,		  1, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_3,					&Sexy::IMAGE_ZOMBATAR_HATS_3,		&Sexy::IMAGE_ZOMBATAR_HATS_3_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  2, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_4,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_4,				ZombatarCategory::ZombatarCategory_Hats,		  3, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_5,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_5,				ZombatarCategory::ZombatarCategory_Hats,		  4, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_6,					&Sexy::IMAGE_ZOMBATAR_HATS_6,		&Sexy::IMAGE_ZOMBATAR_HATS_6_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  5, 0,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_7,					&Sexy::IMAGE_ZOMBATAR_HATS_7,		&Sexy::IMAGE_ZOMBATAR_HATS_7_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  0, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_8,					&Sexy::IMAGE_ZOMBATAR_HATS_8,		&Sexy::IMAGE_ZOMBATAR_HATS_8_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  1, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_9,					&Sexy::IMAGE_ZOMBATAR_HATS_9,		&Sexy::IMAGE_ZOMBATAR_HATS_9_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  2, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_10,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_10,				ZombatarCategory::ZombatarCategory_Hats,		  3, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_11,					&Sexy::IMAGE_ZOMBATAR_HATS_11,		&Sexy::IMAGE_ZOMBATAR_HATS_11_MASK,			ZombatarCategory::ZombatarCategory_Hats,		  4, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_12,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_12,				ZombatarCategory::ZombatarCategory_Hats,		  5, 1,		gZombatarBrightPalletes},
	{ZombatarItem::ZOMBATAR_HAT_13,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_13,				ZombatarCategory::ZombatarCategory_Hats,		  0, 2,		nullptr},
	{ZombatarItem::ZOMBATAR_HAT_14,					nullptr,							&Sexy::IMAGE_ZOMBATAR_HATS_14,				ZombatarCategory::ZombatarCategory_Hats,		  1, 2,		gZombatarBrightPalletes},

	{ZOMBATAR_FACIALHAIR_1, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_1, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_1_MASK, ZombatarCategory_FacialHair, 0, 0, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_2, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_2, ZombatarCategory_FacialHair, 1, 0, nullptr},
	{ZOMBATAR_FACIALHAIR_3, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_3, ZombatarCategory_FacialHair, 2, 0, nullptr},
	{ZOMBATAR_FACIALHAIR_4, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_4, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_4_MASK, ZombatarCategory_FacialHair, 3, 0, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_5, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_5, ZombatarCategory_FacialHair, 4, 0, nullptr},
	{ZOMBATAR_FACIALHAIR_6, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_6, ZombatarCategory_FacialHair, 5, 0, nullptr},
	{ZOMBATAR_FACIALHAIR_7, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_7, ZombatarCategory_FacialHair, 6, 0, nullptr},
	{ZOMBATAR_FACIALHAIR_8, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_8, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_8_MASK, ZombatarCategory_FacialHair, 0, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_9, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_9, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_9_MASK, ZombatarCategory_FacialHair, 1, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_10, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_10, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_10_MASK, ZombatarCategory_FacialHair, 2, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_11, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_11, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_11_MASK, ZombatarCategory_FacialHair, 3, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_12, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_12, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_12_MASK, ZombatarCategory_FacialHair, 4, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_13, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_13, ZombatarCategory_FacialHair, 5, 1, nullptr},
	{ZOMBATAR_FACIALHAIR_14, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_14, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_14_MASK, ZombatarCategory_FacialHair, 6, 1, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_15, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_15, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_15_MASK, ZombatarCategory_FacialHair, 0, 2, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_16, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_16, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_16_MASK, ZombatarCategory_FacialHair, 1, 2, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_17, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_17, ZombatarCategory_FacialHair, 2, 2, nullptr},
	{ZOMBATAR_FACIALHAIR_18, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_18, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_18_MASK, ZombatarCategory_FacialHair, 3, 2, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_19, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_19, ZombatarCategory_FacialHair, 4, 2, nullptr},
	{ZOMBATAR_FACIALHAIR_20, nullptr, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_20, ZombatarCategory_FacialHair, 5, 2, nullptr},
	{ZOMBATAR_FACIALHAIR_21, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_21, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_21_MASK, ZombatarCategory_FacialHair, 6, 2, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_22, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_22, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_22_MASK, ZombatarCategory_FacialHair, 0, 3, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_23, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_23, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_23_MASK, ZombatarCategory_FacialHair, 1, 3, gZombatarDimPalletes},
	{ZOMBATAR_FACIALHAIR_24, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_24, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_24_MASK, ZombatarCategory_FacialHair, 2, 3, gZombatarDimPalletes},

};

Color gZombatarSkinPalletes[] = {
	Color(134, 147, 122),
	Color(79, 135, 94),
	Color(127, 135, 94),
	Color(120, 130, 50),
	Color(156, 163, 105),
	Color(96, 151, 11),
	Color(147, 184, 77),
	Color(82, 143, 54),
	Color(121, 168, 99),
	Color(65, 156, 74),
	Color(107, 178, 114),
	Color(104, 121, 90)
};

Color gZombatarDimPalletes[] = {
	Color(151, 33, 33),
	Color(199, 53, 53),
	Color(220, 112, 47),
	Color(251, 251, 172),
	Color(240, 210, 87),
	Color(165, 126, 65),
	Color(106, 72, 32),
	Color(72, 35, 5),
	Color(50, 56, 61),
	Color(0, 0, 10),
	Color(197, 239, 239),
	Color(63, 109, 242),
	Color(13, 202, 151),
	Color(158, 183, 19),
	Color(30, 210, 64),
	Color(225, 65, 230),
	Color(128, 47, 204)
};

Color gZombatarBrightPalletes[] = {
	Color(238, 19, 24),
	Color(247, 89, 215),
	Color(239, 198, 253),
	Color(160, 56, 241),
	Color(86, 74, 241),
	Color(74, 160, 241),
	Color(199, 244, 251),
	Color(49, 238, 237),
	Color(16, 194, 66),
	Color(112, 192, 33),
	Color(16, 145, 52),
	Color(248, 247, 41),
	Color(227, 180, 20),
	Color(241, 115, 25),
	Color(248, 247, 175),
	Color(103, 85, 54),
	Color(159, 17, 20)
};

                   
static constexpr int ZOMBATAR_FACIAL_HAIR_PAGE_SIZE = 17;
static constexpr int ZOMBATAR_FACIAL_HAIR_PAGE_COUNT =
	(ZOMBATAR_FACIALHAIR_24 - ZOMBATAR_FACIALHAIR_1 + ZOMBATAR_FACIAL_HAIR_PAGE_SIZE) / ZOMBATAR_FACIAL_HAIR_PAGE_SIZE;
static constexpr int ZOMBATAR_COLORS_X = 222;
static constexpr int ZOMBATAR_COLORS_Y = 335;

static Rect ZombatarPageArrowRect(bool next, Image* arrow)
{
	if (!arrow) return Rect();
	const int x = next ? ZOMBATAR_COLORS_X + IMAGE_ZOMBATAR_COLORS_BG->GetWidth() + 16 :
		ZOMBATAR_COLORS_X - 16 - arrow->GetWidth();
	const int y = ZOMBATAR_COLORS_Y + (IMAGE_ZOMBATAR_COLORS_BG->GetHeight() - arrow->GetHeight()) / 2;
	return Rect(x, y, arrow->GetWidth(), arrow->GetHeight());
}
static int ZombatarFacialHairCount(int page)
{
	return pvz_min(ZOMBATAR_FACIAL_HAIR_PAGE_SIZE, ZOMBATAR_FACIALHAIR_24 - ZOMBATAR_FACIALHAIR_1 + 1 -
		page * ZOMBATAR_FACIAL_HAIR_PAGE_SIZE);
}

static Rect ZombatarFacialHairCell(int slot)
{
	const int width = IMAGE_ZOMBATAR_ACCESSORY_BG->GetWidth();
	const int height = IMAGE_ZOMBATAR_ACCESSORY_BG->GetHeight();
	return Rect(166 + (slot % 6) * (width - 4), 138 + (slot / 6) * (height - 4), width, height);
}

static std::vector<ZombatarAppearance> ReadZombatarCollection(LawnApp* app);

static void DrawZombatarPortraitLayer(Graphics* g, Image* image, const Rect& bounds, float x, float y,
	float scaleX = 1.0f, float scaleY = 1.0f)
{
	const float sx = bounds.mWidth / 180.0f;
	const float sy = bounds.mHeight / 180.0f;
	TodDrawImageScaledF(g, image, bounds.mX + (x - 596.0f) * sx, bounds.mY + (y - 116.0f) * sy, scaleX * sx, scaleY * sy);
}

static std::string ZombatarFacialHairTrack(int style, bool outline)
{
	return StrFormat(outline ? "facialHair_%02d_line" : "facialHair_%02d", style);
}

ZombatarWidget::ZombatarWidget(LawnApp* theApp) {
	TodLoadResources("DelayLoad_Zombatar");
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	TodLoadResources("DelayLoad_Almanac");

	mBackButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Back,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_BLANK,
		Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT,
		Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT
	);
	mBackButton->Resize(278, 528.5f, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mHeight);
	mBackButton->mClip = false;
	mBackButton->mTranslateX = 0;
	mBackButton->mTranslateY = 0;

	mFinishButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Finish,
		this,
		"",
		nullptr,
		Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON,
		Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT,
		Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT
	);
	mFinishButton->Resize(445, 472, IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mWidth, IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mHeight);
	mFinishButton->mClip = false;
	mFinishButton->mTranslateX = 0;
	mFinishButton->mTranslateY = 0;

	const auto makeFlowButton = [&](int id, Image* normal, Image* highlight, int x, int y)
	{
		auto* button = MakeNewButton(id, this, _S(""), nullptr, normal, highlight, highlight);
		button->Resize(x - normal->GetWidth() / 2, y, normal->GetWidth(), normal->GetHeight());
		button->mClip = false;
		return button;
	};
	mFinishFlowButtons[0] = makeFlowButton(ZombatarScreen_Confirm, IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT, 230, 386);
	mFinishFlowButtons[1] = makeFlowButton(ZombatarScreen_Cancel, IMAGE_ZOMBATAR_BACK_BUTTON,
		IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT, 570, 386);
	mFinishFlowButtons[2] = makeFlowButton(ZombatarScreen_New, IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON,
		IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT, 400, 450);
	mFinishFlowButtons[3] = makeFlowButton(ZombatarScreen_Delete, IMAGE_BLANK, IMAGE_BLANK, 485, 46);
	mFinishFlowButtons[3]->Resize(430, 46, 110, 38);

	mFinishFlowButtons[4] = makeFlowButton(ZombatarScreen_Previous, IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT, 205, 222);
	mFinishFlowButtons[5] = makeFlowButton(ZombatarScreen_Next, IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT, 595, 222);

	mSkinButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Skin,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON,
		Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON,
		Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT
	);
	mSkinButton->Resize(58, 128, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mHeight);
	mSkinButton->mClip = false;
	mSkinButton->mTranslateX = 0;
	mSkinButton->mTranslateY = 0;

	mHairButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Hair,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON,
		Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT
	);
	mHairButton->Resize(58, 164, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mHeight);
	mHairButton->mClip = false;
	mHairButton->mTranslateX = 0;
	mHairButton->mTranslateY = 0;

	mFacialHairButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_FacialHair,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON,
		Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT
	);
	mFacialHairButton->Resize(58, 200, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mHeight);
	mFacialHairButton->mClip = false;
	mFacialHairButton->mTranslateX = 0;
	mFacialHairButton->mTranslateY = 0;

	mTidbitsButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Tidbits,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON,
		Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT
	);
	mTidbitsButton->Resize(58, 236, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mHeight);
	mTidbitsButton->mClip = false;
	mTidbitsButton->mTranslateX = 0;
	mTidbitsButton->mTranslateY = 0;

	mEyeWearButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_EyeWear,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON,
		Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT
	);
	mEyeWearButton->Resize(58, 272, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mHeight);
	mEyeWearButton->mClip = false;
	mEyeWearButton->mTranslateX = 0;
	mEyeWearButton->mTranslateY = 0;

	mClothesButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Clothes,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON,
		Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT
	);
	mClothesButton->Resize(58, 308, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mHeight);
	mClothesButton->mClip = false;
	mClothesButton->mTranslateX = 0;
	mClothesButton->mTranslateY = 0;

	mAccessoryButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Acessory,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON,
		Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT
	);
	mAccessoryButton->Resize(58, 344, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mHeight);
	mAccessoryButton->mClip = false;
	mAccessoryButton->mTranslateX = 0;
	mAccessoryButton->mTranslateY = 0;

	mHatsButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Hats,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_HATS_BUTTON,
		Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT
	);
	mHatsButton->Resize(58, 380, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mHeight);
	mHatsButton->mClip = false;
	mHatsButton->mTranslateX = 0;
	mHatsButton->mTranslateY = 0;

	mBackdropsButton = MakeNewButton(
		ZombatarWidget::ZombatarScreen_Backdrops,
		this,
		_S(""),
		nullptr,
		Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON,
		Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER,
		Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT
	);
	mBackdropsButton->Resize(58, 416, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mHeight);
	mBackdropsButton->mClip = false;
	mBackdropsButton->mTranslateX = 0;
	mBackdropsButton->mTranslateY = 0;

	for (int i = 0; i < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS; i++) {
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)i);

		NewLawnButton* aZombatarItem = MakeNewButton(
			ZombatarWidget::ZombatarScreen_Items + i,
			this,
			_S(""),
			nullptr,
			Sexy::IMAGE_BLANK,
			Sexy::IMAGE_BLANK,
			Sexy::IMAGE_BLANK
		);
		aZombatarItem->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * aDef.mColumn, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * aDef.mRow, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
		if (aDef.mCategory == ZombatarCategory_FacialHair)
		{
			const auto cell = ZombatarFacialHairCell((i - ZOMBATAR_FACIALHAIR_1) % ZOMBATAR_FACIAL_HAIR_PAGE_SIZE);
			aZombatarItem->Resize(cell.mX, cell.mY, cell.mWidth, cell.mHeight);
		}
		aZombatarItem->mClip = false;
		aZombatarItem->mTranslateX = 0;
		aZombatarItem->mTranslateY = 0;
		mZombatarItems[i] = aZombatarItem;
	}

	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 2; y++) {
			NewLawnButton* mColorPallete = MakeNewButton(
				ZombatarWidget::ZombatarScreen_Palletes + x + y * 9,
				this,
				_S(""),
				nullptr,
				Sexy::IMAGE_BLANK,
				Sexy::IMAGE_BLANK,
				Sexy::IMAGE_BLANK
			);
			mColorPallete->Resize(238 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth) * x, 367 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight) * y, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
			mColorPallete->mClip = false;
			mColorPallete->mTranslateX = 0;
			mColorPallete->mTranslateY = 0;
			mColorPalletes[x + y * 9] = mColorPallete;
		}
	}

	mClearPickButton = MakeNewButton(ZombatarWidget::ZombatarScreen_ClearPick, this, _S(""), nullptr, Sexy::IMAGE_BLANK, Sexy::IMAGE_BLANK, Sexy::IMAGE_BLANK);
	mClearPickButton->Resize(0, 0, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	mClearPickButton->mClip = false;
	mClearPickButton->mTranslateX = 0;
	mClearPickButton->mTranslateY = 0;

	mClearPalleteButton = MakeNewButton(ZombatarWidget::ZombatarScreen_ClearPallete, this, _S(""), nullptr, Sexy::IMAGE_BLANK, Sexy::IMAGE_BLANK, Sexy::IMAGE_BLANK);
	mClearPalleteButton->Resize(0, 0, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
	mClearPalleteButton->mClip = false;
	mClearPalleteButton->mTranslateX = 0;
	mClearPalleteButton->mTranslateY = 0;

	CreateZombatarClothes();
	mZombie = nullptr;
	ResetZombatar();
}

void ZombatarWidget::SetCategory(ZombatarCategory theCategory) {
	mCurCategory = theCategory;

	UpdateItems();
	UpdatePalletes();

	mSkinButton->mButtonImage = theCategory == ZombatarCategory_Skin ? IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->mOverImage = theCategory == ZombatarCategory_Skin ? IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_SKIN_BUTTON;
	mHairButton->mButtonImage = theCategory == ZombatarCategory_Hairs ? IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_HAIR_BUTTON;
	mHairButton->mOverImage = theCategory == ZombatarCategory_Hairs ? IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	mFacialHairButton->mButtonImage = theCategory == ZombatarCategory_FacialHair ? IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
	mFacialHairButton->mOverImage = theCategory == ZombatarCategory_FacialHair ? IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	mTidbitsButton->mButtonImage = theCategory == ZombatarCategory_Tidbits ? IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_TIDBITS_BUTTON;
	mTidbitsButton->mOverImage = theCategory == ZombatarCategory_Tidbits ? IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	mEyeWearButton->mButtonImage = theCategory == ZombatarCategory_EyeWears ? IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
	mEyeWearButton->mOverImage = theCategory == ZombatarCategory_EyeWears ? IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	mClothesButton->mButtonImage = theCategory == ZombatarCategory_Clothes ? IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_CLOTHES_BUTTON;
	mClothesButton->mOverImage = theCategory == ZombatarCategory_Clothes ? IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	mAccessoryButton->mButtonImage = theCategory == ZombatarCategory_Accessories ? IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
	mAccessoryButton->mOverImage = theCategory == ZombatarCategory_Accessories ? IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	mHatsButton->mButtonImage = theCategory == ZombatarCategory_Hats ? IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_HATS_BUTTON;
	mHatsButton->mOverImage = theCategory == ZombatarCategory_Hats ? IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	mBackdropsButton->mButtonImage = theCategory == ZombatarCategory_Backdrops ? IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
	mBackdropsButton->mOverImage = theCategory == ZombatarCategory_Backdrops ? IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;

	mClearPickButton->SetDisabled(true);
	mClearPalleteButton->SetDisabled(true);

	if (theCategory == ZombatarCategory_Hairs) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * 4, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	} else if (theCategory == ZombatarCategory_FacialHair) {
		mClearPickButton->SetDisabled(false);
		const auto cell = ZombatarFacialHairCell(ZombatarFacialHairCount(mFacialHairPage));
		mClearPickButton->Resize(cell.mX, cell.mY, cell.mWidth, cell.mHeight);
	} else if (theCategory == ZombatarCategory_Tidbits) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * 2, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	} else if (theCategory == ZombatarCategory_EyeWears) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * 4, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	} else if (theCategory == ZombatarCategory_Clothes) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	} else if (theCategory == ZombatarCategory_Accessories) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * 3, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	} else if (theCategory == ZombatarCategory_Hats) {
		mClearPickButton->SetDisabled(false);
		mClearPickButton->Resize(166 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4) * 2, 138 + (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 4) * 2, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	}

	if (theCategory == ZombatarCategory::ZombatarCategory_Skin) {
		for (int x = 0; x < 6; x++) {
			for (int y = 0; y < 2; y++) {
				NewLawnButton* mColorPallete = mColorPalletes[x + y * 6];
				mColorPallete->Resize(274 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth) * x, 362 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight) * y, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
			}
		}
	}
	else {
		for (int x = 0; x < 9; x++) {
			for (int y = 0; y < 2; y++) {
				if (x + y * 9 == 17) break;                                        
				NewLawnButton* mColorPallete = mColorPalletes[x + y * 9];
				mColorPallete->Resize(238 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth) * x, 367 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight) * y, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
			}
		}

		mClearPalleteButton->SetDisabled(false);
		mClearPalleteButton->Resize(238 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth) * 8, 367 + (4 + Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight), Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
	}
}

void ZombatarWidget::SetZombatarRef(int* theTarget, int theValue) {
	*theTarget = theValue;
}

void ZombatarWidget::ResetZombatar(bool loadSaved) {
	mFacialHairPage = 0;
	if (mZombie)
	{
		mZombie->DieNoLoot();
		delete mZombie;
		mZombie = nullptr;
	}
	mCurSkinPallete = 0;
	mCurBackground = ZombatarItem::ZOMBATAR_BACKGROUND_BLANK;
	mCurBackgroundPallete = -1;
	mCurHair = -1;
	mCurHairPallete = -1;
	mCurFacialHair = -1;
	mCurFacialHairPallete = -1;
	mCurTidbit = -1;
	mCurTidbitPallete = -1;
	mCurEyeWear = -1;
	mCurEyeWearPallete = -1;
	mCurClothe = -1;
	mCurAccessory = -1;
	mCurAccessoryPallete = -1;
	mCurHat = -1;
	mCurHatPallete = -1;
	if (loadSaved) mSavedZombatars = ReadZombatarCollection(mApp);
	const bool hasSaved = loadSaved && !mSavedZombatars.empty();
	if (hasSaved)
	{
		mSavedIndex = 0;
		static_cast<ZombatarAppearance&>(*this) = mSavedZombatars.front();
	}
	SetCategory(ZombatarCategory_Skin);
	SetupZombie();
	UpdateZombieAvatar();
	SetView(hasSaved ? View::Gallery : View::Editor);
}

static void SetupZombatarHead(LawnApp* app, Zombie* zombie)
{
	auto* body = app->ReanimationTryToGet(zombie->mBodyReanimID);
	if (!body || !body->TrackExists("anim_head1")) return;

	auto* head = app->AddReanimation(0.0f, 0.0f, 0, REANIM_ZOMBATAR);
	head->PlayReanim("anim_head1", REANIM_LOOP, 0, body->mAnimRate);
	auto* track = body->GetTrackInstanceByName("anim_head1");
	track->mForceDontRender = true;
	for (const char* name : {"anim_head2", "anim_hair"})
		if (body->TrackExists(name)) body->AssignRenderGroupToTrack(name, RENDER_GROUP_HIDDEN);
	auto* attachment = AttachReanim(track->mAttachmentID, head, 0, 0);
	body->mFrameBasePose = 0;
	TodScaleRotateTransformMatrix(attachment->mOffset, -20.0f, -2.5f, 0.2f, 1.0f, 1.0f);
	zombie->mZombatarTidbitID = app->ReanimationGetID(head);
}

void ZombatarWidget::SetupZombie() {
	if (!mZombie)
	{
		mZombie = new Zombie();
		mZombie->mBoard = nullptr;
		mZombie->ZombieInitialize(0, ZombieType::ZOMBIE_FLAG, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
		mZombie->mX = mZombie->mPosX = 641;
		mZombie->mY = mZombie->mPosY = 351;

		SetupZombatarHead(mApp, mZombie);
	}
}

ZombatarWidget::~ZombatarWidget() {
	if (mBackButton) delete mBackButton;
	if (mFinishButton) delete mFinishButton;
	for (auto* button : mFinishFlowButtons) delete button;
	if (mSkinButton) delete mSkinButton;
	if (mHairButton) delete mHairButton;
	if (mFacialHairButton) delete mFacialHairButton;
	if (mTidbitsButton) delete mTidbitsButton;
	if (mEyeWearButton) delete mEyeWearButton;
	if (mClothesButton) delete mClothesButton;
	if (mAccessoryButton) delete mAccessoryButton;
	if (mHatsButton) delete mHatsButton;
	if (mBackdropsButton) delete mBackdropsButton;
	if (mClearPickButton) delete mClearPickButton;
	if (mClearPalleteButton) delete mClearPalleteButton;
	for (NewLawnButton* pallete : mColorPalletes) {
		delete pallete;
	}

	for (NewLawnButton* item : mZombatarItems) {
		delete item;
	}

	if (mZombie)
	{
		mZombie->DieNoLoot();
		delete mZombie;
		mZombie = nullptr;
	}
}


bool mOnBackBtn = false;

void ZombatarWidget::Update() {
	mBackButton->MarkDirty();
	mFinishButton->MarkDirty();
	mSkinButton->MarkDirty();
	mHairButton->MarkDirty();
	mFacialHairButton->MarkDirty();
	mTidbitsButton->MarkDirty();
	mEyeWearButton->MarkDirty();
	mClothesButton->MarkDirty();
	mAccessoryButton->MarkDirty();
	mHatsButton->MarkDirty();
	mBackdropsButton->MarkDirty();
	mClearPickButton->MarkDirty();
	mClearPalleteButton->MarkDirty();
	if (mZombie) mZombie->Update();
}

void ZombatarWidget::Draw(Graphics* g) {
	if (mApp->GetDialog(Dialogs::DIALOG_STORE) || mApp->GetDialog(Dialogs::DIALOG_ALMANAC))
		return;

	g->PushState();
	g->SetLinearBlend(true);
	if (mView != View::Editor)
	{
		DrawFinishView(g);
		g->PopState();
		return;
	}
	g->DrawImage(IMAGE_ZOMBATAR_MAIN_BG, 0, 0);

	Graphics leafG(*g);
	leafG.mTransX -= BOARD_WIDTH;
	for (int i = 0; i < 3; i++)
	{
		mApp->ReanimationGet(mApp->mGameSelector->mFlowerReanimID[i])->Draw(&leafG);
	}

	g->DrawImageF(IMAGE_ZOMBATAR_DAY_GRAVE, 8.55f, 432.05f);

	g->DrawImageF(IMAGE_ZOMBATAR_WIDGET_BG, 26, 25);
	DrawZombiePortrait(g);
	DrawZombatarItems(g);
	DrawColorPalletes(g);
	if (mCurCategory == ZombatarCategory_FacialHair)
	{
		for (bool next : {false, true})
		{
			const bool enabled = next ? mFacialHairPage + 1 < ZOMBATAR_FACIAL_HAIR_PAGE_COUNT : mFacialHairPage > 0;
			Image* arrow = next ? IMAGE_ZOMBATAR_NEXT_BUTTON : IMAGE_ZOMBATAR_PREV_BUTTON;
			Image* highlight = next ? IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
			const Rect bounds = ZombatarPageArrowRect(next, arrow ? arrow : highlight);
			Image* image = enabled && highlight ? highlight : arrow;
			if (image) g->DrawImage(image, bounds.mX, bounds.mY);
		}
		TodDrawString(g, StrFormat(_S("Page %d / %d"), mFacialHairPage + 1, ZOMBATAR_FACIAL_HAIR_PAGE_COUNT),
			ZOMBATAR_COLORS_X + IMAGE_ZOMBATAR_COLORS_BG->GetWidth() / 2, ZOMBATAR_COLORS_Y + IMAGE_ZOMBATAR_COLORS_BG->GetHeight() + 19,
			FONT_BRIANNETOD16, Color::Black, DS_ALIGN_CENTER);
	}
	DrawZombieAvatar(g);
}

void ZombatarWidget::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	AddWidget(mBackButton);
	AddWidget(mFinishButton);
	for (auto* button : mFinishFlowButtons) AddWidget(button);
	AddWidget(mSkinButton);
	AddWidget(mHairButton);
	AddWidget(mFacialHairButton);
	AddWidget(mTidbitsButton);
	AddWidget(mEyeWearButton);
	AddWidget(mClothesButton);
	AddWidget(mAccessoryButton);
	AddWidget(mHatsButton);
	AddWidget(mBackdropsButton);
	AddWidget(mClearPickButton);
	AddWidget(mClearPalleteButton);
	for (NewLawnButton* pallete : mColorPalletes) {
		AddWidget(pallete);
	}

	for (NewLawnButton* item : mZombatarItems) {
		AddWidget(item);
	}
}

          
void ZombatarWidget::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	RemoveWidget(mBackButton);
	RemoveWidget(mFinishButton);
	for (auto* button : mFinishFlowButtons) RemoveWidget(button);
	RemoveWidget(mSkinButton);
	RemoveWidget(mHairButton);
	RemoveWidget(mFacialHairButton);
	RemoveWidget(mTidbitsButton);
	RemoveWidget(mEyeWearButton);
	RemoveWidget(mClothesButton);
	RemoveWidget(mAccessoryButton);
	RemoveWidget(mHatsButton);
	RemoveWidget(mBackdropsButton);
	RemoveWidget(mClearPickButton);
	RemoveWidget(mClearPalleteButton);
	for (NewLawnButton* pallete : mColorPalletes) {
		RemoveWidget(pallete);
	}

	for (NewLawnButton* item : mZombatarItems) {
		RemoveWidget(item);
	}
}

          
void ZombatarWidget::OrderInManagerChanged()
{
	PutInfront(mBackButton, this);
	PutInfront(mFinishButton, this);
	for (auto* button : mFinishFlowButtons) PutInfront(button, this);
	PutInfront(mSkinButton, this);
	PutInfront(mHairButton, this);
	PutInfront(mFacialHairButton, this);
	PutInfront(mTidbitsButton, this);
	PutInfront(mEyeWearButton, this);
	PutInfront(mClothesButton, this);
	PutInfront(mAccessoryButton, this);
	PutInfront(mHatsButton, this);
	PutInfront(mBackdropsButton, this);

	for (NewLawnButton* pallete : mColorPalletes) {
		PutInfront(pallete, this);
	}

	for (NewLawnButton* items : mZombatarItems) {
		PutInfront(items, this);
	}

	PutInfront(mClearPickButton, this);
	PutInfront(mClearPalleteButton, this);
	PutBehind(mApp->mGameSelector->mOverlayWidget, this);
}


void ZombatarWidget::SetView(View view)
{
	mView = view;
	const bool editing = view == View::Editor;
	for (auto* button : {mFinishButton, mSkinButton, mHairButton, mFacialHairButton, mTidbitsButton,
		mEyeWearButton, mClothesButton, mAccessoryButton, mHatsButton, mBackdropsButton,
		mClearPickButton, mClearPalleteButton})
		button->SetVisible(editing);
	for (auto* button : mZombatarItems) button->SetVisible(editing);
	for (auto* button : mColorPalletes) button->SetVisible(editing);
	const bool gallery = view == View::Gallery;
	for (int i = 0; i < 4; ++i)
		mFinishFlowButtons[i]->SetVisible(i < 2 ? view == View::Warning : gallery);
	mFinishFlowButtons[4]->SetVisible(gallery && mSavedIndex > 0);
	mFinishFlowButtons[5]->SetVisible(gallery && mSavedIndex + 1 < static_cast<int>(mSavedZombatars.size()));
	mBackButton->SetVisible(view != View::Warning);
	mBackButton->mButtonImage = view == View::Gallery ? IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT : IMAGE_BLANK;
	MarkDirty();
}

void ZombatarWidget::DrawFinishView(Graphics* g)
{
	g->SetColor(Color::Black);
	g->FillRect(0, 0, 800, 600);
	if (mView == View::Warning)
	{
		TodDrawString(g, _S("WARNING!"), 400, 114, FONT_HOUSEOFTERROR28, Color(255, 215, 30), DS_ALIGN_CENTER);
		TodDrawStringWrapped(g, _S("Are you sure you're done creating your Zombatar?\n"
			"If you wish to make any changes after you click \"Finished\" below, you'll need to start over."),
			Rect(64, 171, 672, 150), FONT_DWARVENTODCRAFT24, Color::White, DS_ALIGN_CENTER);
		TodDrawString(g, _S("All done. Let me have it!"), 230, 369, FONT_BRIANNETOD16, Color::White, DS_ALIGN_CENTER);
		TodDrawString(g, _S("Wait! I'm not done!"), 570, 369, FONT_BRIANNETOD16, Color::White, DS_ALIGN_CENTER);
		return;
	}

	DrawZombiePortrait(g, Rect(250, 88, 300, 300));
	TodDrawString(g, StrFormat(_S("%d / %d"), mSavedIndex + 1, static_cast<int>(mSavedZombatars.size())), 250, 72, FONT_BRIANNETOD16, Color::White, DS_ALIGN_LEFT);
	TodDrawString(g, _S("DELETE?"), 485, 72, FONT_DWARVENTODCRAFT18,
		mFinishFlowButtons[3]->mIsOver ? Color(255, 215, 30) : Color::White, DS_ALIGN_CENTER);
}

static SexyString ZombatarProfilePath(const PlayerInfo& profile)
{
	return GetAppDataFolder() + StrFormat(_S("userdata/zombatar-user%d.dat"), profile.mId);
}

static ZombatarAppearance ReadZombatarAppearance(Buffer& data)
{
	ZombatarAppearance appearance{};
	const auto readItem = [&](ZombatarCategory category, int fallback)
	{
		const long item = data.ReadLong();
		return item >= 0 && item < NUM_ZOMBATAR_ITEMS &&
			GetZombatarDefinition((ZombatarItem)item).mCategory == category ? static_cast<int>(item) : fallback;
	};
	const auto readPalette = [&](int count, int fallback)
	{
		const long palette = data.ReadLong();
		return palette >= 0 && palette < count ? static_cast<int>(palette) : fallback;
	};

	appearance.mCurSkinPallete = readPalette(NUM_SKIN_COLOR_PALLETES, 0);
	appearance.mCurHair = readItem(ZombatarCategory_Hairs, -1);
	appearance.mCurHairPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurFacialHair = readItem(ZombatarCategory_FacialHair, -1);
	appearance.mCurFacialHairPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurBackground = readItem(ZombatarCategory_Backdrops, ZOMBATAR_BACKGROUND_BLANK);
	appearance.mCurBackgroundPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurTidbit = readItem(ZombatarCategory_Tidbits, -1);
	appearance.mCurTidbitPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurEyeWear = readItem(ZombatarCategory_EyeWears, -1);
	appearance.mCurEyeWearPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurClothe = readItem(ZombatarCategory_Clothes, -1);
	appearance.mCurAccessory = readItem(ZombatarCategory_Accessories, -1);
	appearance.mCurAccessoryPallete = readPalette(NUM_COLOR_PALLETES, -1);
	appearance.mCurHat = readItem(ZombatarCategory_Hats, -1);
	appearance.mCurHatPallete = readPalette(NUM_COLOR_PALLETES, -1);
	return appearance;
}

static std::vector<ZombatarAppearance> ReadZombatarCollection(LawnApp* app)
{
	if (!app->mPlayerInfo) return {};
	Buffer data;
	if (!app->ReadBufferFromFile(ZombatarProfilePath(*app->mPlayerInfo), &data, false) || data.GetDataLen() < 4)
		return {};
	const long version = data.ReadLong();
	const int length = data.GetDataLen();
	if (version == 1 && length == 17 * 4) return {ReadZombatarAppearance(data)};
	constexpr int recordSize = 16 * 4;
	if (version != 2 || (length - 4) % recordSize != 0) return {};
	std::vector<ZombatarAppearance> saved;
	for (int offset = 4; offset < length; offset += recordSize)
		saved.push_back(ReadZombatarAppearance(data));
	return saved;
}

static bool WriteZombatarCollection(LawnApp* app, const std::vector<ZombatarAppearance>& saved)
{
	if (!app->mPlayerInfo) return false;
	Buffer data;
	data.WriteLong(2);
	for (const auto& a : saved)
	{
		for (int value : {a.mCurSkinPallete, a.mCurHair, a.mCurHairPallete, a.mCurFacialHair, a.mCurFacialHairPallete,
			a.mCurBackground, a.mCurBackgroundPallete, a.mCurTidbit, a.mCurTidbitPallete, a.mCurEyeWear, a.mCurEyeWearPallete,
			a.mCurClothe, a.mCurAccessory, a.mCurAccessoryPallete, a.mCurHat, a.mCurHatPallete})
			data.WriteLong(value);
	}
	MkDir(GetAppDataFolder() + _S("userdata"));
	const auto path = ZombatarProfilePath(*app->mPlayerInfo);
	const auto temp = path + _S(".tmp");
	FILE* file = sexyfopen(temp.c_str(), _S("wb"));
	if (!file) return false;
	const auto length = static_cast<size_t>(data.GetDataLen());
	const bool written = fwrite(data.GetDataPtr(), 1, length, file) == length;
	const bool closed = fclose(file) == 0;
	if (!written || !closed)
	{
		app->EraseFile(temp);
		return false;
	}
#ifdef _USE_WIDE_STRING
	const bool replaced = MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
	const bool replaced = MoveFileExA(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#endif
	if (!replaced) app->EraseFile(temp);
	return replaced;
}

bool ZombatarWidget::SaveZombatar()
{
	mSavedZombatars.push_back(*this);
	if (WriteZombatarCollection(mApp, mSavedZombatars))
	{
		mSavedIndex = static_cast<int>(mSavedZombatars.size()) - 1;
		return true;
	}
	mSavedZombatars.pop_back();
	return false;
}

void ZombatarWidget::ShowSavedZombatar(int index)
{
	if (index < 0 || index >= static_cast<int>(mSavedZombatars.size())) return;
	mSavedIndex = index;
	static_cast<ZombatarAppearance&>(*this) = mSavedZombatars[index];
	UpdateZombieAvatar();
	SetView(View::Gallery);
}

void ZombatarWidget::MouseUp(int x, int y, int theClickCount)
{
	Widget::MouseUp(x, y, theClickCount);
	if (mView != View::Editor || theClickCount != 1 || mCurCategory != ZombatarCategory_FacialHair) return;
	for (bool next : {false, true})
	{
		const int page = mFacialHairPage + (next ? 1 : -1);
		if (page < 0 || page >= ZOMBATAR_FACIAL_HAIR_PAGE_COUNT) continue;
		Image* arrow = next ? IMAGE_ZOMBATAR_NEXT_BUTTON : IMAGE_ZOMBATAR_PREV_BUTTON;
		Image* highlight = next ? IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT : IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
		if (!ZombatarPageArrowRect(next, arrow ? arrow : highlight).Contains(x, y)) continue;
		mFacialHairPage = page;
		SetCategory(mCurCategory);
		mApp->PlaySample(Sexy::SOUND_TAP);
		MarkDirty();
		return;
	}
}

void ZombatarWidget::ButtonDepress(int theId)
{
	if (mApp->mGameSelector->mSlideCounter > 0)
		return;

	if (mView != View::Editor)
	{
		if (mView == View::Warning && theId == ZombatarScreen_Cancel)
			SetView(View::Editor);
		else if (mView == View::Warning && theId == ZombatarScreen_Confirm)
		{
			if (SaveZombatar())
				SetView(View::Gallery);
			else
				mApp->DoDialog(DIALOG_MESSAGE, true, _S("Zombatar"), _S("Could not save your Zombatar."),
					_S("[DIALOG_BUTTON_OK]"), Dialog::BUTTONS_FOOTER);
		}
		else if (mView == View::Gallery && theId == ZombatarScreen_New)
			ResetZombatar(false);
		else if (mView == View::Gallery && theId == ZombatarScreen_Previous)
			ShowSavedZombatar(mSavedIndex - 1);
		else if (mView == View::Gallery && theId == ZombatarScreen_Next)
			ShowSavedZombatar(mSavedIndex + 1);
		else if (mView == View::Gallery && theId == ZombatarScreen_Delete)
		{
			auto remaining = mSavedZombatars;
			remaining.erase(remaining.begin() + mSavedIndex);
			if (WriteZombatarCollection(mApp, remaining))
			{
				mSavedZombatars.swap(remaining);
				if (mSavedZombatars.empty()) ResetZombatar(false);
				else ShowSavedZombatar(pvz_min(mSavedIndex, static_cast<int>(mSavedZombatars.size()) - 1));
			}
			else
				mApp->DoDialog(DIALOG_MESSAGE, true, _S("Zombatar"), _S("Could not delete your Zombatar."),
					_S("[DIALOG_BUTTON_OK]"), Dialog::BUTTONS_FOOTER);
		}
		if (theId != ZombatarScreen_Back || mView == View::Warning) return;
	}


	if (theId >= ZombatarWidget::ZombatarScreen_Skin && theId <= ZombatarWidget::ZombatarScreen_Backdrops) {
		ZombatarCategory aPage = (ZombatarCategory)(theId - (int)ZombatarWidget::ZombatarScreen_Skin);
		if (aPage != mCurCategory)
		{
			SetCategory(aPage);
			mApp->PlaySample(Sexy::SOUND_TAP);
		}

		return;
	}
	
	if (theId >= ZombatarWidget::ZombatarScreen_Palletes && theId < ZombatarWidget::ZombatarScreen_Palletes + NUM_COLOR_PALLETES) {
		int aPallete = theId - ZombatarWidget::ZombatarScreen_Palletes;
		int* aTargetPallete = nullptr;

		if (mCurCategory == ZombatarCategory::ZombatarCategory_Skin) aTargetPallete = &mCurSkinPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetPallete = &mCurHairPallete;
		else if (mCurCategory == ZombatarCategory_FacialHair) aTargetPallete = &mCurFacialHairPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetPallete = &mCurTidbitPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Backdrops) aTargetPallete = &mCurBackgroundPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetPallete = &mCurEyeWearPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetPallete = &mCurAccessoryPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetPallete = &mCurHatPallete;

		if (aTargetPallete && aPallete != *aTargetPallete)
		{
			SetZombatarRef(aTargetPallete, aPallete);
			mApp->PlaySample(Sexy::SOUND_TAP);
			UpdateZombieAvatar();
		}
		return;
	}

	if (theId >= ZombatarWidget::ZombatarScreen_Items && theId < ZombatarWidget::ZombatarScreen_Items + NUM_ZOMBATAR_ITEMS) {
		int aItem = theId - ZombatarWidget::ZombatarWidget::ZombatarScreen_Items;
		if (mZombatarItems[aItem]->mDisabled) return;
		int* aTargetItem = nullptr;

		if (mCurCategory == ZombatarCategory::ZombatarCategory_Backdrops) aTargetItem = &mCurBackground;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetItem = &mCurHair; 
		else if (mCurCategory == ZombatarCategory_FacialHair) aTargetItem = &mCurFacialHair;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetItem = &mCurTidbit;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetItem = &mCurEyeWear;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Clothes) aTargetItem = &mCurClothe;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetItem = &mCurAccessory;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetItem = &mCurHat;

		if (aTargetItem && aItem != *aTargetItem)
		{
			SetZombatarRef(aTargetItem, aItem);
			UpdatePalletes();
			mApp->PlaySample(Sexy::SOUND_TAP);
			UpdateZombieAvatar();
		}
		return;
	}

	if (theId == ZombatarWidget::ZombatarScreen_ClearPick) {
		int aItem = -1;
		int* aTargetItem = nullptr;

		if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetItem = &mCurHair;
		else if (mCurCategory == ZombatarCategory_FacialHair) aTargetItem = &mCurFacialHair;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetItem = &mCurTidbit;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetItem = &mCurEyeWear;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Clothes) aTargetItem = &mCurClothe;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetItem = &mCurAccessory;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetItem = &mCurHat;

		if (aTargetItem && aItem != *aTargetItem)
		{
			SetZombatarRef(aTargetItem, aItem);
			UpdatePalletes();
			mApp->PlaySample(Sexy::SOUND_TAP);
			UpdateZombieAvatar();
		}
	}

	if (theId == ZombatarWidget::ZombatarScreen_ClearPallete) {
		int aPallete = -1;
		int* aTargetPallete = nullptr;

		if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetPallete = &mCurHairPallete;
		else if (mCurCategory == ZombatarCategory_FacialHair) aTargetPallete = &mCurFacialHairPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetPallete = &mCurTidbitPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Backdrops) aTargetPallete = &mCurBackgroundPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetPallete = &mCurEyeWearPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetPallete = &mCurAccessoryPallete;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetPallete = &mCurHatPallete;

		if (aTargetPallete && aPallete != *aTargetPallete)
		{
			SetZombatarRef(aTargetPallete, aPallete);
			mApp->PlaySample(Sexy::SOUND_TAP);
			UpdateZombieAvatar();
		}
		return;
	}

	switch (theId)
	{
		case ZombatarWidget::ZombatarScreen_Finish:
			SetView(View::Warning);
			return;
		case ZombatarWidget::ZombatarScreen_Back:
		{
			mBackButton->SetDisabled(true);
			mBackButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_PRESSED;
			mBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_PRESSED;
			mBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_PRESSED;
			mApp->mGameSelector->SlideTo(0, 0);
			mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector);
			mApp->mGameSelector->mAdventureButton->SetDisabled(false);
			mApp->mGameSelector->mMinigameButton->SetDisabled(false);
			mApp->mGameSelector->mPuzzleButton->SetDisabled(false);
			mApp->mGameSelector->mOptionsButton->SetDisabled(false);
			mApp->mGameSelector->mQuitButton->SetDisabled(false);
			mApp->mGameSelector->mHelpButton->SetDisabled(false);
			mApp->mGameSelector->mChangeUserButton->SetDisabled(false);
			mApp->mGameSelector->mStoreButton->SetDisabled(false);
			mApp->mGameSelector->mAlmanacButton->SetDisabled(false);
			mApp->mGameSelector->mSurvivalButton->SetDisabled(false);
			mApp->mGameSelector->mZenGardenButton->SetDisabled(false);
#ifdef _HAS_ZOMBATAR
			mApp->mGameSelector->mZombatarButton->SetDisabled(false);
#endif
#ifdef _HAS_ACHIEVEMENTS
			mApp->mGameSelector->mAchievementsButton->SetDisabled(false);
#endif
#ifdef _HAS_MORESCREEN 
			mApp->mGameSelector->mQuickPlayButton->SetDisabled(false);
#endif
			break;
		}
	}

	mApp->PlaySample(Sexy::SOUND_TAP);
}

void ZombatarWidget::ButtonMouseEnter(int theId)
{
	if (mApp->mGameSelector->mSlideCounter > 0)
		return;

	if (theId == (int)ZombatarWidget::ZombatarScreen_Back)
	{
		mApp->PlayFoley(FoleyType::FOLEY_BLEEP);
	}
}

void ZombatarWidget::ButtonPress(int theId, int theClickCount)
{
	if (mApp->mGameSelector->mSlideCounter > 0)
		return;
}

void ZombatarWidget::DrawColorPalletes(Graphics* g)
{
	int* aTargetItem = nullptr;
	int* aTargetPallete = nullptr;
	Color* curPalletes = nullptr;
	int palleteCount = 0;


	if (mCurCategory == ZombatarCategory::ZombatarCategory_Skin) {
		aTargetPallete = &mCurSkinPallete;
		curPalletes = gZombatarSkinPalletes;
		palleteCount = NUM_SKIN_COLOR_PALLETES;
	} else if (mCurCategory == ZombatarCategory::ZombatarCategory_Backdrops) {
		aTargetPallete = &mCurBackgroundPallete;
		aTargetItem = &mCurBackground;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) {
		aTargetPallete = &mCurHairPallete;
		aTargetItem = &mCurHair;
	}
	else if (mCurCategory == ZombatarCategory_FacialHair) {
		aTargetPallete = &mCurFacialHairPallete;
		aTargetItem = &mCurFacialHair;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) {
		aTargetPallete = &mCurTidbitPallete;
		aTargetItem = &mCurTidbit;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) {
		aTargetPallete = &mCurEyeWearPallete;
		aTargetItem = &mCurEyeWear;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Clothes) {
		aTargetItem = &mCurClothe;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) {
		aTargetPallete = &mCurAccessoryPallete;
		aTargetItem = &mCurAccessory;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) {
		aTargetPallete = &mCurHatPallete;
		aTargetItem = &mCurHat;
	}

	if (aTargetItem && *aTargetItem >= 0 && *aTargetItem < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS) {
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)*aTargetItem);
		curPalletes = aDef.mColorGroup;
		if (aDef.mColorGroup != nullptr)
		{
			palleteCount = aDef.mCategory != ZombatarCategory::ZombatarCategory_Skin ? NUM_COLOR_PALLETES : NUM_SKIN_COLOR_PALLETES;
		}
	}

	if (palleteCount == 12) {
		g->DrawImageF(IMAGE_ZOMBATAR_COLORS_BG_SMALL, 243, 325);
	}
	else {
		g->DrawImageF(IMAGE_ZOMBATAR_COLORS_BG, ZOMBATAR_COLORS_X, ZOMBATAR_COLORS_Y);
	}

	if (palleteCount == 0)
	{
		std::string aLabel = _S("[ZOMBATAR_COLOR_ITEM_NOT_CHOSEN]");
		if (aTargetItem && *aTargetItem >= 0 && *aTargetItem < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS) {
			ZombatarDefinition& aDef = gZombatarDefinitions[*aTargetItem];
			if (aDef.mColorGroup == nullptr) aLabel = _S("[ZOMBATAR_COLOR_NOT_APPLICABLE]");
		}
		TodDrawString(g, aLabel, 221.5f + IMAGE_ZOMBATAR_COLORS_BG->GetWidth() / 2, 395, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_CENTER);
	}
	else
	{
		for (size_t i = 0; i < palleteCount; i++) {
			NewLawnButton* aColorPallete = mColorPalletes[i];
			if (!aColorPallete || aColorPallete->mDisabled) {
				continue;
			}

			g->PushState();
			g->SetColorizeImages(true);
			g->SetColor(curPalletes[i]);
			bool isSelected = aTargetPallete  && *aTargetPallete == i;
			g->mColor.mAlpha = isSelected ? 255 : aColorPallete->mIsOver ? 128 : 64;
			if (isSelected) {
				g->DrawImageF(IMAGE_ZOMBATAR_COLORPICKER_HIGHLIGHT, aColorPallete->mX - 2.0f, aColorPallete->mY - 2.5f);
			}
			g->DrawImageF(IMAGE_ZOMBATAR_COLORPICKER, aColorPallete->mX, aColorPallete->mY);
			g->PopState();
		}

		if (!mClearPalleteButton->mDisabled) {
			DrawClearPallete(g, mClearPalleteButton, aTargetPallete);
		}
	}
}

static void DrawFacialHairArt(Graphics* g, const ZombatarDefinition& item, float x, float y, float maxWidth, float maxHeight, Color tint)
{
	Image* image = item.mImage ? *item.mImage : nullptr;
	Image* outline = item.mOutlineImage ? *item.mOutlineImage : nullptr;
	Image* bounds = outline ? outline : image;
	if (!bounds || bounds->GetWidth() <= 0 || bounds->GetHeight() <= 0) return;

	const float scale = pvz_min(maxWidth / bounds->GetWidth(), maxHeight / bounds->GetHeight());
	g->PushState();
	g->SetColorizeImages(true);
	if (image)
	{
		g->SetColor(tint);
		TodDrawImageScaledF(g, image, x - image->GetWidth() * scale / 2, y - image->GetHeight() * scale / 2, scale, scale);
	}
	if (outline)
	{
		g->SetColor(Color(255, 255, 255, tint.mAlpha));
		TodDrawImageScaledF(g, outline, x - outline->GetWidth() * scale / 2, y - outline->GetHeight() * scale / 2, scale, scale);
	}
	g->PopState();
}

void ZombatarWidget::DrawPortraitFacialHair(Graphics* g, const Rect& bounds)
{
	if (!mZombie || mCurFacialHair < ZOMBATAR_FACIALHAIR_1 || mCurFacialHair > ZOMBATAR_FACIALHAIR_24) return;

	Reanimation* head = mApp->ReanimationTryToGet(mZombie->mZombatarTidbitID);
	if (!head || !head->TrackExists("tidBits_02")) return;
	Image* reference = head->GetCurrentTrackImage("tidBits_02");
	Image* portraitReference = IMAGE_ZOMBATAR_TIDBITS_3;
	if (!reference || !portraitReference || reference->GetWidth() <= 0 || reference->GetHeight() <= 0) return;

	ReanimatorTransform anchor;
	head->GetCurrentTransform(head->FindTrackIndex("tidBits_02"), &anchor);
	float offsetX = 0.0f, offsetY = 0.0f;
	GetZombatarPortraitOffset(ZOMBATAR_TIDBIT_3, &offsetX, &offsetY);
	const float scaleX = static_cast<float>(portraitReference->GetWidth()) / reference->GetWidth();
	const float scaleY = static_cast<float>(portraitReference->GetHeight()) / reference->GetHeight();
	const auto& item = GetZombatarDefinition(static_cast<ZombatarItem>(mCurFacialHair));
	const Color tint = mCurFacialHairPallete >= 0 && item.mColorGroup ?
		item.mColorGroup[mCurFacialHairPallete] : Color::White;

	const int style = mCurFacialHair - ZOMBATAR_FACIALHAIR_1;
	const auto fillName = ZombatarFacialHairTrack(style, false);
	const auto outlineName = ZombatarFacialHairTrack(style, true);
	if (!head->TrackExists(fillName.c_str()) || !head->TrackExists(outlineName.c_str())) return;
	const int fillIndex = head->FindTrackIndex(fillName.c_str());
	const int outlineIndex = head->FindTrackIndex(outlineName.c_str());

	g->PushState();
	g->SetColorizeImages(true);
	const bool fillFirst = mCurFacialHair == ZOMBATAR_FACIALHAIR_22 || fillIndex < outlineIndex;
	for (int index : {fillFirst ? fillIndex : outlineIndex, fillFirst ? outlineIndex : fillIndex})
	{
		Image* authored = head->GetCurrentTrackImage(index == fillIndex ? fillName.c_str() : outlineName.c_str());
		Image** portraitLayer = index == fillIndex ? item.mImage : item.mOutlineImage;
		Image* image = portraitLayer && *portraitLayer ? *portraitLayer : authored;
		if (!authored || !image || image->GetWidth() <= 0 || image->GetHeight() <= 0) continue;
		if (index == fillIndex && !item.mImage) continue;
		ReanimatorTransform layer;
		head->GetCurrentTransform(index, &layer);
		g->SetColor(index == fillIndex ? tint : Color::White);
		DrawZombatarPortraitLayer(g, image, bounds,
			630.5f + offsetX + (layer.mTransX - anchor.mTransX) * scaleX,
			155.0f + offsetY + (layer.mTransY - anchor.mTransY) * scaleY,
			layer.mScaleX * scaleX * authored->GetWidth() / image->GetWidth(),
			layer.mScaleY * scaleY * authored->GetHeight() / image->GetHeight());
	}
	g->PopState();
}

void ZombatarWidget::DrawZombiePortrait(Graphics* g, const Rect& bounds) {
	g->PushState();
	g->ClipRect(bounds);
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurBackground));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(mCurBackgroundPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurBackgroundPallete]);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, 596, 116);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, 596, 116);
		g->PopState();
	}

	{
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(gZombatarSkinPalletes[mCurSkinPallete]);
		DrawZombatarPortraitLayer(g, IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, bounds, 630.5f, 155);
		g->PopState();
		DrawZombatarPortraitLayer(g, IMAGE_ZOMBATAR_ZOMBIE_BLANK, bounds, 630.5f, 155);
	}

	DrawPortraitFacialHair(g, bounds);

	if (mCurTidbit != -1)
	{
		{
			ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurTidbit));
			g->PushState();
			g->SetColorizeImages(true);
			g->SetColor(mCurTidbitPallete == -1 || aDef.mColorGroup == nullptr || mCurTidbit == ZombatarItem::ZOMBATAR_TIDBIT_1 ? Color::White : aDef.mColorGroup[mCurTidbitPallete]);
			float posX = 630.5f;
			float posY = 155.0f;
			float offsetX = 0;
			float offsetY = 0;
			float outlineX = 0;
			float outlineY = 0;
			GetZombatarPortraitOffset((ZombatarItem)(mCurTidbit), &offsetX, &offsetY);
			GetOutlineOffset((ZombatarItem)(mCurTidbit), &outlineX, &outlineY);
			if (aDef.mImage)
				DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX - 1.5f, posY + offsetY - outlineY - 0.5f);
			g->SetColor(Color::White);
			if (aDef.mOutlineImage)
				DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
			g->PopState();
		}

		if (mCurTidbit == ZombatarItem::ZOMBATAR_TIDBIT_1) {
			{
				ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(ZombatarItem::ZOMBATAR_TIDBIT_3));
				g->PushState();
				g->SetColorizeImages(true);
				g->SetColor(mCurTidbitPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurTidbitPallete]);
				float posX = 630.5f;
				float posY = 155.0f;
				float offsetX = 0;
				float offsetY = 0;
				float outlineX = 0;
				float outlineY = 0;
				GetZombatarPortraitOffset((ZombatarItem)(ZombatarItem::ZOMBATAR_TIDBIT_3), &offsetX, &offsetY);
				GetOutlineOffset((ZombatarItem)(ZombatarItem::ZOMBATAR_TIDBIT_3), &outlineX, &outlineY);
				if (aDef.mImage)
					DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX - 1.5f, posY + offsetY - outlineY - 0.5f);
				g->SetColor(Color::White);
				if (aDef.mOutlineImage)
					DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
				g->PopState();
			}
		}
	}

	if (mCurHair != -1)
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurHair));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(mCurHairPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurHairPallete]);
		float posX = 630.5f;
		float posY = 155.0f;
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)(mCurHair), &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)(mCurHair), &outlineX, &outlineY);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX - 1.5f, posY + offsetY - outlineY - 0.5f);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
		g->PopState();
	}

	if (mCurEyeWear != -1)
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurEyeWear));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(mCurEyeWearPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurEyeWearPallete]);
		float posX = 630.5f;
		float posY = 155.0f;
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)(mCurEyeWear), &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)(mCurEyeWear), &outlineX, &outlineY);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX, posY + offsetY - outlineY);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
		g->PopState();
	}

	if (mCurClothe != -1)
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurClothe));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		float posX = 630.5f;
		float posY = 155.0f;
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)(mCurClothe), &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)(mCurClothe), &outlineX, &outlineY);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX, posY + offsetY - outlineY);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
		g->PopState();
	}

	if (mCurAccessory != -1)
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurAccessory));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(mCurAccessoryPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurAccessoryPallete]);
		float posX = 630.5f;
		float posY = 155.0f;
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)(mCurAccessory), &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)(mCurAccessory), &outlineX, &outlineY);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX, posY + offsetY - outlineY);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
		g->PopState();
	}

	if (mCurHat != -1)
	{
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)(mCurHat));
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(mCurHatPallete == -1 || aDef.mColorGroup == nullptr ? Color::White : aDef.mColorGroup[mCurHatPallete]);
		float posX = 630.5f;
		float posY = 155.0f;
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)(mCurHat), &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)(mCurHat), &outlineX, &outlineY);
		if (aDef.mImage)
			DrawZombatarPortraitLayer(g, *aDef.mImage, bounds, posX + offsetX - outlineX, posY + offsetY - outlineY);
		g->SetColor(Color::White);
		if (aDef.mOutlineImage)
			DrawZombatarPortraitLayer(g, *aDef.mOutlineImage, bounds, posX + offsetX, posY + offsetY);
		g->PopState();
	}
	g->PopState();
}

ZombatarDefinition& GetZombatarDefinition(ZombatarItem theItem) {
	return gZombatarDefinitions[theItem];
}

void ZombatarWidget::DrawZombieAvatar(Graphics* g) {
	g->PushState();
	g->SetClipRect(600, 300, 176, 200);
	g->DrawImage(IMAGE_ALMANAC_GROUNDDAY, 600, 300);
	if (mZombie)
	{
		if (mZombie->BeginDraw(g))
		{
			mZombie->DrawShadow(g);
			mZombie->Draw(g);
			mZombie->EndDraw(g);
		}
	}
	g->PopState();
	g->DrawImageF(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 5, 0);
	g->PopState();
}

void ZombatarWidget::DrawZombatarItems(Graphics* g)
{
	g->DrawImageF(IMAGE_ZOMBATAR_WIDGET_INNER_BG, 152, 125);

	if (mCurCategory == ZombatarCategory::ZombatarCategory_Skin)
	{
		TodDrawStringWrapped(g, TodStringTranslate(_S("[ZOMBATAR_START_TEXT]")), Rect(142.5f, 180, IMAGE_ZOMBATAR_WIDGET_INNER_BG->GetWidth() + 18, 175), Sexy::FONT_DWARVENTODCRAFT15, Color(208, 190, 44, 255), DrawStringJustification::DS_ALIGN_CENTER);
	}
	else
	{
		for (int i = 0; i < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS; i++) {
			ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)i);
			NewLawnButton* aZombatarItem = mZombatarItems[i];
			int* aTargetItem = nullptr;
			if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Backdrops) aTargetItem = &mCurBackground;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetItem = &mCurHair;
			else if (aDef.mCategory == ZombatarCategory_FacialHair) aTargetItem = &mCurFacialHair;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetItem = &mCurTidbit;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetItem = &mCurEyeWear;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Clothes) aTargetItem = &mCurClothe;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetItem = &mCurAccessory;
			else if (aDef.mCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetItem = &mCurHat;

			if (aTargetItem && aDef.mCategory == mCurCategory && !aZombatarItem->mDisabled) {
				DrawZombatarItem(g, aZombatarItem, (ZombatarItem)i, aTargetItem, &aDef);
			}
		}

		int* aTargetItem = nullptr;
		if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) aTargetItem = &mCurHair;
		else if (mCurCategory == ZombatarCategory_FacialHair) aTargetItem = &mCurFacialHair;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) aTargetItem = &mCurTidbit;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) aTargetItem = &mCurEyeWear;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Clothes) aTargetItem = &mCurClothe;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) aTargetItem = &mCurAccessory;
		else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) aTargetItem = &mCurHat;
		if (aTargetItem)
		{
			DrawClearItem(g, mClearPickButton, aTargetItem);
		}
	}
}

void ZombatarWidget::GetOutlineOffset(ZombatarItem theItem, float* offsetX, float* offsetY) {
	switch (theItem) {
	case ZombatarItem::ZOMBATAR_HAIR_1:
		*offsetX = -9.0f;
		*offsetY = -2.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_2:
		*offsetX = -2.0f; 
		*offsetY = -4.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_11:
		*offsetX = -3.0f;
		*offsetY = -3.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_12:
		*offsetX = -2.0f; 
		*offsetY = -4.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_13:
		*offsetX = -3.0f; 
		*offsetY = -2.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_14:
		*offsetX = -3.0f; 
		*offsetY = -6.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_1:
	case ZombatarItem::ZOMBATAR_EYEWEAR_3:
		*offsetX = 1.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_6:
		*offsetY = 1.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_2:
	case ZombatarItem::ZOMBATAR_EYEWEAR_4:
	case ZombatarItem::ZOMBATAR_EYEWEAR_5:
	case ZombatarItem::ZOMBATAR_EYEWEAR_7:
	case ZombatarItem::ZOMBATAR_EYEWEAR_8:
	case ZombatarItem::ZOMBATAR_EYEWEAR_9:
	case ZombatarItem::ZOMBATAR_EYEWEAR_10:
	case ZombatarItem::ZOMBATAR_EYEWEAR_11:
	case ZombatarItem::ZOMBATAR_EYEWEAR_12:
		*offsetX = 1.0f;
		*offsetY = 1.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_1:
		*offsetX = -2.0f; 
		*offsetY = -1.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_3:
		*offsetX = -15.0f;
		*offsetY = 1.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_6:
		*offsetX = -4.0f;
		*offsetY = 2.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_7:
		*offsetX = 1.0f;
		*offsetY = -15.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_8:
	case ZombatarItem::ZOMBATAR_HAT_9:
		*offsetX = 2.0f;
		*offsetY = 2.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_11:
		*offsetX = -1.0f;
		*offsetY = -14.0f;
		break;
	}
}

void ZombatarWidget::GetZombatarItemScale(ZombatarItem theItem, float* scaleX, float* scaleY) {
	switch (theItem) {
		case ZombatarItem::ZOMBATAR_HAIR_14:
			*scaleX = 0.2875f;
			*scaleY = 0.2875f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_10:
			*scaleX = 0.3f;
			*scaleY = 0.3f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_2:
		case ZombatarItem::ZOMBATAR_HAIR_4:
			*scaleX = 0.325f;
			*scaleY = 0.325f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_1:
		case ZombatarItem::ZOMBATAR_HAIR_3:
		case ZombatarItem::ZOMBATAR_HAIR_8:
		case ZombatarItem::ZOMBATAR_HAIR_13:
		case ZombatarItem::ZOMBATAR_HAIR_15:
		case ZombatarItem::ZOMBATAR_HAIR_16:
		case ZombatarItem::ZOMBATAR_HAIR_12:
			*scaleX = 0.35f;
			*scaleY = 0.35f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_11:
			*scaleX = 0.375f;
			*scaleY = 0.375f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_5:
			*scaleX = 0.4f;
			*scaleY = 0.4f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_6:
			*scaleX = 0.425f;
			*scaleY = 0.425f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_7:
			*scaleX = 0.5f;
			*scaleY = 0.5f;
			break;
		case ZombatarItem::ZOMBATAR_HAIR_9:
			*scaleX = 1.4f;
			*scaleY = 0.9f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_1:
			*scaleX = 0.475f;
			*scaleY = 0.475f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_2:
			*scaleX = 0.48f;
			*scaleY = 0.44f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_5:
		case ZombatarItem::ZOMBATAR_TIDBIT_6:
			*scaleX = 0.48f;
			*scaleY = 0.48f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_7:
			*scaleX = 0.49f;
			*scaleY = 0.49f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_11:
			*scaleX = 0.5f;
			*scaleY = 0.5f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_8:
		case ZombatarItem::ZOMBATAR_TIDBIT_10:
			*scaleX = 0.515f;
			*scaleY = 0.515f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_4:
			*scaleX = 0.52f;
			*scaleY = 0.52f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_3:
			*scaleX = 0.75f;
			*scaleY = 0.75f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_13:
			*scaleX = 0.875f;
			*scaleY = 0.625f;
			break;
		case ZombatarItem::ZOMBATAR_TIDBIT_9:
		case ZombatarItem::ZOMBATAR_TIDBIT_12:
		case ZombatarItem::ZOMBATAR_TIDBIT_14:
			*scaleX = 1.0f;
			*scaleY = 1.0f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_1:
			*scaleX = 0.39f;
			*scaleY = 0.39f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_3:
		case ZombatarItem::ZOMBATAR_EYEWEAR_4:
		case ZombatarItem::ZOMBATAR_EYEWEAR_6:
			*scaleX = 0.4f;
			*scaleY = 0.39f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_5:
			*scaleX = 0.4115f;
			*scaleY = 0.39f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_2:
			*scaleX = 0.4125f;
			*scaleY = 0.39f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_10:
			*scaleX = 0.4125f;
			*scaleY = 0.4125f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_8:
		case ZombatarItem::ZOMBATAR_EYEWEAR_11:
			*scaleX = 0.425f;
			*scaleY = 0.425f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_14:
		case ZombatarItem::ZOMBATAR_EYEWEAR_16:
			*scaleX = 0.45f;
			*scaleY = 0.45f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_12:
			*scaleX = 0.475f;
			*scaleY = 0.475f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_15:
			*scaleX = 0.5f;
			*scaleY = 0.5f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_9:
			*scaleX = 0.51f;
			*scaleY = 0.51f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_13:
			*scaleX = 0.85f;
			*scaleY = 0.5f;
			break;
		case ZombatarItem::ZOMBATAR_EYEWEAR_7:
			*scaleX = 0.9575f;
			*scaleY = 0.9575f;
			break;
		case ZombatarItem::ZOMBATAR_CLOTHE_1:
		case ZombatarItem::ZOMBATAR_CLOTHE_2:
		case ZombatarItem::ZOMBATAR_CLOTHE_3:
		case ZombatarItem::ZOMBATAR_CLOTHE_4:
		case ZombatarItem::ZOMBATAR_CLOTHE_5:
		case ZombatarItem::ZOMBATAR_CLOTHE_6:
		case ZombatarItem::ZOMBATAR_CLOTHE_7:
		case ZombatarItem::ZOMBATAR_CLOTHE_8:
		case ZombatarItem::ZOMBATAR_CLOTHE_9:
		case ZombatarItem::ZOMBATAR_CLOTHE_10:
		case ZombatarItem::ZOMBATAR_CLOTHE_11:
		case ZombatarItem::ZOMBATAR_CLOTHE_12:
			*scaleX = 0.475f;
			*scaleY = 0.475f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_8:
			*scaleX = 0.275f;
			*scaleY = 0.275f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_6:
			*scaleX = 0.3f;
			*scaleY = 0.3f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_13:
			*scaleX = 0.3025f;
			*scaleY = 0.3025f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_14:
			*scaleX = 0.345f;
			*scaleY = 0.345f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_10:
			*scaleX = 0.35f;
			*scaleY = 0.35f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_1:
		case ZombatarItem::ZOMBATAR_HAT_2:
		case ZombatarItem::ZOMBATAR_HAT_4:
			*scaleX = 0.375f;
			*scaleY = 0.375f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_12:
			*scaleX = 0.39f;
			*scaleY = 0.39f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_9:
			*scaleX = 0.43f;
			*scaleY = 0.43f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_5:
			*scaleX = 0.44f;
			*scaleY = 0.44f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_11:
			*scaleX = 0.455f;
			*scaleY = 0.35f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_3:
			*scaleX = 0.465f;
			*scaleY = 0.465f;
			break;
		case ZombatarItem::ZOMBATAR_HAT_7:
			*scaleX = 0.565f;
			*scaleY = 0.565f;
			break;
		case ZombatarItem::ZOMBATAR_ACCESSORY_15:
			*scaleX = 0.31125f;
			*scaleY = 0.31125f;
			break;
		case ZombatarItem::ZOMBATAR_ACCESSORY_10:
			*scaleX = 0.425f;
			*scaleY = 0.425f;
			break;
		case ZombatarItem::ZOMBATAR_ACCESSORY_3:
			*scaleX = 0.925f;
			*scaleY = 0.7125f;
			break;
		case ZombatarItem::ZOMBATAR_ACCESSORY_7:
			*scaleX = 1.0f;
			*scaleY = 0.95f;
			break;
		case ZombatarItem::ZOMBATAR_ACCESSORY_1:
		case ZombatarItem::ZOMBATAR_ACCESSORY_2:
		case ZombatarItem::ZOMBATAR_ACCESSORY_4:
		case ZombatarItem::ZOMBATAR_ACCESSORY_5:
		case ZombatarItem::ZOMBATAR_ACCESSORY_6:
		case ZombatarItem::ZOMBATAR_ACCESSORY_8:
		case ZombatarItem::ZOMBATAR_ACCESSORY_9:
		case ZombatarItem::ZOMBATAR_ACCESSORY_11:
		case ZombatarItem::ZOMBATAR_ACCESSORY_12:
		case ZombatarItem::ZOMBATAR_ACCESSORY_13:
		case ZombatarItem::ZOMBATAR_ACCESSORY_14:
			*scaleX = 1.0f;
			*scaleY = 1.0f;
			break;
	}
}

void ZombatarWidget::GetZombatarItemOffset(ZombatarItem theItem, float* offsetX, float* offsetY) {
	switch (theItem) {
	case ZombatarItem::ZOMBATAR_HAIR_1:
		*offsetX = 0.5f;
		*offsetY = 5.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_2:
	case ZombatarItem::ZOMBATAR_HAIR_7:
		*offsetX = 0.5f;
		*offsetY = 7.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_3:
		*offsetY = 15.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_4:
		*offsetY = 2.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_5:
		*offsetX = -3.5f;
		*offsetY = 8.75f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_6:
		*offsetX = 0.5f;
		*offsetY = 2.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_8:
		*offsetX = 0.5f;
		*offsetY = 7.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_9:
		*offsetX = 2.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_10:
		*offsetX = 1.5f;
		*offsetY = 11.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_11:
		*offsetX = 0.5f;
		*offsetY = 1.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_12:
		*offsetX = 1.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_13:
		*offsetX = 0.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_14:
		*offsetX = 0.5f;
		*offsetY = 8.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_16:
		*offsetX = 0.5f;
		*offsetY = 0.5f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_1:
		*offsetY = 3.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_9:
		*offsetY = 5.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_3:
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_2:
	case ZombatarItem::ZOMBATAR_TIDBIT_12:
		*offsetY = 12.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_10:
		*offsetY = 14.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_4:
		*offsetX = 2.0f;
		*offsetY = 16.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_8:
	case ZombatarItem::ZOMBATAR_TIDBIT_11:
		*offsetY = 16.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_6:
		*offsetY = 17.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_5:
		*offsetY = 18.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_7:
		*offsetY = 19.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_14:
		*offsetX = 20.0f;
		*offsetY = 20.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_9:
		*offsetY = 11.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_7:
	case ZombatarItem::ZOMBATAR_EYEWEAR_14:
	case ZombatarItem::ZOMBATAR_EYEWEAR_15:
	case ZombatarItem::ZOMBATAR_EYEWEAR_16:
		*offsetY = 12.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_8:
	case ZombatarItem::ZOMBATAR_EYEWEAR_11:
		*offsetY = 14.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_1:
	case ZombatarItem::ZOMBATAR_EYEWEAR_3:
	case ZombatarItem::ZOMBATAR_EYEWEAR_4:
	case ZombatarItem::ZOMBATAR_EYEWEAR_5:
	case ZombatarItem::ZOMBATAR_EYEWEAR_10:
		*offsetY = 16.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_6:
		*offsetY = 18.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_2:
	case ZombatarItem::ZOMBATAR_EYEWEAR_12:
		*offsetY = 19.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_1:
		*offsetX = 2.0f;
		*offsetY = 2.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_2:
		*offsetX = 4.0f;
		*offsetY = 2.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_4:
		*offsetX = 15.0f;
		*offsetY = 2.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_5:
	case ZombatarItem::ZOMBATAR_ACCESSORY_6:
		*offsetX = 12.0f;
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_8:
		*offsetX = 18.0f;
		*offsetY = 18.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_9:
		*offsetX = 3.0f;
		*offsetY = 14.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_11:
	case ZombatarItem::ZOMBATAR_ACCESSORY_12:
	case ZombatarItem::ZOMBATAR_ACCESSORY_13:
		*offsetX = 18.0f;
		*offsetY = 18.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_10:
		*offsetY = 16.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_14:
		*offsetX = 13.0f;
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_15:
		*offsetY = 15.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_1:
		*offsetX = 1.5f;
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_2:
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_3:
		*offsetX = 6.5f;
		*offsetY = 12.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_4:
		*offsetX = 0.5f;
		*offsetY = 11.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_5:
		*offsetX = 0.5f;
		*offsetY = 10.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_6:
		*offsetX = 2.0f;
		*offsetY = 11.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_7:
		*offsetX = 0.5f;
		*offsetY = 8.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_8:
		*offsetX = 0.5f;
		*offsetY = 0.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_10:
		*offsetX = 0.5f;
		*offsetY = 17.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_11:
		*offsetX = 0.5f;
		*offsetY = 2.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_12:
		*offsetX = 0.5f;
		*offsetY = 3.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_13:
		*offsetY = 11.5f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_14:
		*offsetY = 10.0f;
		break;
	}
}

void ZombatarWidget::GetZombatarPortraitOffset(ZombatarItem theItem, float* offsetX, float* offsetY) {
	switch (theItem) {
	case ZombatarItem::ZOMBATAR_HAIR_1:
		*offsetX = -15.0f;
		*offsetY = -40.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_2:
		*offsetX = -15.0f;
		*offsetY = -15.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_3:
		*offsetX = -15.0f;
		*offsetY = -10.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_4:
		*offsetX = -8.0f;
		*offsetY = -25.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_8:
		*offsetX = -10.0f;
		*offsetY = -25.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_11:
		*offsetX = -13.0f;
		*offsetY = -21.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_13:
		*offsetX = -5.0f;
		*offsetY = -27.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_14:
		*offsetX = -29.0f;
		*offsetY = -42.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_15:
		*offsetX = 7.0f;
		*offsetY = -36.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_16:
		*offsetX = -12.0f;
		*offsetY = -20.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_10:
		*offsetX = -15.0f;
		*offsetY = -8.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_12:
		*offsetX = 13.0f;
		*offsetY = -45.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_5:
		*offsetX = -2.0f;
		*offsetY = -3.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_6:
		*offsetX = 1.0f;
		*offsetY = -27.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_7:
		*offsetX = 13.0f;
		*offsetY = -18.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAIR_9:
		*offsetX = 90.0f;
		*offsetY = 15.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_1:
		*offsetX = -9.0f;
		*offsetY = 24.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_2:
		*offsetX = -9.0f;
		*offsetY = 24.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_3:
		*offsetX = 9.0f;
		*offsetY = 72.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_4:
		*offsetX = -6.0f;
		*offsetY = 23.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_5:
		*offsetX = -6.0f;
		*offsetY = 19.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_6:
		*offsetX = -9.0f;
		*offsetY = 27.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_7:
		*offsetX = -9.0f;
		*offsetY = 33.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_8:
		*offsetX = -4.0f;
		*offsetY = 16.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_9:
		*offsetX = -16.0f;
		*offsetY = 37.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_10:
		*offsetX = -1.0f;
		*offsetY = 32.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_11:
		*offsetX = -1.0f;
		*offsetY = 31.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_12:
		*offsetX = 49.0f;
		*offsetY = 52.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_13:
		*offsetX = 51.0f;
		*offsetY = 11.0f;
		break;
	case ZombatarItem::ZOMBATAR_TIDBIT_14:
		*offsetX = 76.0f;
		*offsetY = 76.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_1:
		*offsetX = -9.0f;
		*offsetY = 33.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_2:
		*offsetX = -6.0f;
		*offsetY = 46.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_3:
		*offsetX = -9.0f;
		*offsetY = 29.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_4:
		*offsetX = -9.0f;
		*offsetY = 39.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_5:
		*offsetX = -7.0f;
		*offsetY = 36.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_6:
		*offsetX = -8.0f;
		*offsetY = 39.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_7:
		*offsetX = 13.0f;
		*offsetY = 51.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_8:
		*offsetX = -5.0f;
		*offsetY = 31.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_9:
		*offsetX = -1.0f;
		*offsetY = 61.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_10:
		*offsetX = -6.0f;
		*offsetY = 36.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_11:
		*offsetX = -6.0f;
		*offsetY = 28.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_12:
		*offsetY = 56.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_13:
		*offsetX = -8.0f;
		*offsetY = 41.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_14:
		*offsetX = -3.0f;
		*offsetY = 24.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_15:
		*offsetX = 4.0f;
		*offsetY = 25.0f;
		break;
	case ZombatarItem::ZOMBATAR_EYEWEAR_16:
		*offsetX = -3.0f;
		*offsetY = 25.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_1:
		*offsetX = 49.0f;
		*offsetY = 69.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_2:
		*offsetX = 37.0f;
		*offsetY = 59.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_3:
		*offsetX = 48.0f;
		*offsetY = 70.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_4:
		*offsetX = 38.0f;
		*offsetY = 70.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_5:
		*offsetX = 52.0f;
		*offsetY = 74.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_6:
		*offsetX = 54.0f;
		*offsetY = 69.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_7:
		*offsetX = 39.0f;
		*offsetY = 64.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_8:
		*offsetX = 50.0f;
		*offsetY = 69.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_9:
		*offsetX = 51.0f;
		*offsetY = 61.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_10:
		*offsetX = 46.0f;
		*offsetY = 68.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_11:
		*offsetX = 46.0f;
		*offsetY = 70.0f;
		break;
	case ZombatarItem::ZOMBATAR_CLOTHE_12:
		*offsetX = 41.0f;
		*offsetY = 73.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_1:
		*offsetX = 65.0f;
		*offsetY = 70.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_2:
		*offsetX = 70.0f;
		*offsetY = 70.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_3:
		*offsetX = 48.0f;
		*offsetY = 73.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_4:
		*offsetX = 93.0f;
		*offsetY = 55.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_5:
	case ZombatarItem::ZOMBATAR_ACCESSORY_6:
		*offsetX = 93.0f;
		*offsetY = 60.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_7:
		*offsetX = 66.0f;
		*offsetY = 71.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_8:
		*offsetX = 80.0f;
		*offsetY = 25.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_9:
		*offsetX = 23.0f;
		*offsetY = 78.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_10:
		*offsetX = 5.0f;
		*offsetY = 60.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_11:
		*offsetX = 97.0f;
		*offsetY = 52.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_12:
		*offsetX = 40.0f;
		*offsetY = 90.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_13:
		*offsetX = 30.0f;
		*offsetY = 105.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_14:
		*offsetX = 95.0f;
		*offsetY = 30.0f;
		break;
	case ZombatarItem::ZOMBATAR_ACCESSORY_15:
		*offsetX = -25.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_1:
		*offsetX = -10.0f;
		*offsetY = -35.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_2:
		*offsetX = 9.0f;
		*offsetY = -28.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_3:
		*offsetX = -2.0f;
		*offsetY = -20.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_4:
		*offsetX = -27.0f;
		*offsetY = -30.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_5:
		*offsetX = 3.0f;
		*offsetY = -24.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_6:
		*offsetX = -20.0f;
		*offsetY = -37.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_7:
		*offsetX = 15.0f;
		*offsetY = -23.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_8:
		*offsetX = -35.0f;
		*offsetY = -40.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_9:
		*offsetY = -40.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_10:
		*offsetX = -25.0f;
		*offsetY = 5.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_11:
		*offsetX = 25.0f;
		*offsetY = -32.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_12:
		*offsetX = 5.0f;
		*offsetY = -25.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_13:
		*offsetX = -20.0f;
		*offsetY = -40.0f;
		break;
	case ZombatarItem::ZOMBATAR_HAT_14:
		*offsetX = -15.0f;
		*offsetY = -35.0f;
		break;
	}
}

void ZombatarWidget::DrawZombatarItem(Graphics* g, NewLawnButton* button, ZombatarItem theItem, int* theTargetItem, ZombatarDefinition* aDef)
{
	g->PushState();
	{
		g->SetColorizeImages(true);
		const bool selected = (int)theItem == *theTargetItem;
		const Color shade = selected ? Color::White : Color(128, 128, 128, button->mIsOver ? 192 : 128);
		Image* tile = selected ? IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT : IMAGE_ZOMBATAR_ACCESSORY_BG;
		if (aDef->mCategory == ZombatarCategory_FacialHair)
		{
			g->SetColor(shade);
			g->DrawImage(tile, Rect(button->mX, button->mY, button->mWidth, button->mHeight),
				Rect(0, 0, tile->GetWidth(), tile->GetHeight()));
			DrawFacialHairArt(g, *aDef, button->mX + button->mWidth / 2.0f, button->mY + button->mHeight / 2.0f,
				button->mWidth - 8.0f, button->mHeight - 8.0f, shade);
			g->PopState();
			return;
		}

		g->SetColor(button->mIsOver ? Color::White : shade);
		g->DrawImageF(tile, button->mX, button->mY);
		g->SetClipRect(button->mX + 9, button->mY + 9, button->mWidth - 18, button->mHeight - 18);

		float scaleX = 0.27f;
		float scaleY = 0.27f;
		GetZombatarItemScale(aDef->mZombatarItem, &scaleX, &scaleY);

		if (aDef->mCategory == ZombatarCategory::ZombatarCategory_Clothes) {
			SDL_DisplayID display = SDL_GetPrimaryDisplay();
			float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;

			TodDrawImageScaledF(g, gZombatarClothes[theItem - ZombatarItem::ZOMBATAR_CLOTHE_1], button->mX - 10.0f, button->mY - 10.0f, scaleX / scale, scaleY / scale);
			g->PopState();
			return;
		}

		float positionX = button->mX + 9.0f;
		float positionY = button->mY + 9.0f;

		float adjustmentX = 0.0f;
		float adjustmentY = 0.0f;
		GetZombatarItemOffset(aDef->mZombatarItem, &adjustmentX, &adjustmentY);
		positionX += adjustmentX;
		positionY += adjustmentY;

		if (aDef->mImage) {
			TodDrawImageScaledF(g, *aDef->mImage, positionX, positionY, scaleX, scaleY);
		}

		float offsetX = 0.0f;
		float offsetY = 0.0f;
		GetOutlineOffset(aDef->mZombatarItem, &offsetX, &offsetY);
		offsetX *= scaleX;
		offsetY *= scaleY;
		positionX += offsetX;
		positionY += offsetY;

		if (aDef->mOutlineImage) {
			TodDrawImageScaledF(g, *aDef->mOutlineImage, positionX, positionY, scaleX, scaleY);
		}
	}

	if (theItem == ZombatarItem::ZOMBATAR_TIDBIT_1) {
		ZombatarDefinition jDef = GetZombatarDefinition(ZombatarItem::ZOMBATAR_TIDBIT_3);

		float scaleX = 0.27f;
		float scaleY = 0.27f;
		GetZombatarItemScale(jDef.mZombatarItem, &scaleX, &scaleY);
		scaleX *= 0.6385f;
		scaleY *= 0.6185f;

		float positionX = button->mX + 9.0f;
		float positionY = button->mY + 9.0f;

		float adjustmentX = 0.0f;
		float adjustmentY = 0.0f;
		GetZombatarItemOffset(jDef.mZombatarItem, &adjustmentX, &adjustmentY);
		positionX += adjustmentX + 8;
		positionY += adjustmentY + 15;

		if (jDef.mImage) {
			TodDrawImageScaledF(g, *jDef.mImage, positionX, positionY, scaleX, scaleY);
		}

		float offsetX = 0.0f;
		float offsetY = 0.0f;
		GetOutlineOffset(jDef.mZombatarItem, &offsetX, &offsetY);
		offsetX *= scaleX;
		offsetY *= scaleY;
		positionX += offsetX;
		positionY += offsetY;

		if (jDef.mOutlineImage) {
			TodDrawImageScaledF(g, *jDef.mOutlineImage, positionX, positionY, scaleX, scaleY);
		}
	}
	g->PopState();
}

void ZombatarWidget::DrawClearItem(Graphics* g, NewLawnButton* button, int* theTargetItem)
{
	g->PushState();
	g->SetColorizeImages(true);
	g->SetColor(button->mIsOver ? Color::White : Color(128, 128, 128, 128));
	if (mCurCategory == ZombatarCategory_FacialHair)
	{
		const Rect bounds(button->mX, button->mY, button->mWidth, button->mHeight);
		Image* tile = IMAGE_ZOMBATAR_ACCESSORY_BG;
		g->DrawImage(tile, bounds, Rect(0, 0, tile->GetWidth(), tile->GetHeight()));
		Image* clear = IMAGE_ZOMBATAR_ACCESSORY_BG_NONE;
		g->SetColor(Color::White);
		g->ClipRect(bounds);
		g->DrawImage(clear, bounds.mX + (bounds.mWidth - clear->GetWidth()) / 2,
			bounds.mY + (bounds.mHeight - clear->GetHeight()) / 2);
	}
	else
	{
		g->DrawImageF(IMAGE_ZOMBATAR_ACCESSORY_BG, button->mX, button->mY);
		g->SetClipRect(button->mX + 9, button->mY + 9, button->mWidth - 18, button->mHeight - 18);
		g->DrawImageF(IMAGE_ZOMBATAR_ACCESSORY_BG_NONE, button->mX, button->mY);
	}

	g->PopState();
}

void ZombatarWidget::DrawClearPallete(Graphics* g, NewLawnButton* button, int* theTargetItem)
{
	g->PushState();
	g->SetColorizeImages(true);
	g->SetColor(Color::White);
	bool aIsSelected = theTargetItem && *theTargetItem == -1;
	if (aIsSelected) {
		g->DrawImageF(IMAGE_ZOMBATAR_COLORPICKER_HIGHLIGHT, button->mX - 1.5f, button->mY - 3.5f);
	}
	g->mColor.mAlpha = aIsSelected ? 255 : button->mIsOver ? 128 : 64;
	g->DrawImageF(IMAGE_ZOMBATAR_COLORPICKER_NONE, button->mX, button->mY);
	g->PopState();
}

void ZombatarWidget::UpdatePalletes() {
	int* aTargetItem = nullptr;
	int* aTargetPallete = nullptr;
	Color* curPalletes = nullptr;
	int palleteCount = 0;

	if (mCurCategory == ZombatarCategory::ZombatarCategory_Skin) {
		aTargetPallete = &mCurSkinPallete;
		curPalletes = gZombatarSkinPalletes;
		palleteCount = NUM_SKIN_COLOR_PALLETES;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Backdrops) {
		aTargetItem = &mCurBackground;
		aTargetPallete = &mCurBackgroundPallete;
		curPalletes = gZombatarBrightPalletes;
	} 
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hairs) {
		aTargetPallete = &mCurHairPallete;
		aTargetItem = &mCurHair;
	}
	else if (mCurCategory == ZombatarCategory_FacialHair) {
		aTargetPallete = &mCurFacialHairPallete;
		aTargetItem = &mCurFacialHair;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Tidbits) {
		aTargetPallete = &mCurTidbitPallete;
		aTargetItem = &mCurTidbit;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_EyeWears) {
		aTargetPallete = &mCurEyeWearPallete;
		aTargetItem = &mCurEyeWear;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Accessories) {
		aTargetPallete = &mCurAccessoryPallete;
		aTargetItem = &mCurAccessory;
	}
	else if (mCurCategory == ZombatarCategory::ZombatarCategory_Hats) {
		aTargetPallete = &mCurHatPallete;
		aTargetItem = &mCurHat;
	}

	if (aTargetItem && *aTargetItem >= 0 && *aTargetItem < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS) {
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)*aTargetItem);
		curPalletes = aDef.mColorGroup;

		if (aDef.mColorGroup != nullptr)
		{
			palleteCount = aDef.mCategory != ZombatarCategory::ZombatarCategory_Skin ? NUM_COLOR_PALLETES : NUM_SKIN_COLOR_PALLETES;
		}
	}

	for (NewLawnButton* aPallete : mColorPalletes) {
		aPallete->SetDisabled(true);
	}
	for (size_t i = 0; i < palleteCount; i++) {
		mColorPalletes[i]->SetDisabled(false);
	}
}

void ZombatarWidget::UpdateItems() {
	for (int i = 0; i < (int)ZombatarItem::NUM_ZOMBATAR_ITEMS; i++) {
		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)i);
		NewLawnButton* aZombatarItem = mZombatarItems[i];
		const bool onPage = aDef.mCategory != ZombatarCategory_FacialHair ||
			(i - ZOMBATAR_FACIALHAIR_1) / ZOMBATAR_FACIAL_HAIR_PAGE_SIZE == mFacialHairPage;
		aZombatarItem->SetDisabled(aDef.mCategory != mCurCategory || !onPage);
	}
}

static void ApplyZombatarHead(LawnApp* app, Zombie* zombie, const ZombatarAppearance& a)
{
	auto* head = app->ReanimationTryToGet(zombie->mZombatarTidbitID);
	if (!head) return;
	head->AssignRenderGroupToPrefix("", RENDER_GROUP_HIDDEN);

	const auto show = [&](const std::string& name, Color color)
	{
		if (!head->TrackExists(name.c_str())) return;
		auto* track = head->GetTrackInstanceByName(name.c_str());
		track->mRenderGroup = RENDER_GROUP_NORMAL;
		track->mTrackColor = color;
	};
	const auto showItem = [&](int item, int palette, const char* prefix, int style)
	{
		if (item < 0) return;
		const auto& def = GetZombatarDefinition(static_cast<ZombatarItem>(item));
		show(StrFormat("%s_%02d", prefix, style),
			palette >= 0 && def.mColorGroup ? def.mColorGroup[palette] : Color::White);
		show(StrFormat("%s_%02d_line", prefix, style), Color::White);
	};
	const Color skin = a.mCurSkinPallete == 0 ? Color::White : gZombatarSkinPalletes[a.mCurSkinPallete];
	show("anim_head1", skin);
	show("anim_head2", skin);
	if (a.mCurHair < 0) show("anim_hair", Color::White);
	showItem(a.mCurHair, a.mCurHairPallete, "hair", a.mCurHair - ZOMBATAR_HAIR_1);
	showItem(a.mCurFacialHair, a.mCurFacialHairPallete, "facialHair", a.mCurFacialHair - ZOMBATAR_FACIALHAIR_1);
	showItem(a.mCurTidbit, a.mCurTidbitPallete, "tidBits", a.mCurTidbit - ZOMBATAR_TIDBIT_1);
	showItem(a.mCurEyeWear, a.mCurEyeWearPallete, "eyeWear", a.mCurEyeWear - ZOMBATAR_EYEWEAR_1);
	constexpr int accessoryTracks[] = {0, 1, 2, 3, 4, 14, 5, 6, 12, 7, 9, 10, 11, 13, 8};
	if (a.mCurAccessory >= 0)
		showItem(a.mCurAccessory, a.mCurAccessoryPallete, "accessories", accessoryTracks[a.mCurAccessory - ZOMBATAR_ACCESSORY_1]);
	showItem(a.mCurHat, a.mCurHatPallete, "hats", a.mCurHat - ZOMBATAR_HAT_1);

}

void ZombatarWidget::UpdateZombieAvatar()
{
	if (mZombie) ApplyZombatarHead(mApp, mZombie, *this);
}

void ApplyRandomZombatar(Zombie& zombie)
{
	auto saved = ReadZombatarCollection(zombie.mApp);
	if (saved.empty() || !TodLoadResources("DelayLoad_Zombatar")) return;
	SetupZombatarHead(zombie.mApp, &zombie);
	ApplyZombatarHead(zombie.mApp, &zombie, saved[Rand(static_cast<int>(saved.size()))]);
}

MemoryImage* gZombatarClothes[NUM_CLOTHES];

void ZombatarWidget::CreateZombatarClothes() {
	for (int i = ZombatarItem::ZOMBATAR_CLOTHE_1; i <= ZombatarItem::ZOMBATAR_CLOTHE_12; i++) {
		int clotheNum = i - ZombatarItem::ZOMBATAR_CLOTHE_1;

		SDL_DisplayID display = SDL_GetPrimaryDisplay();
		float scale = (float)SDL_GetCurrentDisplayMode(display)->h / 600.0f;

		SDL3Image* theImage = new SDL3Image(LawnApp::mSDLRenderer);
		theImage->mWidth = 147 * scale;
		theImage->mHeight = 146 * scale;
		theImage->mD3DData = SDL_CreateTexture(LawnApp::mSDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, theImage->mWidth, theImage->mHeight);
		SDL_SetTextureBlendMode((SDL_Texture*)theImage->mD3DData, SDL_BLENDMODE_BLEND);

		Graphics aMemoryGraphics(theImage);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)theImage->mD3DData);
		aMemoryGraphics.SetLinearBlend(true);
		aMemoryGraphics.PushState();
		aMemoryGraphics.SetColorizeImages(true);
		aMemoryGraphics.SetColor(Color(134, 147, 122));
		TodDrawImageScaledF(&aMemoryGraphics, IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, 0, 0, scale, scale);
		aMemoryGraphics.PopState();
		TodDrawImageScaledF(&aMemoryGraphics, IMAGE_ZOMBATAR_ZOMBIE_BLANK, 0, 0, scale, scale);

		ZombatarDefinition& aDef = GetZombatarDefinition((ZombatarItem)i);
		aMemoryGraphics.PushState();
		aMemoryGraphics.SetColorizeImages(true);
		aMemoryGraphics.SetColor(Color::White);
		float offsetX = 0;
		float offsetY = 0;
		float outlineX = 0;
		float outlineY = 0;
		GetZombatarPortraitOffset((ZombatarItem)i, &offsetX, &offsetY);
		GetOutlineOffset((ZombatarItem)i, &outlineX, &outlineY);
		offsetX *= scale;
		offsetY *= scale;
		outlineX *= scale;
		outlineY *= scale;
		if (aDef.mImage)
			TodDrawImageScaledF(&aMemoryGraphics, *aDef.mImage, offsetX - outlineX, offsetY - outlineY, scale, scale);
		aMemoryGraphics.SetColor(Color::White);
		if (aDef.mOutlineImage)
			TodDrawImageScaledF(&aMemoryGraphics, *aDef.mOutlineImage, offsetX, offsetY, scale, scale);
		aMemoryGraphics.PopState();

		SDL_SetRenderTarget(LawnApp::mSDLRenderer, nullptr);

		MemoryImage* aImage = new MemoryImage();
		int aBitsCount = theImage->mWidth * theImage->mHeight;
		aImage->mWidth = theImage->mWidth;
		aImage->mHeight = theImage->mHeight;
		aImage->mBits = new unsigned long[aBitsCount + 1];
		aImage->mHasTrans = true;
		aImage->mHasAlpha = true;
		memset(aImage->mBits, 0, aBitsCount * sizeof(unsigned long));
		aImage->mBits[aBitsCount] = Sexy::MEMORYCHECK_ID;

		SDL_Texture* oldRenderTarget = SDL_GetRenderTarget(LawnApp::mSDLRenderer);
		SDL_SetRenderTarget(LawnApp::mSDLRenderer, (SDL_Texture*)theImage->mD3DData);

		SDL_Surface* surface = SDL_RenderReadPixels(LawnApp::mSDLRenderer, NULL);
		uint8_t* srcBytes = static_cast<uint8_t*>(surface->pixels);

		for (int y = 0; y < theImage->mHeight; ++y)
		{
			memcpy(aImage->mBits + y * theImage->mWidth, srcBytes + y * surface->pitch, theImage->mWidth * 4);
		}

		SDL_SetRenderTarget(LawnApp::mSDLRenderer, oldRenderTarget);
		SDL_DestroySurface(surface);

		gZombatarClothes[clotheNum] = aImage;
		delete theImage;
	}
}

void DisposeZombatarClothesCache() {
	for (MemoryImage* aImage : gZombatarClothes) {
		delete aImage;
	}
}
#endif