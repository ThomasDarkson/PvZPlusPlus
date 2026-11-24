#include "api.hpp"
#include "../../Lawn/Widget/SeedChooserScreen.h"
#include "../../SexyAppFramework/SoundManager.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_hints.h>
#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cmath>

                                                                                  
bool (*gAppCloseRequest)() = nullptr;
bool (*gAppHasUsedCheatKeys)() = nullptr;
SexyString (*gGetCurrentLevelName)() = nullptr;

namespace OpenPVZ
{
    constexpr int UpdateDialogId = 950;
    constexpr const char* LatestReleaseUrl = "https://github.com/ThomasDarkson/PvZPlusPlus/releases/latest";
    constexpr int Width = 1280, Height = 720;
    constexpr int RenderWidths[] = {Width, 1600, 1920, 2560, 3840, 7680};
    constexpr int RenderSizeCount = int(sizeof(RenderWidths) / sizeof(RenderWidths[0]));
    int frameWidth = Width, frameHeight = Height;
    constexpr int GameWidth = 800, GameHeight = 600;
    constexpr int DialogWidth = 1066, DialogPad = 133;
    constexpr int OffsetX = (Width - GameWidth) / 2, OffsetY = (Height - GameHeight) / 2;
    std::string error, status = "Not initialized", previousDirectory;
    std::vector<unsigned char> argb(Width * Height * 4);
    SDL_Texture* target = nullptr;
    SDL_Texture* dialogCanvas = nullptr;
    SDL_Texture* gpuProbeTexture = nullptr;
    SDL_Texture* gpuOutputTexture = nullptr;
    bool mouseKnown = false;
    int mouseX = 0, mouseY = 0;
    bool registered = false, video = false, ttf = false, timer = false;
    bool focused = true;
    bool initializing = false;
    double accumulator = 0;
    void trace(const std::string& line)
    {
        status = line;
        std::ofstream stream(previousDirectory + "\\pvz-native.log", std::ios::app);
        stream << line << '\n';
    }
    void require(bool ok, const char* operation)
    {
        if (!ok) throw std::runtime_error(std::string(operation) + ": " + SDL_GetError());
    }

    void ResizeFrameTarget(SDL_Renderer* renderer)
    {
        const int width = RenderWidths[pvz_window_size()];
        const int height = width * Height / Width;
        if (target && width == frameWidth) return;

        std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 4);
        auto* next = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
        require(next != nullptr, "SDL_CreateTexture host target");
        if (!SDL_SetRenderTarget(renderer, next))
        {
            SDL_DestroyTexture(next);
            require(false, "SDL_SetRenderTarget resized frame");
        }
        if (target) SDL_DestroyTexture(target);
        if (dialogCanvas) SDL_DestroyTexture(dialogCanvas);
        dialogCanvas = nullptr;
        target = next;
        argb.swap(pixels);
        frameWidth = width;
        frameHeight = height;
    }

    #include "../../Lawn/VersusMode.hpp"

    class HostedLawnApp final : public LawnApp
    {
    public:
        VersusMode versus{*this};
        double VersusEntranceDuration() const { return (versusEntranceMode == 0 ? 20.0 : 23.0) / 12.0; }
        int versusEntranceMode = 0;
        static constexpr int VersusStoneY = 401;
        SharedImageRef versusSign, coopSign, versusButtons[2][3];
        bool versusSignLoaded = false;
        SeedChooserScreen* coopMenuPressed = nullptr;
        int versusCueSounds[2] = {-1, -1};
        int versusButtonDown = -1;
        double versusEntranceTime = -1.0;

        static Rect VersusButtonRect(int mode) { return Rect(55, VersusStoneY + 157 + mode * 56, 85, 59); }

        bool VersusButtonAvailable()
        {
            return pvz_menu_ready() && !mHostedSelectorPanel && !versus.Active() &&
                mGameSelector->mSelectorState == SELECTOR_IDLE && LoadVersusStone();
        }

        bool LoadVersusStone()
        {
            if (!versusSignLoaded)
            {
                versusSignLoaded = true;
                versusSign = GetSharedImage("../versus/menu/vs-stone-animation");
                coopSign = GetSharedImage("../versus/menu/coop-stone-animation");
                constexpr const char* names[2][3] = {
                    {"vs-normal", "vs-selected", "vs-pressed"},
                    {"coop-normal", "coop-selected", "coop-pressed"},
                };
                for (int mode = 0; mode < 2; ++mode)
                    for (int state = 0; state < 3; ++state)
                        versusButtons[mode][state] = GetSharedImage(std::string("../versus/menu/") + names[mode][state]);
            }
            return (Image*)versusSign && (Image*)versusButtons[0][0] &&
                (Image*)versusButtons[0][1] && (Image*)versusButtons[0][2];
        }

        void StartVersusEntrance(int mode = 0)
        {
            if (versusEntranceTime >= 0.0) return;
            versusEntranceTime = 0.0;
            versusEntranceMode = mode;
            int& cue = versusCueSounds[mode];
            if (cue < 0 && mSoundManager)
                cue = mSoundManager->LoadSound(mode == 0 ? "../versus/menu/vs-cue" : "../versus/menu/coop-cue");
            if (cue >= 0) PlaySample(cue);
        }

        bool HandleVersusButton(int kind, int x, int y, int button)
        {
            if (versusEntranceTime >= 0.0) return true;
            if (!VersusButtonAvailable())
            {
                versusButtonDown = -1;
                return false;
            }
            if (button != 1) return false;
            if (kind == 1)
            {
                for (int mode = 0; mode < 2; ++mode)
                {
                    if (!(Image*)versusButtons[mode][0] || !VersusButtonRect(mode).Contains(x, y)) continue;
                    versusButtonDown = mode;
                    return true;
                }
            }
            if (kind == 2 && versusButtonDown >= 0)
            {
                const int mode = versusButtonDown;
                versusButtonDown = -1;
                if (VersusButtonRect(mode).Contains(x, y)) StartVersusEntrance(mode);
                return true;
            }
            return false;
        }

        bool HandleCoopChooserMenu(int kind, int x, int y, int button)
        {
            if (button != 1 || (kind != 1 && kind != 2)) return false;
            auto* chooser = mSeedChooserScreen;
            if (!mLocalCoop || !chooser || !mBoard || !mBoard->mCutScene->mSeedChoosing || GetDialogCount())
            {
                coopMenuPressed = nullptr;
                return false;
            }
            const auto* menu = chooser->mMenuButton;
            const bool over = !menu->mDisabled && !menu->mBtnNoDraw &&
                Rect(menu->mX, menu->mY, menu->mWidth, menu->mHeight).Contains(x, y);
            if (kind == 1)
            {
                coopMenuPressed = over ? chooser : nullptr;
                if (over) PlaySample(Sexy::SOUND_GRAVEBUTTON);
                return over;
            }
            const bool clicked = coopMenuPressed == chooser;
            coopMenuPressed = nullptr;
            if (clicked && over) DoNewOptions(false);
            return clicked;
        }

        void DrawVersusStone(Graphics& g)
        {
            if (versus.Active() || !LoadVersusStone()) return;
            const bool interactive = VersusButtonAvailable();
            constexpr int cellWidth = 192, cellHeight = 360;
            const bool entering = versusEntranceTime >= 0.0;
            const bool coopEntrance = entering && versusEntranceMode == 1 && (Image*)coopSign;
            const int columns = coopEntrance ? 12 : 10;
            const int frame = entering ? 1 + pvz_min(coopEntrance ? 114 : 99, int(versusEntranceTime * 60.0)) : 0;
            Image* sign = coopEntrance ? (Image*)coopSign : (Image*)versusSign;
            g.DrawImage(sign, Rect(30, VersusStoneY, 161, 302),
                Rect(frame % columns * cellWidth, frame / columns * cellHeight, cellWidth, cellHeight));
            for (int mode = 0; mode < 2; ++mode)
            {
                const Rect button = VersusButtonRect(mode);
                const bool over = interactive && mouseKnown && button.Contains(mouseX, mouseY);
                const bool pressed = (entering && mode == versusEntranceMode) || (over && versusButtonDown == mode);
                Image* image = versusButtons[mode][pressed ? 2 : over ? 1 : 0];
                if (image) g.DrawImage(image, button.mX, button.mY, button.mWidth, button.mHeight);
            }
        }

        void DrawVersusEntranceIris(Graphics& g)
        {
            if (versusEntranceTime < 0.0) return;
            constexpr float irisX = 111.0f;
            constexpr float irisY = VersusStoneY + 71.0f;
            const float fullRadius = std::hypot(Width - irisX, irisY);
            constexpr float focusRadius = 92.0f;
            constexpr double focusTime = 0.30;
            constexpr double closeStart = focusTime + 1.0;
            float radius = focusRadius;
            if (versusEntranceTime < focusTime)
            {
                const float remaining = float(1.0 - versusEntranceTime / focusTime);
                radius += (fullRadius - focusRadius) * remaining * remaining;
            }
            else if (versusEntranceTime >= closeStart)
            {
                const double closeDuration = VersusEntranceDuration() - closeStart;
                const float remaining = std::clamp(float((VersusEntranceDuration() - versusEntranceTime) / closeDuration), 0.0f, 1.0f);
                radius *= remaining;
            }
            DrawVersusIris(g, irisX, irisY, radius);
        }
        Widget* mHostedSelectorPanel = nullptr;
        HostedLawnApp() { mMusic = nullptr; }
        void DoParseCmdLine() override
        {
                                                                                 
                                                                                
            mCmdLineParsed = true;
        }
        Widget* HostedFullCanvas()
        {
            Widget* canvas = GetDialog(Dialogs::DIALOG_ALMANAC);
            if (!canvas) canvas = GetDialog(Dialogs::DIALOG_STORE);
            return canvas && canvas->mVisible ? canvas : nullptr;
        }
        void MapHostedMouse(int& x, int& y)
        {
            if (auto* canvas = HostedFullCanvas())
            {
                const Point origin = canvas->GetAbsPos();
                x = origin.mX - DialogPad + int(std::floor(double(x) * DialogWidth / Width));
                y = origin.mY + int(std::floor(double(y) * canvas->mHeight / Height));
            }
        }
        void RefreshHostedMouse()
        {
            if (!mouseKnown) return;
            int x = mouseX, y = mouseY;
            MapHostedMouse(x, y);
            if (x != mWidgetManager->mLastMouseX || y != mWidgetManager->mLastMouseY)
                mWidgetManager->MouseMove(x, y);
        }
        void PresentHostedCanvas()
        {
            auto* canvas = HostedFullCanvas();
            if (!canvas) return;
            const Point origin = canvas->GetAbsPos();
            const float scale = float(frameWidth) / Width;
            const int captureWidth = int(std::lround(DialogWidth * scale));
            const int captureHeight = int(std::lround(GameHeight * scale));
            if (!dialogCanvas)
            {
                dialogCanvas = SDL_CreateTexture(mSDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, captureWidth, captureHeight);
                require(dialogCanvas != nullptr, "SDL_CreateTexture dialog canvas");
                require(SDL_SetTextureBlendMode(dialogCanvas, SDL_BLENDMODE_NONE), "SDL_SetTextureBlendMode dialog canvas");
                require(SDL_SetTextureScaleMode(dialogCanvas, SDL_SCALEMODE_LINEAR), "SDL_SetTextureScaleMode dialog canvas");
            }
                                                                                
                                                                   
            const SDL_FRect source = {(origin.mX - DialogPad) * scale, origin.mY * scale,
                DialogWidth * scale, GameHeight * scale};
            const SDL_FRect capture = {0, 0, float(captureWidth), float(captureHeight)};
            const SDL_FRect screen = {0, 0, float(Width), float(Height)};
            require(SDL_SetRenderTarget(mSDLRenderer, dialogCanvas), "SDL_SetRenderTarget dialog canvas");
            require(SDL_SetRenderClipRect(mSDLRenderer, nullptr), "SDL_SetRenderClipRect dialog canvas");
            require(SDL_SetTextureBlendMode(target, SDL_BLENDMODE_NONE), "SDL_SetTextureBlendMode source canvas");
            require(SDL_RenderTexture(mSDLRenderer, target, &source, &capture), "SDL_RenderTexture capture dialog");
            require(SDL_SetRenderTarget(mSDLRenderer, target), "SDL_SetRenderTarget present dialog");
            require(SDL_SetRenderClipRect(mSDLRenderer, nullptr), "SDL_SetRenderClipRect present dialog");
            require(SDL_RenderTexture(mSDLRenderer, dialogCanvas, nullptr, &screen), "SDL_RenderTexture present dialog");
        }
        void UpdateHostedDialogs()
        {
            if (mSeedChooserScreen) mSeedChooserScreen->UpdateHostedRepickWarning();
            if (mZenGarden) mZenGarden->UpdateHostedSale();
            auto* restart = GetDialog(Dialogs::DIALOG_CONFIRM_RESTART);
            if (restart && restart->mResult != 0x7fffffff)
            {
                const bool confirmed = restart->mResult == Dialog::ID_YES;
                KillDialog(Dialogs::DIALOG_CONFIRM_RESTART);
                if (confirmed && mBoard)
                {
                    mMusic->StopAllMusic();
                    mSoundSystem->CancelPausedFoley();
                    KillNewOptionsDialog();
                    mBoardResult = BoardResult::BOARDRESULT_RESTART;
                    if (versus.Active() && mGameMode == GAMEMODE_VERSUS) versus.RestartMatch();
                    else FinishRestartConfirmDialog();
                }
            }
            if (mCreditScreen && mCreditScreen->mHostedPauseDialog >= 0)
                mCreditScreen->UpdateHostedPause();
            auto* store = static_cast<StoreScreen*>(GetDialog(Dialogs::DIALOG_STORE));
            if (store && store->mResult != 0x7fffffff)
            {
                const bool tree = store->mGoToTreeNow;
                KillStoreScreen();
                if (mZenGarden && mBoard && mGameMode == GAMEMODE_CHALLENGE_ZEN_GARDEN)
                    mZenGarden->FinishStoreVisit(tree);
                else if (tree)
                {
                    if (mGameSelector) KillGameSelector();
                    PreNewGame(GameMode::GAMEMODE_TREE_OF_WISDOM, false);
                }
                else if (mGameMode == GAMEMODE_TREE_OF_WISDOM)
                    mMusic->MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_ZEN_GARDEN);
                else if (mGameSelector)
                    mMusic->MakeSureMusicIsPlaying(MusicTune::MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);
            }
        }
        void UpdateSelectorPanels()
        {
            if (!mGameSelector) { mHostedSelectorPanel = nullptr; return; }
            auto* selector = mGameSelector;
            Widget* focus = mWidgetManager->mFocusWidget;
            Widget* next = nullptr;
            if (selector->mDestX != 0 || selector->mDestY != 0)
            {
                if (focus == selector->mZombatarWidget || focus == selector->mAchievementsWidget) next = focus;
#ifdef _HAS_LEVELSELECTOR
                if (focus == selector->mLevelSelectorWidget) next = focus;
#endif
                if (!next) next = mHostedSelectorPanel;                                             
            }
            if (next && next != mHostedSelectorPanel) next->mY = OffsetY;
            mHostedSelectorPanel = next;
            if (next && GetDialogCount() == 0 && (focus == selector || !focus))
                mWidgetManager->SetFocus(next);
            if (next)
            {
                next->mX = OffsetX;
                if (next != selector->mAchievementsWidget) next->mY = OffsetY;
            }
                                                                                
                                                                     
#ifdef _HAS_LEVELSELECTOR
            selector->mLevelSelectorWidget->mVisible = mHostedSelectorPanel == selector->mLevelSelectorWidget;
#endif
            selector->mZombatarWidget->mVisible = mHostedSelectorPanel == selector->mZombatarWidget;
            selector->mAchievementsWidget->mVisible = mHostedSelectorPanel == selector->mAchievementsWidget;
        }
        int MsgBox(const std::string& text, const std::string& title, int flags) override
        {
            throw std::runtime_error("PVZ [" + title + "]: " + text);
        }
        int MsgBox(const std::wstring& text, const std::wstring& title, int flags) override
        {
            throw std::runtime_error("PVZ [" + WStringToString(title) + "]: " + WStringToString(text));
        }
        void Popup(const std::string& text) override
        {
            throw std::runtime_error("PVZ resource error: " + text);
        }
        void Popup(const std::wstring& text) override
        {
            throw std::runtime_error("PVZ resource error: " + WStringToString(text));
        }
        void DialogButtonDepress(int dialogId, int buttonId) override
        {
            if (dialogId != UpdateDialogId)
            {
                LawnApp::DialogButtonDepress(dialogId, buttonId);
                return;
            }
            if (!GetDialog(UpdateDialogId)) return;
            KillDialog(UpdateDialogId);
            if (buttonId != Dialog::ID_YES) return;
            auto openUrl = reinterpret_cast<bool (SDLCALL*)(const char*)>(
                OpenPVZImports::resolve("SDL3.dll", "SDL_OpenURL"));
            if (!openUrl || !openUrl(LatestReleaseUrl))
                DoDialog(DIALOG_INFO, true, "Update available",
                    std::string("Couldn't open your browser. Download the latest version at:\n") + LatestReleaseUrl,
                    "OK", Dialog::BUTTONS_FOOTER);
        }
        void InitPropertiesHook() override
        {
            SexyApp::InitPropertiesHook();
            mTitle = "Plants vs Zombies++";
                                                                                         
            mProdName = "openPVZ";
            mFullCompanyName = "ThomasDarkson";
            mRegKey = "ThomasDarkson\\openPVZ";
            mIsWindowed = mIsPhysWindowed = true;
            mFullScreenWindow = false;
        }
        void MakeWindow() override
        {
            trace("Creating isolated hidden SDL3 render window");
            mWidth = GameWidth;
            mHeight = GameHeight;
            mIsWindowed = mIsPhysWindowed = true;
            mFullScreenWindow = false;
            mSDLWindow = SDL_CreateWindow("Plants vs Zombies++ render owner", Width, Height, SDL_WINDOW_HIDDEN);
            require(mSDLWindow != nullptr, "SDL_CreateWindow");
            mSDLRenderer = SDL_CreateRenderer(mSDLWindow, "direct3d11");
            require(mSDLRenderer != nullptr, "SDL_CreateRenderer direct3d11");
            require(SDL_SetRenderVSync(mSDLRenderer, 0), "SDL_SetRenderVSync");
            mHWnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(mSDLWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
            mDDInterface = new DDInterface(this);
            mDDInterface->mWidth = Width;
            mDDInterface->mHeight = Height;
            mDDInterface->mPresentationRect = Rect(0, 0, Width, Height);
            mDDInterface->mWideScreenOffsetX = OffsetX;
            mDDInterface->mWideScreenOffsetY = OffsetY;
            mDDInterface->mWideScreenExtraWidth = Width - GameWidth;
            mDDInterface->mWideScreenExtraHeight = Height - GameHeight;
            mDDInterface->mIsWidescreen = true;
            mScreenBounds = Rect(0, 0, Width, Height);
            mWidgetManager->mWidth = Width;
            mWidgetManager->mHeight = Height;
            mWidgetManager->Resize(Rect(0, 0, Width, Height), Rect(0, 0, Width, Height));
            auto* image = new SDL3Image(mSDLRenderer);
            image->mWidth = Width;
            image->mHeight = Height;
            image->mD3DData = SDL_CreateTexture(mSDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
            require(image->mD3DData != nullptr, "SDL_CreateTexture widget surface");
            mWidgetManager->mImage = image;
            ResizeFrameTarget(mSDLRenderer);
            gBoardBounds = Rect(0, 0, Width, Height);
            trace(std::string("Renderer ready: ") + SDL_GetRendererName(mSDLRenderer));
        }
        void EnforceCursor() override {}                                                
        void Redraw(Rect*) override {}                                                         
        void SwitchScreenMode(bool, bool, bool = false) override
        {
                                                                                         
            mIsWindowed = mIsPhysWindowed = true;
        }
        bool Step(double dt)
        {
            if (mShutdown || mCloseRequest || mLoadingFailed) return false;
            mActive = mHasFocus = true;
            mMinimized = false;
            UpdateSelectorPanels();
            RefreshHostedMouse();
            mLastTime = mLastTimerTime = timeGetTime();
            accumulator += pvz_min(pvz_max(dt, 0.0), 0.1);
            double tick = pvz_max(mFrameTime, 1) / 1000.0;
            while (accumulator >= tick)
            {
                DoUpdateFrames();
                DoUpdateFramesF(1.0f);
                UpdateHostedDialogs();
                pvz_host_update_achievements(this);
                if (versusEntranceTime >= 0.0)
                {
                    versusEntranceTime += tick;
                    if (versusEntranceTime >= VersusEntranceDuration() + 0.05)
                    {
                        versusEntranceTime = -1.0;
                        if (VersusButtonAvailable())
                        {
                            if (versusEntranceMode == 0) versus.Open();
                            else versus.OpenCoop();
                        }
                    }
                }
                versus.Update();
                RefreshHostedMouse();
                ProcessSafeDeleteList();
                accumulator -= tick;
                if (mShutdown || mCloseRequest || mLoadingFailed) return false;
            }
            UpdateSelectorPanels();
            ResizeFrameTarget(mSDLRenderer);
            require(SDL_SetRenderTarget(mSDLRenderer, target), "SDL_SetRenderTarget frame");
            const float scale = float(frameWidth) / Width;
            require(SDL_SetRenderScale(mSDLRenderer, scale, scale), "SDL_SetRenderScale frame");
            require(SDL_SetRenderClipRect(mSDLRenderer, nullptr), "SDL_SetRenderClipRect frame");
            require(SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, 255), "SDL_SetRenderDrawColor");
            require(SDL_RenderClear(mSDLRenderer), "SDL_RenderClear");
                                                                                  
                                                                             
            if (Sexy::IMAGE_BACKGROUND1 && !mCreditScreen)
            {
                Graphics backdrop(mWidgetManager->mImage);
                backdrop.DrawImage(Sexy::IMAGE_BACKGROUND1, Rect(0, 0, Width, Height),
                    Rect(0, 0, Sexy::IMAGE_BACKGROUND1->GetWidth(), Sexy::IMAGE_BACKGROUND1->GetHeight()));
            }
            mWidgetManager->Resize(Rect(0, 0, Width, Height), Rect(0, 0, Width, Height));
            mWidgetManager->MarkAllDirty();
            mWidgetManager->DrawScreen();
            require(SDL_SetRenderTarget(mSDLRenderer, target), "SDL_SetRenderTarget after widget draw");
            if (mHostedSelectorPanel && mGameSelector)
            {
                Graphics frame(mWidgetManager->mImage);
                frame.ClearClipRect();
                OpenPVZMenu::drawPanelFrame(&frame, Rect(OffsetX, OffsetY, GameWidth, GameHeight));
                frame.SetColor(Color(60, 65, 45));
                frame.FillRect(Rect(OffsetX - 12, OffsetY - 36, GameWidth + 24, 24));
                const char* title = mHostedSelectorPanel == mGameSelector->mAchievementsWidget ? "Achievements" : "Zombatar";
#ifdef _HAS_LEVELSELECTOR
                if (mHostedSelectorPanel == mGameSelector->mLevelSelectorWidget) title = "Level Select";
#endif
                TodDrawString(&frame, title, Width/2, OffsetY - 17, Sexy::FONT_DWARVENTODCRAFT18GREENINSET, Color::White, DS_ALIGN_CENTER);
                frame.SetColor(Color(110, 112, 90));
                frame.FillRect(Rect(OffsetX + GameWidth - 24, OffsetY - 34, 24, 20));
                TodDrawString(&frame, _S("X"), OffsetX + GameWidth - 12, OffsetY - 17,
                    Sexy::FONT_DWARVENTODCRAFT18GREENINSET, Color::White, DS_ALIGN_CENTER);
            }
            PresentHostedCanvas();
            Graphics overlay(mWidgetManager->mImage);
            if (versus.Active()) versus.Draw(overlay);
            else
            {
                versus.DrawDropIn(overlay);
                DrawVersusEntranceIris(overlay);
            }
            Graphics notification(mWidgetManager->mImage);
            notification.ClearClipRect();
            notification.mTransX = OffsetX;
            notification.mTransY = OffsetY;
            pvz_host_draw_achievements(this, &notification);
            return true;
        }
        void CapturePixels()
        {
            require(SDL_SetRenderTarget(mSDLRenderer, target), "SDL_SetRenderTarget readback");
            require(SDL_SetRenderClipRect(mSDLRenderer, nullptr), "SDL_SetRenderClipRect readback");
            require(SDL_SetRenderScale(mSDLRenderer, 1.0f, 1.0f), "SDL_SetRenderScale readback");
            SDL_Surface* raw = SDL_RenderReadPixels(mSDLRenderer, nullptr);
            require(raw != nullptr, "SDL_RenderReadPixels");
            SDL_Surface* rgba = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(raw);
            require(rgba != nullptr, "SDL_ConvertSurface");
            if (rgba->w != frameWidth || rgba->h != frameHeight)
            {
                const std::string actual = std::to_string(rgba->w) + "x" + std::to_string(rgba->h);
                SDL_DestroySurface(rgba);
                throw std::runtime_error("Unexpected offscreen frame dimensions: read " + actual +
                    ", expected " + std::to_string(frameWidth) + "x" + std::to_string(frameHeight));
            }
            for (int y = 0; y < frameHeight; ++y)
            {
                auto* src = static_cast<const unsigned char*>(rgba->pixels) + y * rgba->pitch;
                auto* dst = argb.data() + y * frameWidth * 4;
                for (int x = 0; x < frameWidth; ++x)
                {
                    dst[4*x] = 255;
                    dst[4*x+1] = src[4*x];
                    dst[4*x+2] = src[4*x+1];
                    dst[4*x+3] = src[4*x+2];
                }
            }
            SDL_DestroySurface(rgba);
        }
    };
    HostedLawnApp* app = nullptr;
}

                                                                           
                                                                            
void OpenPVZMenu::drawVersusStone(GameSelector* selector, Graphics& art)
{
    auto* app = OpenPVZ::app;
    if (app && app->mGameSelector == selector) app->DrawVersusStone(art);
}

void pvz_host_layout_widget(Sexy::Widget* widget)
{
    using namespace OpenPVZ;
    if (dynamic_cast<Board*>(widget) || dynamic_cast<GameSelector*>(widget))
    {
        widget->mWidth = Width;
        widget->mHeight = Height;
    }
    else if (dynamic_cast<TitleScreen*>(widget))
    {
        widget->Move(0, 0);
        widget->mWidth = Width;
        widget->mHeight = Height;
    }
    else if (dynamic_cast<Dialog*>(widget) ||
             dynamic_cast<ChallengeScreen*>(widget) || dynamic_cast<AwardScreen*>(widget) ||
             dynamic_cast<CreditScreen*>(widget))
    {
        widget->Move(widget->mX + OffsetX, widget->mY + OffsetY);
    }

}

void pvz_host_trace(const char* phase) { OpenPVZ::trace(phase); }

int pvz_init(const char* dataDirectory)
{
    using namespace OpenPVZ;
    try
    {
        if (app) throw std::runtime_error("Game is already initialized");
        char cwd[32768];
        if (!_getcwd(cwd, sizeof(cwd))) throw std::runtime_error("Cannot determine working directory");
        previousDirectory = cwd;
        error.clear();
        initializing = true;
        {
            std::ofstream stream(previousDirectory + "\\pvz-native.log", std::ios::trunc);
            stream << "Plants vs Zombies++ initialization diagnostics r39 - restart dialog and achievement display fixes\n";
        }
        trace("Initializing PVZ engine via hxcpp_JIT (GOTY, 1280x720)");
        if (_chdir(dataDirectory) != 0) throw std::runtime_error("Cannot open game-data directory");
        if (!Sexy::FileExists("properties/resources.xml"))
            throw std::runtime_error("Imported assets are missing properties/resources.xml");
                                                                                    
        if (!gPakInterface->AddPakFile("..\\dependency.pak"))
            throw std::runtime_error("Missing or invalid bundled game-data/dependency.pak");
        if (!OpenPVZImports::resolve("bass.dll", "BASS_GetVersion"))
            throw std::runtime_error(OpenPVZImports::errors());
        SDL_SetMainReady();
        require(SDL_RegisterApp("OpenPVZ_SDL3_Host", CS_BYTEALIGNCLIENT | CS_OWNDC, GetModuleHandleW(nullptr)), "SDL_RegisterApp");
        registered = true;
        require(SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "SDL_InitSubSystem");
        video = true;
        require(TTF_Init(), "TTF_Init");
        ttf = true;
        gHInstance = GetModuleHandleW(nullptr);
        TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
        gGetCurrentLevelName = LawnGetCurrentLevelName;
        gAppCloseRequest = LawnGetCloseRequest;
        gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;
        app = new HostedLawnApp();
        gLawnApp = app;
        app->mChangeDirTo = dataDirectory;
        app->mOnlyAllowOneCopyToRun = false;
        trace("BEGIN LawnApp::Init");
        app->Init();
        trace("END LawnApp::Init");
        app->mWidgetManager->Resize(Rect(0, 0, Width, Height), Rect(0, 0, Width, Height));
        if (app->mLoadingFailed || app->mShutdown) throw std::runtime_error("PVZ initialization failed; see resource error and pvz-native.log");
        if (Sexy::FileExists("extension/compiled/reanim/versus/Zombie_Target.reanim.compiled") &&
            Sexy::FileExists("extension/compiled/reanim/versus/VSgravestone.reanim.compiled"))
        {
            const auto& targetDefinition = gReanimatorDefArray[REANIM_ZOMBIE_TARGET];
            const auto& graveDefinition = gReanimatorDefArray[REANIM_VS_GRAVESTONE];
            if (!targetDefinition.mTracks.tracks || !graveDefinition.mTracks.tracks)
                throw std::runtime_error("Versus animation loading failed; see pvz-native.log");
            trace("PASS: Target Zombie and Versus Gravestone animation definitions loaded");
        }
        app->mActive = app->mHasFocus = app->mRunning = true;
        app->mMinimized = false;
        app->mLastTime = app->mLastUserInputTick = app->mLastTimerTime = timeGetTime();
        timer = timeBeginPeriod(1) == TIMERR_NOERROR;
        initializing = false;
        trace("PVZ initialized; entering host-driven loading/title screen");
        return 1;
    }
    catch (const std::exception& e) { initializing = false; error = e.what(); trace("ERROR: " + error); return 0; }
    catch (...) { initializing = false; error = "Unknown native initialization exception"; trace(error); return 0; }
}
int pvz_frame(double deltaSeconds)
{
    try
    {
        if (!OpenPVZ::app) return 0;
        if (!OpenPVZ::focused) return 3;
        if (!OpenPVZ::app->Step(deltaSeconds)) return 2;
        return 1;
    }
    catch (const std::exception& e) { OpenPVZ::error = e.what(); return 0; }
    catch (...) { OpenPVZ::error = "Unknown native frame exception"; return 0; }
}
int pvz_frame_width() { return OpenPVZ::frameWidth; }
const unsigned char* pvz_pixels()
{
    using namespace OpenPVZ;
    if (!app || !target) return nullptr;
    try
    {
        app->CapturePixels();
        return argb.data();
    }
    catch (const std::exception& e) { error = e.what(); return nullptr; }
    catch (...) { error = "Unknown native frame readback exception"; return nullptr; }
}

void* pvz_gpu_output_frame()
{
    using namespace OpenPVZ;
    auto* renderer = LawnApp::mSDLRenderer;
    if (!renderer || !target || !pvz_gpu_probe_device()) return nullptr;
    if (!gpuOutputTexture)
        gpuOutputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, frameWidth, frameHeight);
    if (!gpuOutputTexture) return nullptr;

    float width = 0, height = 0;
    if (!SDL_GetTextureSize(gpuOutputTexture, &width, &height) || width != frameWidth || height != frameHeight)
        return nullptr;

    auto* previous = SDL_GetRenderTarget(renderer);
    if (!SDL_SetRenderTarget(renderer, gpuOutputTexture)) return nullptr;
    const bool copied = SDL_SetRenderScale(renderer, 1.0f, 1.0f) &&
        SDL_SetRenderClipRect(renderer, nullptr) &&
        SDL_SetTextureBlendMode(target, SDL_BLENDMODE_NONE) &&
        SDL_RenderTexture(renderer, target, nullptr, nullptr) && SDL_FlushRenderer(renderer);
    if (!SDL_SetRenderTarget(renderer, previous) || !copied) return nullptr;
    return SDL_GetPointerProperty(SDL_GetTextureProperties(gpuOutputTexture),
        SDL_PROP_TEXTURE_D3D11_TEXTURE_POINTER, nullptr);
}

void pvz_gpu_output_end()
{
    if (OpenPVZ::gpuOutputTexture) SDL_DestroyTexture(OpenPVZ::gpuOutputTexture);
    OpenPVZ::gpuOutputTexture = nullptr;
}
void* pvz_gpu_probe_device()
{
    auto* renderer = LawnApp::mSDLRenderer;
    return renderer ? SDL_GetPointerProperty(SDL_GetRendererProperties(renderer),
        SDL_PROP_RENDERER_D3D11_DEVICE_POINTER, nullptr) : nullptr;
}

void* pvz_gpu_probe_frame(int frame)
{
    using namespace OpenPVZ;
    auto* renderer = LawnApp::mSDLRenderer;
    if (!renderer || !pvz_gpu_probe_device()) return nullptr;
    if (!gpuProbeTexture)
        gpuProbeTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 16, 16);
    if (!gpuProbeTexture) return nullptr;

    auto* previous = SDL_GetRenderTarget(renderer);
    if (!SDL_SetRenderTarget(renderer, gpuProbeTexture)) return nullptr;
    const SDL_FRect lowerHalf = {0, 8, 16, 8};
    const bool drawn = SDL_SetRenderScale(renderer, 1.0f, 1.0f) &&
        SDL_SetRenderClipRect(renderer, nullptr) &&
        SDL_SetRenderDrawColor(renderer, frame == 0 ? 255 : 0, frame == 0 ? 0 : 255, 0, 255) &&
        SDL_RenderClear(renderer) && SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255) &&
        SDL_RenderFillRect(renderer, &lowerHalf) && SDL_FlushRenderer(renderer);
    if (!SDL_SetRenderTarget(renderer, previous) || !drawn) return nullptr;
    return SDL_GetPointerProperty(SDL_GetTextureProperties(gpuProbeTexture),
        SDL_PROP_TEXTURE_D3D11_TEXTURE_POINTER, nullptr);
}

void pvz_gpu_probe_end()
{
    if (OpenPVZ::gpuProbeTexture) SDL_DestroyTexture(OpenPVZ::gpuProbeTexture);
    OpenPVZ::gpuProbeTexture = nullptr;
}

const char* pvz_error() { return OpenPVZ::error.c_str(); }
const char* pvz_status() { return OpenPVZ::status.c_str(); }
void pvz_mouse(int kind, int x, int y, int button)
{
    auto* app = OpenPVZ::app;
    if (!app || app->mShutdown) return;
    app->versus.SetMousePosition(x, y);
    if (app->versus.HandleConcedeMouse(kind, x, y, button)) return;
    if (app->versus.Active() && !app->versus.PauseDialogOpen() && !app->versus.CoopUsesNativeInput())
    {
        app->versus.Mouse(kind, x, y, button);
        return;
    }
    OpenPVZ::mouseKnown = true;
    OpenPVZ::mouseX = x;
    OpenPVZ::mouseY = y;
    if (app->HandleVersusButton(kind, x, y, button)) return;
    app->MapHostedMouse(x, y);
    auto* manager = app->mWidgetManager;
    app->UpdateSelectorPanels();
    if (app->mHostedSelectorPanel && app->GetDialogCount() == 0 && !Rect(OpenPVZ::OffsetX, OpenPVZ::OffsetY, OpenPVZ::GameWidth, OpenPVZ::GameHeight).Contains(x, y))
    {
        if (kind == 2 && Rect(OpenPVZ::OffsetX + OpenPVZ::GameWidth - 24, OpenPVZ::OffsetY - 34, 24, 20).Contains(x, y))
        {
            app->mGameSelector->ShowGameSelectorScreen();
            app->UpdateSelectorPanels();
        }
        manager->MouseMove(-10000, -10000);
        if (kind == 2) manager->MouseUp(-10000, -10000, button);
        return;
    }
    app->mMouseIn = true;
    app->mLastUserInputTick = timeGetTime();
    manager->MouseMove(x, y);
    if (app->HandleCoopChooserMenu(kind, x, y, button)) return;
    if (kind == 1) manager->MouseDown(x, y, button);
    else if (kind == 2) manager->MouseUp(x, y, button);
    else if (kind == 3) manager->MouseWheel(button);
    app->RefreshHostedMouse();
}
Sexy::KeyCode pvz_host_key_code(int key)
{
    using namespace Sexy;
    switch (key)
    {
        case 8: return KEYCODE_BACK;
        case 9: return KEYCODE_TAB;
        case 12: return KEYCODE_CLEAR;
        case 13: return KEYCODE_RETURN;
        case 16: return KEYCODE_SHIFT;
        case 17: return KEYCODE_CONTROL;
        case 18: return KEYCODE_MENU;
        case 19: return KEYCODE_PAUSE;
        case 20: return KEYCODE_CAPITAL;
        case 27: return KEYCODE_ESCAPE;
        case 32: return KEYCODE_SPACE;
        case 33: return KEYCODE_PRIOR;
        case 34: return KEYCODE_NEXT;
        case 35: return KEYCODE_END;
        case 36: return KEYCODE_HOME;
        case 37: return KEYCODE_LEFT;
        case 38: return KEYCODE_UP;
        case 39: return KEYCODE_RIGHT;
        case 40: return KEYCODE_DOWN;
        case 45: return KEYCODE_INSERT;
        case 46: return KEYCODE_DELETE;
        case 91: return KEYCODE_LWIN;
        case 92: return KEYCODE_RWIN;
        case 93: return KEYCODE_APPS;
        case 106: return KEYCODE_MULTIPLY;
        case 107: return KEYCODE_ADD;
        case 108: return KEYCODE_SEPARATOR;
        case 109: return KEYCODE_SUBTRACT;
        case 110: return KEYCODE_DECIMAL;
        case 111: return KEYCODE_DIVIDE;
        case 144: return KEYCODE_NUMLOCK;
        case 145: return KEYCODE_SCROLL;
    }
    if (key >= 48 && key <= 90) return static_cast<KeyCode>(key);
    if (key >= 96 && key <= 105) return static_cast<KeyCode>(KEYCODE_NUMPAD0 + key - 96);
    if (key >= 112 && key <= 135) return static_cast<KeyCode>(KEYCODE_F1 + key - 112);
    return KEYCODE_UNKNOWN;
}

void pvz_key(int kind, int key, int character)
{
    auto* versusApp = OpenPVZ::app;
    if (versusApp && versusApp->versus.Active() && !versusApp->versus.CoopUsesNativeInput())
    {
        if (key == 27) versusApp->versus.Escape(kind);
        return;
    }
    if (versusApp && versusApp->versusEntranceTime >= 0.0) return;
    if (kind == 1 && key == 86 && versusApp && !versusApp->mHostedSelectorPanel && pvz_menu_ready())
    {
        if (versusApp->VersusButtonAvailable()) versusApp->StartVersusEntrance();
        else versusApp->versus.Open();
        return;
    }
    if (kind == 1 && key == 27 && OpenPVZ::app && OpenPVZ::app->mGameSelector && OpenPVZ::app->GetDialogCount() == 0 && OpenPVZ::app->mHostedSelectorPanel)
    {
        OpenPVZ::app->mGameSelector->ShowGameSelectorScreen();
        OpenPVZ::app->UpdateSelectorPanels();
        return;
    }

    auto* app = OpenPVZ::app;
    if (!app || app->mShutdown) return;
    app->mLastUserInputTick = timeGetTime();
    if (kind == 1)
    {
        app->mWidgetManager->KeyDown(pvz_host_key_code(key));
        if (character >= 32 && character <= 126) app->mWidgetManager->KeyChar(static_cast<SexyChar>(character));
    }
    else app->mWidgetManager->KeyUp(pvz_host_key_code(key));
}
void pvz_focus(bool focused)
{
    if (!focused && OpenPVZ::focused && OpenPVZ::app)
    {
        OpenPVZ::app->versusButtonDown = -1;
        OpenPVZ::app->coopMenuPressed = nullptr;
        OpenPVZ::app->versus.LostFocus();
    }
    OpenPVZ::focused = focused;
}
void pvz_shutdown()
{
    using namespace OpenPVZ;
    if (app)
    {
                                                                                   
        app->mLoadingThreadCompleted = true;
        pvz_host_clear_achievements();
        pvz_host_free_menu_geometry();
        pvz_host_free_dialog_images();
        app->Shutdown();
        app->mRunning = false;
        app->mHWnd = nullptr;                                                        
        delete app;
        app = nullptr;
        gLawnApp = nullptr;
    }
    pvz_gpu_probe_end();
    pvz_gpu_output_end();
    if (dialogCanvas) SDL_DestroyTexture(dialogCanvas);
    dialogCanvas = nullptr;
    mouseKnown = false;
    if (target) SDL_DestroyTexture(target);
    target = nullptr;
    if (LawnApp::mSDLRenderer) SDL_DestroyRenderer(LawnApp::mSDLRenderer);
    if (LawnApp::mSDLWindow) SDL_DestroyWindow(LawnApp::mSDLWindow);
    LawnApp::mSDLRenderer = nullptr;
    LawnApp::mSDLWindow = nullptr;
    if (ttf) TTF_Quit();
    ttf = false;
    if (video) SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    video = false;
    if (registered) SDL_UnregisterApp();
    registered = false;
    if (timer) timeEndPeriod(1);
    timer = false;
    if (!previousDirectory.empty()) _chdir(previousDirectory.c_str());
}

const char* pvz_scene()
{
    static std::string value;
    value = gGetCurrentLevelName ? gGetCurrentLevelName() : "Before App";
    return value.c_str();
}

namespace OpenPVZ
{
    const SexyString UnlockedProfile = _S("[All Unlocked]");
    SexyString normalProfile;
    void unlockProfile(PlayerInfo* profile)
    {
        profile->mFinishedAdventure = pvz_max(profile->mFinishedAdventure, 2);
        profile->mLevel = 1;
        profile->mCoins = pvz_max(profile->mCoins, 99999);
        profile->mHasUsedCheatKeys = true;
        profile->mHasUnlockedMinigames = true;
        profile->mHasUnlockedPuzzleMode = true;
        profile->mHasUnlockedSurvivalMode = true;
        profile->mHasSeenUpsell = true;
        profile->mNeedsMessageOnGameSelector = 0;
        profile->mNeedsMagicTacoReward = 0;
        for (int& record : profile->mChallengeRecords) record = pvz_max(record, 50);
        for (int item = STORE_ITEM_PLANT_GATLINGPEA; item <= STORE_ITEM_PLANT_IMITATER; ++item)
            profile->mPurchases[item] = 1;
        const StoreItem upgrades[] = { STORE_ITEM_POOL_CLEANER, STORE_ITEM_ROOF_CLEANER,
            STORE_ITEM_PHONOGRAPH, STORE_ITEM_GARDENING_GLOVE, STORE_ITEM_MUSHROOM_GARDEN,
            STORE_ITEM_WHEEL_BARROW, STORE_ITEM_AQUARIUM_GARDEN, STORE_ITEM_TREE_OF_WISDOM,
            STORE_ITEM_FIRSTAID, STORE_ITEM_GOLD_WATERINGCAN, STORE_ITEM_STINKY_THE_SNAIL };
        for (auto item : upgrades) profile->mPurchases[item] = 1;
        profile->mPurchases[STORE_ITEM_PACKET_UPGRADE] = 4;
        profile->mPurchases[STORE_ITEM_BONUS_LAWN_MOWER] = 1;
        profile->mPurchases[STORE_ITEM_RAKE] = 3;
        for (auto item : { STORE_ITEM_FERTILIZER, STORE_ITEM_BUG_SPRAY, STORE_ITEM_CHOCOLATE, STORE_ITEM_TREE_FOOD })
            profile->mPurchases[item] = PURCHASE_COUNT_OFFSET + 999;
        profile->SaveDetails();
    }
}
int pvz_menu_ready()
{
    auto* app = OpenPVZ::app;
    return app && app->mPlayerInfo && app->mGameSelector && !app->mGameSelector->mStartingGame &&
        app->mGameSelector->mSlideCounter == 0 && app->mGameSelector->mDestX == 0 &&
        app->mGameSelector->mDestY == 0 && app->GetDialogCount() == 0;
}
int pvz_update_warning()
{
    if (!pvz_menu_ready()) return 0;
    auto* app = OpenPVZ::app;
    auto* hosted = dynamic_cast<OpenPVZ::HostedLawnApp*>(app);
    if (!app->mHasFocus || app->mGameSelector->mSelectorState != SELECTOR_IDLE ||
        (hosted && (hosted->mHostedSelectorPanel || hosted->versus.Active() || hosted->versusEntranceTime >= 0))) return 0;
    auto* dialog = static_cast<LawnDialog*>(app->DoDialog(OpenPVZ::UpdateDialogId, true, "Update available",
        "PvZ++ is outdated. Please update if you didn't mod the game",
        "", Dialog::BUTTONS_YES_NO));
    dialog->mLawnYesButton->SetLabel("Update");
    dialog->mLawnNoButton->SetLabel("Later");
    return 1;
}

int pvz_all_unlocked()
{
    auto* app = OpenPVZ::app;
    return app && app->mPlayerInfo && app->mPlayerInfo->mName == OpenPVZ::UnlockedProfile;
}
int pvz_menu_action(int action)
{
    using namespace OpenPVZ;
    if (!pvz_menu_ready()) return 0;
    try
    {
        if (action != 2) return 0;
        PlayerInfo* profile = nullptr;
        if (!pvz_all_unlocked())
        {
            normalProfile = app->mPlayerInfo->mName;
            profile = app->mProfileMgr->GetProfile(UnlockedProfile);
            if (!profile) profile = app->mProfileMgr->AddProfile(UnlockedProfile);
            if (!profile) throw std::runtime_error("Cannot create All Unlocked profile");
            unlockProfile(profile);
        }
        else
        {
            profile = app->mProfileMgr->GetProfile(normalProfile);
            if (!profile)
                for (auto& entry : app->mProfileMgr->GetProfileMap())
                    if (entry.first != UnlockedProfile) { profile = app->mProfileMgr->GetProfile(entry.first); break; }
            if (!profile) throw std::runtime_error("Create a normal profile using Change User first");
        }
        app->WriteCurrentUserConfig();
        app->KillGameSelector();
        app->mPlayerInfo = profile;
        app->mAdventureReplay = false;
        app->mPendingAdventureLevel = 0;
        app->PreloadForUser();
        app->ShowGameSelector();
        app->WriteCurrentUserConfig();
        app->mProfileMgr->Save();
        return 1;
    }
    catch (const std::exception& e) { error = e.what(); return -1; }
}

namespace { int gHostedWindowSize = 0; }
int pvz_window_size() { return gHostedWindowSize; }
void pvz_set_window_size(int index) { if (index >= 0 && index < OpenPVZ::RenderSizeCount) gHostedWindowSize = index; }
void pvz_host_cycle_window_size() { gHostedWindowSize = (gHostedWindowSize + 1) % OpenPVZ::RenderSizeCount; }

namespace { int gHostedDisplayFlags = 2 | PVZ_CUSTOM_CONTENT_DISABLED; }
namespace { int gHostedSharpness = 0; }
int pvz_sharpness() { return gHostedSharpness; }
void pvz_set_sharpness(int percent) { gHostedSharpness = pvz_max(0, pvz_min(100, percent)); }

namespace { int gHostedFpsLimit = 60; }
int pvz_fps_limit() { return gHostedFpsLimit; }
void pvz_set_fps_limit(int limit) { gHostedFpsLimit = pvz_max(30, pvz_min(360, limit)); }
void pvz_host_cycle_fps_limit()
{
    constexpr int limits[] = {30, 45, 60, 75, 90, 100, 120, 144, 165, 180, 200, 240, 300, 360};
    for (int limit : limits)
        if (limit > gHostedFpsLimit)
        {
            gHostedFpsLimit = limit;
            return;
        }
    gHostedFpsLimit = limits[0];
}

int pvz_display_flags() { return gHostedDisplayFlags; }
void pvz_set_display_flags(int flags) { gHostedDisplayFlags = flags & (3 | PVZ_CUSTOM_CONTENT_DISABLED); }
void pvz_host_set_fullscreen(bool enabled) { gHostedDisplayFlags = (gHostedDisplayFlags & ~1) | (enabled ? 1 : 0); }
void pvz_host_set_vsync(bool enabled) { gHostedDisplayFlags = (gHostedDisplayFlags & ~2) | (enabled ? 2 : 0); }




