#include "config.hpp"
#include "external.hpp"
#include "crt-compat.hpp"
#include <map>
#include "../../Lawn/BoardInclude.h"
#include "../../Lawn/System/Music.h"
#include "../../Lawn/System/PlayerInfo.h"
#include "../../Lawn/System/ProfileMgr.h"
#include "../../Lawn/Widget/AchievementsScreen.h"
#include "../../Lawn/Widget/GameSelector.h"
#include "../../Lawn/Widget/LawnDialog.h"
#include "../../Lawn/Widget/StoreScreen.h"
#include "../../SexyAppFramework/DDImage.h"
#include "../../SexyAppFramework/DDInterface.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/EffectSystem.h"
#include "../../include/ImageLib/ImageLib.h"
#include "../../PakLib/PakInterface.h"
#include "../../Lawn/ZenGarden.h"
#include "../../Lawn/System/ReanimationLawn.h"
#include "../../Lawn/Widget/AwardScreen.h"
#include "../../Lawn/Widget/ChallengeScreen.h"
#include "../../Lawn/Widget/CreditScreen.h"
#include "../../Lawn/Widget/TitleScreen.h"
#include "../../Sexy.TodLib/ReanimAtlas.h"
#include "../../Sexy.TodLib/TodFoley.h"

using namespace Sexy;

namespace OpenPVZImports
{
    std::string& errors();
    FARPROC resolve(const char* dll, const char* symbol);
}

#include "achievement-notifications.cpp"
#include "widescreen-dialogs.cpp"
#include "widescreen-menu.cpp"
#include "bridge.cpp"
