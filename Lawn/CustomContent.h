#pragma once

#include <algorithm>
#include <vector>
#include "../ConstEnums.h"

namespace CustomContent
{
    inline const std::vector<SeedType> Plants = {};
    inline const std::vector<ZombieType> Zombies = {};

    inline int PlantSlot(SeedType type)
    {
        const auto found = std::find(Plants.begin(), Plants.end(), type);
        return found == Plants.end() ? -1 : static_cast<int>(found - Plants.begin());
    }

    inline int ZombieSlot(ZombieType type)
    {
        const auto found = std::find(Zombies.begin(), Zombies.end(), type);
        return found == Zombies.end() ? -1 : static_cast<int>(found - Zombies.begin());
    }
}
