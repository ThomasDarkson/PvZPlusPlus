#include "../Board.h"
#include "GameButton.h"
#include "../Cutscene.h"
#include "AlmanacDialog.h"
#include "../LawnCommon.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "NewOptionsDialog.h"
#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../SexyAppFramework/Font.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

#ifdef OPENFL_HOST
#include "LawnDialog.h"
#include <shellapi.h>
#include <algorithm>

namespace
{
    static SexyString CreditsVersion(const SexyString& version)
    {
        if (std::count(version.begin(), version.end(), _S('.')) == 3 &&
            version.find_first_not_of(_S("0123456789.")) == SexyString::npos)
            return version.substr(0, version.rfind(_S('.')));
        return version;
    }

    const struct CreditsEntry
    {
        const SexyChar* prefix;
        const SexyChar* name;
        const char* url;
    } creditsEntries[] = {
        {_S("Made by "), _S("Thomas Darkson"), "https://thomasdarkson.com"},
        {_S("stabledecompile by "), _S("InLiothixi"), "https://github.com/InLiothixi"},
        {_S("Plants vs. Zombies by "), _S("PopCap Games"), "https://www.ea.com/ea-studios/popcap"},
    };

    class ScriptableCreditsDialog final : public LawnDialog
    {
        static constexpr int FirstCreditBaseline = 146;
        static constexpr int CreditLineSpacing = 24;
        static constexpr int ContentShiftLeft = 8;
        int mPressedLink = -1;

        Rect LinkBounds(const CreditsEntry& entry, int baseline) const
        {
            const int prefixWidth = mLinesFont->StringWidth(entry.prefix);
            const int nameWidth = mLinesFont->StringWidth(entry.name);
            return Rect((mWidth - prefixWidth - nameWidth) / 2 + prefixWidth - ContentShiftLeft,
                baseline - mLinesFont->GetAscent(), nameWidth, mLinesFont->GetHeight() + 3);
        }

        int LinkAt(int x, int y) const
        {
            for (int i = 0; i < LENGTH(creditsEntries); ++i)
                if (LinkBounds(creditsEntries[i], FirstCreditBaseline + i * CreditLineSpacing).Contains(x, y)) return i;
            return -1;
        }

    public:
        explicit ScriptableCreditsDialog(LawnApp* app)
            : LawnDialog(app, Dialogs::DIALOG_SCRIPTABLE_CREDITS, true,
                _S(""), _S(""), _S("[DIALOG_BUTTON_OK]"), BUTTONS_FOOTER)
        {
        }

        void Resize(int x, int y, int width, int height) override
        {
            LawnDialog::Resize(x, y, width, height);
            const int buttonWidth = mLawnYesButton->mWidth;
            mLawnYesButton->Resize((width - buttonWidth) / 2 - ContentShiftLeft, 224, buttonWidth, mLawnYesButton->mHeight);
        }

        void Draw(Graphics* g) override
        {
            LawnDialog::Draw(g);
            TodDrawString(g, _S("Plants vs Zombies++ ") + CreditsVersion(mApp->mProductVersion), mWidth / 2 - ContentShiftLeft - 6, 112,
                mHeaderFont, mColors[Dialog::COLOR_HEADER], DS_ALIGN_CENTER);
            g->SetFont(mLinesFont);
            for (int i = 0; i < LENGTH(creditsEntries); ++i)
            {
                const auto& entry = creditsEntries[i];
                const int baseline = FirstCreditBaseline + i * CreditLineSpacing;
                const Rect link = LinkBounds(entry, baseline);
                g->SetColor(mColors[Dialog::COLOR_LINES]);
                g->DrawString(entry.prefix, link.mX - mLinesFont->StringWidth(entry.prefix), baseline);
                g->SetColor(Color(160, 220, 255));
                g->DrawString(entry.name, link.mX, baseline);
                g->DrawLine(link.mX, baseline + 2, link.mX + link.mWidth - 1, baseline + 2);
            }
        }

        void KeyDown(KeyCode key) override
        {
            if (key == KEYCODE_ESCAPE)
                ButtonDepress(Dialog::ID_OK);
            else
                LawnDialog::KeyDown(key);
        }

        void MouseMove(int x, int y) override
        {
            mApp->SetCursor(LinkAt(x, y) >= 0 ? CURSOR_HAND : CURSOR_POINTER);
        }

        void MouseLeave() override
        {
            mApp->SetCursor(CURSOR_POINTER);
        }

        void MouseDown(int x, int y, int button, int clicks) override
        {
            if (button != 0) return;
            mPressedLink = LinkAt(x, y);
            if (mPressedLink < 0) LawnDialog::MouseDown(x, y, button, clicks);
        }

        void MouseUp(int x, int y, int button, int clicks) override
        {
            if (button != 0) return;
            LawnDialog::MouseUp(x, y, button, clicks);
            const int link = LinkAt(x, y);
            const int pressed = mPressedLink;
            mPressedLink = -1;
            if (link >= 0 && link == pressed)
                ShellExecuteA(nullptr, "open", creditsEntries[link].url, nullptr, nullptr, SW_SHOWNORMAL);
        }
    };
}

static void DrawHostedOptionsButton(Graphics* g, LawnStoneButton* button, bool action = false)
{
    if (!button->mVisible) return;
    Graphics control(*g);
    const bool hovered = button->mIsOver && !button->mDisabled;
    const bool down = button->mIsDown && hovered;
    Image* left = down ? IMAGE_BUTTON_DOWN_LEFT : IMAGE_BUTTON_LEFT;
    Image* middle = down ? IMAGE_BUTTON_DOWN_MIDDLE : IMAGE_BUTTON_MIDDLE;
    Image* right = down ? IMAGE_BUTTON_DOWN_RIGHT : IMAGE_BUTTON_RIGHT;
    const int x = button->mX, y = button->mY, w = button->mWidth, h = button->mHeight;
    const int cap = pvz_min(w / 3, FloatRoundToInt(left->GetWidth() * float(h) / left->GetHeight()));
    auto part = [&](Image* image, int dx, int width)
    {
        control.DrawImage(image, Rect(dx, y, width, h), Rect(0, 0, image->GetWidth(), image->GetHeight()));
    };
    part(left, x, cap);
    part(middle, x + cap, w - cap * 2);
    part(right, x + w - cap, cap);
    Font* font = action ? (hovered ? FONT_DWARVENTODCRAFT18BRIGHTGREENINSET : FONT_DWARVENTODCRAFT18GREENINSET)
        : FONT_DWARVENTODCRAFT15;
    Color color = !action && hovered ? Color(132, 255, 0) : Color::White;
    if (font->StringWidth(button->mLabel) > w - 12)
    {
        font = FONT_DWARVENTODCRAFT15;
        if (action) color = Color(132, 255, 0);
        if (font->StringWidth(button->mLabel) > w - 12) font = FONT_DWARVENTODCRAFT12;
    }
    TodDrawString(&control, button->mLabel, x + w / 2, y + (h - font->GetHeight()) / 2 + font->GetAscent(),
        font, color, DS_ALIGN_CENTER);
}
#endif

          
NewOptionsDialog::NewOptionsDialog(LawnApp* theApp, bool theFromGameSelector) : 
	Dialog(nullptr, nullptr, Dialogs::DIALOG_NEWOPTIONS, true, _S("[OPTIONS]"), _S(""), _S(""), Dialog::BUTTONS_NONE)
{
    mApp = theApp;
    mFromGameSelector = theFromGameSelector;
    SetColor(Dialog::COLOR_BUTTON_TEXT, Color(255, 255, 100));
    mAlmanacButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Almanac, this, _S("[VIEW_ALMANAC_BUTTON]"));
    mRestartButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Restart, this, _S("[RESTART_LEVEL]"));
    mBackToMainButton = MakeButton(NewOptionsDialog::NewOptionsDialog_MainMenu, this, _S("[MAIN_MENU_BUTTON]"));

                                                                                                                             
                                                                                                                    
                                                                                                               

    mBackToGameButton = MakeNewButton(
        Dialog::ID_OK, 
        this, 
        _S("[BACK_TO_GAME]"), 
        nullptr, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0, 
        IMAGE_OPTIONS_BACKTOGAMEBUTTON2
    );
    mBackToGameButton->mTranslateX = 0;
    mBackToGameButton->mTranslateY = 0;
    mBackToGameButton->mTextOffsetX = -2;
    mBackToGameButton->mTextOffsetY = -5;
    mBackToGameButton->mTextDownOffsetX = 0;
    mBackToGameButton->mTextDownOffsetY = 1;
    mBackToGameButton->SetFont(FONT_DWARVENTODCRAFT36GREENINSET);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL, Color::White);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color::White);
    mBackToGameButton->mHiliteFont = FONT_DWARVENTODCRAFT36BRIGHTGREENINSET;
    
    mMusicVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_MusicVolume, this);
    double aMusicVolume = theApp->GetMusicVolume();
    aMusicVolume = pvz_max(0.0, pvz_min(1.0, aMusicVolume));
    mMusicVolumeSlider->SetValue(aMusicVolume);

    mSfxVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_SoundVolume, this);
    mSfxVolumeSlider->SetValue(theApp->GetSfxVolume() / 0.65);

#ifdef OPENFL_HOST
    const int widths[] = {1280, 1600, 1920, 2560, 3840, 7680}, heights[] = {720, 900, 1080, 1440, 2160, 4320};
    const int size = pvz_window_size();
    mWindowSizeButton = MakeButton(NewOptionsDialog_WindowSize, this, StrFormat("%d x %d", widths[size], heights[size]));
    mWindowSizeButton->mBtnNoDraw = true;
    mFpsLimitButton = MakeButton(NewOptionsDialog_FpsLimit, this, StrFormat("%d FPS", pvz_fps_limit()));
    mFpsLimitButton->mBtnNoDraw = true;
    mSharpnessSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog_Sharpness, this);
    mSharpnessSlider->SetValue(pvz_sharpness() / 100.0);
    mScriptableCreditsButton = MakeButton(NewOptionsDialog_ScriptableCredits, this, _S("Credits (PvZ++)"));
    mScriptableCreditsButton->mBtnNoDraw = true;
    mScriptableCreditsButton->SetVisible(mFromGameSelector);
#endif
    mFullscreenCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_Fullscreen, this, !theApp->mIsWindowed);
    mHardwareAccelerationCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_HardwareAcceleration, this, theApp->mEnableVsync);
#ifdef OPENFL_HOST
    mFullscreenCheckbox->SetChecked((pvz_display_flags() & 1) != 0, false);
    mHardwareAccelerationCheckbox->SetChecked((pvz_display_flags() & 2) != 0, false);
#endif

    if (mFromGameSelector)
    {
        mRestartButton->SetVisible(false);
        mBackToGameButton->SetLabel(_S("[DIALOG_BUTTON_OK]"));
        if (mApp->HasFinishedAdventure() && !mApp->IsTrialStageLocked())
        {
            mBackToMainButton->SetLabel(_S("[CREDITS]"));
        }
        else
        {
            mBackToMainButton->SetVisible(false);
        }
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ICE || 
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || 
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
    {
        mRestartButton->SetVisible(false);
    }
    if (mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO && !mApp->mBoard->mCutScene->IsSurvivalRepick())
    {
        mRestartButton->SetVisible(false);
    }
    if (!mApp->CanShowAlmanac() || 
        mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO || 
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN ||
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM || 
        mFromGameSelector)
    {
        mAlmanacButton->SetVisible(false);
    }

                            
       
                                                
                                              
                                                           
                                                 
       
          
       
                                               
                                               
                                               
       
}

                        
NewOptionsDialog::~NewOptionsDialog()
{
#ifdef OPENFL_HOST
    delete mWindowSizeButton;
    delete mFpsLimitButton;
    delete mSharpnessSlider;
    delete mScriptableCreditsButton;
#endif
    delete mMusicVolumeSlider;
    delete mSfxVolumeSlider;
    delete mFullscreenCheckbox;
    delete mHardwareAccelerationCheckbox;
    delete mAlmanacButton;
    delete mRestartButton;
    delete mBackToMainButton;
    delete mBackToGameButton;
                             
                             
                             
}

          
int NewOptionsDialog::GetPreferredHeight(int theWidth)
{
    return IMAGE_OPTIONS_MENUBACK->mWidth;
}

          
void NewOptionsDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::AddedToManager(theWidgetManager);
                                 
                                 
                                 
    AddWidget(mAlmanacButton);
    AddWidget(mRestartButton);
    AddWidget(mBackToMainButton);
    AddWidget(mMusicVolumeSlider);
    AddWidget(mSfxVolumeSlider);
    AddWidget(mHardwareAccelerationCheckbox);
    AddWidget(mFullscreenCheckbox);
    AddWidget(mBackToGameButton);
#ifdef OPENFL_HOST
    AddWidget(mWindowSizeButton);
    AddWidget(mFpsLimitButton);
    AddWidget(mSharpnessSlider);
    AddWidget(mScriptableCreditsButton);
#endif
}

          
void NewOptionsDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::RemovedFromManager(theWidgetManager);
                                    
                                    
                                    
    RemoveWidget(mAlmanacButton);
    RemoveWidget(mRestartButton);
    RemoveWidget(mBackToMainButton);
    RemoveWidget(mMusicVolumeSlider);
    RemoveWidget(mSfxVolumeSlider);
    RemoveWidget(mHardwareAccelerationCheckbox);
    RemoveWidget(mFullscreenCheckbox);
    RemoveWidget(mBackToGameButton);
#ifdef OPENFL_HOST
    RemoveWidget(mWindowSizeButton);
    RemoveWidget(mFpsLimitButton);
    RemoveWidget(mSharpnessSlider);
    RemoveWidget(mScriptableCreditsButton);
#endif
}

          
void NewOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    Dialog::Resize(theX, theY, theWidth, theHeight);
    mMusicVolumeSlider->Resize(199, 116, 135, 40);
    mSfxVolumeSlider->Resize(199, 143, 135, 40);
    mHardwareAccelerationCheckbox->Resize(283, 175, 46, 45);
    mFullscreenCheckbox->Resize(284, 206, 46, 45);
    mAlmanacButton->Resize(107, 241, 209, 46);
    mRestartButton->Resize(mAlmanacButton->mX, mAlmanacButton->mY + 43, 209, 46);
    mBackToMainButton->Resize(mRestartButton->mX, mRestartButton->mY + 43, 209, 46);
    mBackToGameButton->Resize(30, 381, mBackToGameButton->mWidth, mBackToGameButton->mHeight);

                                                                
                                                                                     
                                                                                       

    if (mFromGameSelector)
    {
        mMusicVolumeSlider->mY += 5;
        mSfxVolumeSlider->mY += 10;
        mHardwareAccelerationCheckbox->mY += 15;
        mFullscreenCheckbox->mY += 20;

                                    
                                    
                                  
                                                                                           
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM || mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ICE)
    {
        mAlmanacButton->mY += 43;
    }
#ifdef OPENFL_HOST
    const int controlsOffsetY = mFromGameSelector ? 0 : -8;
    mWindowSizeButton->Resize(215, 259 + controlsOffsetY, 114, 28);
    mFpsLimitButton->Resize(215, 291 + controlsOffsetY, 114, 28);
    mSharpnessSlider->Resize(215, 323 + controlsOffsetY, 114, 32);
    const int actionWidth = 170;
    const int actionX = (theWidth - actionWidth) / 2;
    mAlmanacButton->Resize(actionX, 357 + controlsOffsetY, actionWidth, 34);
    mRestartButton->Resize(actionX, 394 + controlsOffsetY, actionWidth, 34);
    mBackToMainButton->Resize(actionX, (mFromGameSelector ? 357 : 431) + controlsOffsetY, actionWidth, 34);
    const int creditsY = mBackToMainButton->mVisible ? 394 : 357;
    mScriptableCreditsButton->Resize(actionX, creditsY, actionWidth, 28);
    mAlmanacButton->mBtnNoDraw = true;
    mRestartButton->mBtnNoDraw = true;
    mBackToMainButton->mBtnNoDraw = true;
    mBackToGameButton->mY += 119;
#endif

}

          
void NewOptionsDialog::Draw(Sexy::Graphics* g)
{
#ifdef OPENFL_HOST
    g->DrawImage(IMAGE_OPTIONS_MENUBACK, Rect(0, 0, mWidth, mHeight), Rect(0, 0, IMAGE_OPTIONS_MENUBACK->GetWidth(), IMAGE_OPTIONS_MENUBACK->GetHeight()));
#else
    g->DrawImage(IMAGE_OPTIONS_MENUBACK, 0, 0);
#endif

#ifdef OPENFL_HOST
    DrawHostedOptionsButton(g, mWindowSizeButton);
    DrawHostedOptionsButton(g, mFpsLimitButton);
    DrawHostedOptionsButton(g, mAlmanacButton, true);
    DrawHostedOptionsButton(g, mRestartButton, true);
    DrawHostedOptionsButton(g, mBackToMainButton, true);
    DrawHostedOptionsButton(g, mScriptableCreditsButton, true);
#endif
    int aMusicOffset = 0;
    int aSfxOffset = 0;
    int a3DAccelOffset = 0;
    int aFullScreenOffset = 0;
    if (mFromGameSelector)
    {
        aMusicOffset = 5;
        aSfxOffset = 10;
        a3DAccelOffset = 15;
        aFullScreenOffset = 20;
    }
    Sexy::Color aTextColor(107, 109, 145);

                            
     
                                                                                         
             
                                         
      
                                                                                                                                                             
                                                                                                                                            
     
          
    {
        TodDrawString(g, TodStringTranslate(_S("[MUSIC_LABEL]")), 186, 140 + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[SOUND_LABEL]")), 186, 167 + aSfxOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
#ifdef OPENFL_HOST
        TodDrawString(g, _S("Window size"), 202, mWindowSizeButton->mY + 22, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("FPS limit"), 202, mFpsLimitButton->mY + 22, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, StrFormat("Sharpness %d%%", pvz_sharpness()), 202, mSharpnessSlider->mY + 21, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
#endif
        TodDrawString(g, _S("VSync"), 274, 197 + a3DAccelOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[FULLSCREEN_LABEL]")), 274, 229 + aFullScreenOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    }
}

          
void NewOptionsDialog::SliderVal(int theId, double theVal)
{
    switch (theId)
    {
#ifdef OPENFL_HOST
    case NewOptionsDialog::NewOptionsDialog_Sharpness:
        pvz_set_sharpness(int(theVal * 100.0 + 0.5));
        MarkDirty();
        break;
#endif
    case NewOptionsDialog::NewOptionsDialog_MusicVolume:
        mApp->SetMusicVolume(theVal);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
                                             
         
                                                
           
        break;

    case NewOptionsDialog::NewOptionsDialog_SoundVolume:
        mApp->SetSfxVolume(theVal * 0.65);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        if (!mSfxVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }
        break;
    }
}

          
void NewOptionsDialog::CheckboxChecked(int theId, bool checked)
{
    mApp->PlaySample(SOUND_BUTTONCLICK);
#ifdef OPENFL_HOST
    if (theId == NewOptionsDialog_Fullscreen) pvz_host_set_fullscreen(checked);
    if (theId == NewOptionsDialog_HardwareAcceleration) pvz_host_set_vsync(checked);
#endif

                    
     
                                                       
     
                                               
         
                           
                                               
                     
                                           
                                          
                                 
                                      
              

                                                         
         
              
     

                                                                 
     
                    
         

                                                   
             

                                                                        
                               
                                         
                         
                                                 
                                                
                                     
                                          
                  
             
                                                          
             
                               
                                         
                         
                                           
                                          
                                     
                                          
                  
             
         
              
     
       
}

          
void NewOptionsDialog::KeyDown(Sexy::KeyCode theKey)
{
    if (mApp->mBoard)
    {
        mApp->mBoard->DoTypingCheck(theKey);
    }

    if (theKey == KeyCode::KEYCODE_SPACE || theKey == KeyCode::KEYCODE_RETURN)
    {
        Dialog::ButtonDepress(Dialog::ID_OK);
    }
    else if (theKey == KeyCode::KEYCODE_ESCAPE)
    {
        Dialog::ButtonDepress(                     Dialog::ID_OK);
    }
}

          
void NewOptionsDialog::ButtonPress(int theId)
{
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

          
void NewOptionsDialog::ButtonDepress(int theId)
{
#ifdef OPENFL_HOST
    if (theId == NewOptionsDialog_ScriptableCredits && mFromGameSelector)
    {
        auto* credits = new ScriptableCreditsDialog(mApp);
        mApp->CenterDialog(credits, 630, 310);
        mApp->AddDialog(credits);
        mWidgetManager->SetFocus(credits);
        return;
    }
    if (theId == NewOptionsDialog_FpsLimit)
    {
        pvz_host_cycle_fps_limit();
        mFpsLimitButton->SetLabel(StrFormat("%d FPS", pvz_fps_limit()));
        MarkDirty();
        return;
    }
    if (theId == NewOptionsDialog_WindowSize)
    {
        pvz_host_cycle_window_size();
        pvz_host_set_fullscreen(false);
        mFullscreenCheckbox->SetChecked(false, false);
        const int widths[] = {1280, 1600, 1920, 2560, 3840, 7680}, heights[] = {720, 900, 1080, 1440, 2160, 4320};
        const int size = pvz_window_size();
        mWindowSizeButton->SetLabel(StrFormat("%d x %d", widths[size], heights[size]));
        MarkDirty();
        return;
    }
#endif
    Dialog::ButtonDepress(theId);

    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_Almanac:
    {
        AlmanacDialog* aDialog = mApp->DoAlmanacDialog(SeedType::SEED_NONE, ZombieType::ZOMBIE_INVALID);
#ifndef OPENFL_HOST
        aDialog->WaitForResult(true);
#endif
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_MainMenu:
    {
        if (mFromGameSelector)
        {
            mApp->KillNewOptionsDialog();
            mApp->KillGameSelector();
            mApp->ShowAwardScreen(AwardType::AWARD_CREDITS_ZOMBIENOTE, false);
        }
        else if (mApp->mBoard && mApp->mBoard->NeedSaveGame())
        {
            mApp->DoConfirmBackToMain();
        }
        else if (mApp->mBoard && mApp->mBoard->mCutScene && mApp->mBoard->mCutScene->IsSurvivalRepick())
        {
            mApp->DoConfirmBackToMain();
        }
        else
        {
            mApp->mBoardResult = BoardResult::BOARDRESULT_QUIT;
            mApp->DoBackToMain();
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Restart:
    {
        if (mApp->mBoard)
        {
            SexyString aDialogTitle;
            SexyString aDialogMessage;
            if (mApp->IsPuzzleMode())
            {
                aDialogTitle = _S("[RESTART_PUZZLE_HEADER]");
                aDialogMessage = _S("[RESTART_PUZZLE_BODY]");
            }
            else if (mApp->IsChallengeMode())
            {
                aDialogTitle = _S("[RESTART_CHALLENGE_HEADER]");
                aDialogMessage = _S("[RESTART_CHALLENGE_BODY]");
            }
            else if (mApp->IsSurvivalMode())
            {
                aDialogTitle = _S("[RESTART_SURVIVAL_HEADER]");
                aDialogMessage = _S("[RESTART_SURVIVAL_BODY]");
            }
            else
            {
                aDialogTitle = _S("[RESTART_LEVEL_HEADER]");
                aDialogMessage = _S("[RESTART_LEVEL_BODY]");
            }

            LawnDialog* aDialog = (LawnDialog*)mApp->DoDialog(Dialogs::DIALOG_CONFIRM_RESTART, true, aDialogTitle, aDialogMessage, _S(""), Dialog::BUTTONS_YES_NO);
            aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[RESTART_LABEL]"));
            aDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_CANCEL]"));
            
#ifndef OPENFL_HOST
            if (aDialog->WaitForResult(true) == Dialog::ID_YES)
            {
                mApp->mMusic->StopAllMusic();
                mApp->mSoundSystem->CancelPausedFoley();
                mApp->KillNewOptionsDialog();
                mApp->mBoardResult = BoardResult::BOARDRESULT_RESTART;
                mApp->mSawYeti = mApp->mBoard->mKilledYeti;
                mApp->PreNewGame(mApp->mGameMode, false, mApp->mBoard->mLevel);
            }
#endif
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Update:
        mApp->CheckForUpdates();
        break;

    case NewOptionsDialog::NewOptionsDialog_Language:
    {
        mApp->KillNewOptionsDialog();
        mApp->KillGameSelector();
        mApp->ShowLanagugeScreen();
        break;
    }
    }
}
