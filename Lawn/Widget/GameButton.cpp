#include "GameButton.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/Font.h"
#include "../../SexyAppFramework/SysFont.h"
#include "../../SexyAppFramework/Graphics.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"

static Color gGameButtonColors[6] = { Color(0, 0, 0), Color(0, 0, 0), Color(0, 0, 0), Color(255, 255, 255), Color(132, 132, 132), Color(212, 212, 212) };

          
void DrawStoneButton(Graphics* g, int x, int y, int theWidth, int theHeight, bool isDown, bool isHighLighted, const SexyString& theLabel)
{
	g->PushState();
	Image* aLeftImage = Sexy::IMAGE_BUTTON_LEFT;
	Image* aMiddleImage = Sexy::IMAGE_BUTTON_MIDDLE;
	Image* aRightImage = Sexy::IMAGE_BUTTON_RIGHT;
	int aFontX = x;
	int aFontY = y;
	int aImageX = x;
	if (isDown)
	{
		aLeftImage = Sexy::IMAGE_BUTTON_DOWN_LEFT;
		aMiddleImage = Sexy::IMAGE_BUTTON_DOWN_MIDDLE;
		aRightImage = Sexy::IMAGE_BUTTON_DOWN_RIGHT;
		aFontX++;
		aFontY++;
		aImageX++;
	}

	int aWidth = aLeftImage->GetWidth();

	int aRepeat = (theWidth - aLeftImage->mWidth - aRightImage->mWidth) / aMiddleImage->mWidth;
	g->DrawImage(aLeftImage, aImageX, y);
	aImageX += aLeftImage->mWidth;
	while (aRepeat > 0)
	{
		aWidth += aMiddleImage->GetWidth();
		g->DrawImage(aMiddleImage, aImageX, y);
		aImageX += aMiddleImage->mWidth;
		--aRepeat;
	}
	aWidth += aRightImage->GetWidth() - 4;
	g->DrawImage(aRightImage, aImageX, y);


	g->SetFont(isHighLighted ? Sexy::FONT_DWARVENTODCRAFT18BRIGHTGREENINSET : Sexy::FONT_DWARVENTODCRAFT18GREENINSET);
	aFontX += (aWidth - g->GetFont()->StringWidth(theLabel)) / 2 + 1;
	aFontY += (theHeight - g->GetFont()->GetAscent() / 6 - 1 + g->GetFont()->GetAscent()) / 2 - 4;
	g->SetColor(Color::White);
	g->DrawString(theLabel, aFontX, aFontY);
	g->PopState();
}

          
GameButton::GameButton(int theId)
{
	mLabel = _S("");
	mNormalRect = mOverRect = mDownRect = mDisabledRect = Rect();
	mOverAlpha = mOverAlphaSpeed = mOverAlphaFadeInSpeed = 0;
	mApp = (LawnApp*)gSexyAppBase;
	mId = theId;
	mLabelJustify = 0;
	mFont = nullptr;
	mButtonImage = mOverImage = mDownImage = mDisabledImage = mOverOverlayImage = nullptr;
	mInverted = mBtnNoDraw = mFrameNoDraw = false;
	mIsOver = mIsDown = mDisabled = false;
	mX = mY = mWidth = mHeight = 0;
	mParentWidget = nullptr;
	mDrawStoneButton = false;
	mTextOffsetX = mTextOffsetY = mButtonOffsetX = mButtonOffsetY = 0;
	for (int i = 0; i < 6; i++) mColors[i] = gGameButtonColors[i];
}

          
GameButton::~GameButton()
{
	if (mFont)
		delete mFont;
}

bool GameButton::HaveButtonImage(Image* theImage, Rect& theRect)
{
	return theImage != nullptr || theRect.mWidth != 0;
}

          
void GameButton::DrawButtonImage(Graphics* g, Image* theImage, Rect& theRect, int theX, int theY)
{
	int aPosX = theX + mButtonOffsetX;
	int aPosY = theY + mButtonOffsetY;

	if (theRect.mWidth != 0)
	{
		if (mButtonImage) g->DrawImage(mButtonImage, aPosX, aPosY, theRect);
	}
	else if (theImage)
		g->DrawImage(theImage, aPosX, aPosY);
}

void GameButton::SetDisabled(bool theDisabled)
{
	mDisabled = theDisabled;
}

void GameButton::SetFont(Font* theFont)
{
	if (mFont)
		delete mFont;

	mFont = theFont->Duplicate();
}

bool GameButton::IsButtonDown()
{
	return mIsDown && mIsOver && !mDisabled && !mBtnNoDraw;
}

          
                         
void GameButton::Draw(Graphics* g)
{
	if (mBtnNoDraw)
		return;

	bool isDown = IsButtonDown() ^ mInverted;
	bool isHighLighted = IsMouseOver();
	if (mDrawStoneButton)
	{
		DrawStoneButton(g, mX, mY, mWidth, mHeight, isDown, isHighLighted, mLabel);
		return;
	}

	g->mTransX += mX;
	g->mTransY += mY;
	if (!mFont && mLabel.size() > 0)
		mFont = new SysFont(mApp, "Arial Unicode MS", 10);

	int aFontX = mTextOffsetX;
	int aFontY = mTextOffsetY;
	if (mFont)
	{
		if (mLabelJustify == GameButton::BUTTON_LABEL_CENTER)
			aFontX += (mWidth - mFont->StringWidth(mLabel)) / 2;
		else if (mLabelJustify == GameButton::BUTTON_LABEL_RIGHT)
			aFontX += mWidth - mFont->StringWidth(mLabel);

		aFontY += (mHeight - mFont->GetAscent() / 6 + mFont->GetAscent() - 1) / 2;
	}
	g->SetFont(mFont);

	if (!isDown)
	{
		if (mDisabled && HaveButtonImage(mDisabledImage, mDisabledRect))
			DrawButtonImage(g, mDisabledImage, mDisabledRect, 0, 0);
		else if (mOverAlpha > 0.0f && HaveButtonImage(mOverImage, mOverRect))
		{
			if (HaveButtonImage(mButtonImage, mNormalRect) && mOverAlpha < 1.0f)                          
				DrawButtonImage(g, mButtonImage, mNormalRect, 0, 0);

			g->SetColorizeImages(true);
			g->SetColor(Color(255, 255, 255, mOverAlpha * 255));
			DrawButtonImage(g, mOverImage, mOverRect, 0, 0);
			g->SetColorizeImages(false);
		}
		else if (isHighLighted && HaveButtonImage(mOverImage, mOverRect))
			DrawButtonImage(g, mOverImage, mOverRect, 0, 0);
		else if (HaveButtonImage(mButtonImage, mNormalRect))
			DrawButtonImage(g, mButtonImage, mNormalRect, 0, 0);

		g->SetColor(mColors[isHighLighted ? GameButton::COLOR_LABEL_HILITE : GameButton::COLOR_LABEL]);
		g->DrawString(mLabel, aFontX, aFontY);

		if (isHighLighted && mOverOverlayImage)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			DrawButtonImage(g, mOverOverlayImage, mNormalRect, 0, 0);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}
	}
	else
	{
		if (HaveButtonImage(mDownImage, mDownRect))
			DrawButtonImage(g, mDownImage, mDownRect, 0, 0);
		else if (HaveButtonImage(mOverImage, mOverRect))
			DrawButtonImage(g, mOverImage, mOverRect, 1, 1);
		else if (HaveButtonImage(mButtonImage, mNormalRect))
			DrawButtonImage(g, mButtonImage, mNormalRect, 1, 1);

		g->SetColor(mColors[GameButton::COLOR_LABEL_HILITE]);
		g->DrawString(mLabel, aFontX + 1, aFontY + 1);
		if (isHighLighted && mOverOverlayImage)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			DrawButtonImage(g, mOverOverlayImage, mNormalRect, 0, 0);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}
	}

	g->Translate(-mX, -mY);
}

void GameButton::Resize(int theX, int theY, int theWidth, int theHeight)
{
	mX = theX;
	mY = theY;
	mWidth = theWidth;
	mHeight = theHeight;
}

          
bool GameButton::IsMouseOver()
{
	return mIsOver && !mDisabled && !mBtnNoDraw;
}

          
                         
void GameButton::Update()
{
	WidgetManager* aManager = mApp->mWidgetManager;
	int aMouseX = aManager->mLastMouseX, aMouseY = aManager->mLastMouseY;
	if (mParentWidget)
	{
		Point anAbsPos = mParentWidget->GetAbsPos();
		aMouseX -= anAbsPos.mX;
		aMouseY -= anAbsPos.mY;
	}

	if ((aManager->mFocusWidget && aManager->mFocusWidget == mParentWidget) || mApp->GetDialogCount() <= 0)
	{
		mIsOver = Rect(mX, mY, mWidth, mHeight).Contains(aMouseX, aMouseY);
		mIsDown = aManager->mDownButtons & 5;
	}
	else
	{
		mIsOver = false;
		mIsDown = false;
	}

	if (!mIsDown && !mIsOver && mOverAlpha > 0)
	{
		if (mOverAlphaSpeed < 0)
		{
			mOverAlpha = 0;
			return;
		}
		mOverAlpha -= mOverAlphaSpeed;
		if (mOverAlpha < 0)
			mOverAlpha = 0;
	}
	else if (mIsOver && mOverAlphaFadeInSpeed > 0 && mOverAlpha < 1)
	{
		if (mOverAlphaFadeInSpeed > 0)
		{
			mOverAlpha += mOverAlphaFadeInSpeed;
			if (mOverAlpha > 1)
				mOverAlpha = 1;
		}
		else
			mOverAlpha = 1;
	}
}

          
void GameButton::SetLabel(const SexyString& theLabel)
{
	mLabel = TodStringTranslate(theLabel);
}

          
void NewLawnButton::SetLabel(const SexyString& theLabel)
{
	mLabel = TodStringTranslate(theLabel);
}

          
void LawnStoneButton::SetLabel(const SexyString& theLabel)
{
	mLabel = TodStringTranslate(theLabel);
}

          
void LawnStoneButton::Draw(Graphics* g)
{
	if (mBtnNoDraw)
		return;

	bool isDown = (mIsDown && mIsOver && !mDisabled) ^ mInverted;
	DrawStoneButton(g, 0, 0, mWidth, mHeight, isDown, mIsOver, mLabel);
}

ContentPageSelector::ContentPageSelector(Widget* owner, int x, int y)
{
	mPrevious.Resize(x, y, 32, 30);
	mNext.Resize(x + 108, y, 32, 30);
	mPrevious.SetLabel(_S("<"));
	mNext.SetLabel(_S(">"));
	for (auto* arrow : { &mPrevious, &mNext })
	{
		arrow->mParentWidget = owner;
		arrow->mBtnNoDraw = true;
		arrow->SetFont(Sexy::FONT_DWARVENTODCRAFT18YELLOW);
		arrow->mColors[GameButton::COLOR_LABEL] = Color::White;
		arrow->mColors[GameButton::COLOR_LABEL_HILITE] = Color(255, 240, 0);
	}
}

void ContentPageSelector::SetCustomEntryCount(std::size_t count, std::size_t capacity)
{
	TOD_ASSERT(capacity > 0);
	const auto customPages = count == 0 ? 0 : 1 + (count - 1) / capacity;
	mPageCount = 1 + customPages;
	if (mPage >= mPageCount) mPage = mPageCount - 1;
}

void ContentPageSelector::Update(bool visible, bool enabled)
{
	mVisible = visible;
	mPrevious.mBtnNoDraw = mNext.mBtnNoDraw = !visible;
	mPrevious.SetDisabled(!enabled || mPage == 0);
	mNext.SetDisabled(!enabled || mPage + 1 >= mPageCount);
	mPrevious.Update();
	mNext.Update();
}

bool ContentPageSelector::IsMouseOver()
{
	return mPrevious.IsMouseOver() || mNext.IsMouseOver();
}

bool ContentPageSelector::Click()
{
	if (mPage > 0 && mPrevious.IsMouseOver())
		--mPage;
	else if (mPage + 1 < mPageCount && mNext.IsMouseOver())
		++mPage;
	else
		return false;
	return true;
}

void ContentPageSelector::Draw(Graphics* g, bool drawBackground)
{
	if (!mVisible) return;
	if (drawBackground)
	{
		const Rect bounds(mPrevious.mX - 4, mPrevious.mY + 2,
			mNext.mX + mNext.mWidth - mPrevious.mX + 8, mPrevious.mHeight - 4);
		g->PushState();
		g->SetColor(Color(34, 23, 17, 215));
		g->FillRect(bounds);
		g->SetColor(Color(135, 103, 68));
		g->DrawRect(bounds);
		g->PopState();
	}
	mPrevious.Draw(g);
	mNext.Draw(g);
	const auto label = StringToSexyString(std::to_string(mPage + 1) + " / " + std::to_string(mPageCount));
	TodDrawString(g, label, mPrevious.mX + 70, mPrevious.mY + 22,
		Sexy::FONT_DWARVENTODCRAFT18YELLOW, Color::White, DS_ALIGN_CENTER);
}

          
LawnStoneButton* MakeButton(int theId, ButtonListener* theListener, const SexyString& theText)
{
	LawnStoneButton* aButton = new LawnStoneButton(nullptr, theId, theListener);
	aButton->SetLabel(theText);

	aButton->mTranslateX = 1;
	aButton->mTranslateY = 1;
	aButton->mHasAlpha = true;
	aButton->mHasTransparencies = true;
	aButton->mHeight = 33;
	return aButton;
}

          
                         
NewLawnButton::NewLawnButton(Image* theComponentImage, int theId, ButtonListener* theListener) : DialogButton(theComponentImage, theId, theListener)
{
	mHiliteFont = nullptr;
	mTextDownOffsetX = 0;
	mTextDownOffsetY = 0;
	mButtonOffsetX = 0;
	mButtonOffsetY = 0;
	mUsePolygonShape = false;
	SetColor(ButtonWidget::COLOR_BKG, Color::White);
}

NewLawnButton::~NewLawnButton()
{
	                  
	                      
}

          
                         
void NewLawnButton::Draw(Graphics* g)
{
	if (mBtnNoDraw)
		return;

	Render(g);
}

void NewLawnButton::Render(Graphics* g)
{
	bool isDown = (mIsDown && mIsOver && !mDisabled) ^ mInverted;
	int aFontX = mTextOffsetX + mTranslateX;
	int aFontY = mTextOffsetY + mTranslateY;
	if (mFont)
	{
		if (mLabelJustify == ButtonWidget::BUTTON_LABEL_CENTER)
			aFontX += (mWidth - mFont->StringWidth(mLabel)) / 2;
		else if (mLabelJustify == ButtonWidget::BUTTON_LABEL_RIGHT)
			aFontX += mWidth - mFont->StringWidth(mLabel);

		aFontY += (mHeight - mFont->GetAscent() / 6 + mFont->GetAscent() - 1) / 2;
	}

	g->SetColorizeImages(true);
	if (!isDown)
	{
		g->SetColor(mColors[ButtonWidget::COLOR_BKG]);
		if (mDisabled && HaveButtonImage(mDisabledImage, mDisabledRect))
			DrawButtonImage(g, mDisabledImage, mDisabledRect, mButtonOffsetX, mButtonOffsetY);
		else if (mOverAlpha > 0.0f && HaveButtonImage(mOverImage, mOverRect))
		{
			if (HaveButtonImage(mButtonImage, mNormalRect) && mOverAlpha < 1.0f)                          
				DrawButtonImage(g, mButtonImage, mNormalRect, mButtonOffsetX, mButtonOffsetY);

			g->mColor.mAlpha = mOverAlpha * 255;
			DrawButtonImage(g, mOverImage, mOverRect, mButtonOffsetX, mButtonOffsetY);
		}
		else if ((mIsOver || mIsDown) && HaveButtonImage(mOverImage, mOverRect))
			DrawButtonImage(g, mOverImage, mOverRect, mButtonOffsetX, mButtonOffsetY);
		else if (HaveButtonImage(mButtonImage, mNormalRect))
			DrawButtonImage(g, mButtonImage, mNormalRect, mButtonOffsetX, mButtonOffsetY);

		g->SetColorizeImages(false);
		if (mIsOver)
		{
			g->SetFont(mHiliteFont ? mHiliteFont : mFont);
			g->SetColor(mColors[ButtonWidget::COLOR_LABEL_HILITE]);
		}
		else
		{
			g->SetFont(mFont);
			g->SetColor(mColors[ButtonWidget::COLOR_LABEL]);
		}
		g->DrawString(mLabel, aFontX, aFontY);
	}
	else
	{
		g->SetColor(mColors[ButtonWidget::COLOR_BKG]);
		if (HaveButtonImage(mDownImage, mDownRect))
			DrawButtonImage(g, mDownImage, mDownRect, mButtonOffsetX + mTranslateX, mButtonOffsetY + mTranslateY);
		else if (HaveButtonImage(mOverImage, mOverRect))
			DrawButtonImage(g, mOverImage, mOverRect, mButtonOffsetX + mTranslateX, mButtonOffsetY + mTranslateY);
		else
			DrawButtonImage(g, mButtonImage, mNormalRect, mButtonOffsetX + mTranslateX, mButtonOffsetY + mTranslateY);

		g->SetColorizeImages(false);
		g->SetFont(mHiliteFont ? mHiliteFont : mFont);
		g->SetColor(mColors[ButtonWidget::COLOR_LABEL_HILITE]);
		g->DrawString(mLabel, aFontX + mTextDownOffsetX, aFontY + mTextDownOffsetY);
	}
}

          
                         
bool NewLawnButton::IsPointVisible(int x, int y)
{
#ifdef OPENFL_HOST
    if (mHostedHitImage)
        return pvz_host_image_point(mHostedHitImage, x, y, mWidth, mHeight, -1);
#endif
	if (!mUsePolygonShape)
		return DialogButton::IsPointVisible(x, y);

	return TodIsPointInPolygon(mPolygonShape, 4, SexyVector2(x, y));
}

          
                         
NewLawnButton* MakeNewButton(int theId, ButtonListener* theListener, const SexyString& theText, Font* theFont, Image* theImageNormal, Image* theImageOver, Image* theImageDown)
{
	NewLawnButton* aButton = new NewLawnButton(nullptr, theId, theListener);
	aButton->SetFont(theFont == nullptr ? Sexy::FONT_BRIANNETOD12 : theFont);
	aButton->SetLabel(theText);

	aButton->mWidth = theImageNormal->mWidth;
	aButton->mHeight = theImageNormal->mHeight;
	aButton->mButtonImage = theImageNormal;
	aButton->mDownImage = theImageDown;
	aButton->mOverImage = theImageOver;
	aButton->mHasAlpha = true;
	aButton->mHasTransparencies = true;
	aButton->mTranslateX = 1;
	aButton->mTranslateY = 1;

	return aButton;
}

void NewLawnButton::SetOffset(int theX, int theY) {
	this->mButtonOffsetX = theX;
	this->mButtonOffsetY = theY;
}
