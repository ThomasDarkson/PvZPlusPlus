#include <memory>
                                                                  
                                                                                 
namespace OpenPVZMenu
{
    void drawVersusStone(GameSelector* selector, Graphics& art);
    inline Rect bounds(GameSelector* selector, float x, float y, float width, float height)
    {
        const float sx = selector->mApp->mDDInterface->mWidth / 1066.0f;
        const float sy = selector->mApp->mDDInterface->mHeight / 600.0f;
        return Rect(FloatRoundToInt(x * sx), FloatRoundToInt(y * sy),
            FloatRoundToInt(width * sx), FloatRoundToInt(height * sy));
    }
    std::map<std::pair<Image*, int>, Rect> alphaBoundsCache;
    Rect alphaBounds(Image* image, int cell = -1)
    {
        const auto key = std::make_pair(image, cell);
        auto found = alphaBoundsCache.find(key);
        if (found != alphaBoundsCache.end()) return found->second;
        auto* memory = dynamic_cast<MemoryImage*>(image);
        const int width = cell < 0 ? image->GetWidth() : image->GetCelWidth();
        const int height = cell < 0 ? image->GetHeight() : image->GetCelHeight();
        Rect bounds(0, 0, width, height);
        if (memory)
        {
            auto* bits = memory->GetBits();
            int left = width, right = -1, top = height, bottom = -1;
            const int offset = cell < 0 ? 0 : cell * width;
            for (int y = 0; y < height; ++y)
            {
                if (cell == -2)
                {
                    int opaque = 0;
                    for (int x = 0; x < width; ++x)
                        opaque += (bits[y * image->GetWidth() + x] >> 24) >= 32;
                    if (opaque < width * .4f) continue;                              
                }
                for (int x = 0; x < width; ++x)
                    if ((bits[y * image->GetWidth() + offset + x] >> 24) >= 32)
                    {
                        left = pvz_min(left, x); right = pvz_max(right, x);
                        top = pvz_min(top, y); bottom = pvz_max(bottom, y);
                    }
            }
            bounds = right < left ? Rect(0, 0, 0, 0) : Rect(left, top, right-left+1, bottom-top+1);
        }
        alphaBoundsCache.emplace(key, bounds);
        return bounds;
    }
    Image* zombatarImage(GameSelector* selector, bool hover)
    {
        if (hover) return Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS;
        Reanimation* sign = selector->mApp->ReanimationGet(selector->mWoodSignID);
        ReanimatorTransform transform;
        sign->GetCurrentTransform(sign->FindTrackIndex("woodsign3"), &transform);
        Image* image = transform.mImage;
        if (image && sign->mDefinition->mReanimAtlas)
        {
            auto* entry = sign->mDefinition->mReanimAtlas->GetEncodedReanimAtlas(image);
            if (entry) image = entry->mOriginalImage;
        }
        return image ? image : Sexy::IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS;
    }
    struct FlowerSpot { float nativeX, nativeY; };
    const FlowerSpot FlowerSpots[3] = {{765, 483}, {663, 455}, {701, 439}};
    inline Rect gotyBounds(GameSelector* selector, float x, float y, float w, float h)
    {
        const float scale = selector->mApp->mDDInterface->mHeight / 600.0f;
        const float left = (selector->mApp->mDDInterface->mWidth - 800 * scale) / 2;
        return Rect(FloatRoundToInt(left + x * scale), FloatRoundToInt(y * scale),
            FloatRoundToInt(w * scale), FloatRoundToInt(h * scale));
    }
    void drawPanelFrame(Graphics* g, const Rect& content)
    {
        const int e = 12, x = content.mX, y = content.mY, w = content.mWidth, h = content.mHeight;
        auto tile = [&](Image* image, Rect dest)
        {
            g->DrawImage(image, dest, Rect(0, 0, image->GetWidth(), image->GetHeight()));
        };
        tile(Sexy::IMAGE_DIALOG_TOPLEFT, Rect(x-e, y-e, e, e));
        tile(Sexy::IMAGE_DIALOG_TOPMIDDLE, Rect(x, y-e, w, e));
        tile(Sexy::IMAGE_DIALOG_TOPRIGHT, Rect(x+w, y-e, e, e));
        tile(Sexy::IMAGE_DIALOG_CENTERLEFT, Rect(x-e, y, e, h));
        tile(Sexy::IMAGE_DIALOG_CENTERRIGHT, Rect(x+w, y, e, h));
        tile(Sexy::IMAGE_DIALOG_BOTTOMLEFT, Rect(x-e, y+h, e, e));
        tile(Sexy::IMAGE_DIALOG_BOTTOMMIDDLE, Rect(x, y+h, w, e));
        tile(Sexy::IMAGE_DIALOG_BOTTOMRIGHT, Rect(x+w, y+h, e, e));
    }
    Image* levelSign(GameSelector* selector, Image* wood, bool hover)
    {
                                                                              
                                                        
        std::unique_ptr<MemoryImage> image(new MemoryImage(selector->mApp));
        image->Create(wood->GetWidth(), wood->GetHeight());
        Graphics label(image.get());
        label.DrawImage(wood, 0, 0);
        label.DrawImage(wood, Rect(22, 13, 247, 22), Rect(22, 33, 247, 5));
        const SexyString text = _S("Want to replay a level? Click here.");
        Font* font = Sexy::FONT_DWARVENTODCRAFT12;
        const float angle = -0.018f;
        const float textWidth = font->StringWidth(text);
        const float scale = pvz_min(1.04f, 244.0f / textWidth);
        const float halfWidth = textWidth / 2.0f;
        SexyMatrix3 matrix;
        matrix.LoadIdentity();
        matrix.m00 = matrix.m11 = std::cos(angle) * scale;
        matrix.m01 = -std::sin(angle) * scale;
        matrix.m10 = std::sin(angle) * scale;
        matrix.m02 = wood->GetWidth() / 2.0f - halfWidth * matrix.m00;
        matrix.m12 = 27.0f - halfWidth * matrix.m10;
        TodDrawStringMatrix(&label, font, matrix, text, hover ? Color(132, 255, 0) : Color::White);
        return image.release();
    }
    float entranceOffset(GameSelector* selector, const char* track, bool wood = false)
    {
        if (selector->mSelectorState != SELECTOR_OPEN) return 0;
        Reanimation* entrance = selector->mApp->ReanimationGet(wood ? selector->mWoodSignID : selector->mSelectorReanimID);
        const int index = entrance->FindTrackIndex(track);
        ReanimatorTransform transform;
        SexyTransform2D settled;
        entrance->GetCurrentTransform(index, &transform);
        entrance->GetTrackBasePoseMatrix(index, settled);
        return (transform.mTransY - settled.m12) * selector->mApp->mDDInterface->mHeight / 600.0f;
    }
    inline bool mainVisible(GameSelector* selector)
    {
        return true;                                                          
    }
}

Image* GameSelector::HostedMenuImage(const std::string& name)
{
    auto found = mHostedMenuImages.find(name);
    if (found != mHostedMenuImages.end()) return found->second;
    Image* image;
    if (name == "level_sign" || name == "level_sign_hover")
    {
        const bool hover = name == "level_sign_hover";
        image = OpenPVZMenu::levelSign(this, HostedMenuImage(hover ? "sign2_hover" : "sign2"), hover);
    }
    else
        image = mApp->GetImage("../widescreen-assets/menu/" + name, false);
    if (!image) throw std::runtime_error("Missing widescreen menu asset: " + name);
    mHostedMenuImages[name] = image;
    return image;
}

void GameSelector::LayoutHostedMenu()
{
    const bool visible = OpenPVZMenu::mainVisible(this);
    const bool ready = visible && mDestX == 0 && mDestY == 0 && mSlideCounter == 0 && mSelectorState == SELECTOR_IDLE && !mStartingGame && mApp->mPlayerInfo;
    bool changed = false;
    auto commit = [&](NewLawnButton* button, const Rect& rect, bool unlocked = true, bool hideWhenLocked = true)
    {
        const bool shown = visible && (unlocked || !hideWhenLocked);
        const bool interactive = ready && unlocked;
        const bool moved = button->mX != rect.mX || button->mY != rect.mY ||
            button->mWidth != rect.mWidth || button->mHeight != rect.mHeight ||
            button->mVisible != shown || button->mMouseVisible != interactive || button->mDisabled != !interactive;
        if (moved) button->MarkDirtyFull();
                                                                              
                                                                        
        button->mX = rect.mX;
        button->mY = rect.mY;
        button->mWidth = rect.mWidth;
        button->mHeight = rect.mHeight;
        button->mVisible = shown;
        button->mMouseVisible = interactive;
        button->mDisabled = !interactive;
        button->mBtnNoDraw = true;                                                     
        button->mUsePolygonShape = false;
        button->mHostedHitImage = nullptr;
        button->mMouseInsets = Insets(0, 0, 0, 0);
        if (moved) button->MarkDirty();
        changed = changed || moved;
    };
    auto place = [&](NewLawnButton* button, float x, float y, float w, float h, bool unlocked = true, bool hideWhenLocked = true)
    {
        commit(button, OpenPVZMenu::bounds(this, x, y, w, h), unlocked, hideWhenLocked);
    };
    place(mAdventureButton, 538, mShowStartButton ? 65 : 79.7f, mShowStartButton ? 331 : 330, mShowStartButton ? 146 : 120);
    place(mMinigameButton, 539, 173.1f, 313, 133, !mMinigamesLocked, false);
    place(mPuzzleButton, 543, 257.5f, 286, 122, !mPuzzleLocked, false);
    place(mSurvivalButton, 546, 328, 266, 123, !mSurvivalLocked, false);
                                                                                
    NewLawnButton* stones[] = {mAdventureButton, mMinigameButton, mPuzzleButton, mSurvivalButton};
    for (auto* button : stones)
    {
        button->mUsePolygonShape = true;
        const float w = button->mWidth, h = button->mHeight;
        button->mPolygonShape[0] = SexyVector2(w * .02f, h * .02f);
        button->mPolygonShape[1] = SexyVector2(w * .99f, h * .25f);
        button->mPolygonShape[2] = SexyVector2(w * .94f, h * .99f);
        button->mPolygonShape[3] = SexyVector2(w * .01f, h * .65f);
    }
    place(mZenGardenButton, 304, 401, 191, 163, mApp->CanShowZenGarden() || mUnlockSelectorCheat);
    place(mAlmanacButton, 460, 428, 99, 99, mApp->CanShowAlmanac() || mUnlockSelectorCheat);
    place(mStoreButton, 538, 482, 130, 89, mApp->CanShowStore() || mUnlockSelectorCheat);
    place(mOptionsButton, 698, 490, 81, 31);
    place(mHelpButton, 780, 529, 48, 22);
    place(mQuitButton, 853, 515, 47, 27);
    place(mChangeUserButton, 158, 126, 291, 71);
#ifdef _HAS_ZOMBATAR
    place(mZombatarButton, 158, 174, 291, 71);
#endif
#ifdef _HAS_LEVELSELECTOR
    place(mLevelSelectButton, 158, 222, 291, 71);
#endif
    auto signHit = [&](NewLawnButton* button, Image* image)
    {
                                                                             
                                                                 
        button->mHostedHitImage = image;
        const Rect wood = OpenPVZMenu::alphaBounds(image, -2);
        const float sx = float(button->mWidth) / image->GetWidth();
        const float sy = float(button->mHeight) / image->GetHeight();
        button->mMouseInsets = Insets(FloatRoundToInt(wood.mX * sx), FloatRoundToInt(wood.mY * sy),
            button->mWidth - FloatRoundToInt((wood.mX + wood.mWidth) * sx),
            button->mHeight - FloatRoundToInt((wood.mY + wood.mHeight) * sy));
    };
    signHit(mChangeUserButton, HostedMenuImage("sign2"));
#ifdef _HAS_ZOMBATAR
    signHit(mZombatarButton, OpenPVZMenu::zombatarImage(this, false));
#endif
#ifdef _HAS_LEVELSELECTOR
    signHit(mLevelSelectButton, HostedMenuImage("level_sign"));
#endif
#ifdef _HAS_ACHIEVEMENTS
    Image* pedestal = Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL;
    commit(mAchievementsButton, OpenPVZMenu::gotyBounds(this, 20, 408, pedestal->GetWidth(), pedestal->GetHeight()));
    mAchievementsButton->mHostedHitImage = pedestal;
    changed = changed || mTrophyButton->mVisible || mTrophyButton->mMouseVisible;
    mTrophyButton->mVisible = mTrophyButton->mMouseVisible = false;
#endif
    if (changed && mWidgetManager) mWidgetManager->RehupMouse();
}

void GameSelector::DrawHostedMenu(Graphics* g)
{
    Graphics art(*g);
    art.ClearClipRect();
    art.mTransX = art.mTransY = 0;
    auto layer = [&](const char* name, float x, float y, float w, float h, float offset = 0)
    {
        Image* image = HostedMenuImage(name);
        Graphics layerArt(art);
        layerArt.mTransY += offset;
        layerArt.DrawImage(image, OpenPVZMenu::bounds(this, x, y, w, h), Rect(0, 0, image->GetWidth(), image->GetHeight()));
    };
    const float centerOffset = OpenPVZMenu::entranceOffset(this, "SelectorScreen_BG_Center");
    const float leftOffset = OpenPVZMenu::entranceOffset(this, "SelectorScreen_BG_Left");
    const float rightOffset = OpenPVZMenu::entranceOffset(this, "SelectorScreen_BG_Right");
    const float sign1Offset = OpenPVZMenu::entranceOffset(this, "woodsign1", true);
    const float sign2Offset = OpenPVZMenu::entranceOffset(this, "woodsign2", true);
    const float sign3Offset = OpenPVZMenu::entranceOffset(this, "woodsign3", true);
    layer("bg", 0, 0, 1066, 600);
    layer("center", 213, 250, 720, 350, centerOffset);
    layer("left", 0, -79.8f, 609, 680, leftOffset);
    layer("right", 204, 41, 863, 560, rightOffset);
    if (!OpenPVZMenu::mainVisible(this)) return;
    layer("sign1", 155, -8, 293, 150, sign1Offset);
    layer(mChangeUserButton->mIsOver ? "sign2_hover" : "sign2", 158, 126, 291, 71, sign2Offset);
    art.mTransY = rightOffset;
    auto draw = [&](NewLawnButton* button, const char* normal, const char* hover)
    {
        if (!button->mVisible) return;
        const bool locked = (button == mMinigameButton && mMinigamesLocked) ||
            (button == mPuzzleButton && mPuzzleLocked) || (button == mSurvivalButton && mSurvivalLocked);
        Graphics buttonArt(art);
        if (locked)
        {
            buttonArt.SetColorizeImages(true);
            buttonArt.SetColor(Color(105, 105, 105));
        }
        Image* image = HostedMenuImage((button->mIsOver || button->mIsDown) && !button->mDisabled ? hover : normal);
        buttonArt.DrawImage(image, Rect(button->mX, button->mY, button->mWidth, button->mHeight),
            Rect(0, 0, image->GetWidth(), image->GetHeight()));
    };
    draw(mAdventureButton, mShowStartButton ? "start" : "adventure", mShowStartButton ? "start_hover" : "adventure_hover");
    draw(mMinigameButton, "minigames", "minigames_hover");
    draw(mPuzzleButton, "puzzle", "puzzle_hover");
    draw(mSurvivalButton, "survival", "survival_hover");
    draw(mZenGardenButton, "zen", "zen_hover");
    draw(mAlmanacButton, "almanac", "almanac_hover");
    draw(mStoreButton, "shop", "shop_hover");
    draw(mOptionsButton, "options", "options_hover");
    draw(mHelpButton, "help", "help_hover");
    draw(mQuitButton, "quit", "quit_hover");
                                                                              
                                                                               
                                                                                   
    const float flowerScale = mApp->mDDInterface->mHeight / 600.0f;
    const float flowerLeft = (mApp->mDDInterface->mWidth - 800 * flowerScale) / 2;
    const char* flowerTracks[] = {"flower1", "flower2", "flower3"};
    for (int i = 0; i < 3; ++i)
    {
        Reanimation* flower = mApp->ReanimationGet(mFlowerReanimID[i]);
        Image* savedImages[3];
        for (int j = 0; j < 3; ++j)
        {
            savedImages[j] = flower->GetImageOverride(flowerTracks[j]);
            flower->SetImageOverride(flowerTracks[j], HostedMenuImage(flowerTracks[j]));
        }
        const auto saved = flower->mOverlayMatrix;
        flower->mOverlayMatrix.m00 *= flowerScale;
        flower->mOverlayMatrix.m01 *= flowerScale;
        flower->mOverlayMatrix.m02 = flowerLeft + saved.m02 * flowerScale;
        flower->mOverlayMatrix.m10 *= flowerScale;
        flower->mOverlayMatrix.m11 *= flowerScale;
        flower->mOverlayMatrix.m12 *= flowerScale;
        flower->Draw(&art);
        flower->mOverlayMatrix = saved;
        for (int j = 0; j < 3; ++j)
            flower->SetImageOverride(flowerTracks[j], savedImages[j]);
    }
    auto label = [&](const SexyString& text, float x, float y, Font* font, Color color)
    {
        Rect point = OpenPVZMenu::bounds(this, x, y, 0, 0);
        TodDrawString(&art, text, point.mX, point.mY, font, color, DS_ALIGN_CENTER);
    };
    art.mTransY = sign1Offset;
    label(mApp->mPlayerInfo ? mApp->mPlayerInfo->mName : _S("Player"), 301, 105, Sexy::FONT_HOUSEOFTERROR20, Color::White);
    art.mTransY = rightOffset;
    if (!mShowStartButton && !mApp->IsIceDemo() && mApp->mPlayerInfo)
    {
                                                                            
                                                                            
        int stage = pvz_min(pvz_max((std::abs(mLevel) - 1) / 10 + 1, 1), 6);
        int sub = std::abs(mLevel) - (stage - 1) * 10;
        const bool trial = mApp->IsTrialStageLocked() && (mLevel >= 25 || mApp->HasFinishedAdventure());
        if (trial) { stage = 3; sub = 4; }
        const int press = mAdventureButton->mIsDown && mAdventureButton->mIsOver ? 1 : 0;
        Image* numbers = HostedMenuImage("levelnumbers");
        const int width = numbers->GetWidth() / 10, height = numbers->GetHeight();
        auto digit = [&](int value, float x, float y)
        {
            Graphics numberArt(art);
            if (mStartingGame)
            {
                numberArt.SetColorizeImages(true);
                numberArt.SetColor(mStartingGameCounter % 20 < 10 ? Color(80, 80, 80) : Color::White);
            }
            numberArt.DrawImage(numbers, OpenPVZMenu::bounds(this, x + press, y + press, width, height),
                Rect(value * width, 0, width, height));
        };
        digit(stage, 690 - (!trial && stage == 4 ? 1 : 0), 166 + (!trial && stage == 1 ? 1 : 0));
        const std::string subDigits = std::to_string(sub);
        for (int i = 0; i < int(subDigits.size()); ++i)
            digit(subDigits[i] - '0', 708 + 9 * i - (!trial && sub == 3 ? 1 : 0), 169 + i);
    }
    art.mTransY = sign3Offset;
#ifdef _HAS_ZOMBATAR
    if (mZombatarButton->mVisible)
    {
        Image* image = OpenPVZMenu::zombatarImage(this, mZombatarButton->mIsOver || mZombatarButton->mIsDown);
        art.DrawImage(image, Rect(mZombatarButton->mX, mZombatarButton->mY, mZombatarButton->mWidth, mZombatarButton->mHeight),
            Rect(0, 0, image->GetWidth(), image->GetHeight()));
    }
#endif
#ifdef _HAS_LEVELSELECTOR
    if (mLevelSelectButton->mVisible)
    {
        const bool hover = mLevelSelectButton->mIsOver && !mLevelSelectButton->mDisabled;
        Image* sign = HostedMenuImage(hover ? "level_sign_hover" : "level_sign");
        Rect rect(mLevelSelectButton->mX, mLevelSelectButton->mY, mLevelSelectButton->mWidth, mLevelSelectButton->mHeight);
        art.DrawImage(sign, rect, Rect(0, 0, sign->GetWidth(), sign->GetHeight()));
    }
#endif
    art.mTransY = leftOffset;
    OpenPVZMenu::drawVersusStone(this, art);
#ifdef _HAS_ACHIEVEMENTS
    if (mAchievementsButton->mVisible)
    {
        Image* pedestal = mAchievementsButton->mIsOver ? Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL_PRESS : Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_PEDESTAL;
        art.DrawImage(pedestal, Rect(mAchievementsButton->mX, mAchievementsButton->mY, mAchievementsButton->mWidth, mAchievementsButton->mHeight),
            Rect(0, 0, pedestal->GetWidth(), pedestal->GetHeight()));
        if (mHasTrophy)
        {
            Image* trophy = Sexy::IMAGE_SUNFLOWER_TROPHY;
            Rect dest = OpenPVZMenu::gotyBounds(this, 10.5f, 275.5f, trophy->GetCelWidth(), trophy->GetCelHeight());
            const int cell = trophy->GetCelWidth();
            art.DrawImage(trophy, dest, Rect(mApp->EarnedGoldTrophy() ? cell : 0, 0, cell, trophy->GetCelHeight()));
        }
    }
#endif
    art.mTransY = 0;
    if (mToolTip->mVisible && mWidgetManager->mFocusWidget == this)
        mToolTip->Draw(&art);
    if (mStartingGame)
    {
                                                                              
        if (auto* hand = mApp->ReanimationTryToGet(mHandReanimID))
        {
            const auto saved = hand->mOverlayMatrix;
            hand->mOverlayMatrix.m00 *= flowerScale;
            hand->mOverlayMatrix.m01 *= flowerScale;
            hand->mOverlayMatrix.m02 = flowerLeft + saved.m02 * flowerScale;
            hand->mOverlayMatrix.m10 *= flowerScale;
            hand->mOverlayMatrix.m11 *= flowerScale;
            hand->mOverlayMatrix.m12 = saved.m12 * flowerScale;
            hand->Draw(&art);
            hand->mOverlayMatrix = saved;
        }
        art.SetColor(Color(0, 0, 0, pvz_min(255, mStartingGameCounter / 2)));
        art.FillRect(Rect(0, 0, mApp->mDDInterface->mWidth, mApp->mDDInterface->mHeight));
    }
}

bool pvz_host_flower_point(GameSelector* selector, int& x, int& y)
{
    if (selector->mSelectorState != SELECTOR_IDLE || selector->mDestX || selector->mDestY || selector->mStartingGame || selector->mApp->GetDialogCount()) return false;
    const float scale = selector->mApp->mDDInterface->mHeight / 600.0f;
    for (const auto& spot : OpenPVZMenu::FlowerSpots)
    {
        Rect center = OpenPVZMenu::gotyBounds(selector, spot.nativeX, spot.nativeY, 0, 0);
        const float dx = (x - center.mX) / scale, dy = (y - center.mY) / scale;
        if (dx*dx + dy*dy >= 20*20) continue;
        x = FloatRoundToInt(spot.nativeX + dx);
        y = FloatRoundToInt(spot.nativeY + dy);
        return true;
    }
    return false;
}

void pvz_host_update_trophy_tooltip(GameSelector* selector)
{
    ToolTipWidget* tip = selector->mToolTip;
    tip->mVisible = false;
    LawnApp* app = selector->mApp;
    if (selector->mSelectorState != SELECTOR_IDLE || !selector->mHasTrophy || !app->mPlayerInfo || !app->HasFinishedAdventure() ||
        app->GetDialogCount() || selector->mWidgetManager->mFocusWidget != selector ||
        selector->mDestX || selector->mDestY || selector->mStartingGame) return;
    Image* trophy = Sexy::IMAGE_SUNFLOWER_TROPHY;
    Rect hit = OpenPVZMenu::gotyBounds(selector, 10.5f, 275.5f, trophy->GetCelWidth(), trophy->GetCelHeight());
    const int cell = app->EarnedGoldTrophy() ? 1 : 0;
    if (!pvz_host_image_point(trophy, selector->mWidgetManager->mLastMouseX - hit.mX,
        selector->mWidgetManager->mLastMouseY - hit.mY, hit.mWidth, hit.mHeight, cell)) return;
    SexyString count = LawnApp::Pluralize(app->mPlayerInfo->mFinishedAdventure,
        _S("[GOLD_SUNFLOWER_TOOLTIP]"), _S("[GOLD_SUNFLOWER_TOOLTIP_PLURAL]"));
    if (!app->EarnedGoldTrophy())
        count += _S("\n") + TodStringTranslate(_S("[SILVER_SUNFLOWER_TOOLTIP]"));
    tip->SetLabel(count);
    const Rect visible = OpenPVZMenu::alphaBounds(trophy, cell);
    const float sx = float(hit.mWidth) / trophy->GetCelWidth();
    const float sy = float(hit.mHeight) / trophy->GetCelHeight();
    tip->mCenter = true;
    tip->mX = hit.mX + FloatRoundToInt((visible.mX + visible.mWidth / 2.0f) * sx);
    tip->mY = hit.mY + FloatRoundToInt((visible.mY + visible.mHeight) * sy) + 6;
    tip->mMaxBottom = app->mDDInterface->mHeight;
    tip->mVisible = true;
    tip->Update();
}

bool pvz_host_image_point(Image* image, int x, int y, int width, int height, int cell)
{
    if (!image || width <= 0 || height <= 0 || x < 0 || y < 0 || x >= width || y >= height) return false;
    auto* memory = dynamic_cast<MemoryImage*>(image);
    if (!memory) return true;
    const int sourceWidth = cell < 0 ? image->GetWidth() : image->GetCelWidth();
    const int sourceHeight = cell < 0 ? image->GetHeight() : image->GetCelHeight();
    const int sourceX = int(int64_t(x) * sourceWidth / width) + (cell < 0 ? 0 : cell * sourceWidth);
    const int sourceY = int(int64_t(y) * sourceHeight / height);
    return (memory->GetBits()[sourceY * image->GetWidth() + sourceX] >> 24) >= 32;
}
void pvz_host_free_menu_geometry()
{
    OpenPVZMenu::alphaBoundsCache.clear();
}
