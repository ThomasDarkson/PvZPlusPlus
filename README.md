[![](https://thomasdarkson.com/assets/stickers/sticker.png)](https://thomasdarkson.com)

[<p align="center"><img src="https://thomasdarkson.com/pvzpp/shit_logo.png"></p>](https://thomasdarkson.com)

<p align="center"><img src="https://stats.thomasdarkson.com/?repo=ThomasDarkson/PvZPlusPlus"></p>

## Disclaimer
PvZ++'s own code is licensed under the Apache 2.0 license. The license does not cover any game assets.

**This is a fan project. It is not affiliated with or endorsed by PopCap Games or EA.**

PvZ++ doesn't redistribute assets from the GOTY Edition. However, it **does include some assets from the console editions**, mainly from the PS3 version, plus the cage from the Nintendo DS version for Zombie Trap, and the zombie eating the Xbox 360 gamepad image for the VS Mode from the Xbox 360 version. These are needed for features like widescreen, VS Mode and Co-op. These assets belong to EA and PopCap Games. If you are a rights holder and want them removed, please [contact me](mailto:thomasdarkson786@gmail.com) and I'll take down the project and cease development.

---

Plants vs Zombies++ (PvZ++) is a fork of [stabledecompile](https://github.com/InLiothixi/stabledecompile), aimed to make modding Plants vs. Zombies easier with Just-in-Time compiling. Basically, you can edit the source code of the game without having to install any C++ building tools, the code is compiled when you start the game. The game also is not dependent on `.pak` files, so you can edit the assets without extracting and repacking .pak files.  

> [!NOTE]
> For both using and developing mods with this project, **you must own GOTY Edition from Steam**. Any other edition will not work, and the copy must be genuine.
> The game asks for main.pak file from your own copy at start up for one time only to copy the GOTY assets.

## Requirements
- Windows 10 x64 or Windows 11
- Any GPU that supports DirectX 11
- **Legal copy of [Plants vs. Zombies GOTY Edition](https://store.steampowered.com/app/3590/Plants_vs_Zombies_GOTY_Edition/) on Steam**

## Installation
1. [Download](https://github.com/ThomasDarkson/PvZPlusPlus/releases/latest) the latest zip from Releases and unpack it anywhere.
2. Run `PvZ++.exe`.
3. When the game asks for `main.pak`, provide the file from your own copy of Plants vs. Zombies GOTY Edition. This only happens the first time.
4. Wait for the game to compile, then play.

## New Features
### Easier Modding
PvZ++ compiles the game's code when you run the executable, with what we call a JIT (Just-in-Time) compiler. If you are not a modder, this doesn't concern you at all and you can skip reading this part.

Since the code is compiled when the game boots, this means you can edit the source code to your heart's content and the game will run without recompiling the executable. It only rebuilds code. You don't even need to have any C++ compiling tools installed on your PC because the compiling tools are shipped with PvZ++. The game is written in C++ and PvZ++ compiles the C++ code in **C++23**, meaning you can use C++23's features if you are editing the source code.

The source code of the game is in `pvz-src/`.

The original game's assets are in main.pak, which you need to extract and repack to make any changes to it. PvZ++ extracts the assets from the main.pak you provide from your legal copy, and you can edit the assets directly. 

The extracted assets are in `game-data\imported-assets`.

### Full Widescreen Support
stabledecompile has widescreen, but it is unfinished. PvZ++ has full widescreen support, for gameplay and all menus.

<details>
  <summary><strong>Click to Show Images</strong></summary>
  <br>
  
  ![](https://thomasdarkson.com/pvzpp/widescreen/1.png)
  ![](https://thomasdarkson.com/pvzpp/widescreen/2.png)
</details>

### VS and Co-op
VS mode and Co-op are both fully implemented. One player must use a gamepad. The other uses the mouse. Xbox and PlayStation controllers are supported.

<details>
  <summary><strong>Click to Show Images (VS)</strong></summary>
  <br>
  
  ![](https://thomasdarkson.com/pvzpp/vs/2.png)
  ![](https://thomasdarkson.com/pvzpp/vs/3.png)
  ![](https://thomasdarkson.com/pvzpp/vs/1.png)
</details>

<br>Co-op is extended to normal levels, so you can play either normal Co-op levels or the full adventure mode with a friend!

<details>
  <summary><strong>Click to Show Images (Co-op)</strong></summary>
  <br>
  
  ![](https://thomasdarkson.com/pvzpp/coop/1.png)
  ![](https://thomasdarkson.com/pvzpp/coop/2.png)
  ![](https://thomasdarkson.com/pvzpp/coop/3.png)
</details>

### Extra Mini-games
2 mini-games are added.

| Platform | Game |
|---|---|
| Xbox 360, PlayStation 3 and Vita | Heavy Weapon |
| Nintendo DS | Zombie Trap |

Zombie Trap existed in stabledecompile, but it was not functional. It is fully playable now.

<details>
  <summary><strong>Click to Show Images</strong></summary>
  <br>
  
  ![](https://thomasdarkson.com/pvzpp/minigames/heavy_weapon.png)
  ![](https://thomasdarkson.com/pvzpp/minigames/trap.png)
</details>

### Full GOTY Support
Full support for Zombatar and GOTY achievements.

![](https://thomasdarkson.com/pvzpp/zombatar.png)

### More Achievements
50 achievements are added from console and mobile editions, 71 achievements in total.

| Platform | Count | Achievements |
|---|---:|---|
| **iOS and Android** | 27 | 20 Below Zero, Blind Faith, Chilli Free, Chill Out, Diamond Beghouler, Enlightened, Flower Power, Gotcha!, Greenfingers, I, Win, Last Mown Standing, Lawn Mower Man, Lucky Spin, Melon-y Lane, Monster Mash, Peking Express, Pool’s Closed, Pyromaniac, Second Life, Shooting Star, Smashing!, Sol Invictus, Sproing! Sproing!, Still Standing, The Stuff of Legends, Wall-Not Attack, Where the Sun Don’t Shine  |
| **Mobile and console** | 2 | Alive and Planting and Defcorn 5 |
| **Xbox 360 / PlayStation** | 7 | Close Shave, Crash of the Titan, Nom Nom Nom, Shopaholic, Soil Your Plants, ZFF's 4 Evr, Versus vs. Versus |
| **Nintendo DS / DSiWare** | 5 | Book Learner, Nebulaphile, Photosensitive, Think Hard, Tough Beans |
| **Android, version 0.1.75** | 1 | Future Star |
| **Endless Edition (China)** | 8 | Purple Card Master 1, Purple Card Master 2, Purple Card Master 3, Purple Card Master 4, Purple Card Master 5, Purple Card Master 6, Purple Card Master 7, Purple Card Master 8  |

### Level Selector
stabledecompile also has a level selector, but it is normally inaccessible and broken, with bugs overwriting your progress after you replay a level and conveyor belt levels not working, but all of those bugs have been fixed.

![](https://thomasdarkson.com/pvzpp/level_selector.png)

### More Graphic Options
Original game rendered the game at 800x600. It is safe to say that this is pretty low by today's standards. PvZ++ renders the game at 1280x720 by default, and can render up to 8K. Available resolution options are 1280x720, 1600x900, 1920x1080, 2560x1440, 3840x2160 and 7680x4320. Note that the game is still using assets from the original GOTY edition, so the original low resolution artwork can limit sharpness.

PvZ++ features an **FPS Limit** option that supports up to 360 FPS. The game's engine was updated to support high FPS numbers.

PvZ++ also features a **Sharpness** slider, which applies a sharpness filter over the whole game. 100% sharpness may make the edges harsh.

![](https://thomasdarkson.com/pvzpp/options.png)

### Extended Almanac
The Suburban Almanac now shows plants' and zombies' health, their special ability and their range.

![](https://thomasdarkson.com/pvzpp/almanac/1.png)
![](https://thomasdarkson.com/pvzpp/almanac/2.png)

## Compiling
*This step is for compiling the PvZ++ project itself, you don't need to do this if you aim to mod Plants vs. Zombies.*

PvZ++ is built with Haxe, which is why you need it. [hxcpp_JIT](https://lib.haxe.org/p/hxcpp_JIT/) is the library that compiles the game's C++ code at runtime.

Requirements:
- Latest version of [Haxe](https://haxe.org/) and the libraries below
    - [hxcpp](https://lib.haxe.org/p/hxcpp/)
    - [OpenFL](https://lib.haxe.org/p/openfl/)
    - [Lime](https://lib.haxe.org/p/lime/)
    - [hxcpp_JIT](https://lib.haxe.org/p/hxcpp_JIT/)
    - [HxSysInfo](https://lib.haxe.org/p/HxSysInfo/)
- [Visual Studio 2019](https://aka.ms/vs/16/release/vs_community.exe)
    - MSVC v142 - VS 2019 C++ x64/x86 build tools (Available in Individual components)
    - Windows 10 SDK (10.0.19041.0) (Available in Individual components)
- Windows 10 (x64 only) or Windows 11

### Setting up the Haxe libraries
*You can skip this step if you know how to set up Haxelib and install the libraries, or if they are already installed.*

After installing Haxe, run Command Prompt **as administrator** and enter this command to set up Haxelib. 

`haxelib setup`

It will ask for a path with write access. Leave it blank and press Enter.

After this, installing the libraries is easy. Just enter `haxelib install <library>`.

```
haxelib install hxcpp
haxelib install openfl
haxelib install lime
haxelib install hxcpp_JIT
haxelib install HxSysInfo
```

After all of these are installed, run:
```
haxelib run lime setup
```

This will install the `lime` command, which is needed to build the game.

Clone [this repository](https://github.com/ThomasDarkson/PvZPlusPlus) with Git or GitHub Desktop and enter:
```
cd path/to/pvz++
```

Now to build the game, run:
```
lime test windows
```

This will take a while and your CPU usage might spike, because it is compiling the game. After it is done, the game will launch and your executable will be in `export\windows\bin\PvZ++.exe`.

## Credits

Credits are also available in-game.

### [Thomas Darkson](https://thomasdarkson.com)
- Made Plants vs Zombies++

### [InLiothixi](https://github.com/InLiothixi)
- Made stabledecompile, PvZ++ wouldn't be possible without it

### [PopCap Games](https://www.ea.com/ea-studios/popcap)
- Made Plants vs. Zombies. The game's code and assets belong to EA and PopCap Games.

### [PvZWidescreen Mod](https://github.com/HenryJk/PvZWidescreen)
- Some of the widescreen assets were taken from this mod

### Haxe and OpenFL (and Lime)
- The engine that PvZ++ uses