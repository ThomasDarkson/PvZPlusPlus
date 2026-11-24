#include "ScrollbarWidget.h"
#include "../../SexyAppFramework/WidgetManager.h"

LawnScrollbarWidget::LawnScrollbarWidget(LawnApp* theApp, ScrollbarMode theScrollbarMode) {
	mApp = theApp;
	mScrollbarMode = theScrollbarMode;
	mThumbColor = Color(63, 64, 86);              
	mThumbInnerHighlight = Color(80, 81, 108);
	mThumbInnerHighlight2 = Color(84, 85, 113);                                
	mThumbInnerHighlightShadow = Color(44, 45, 59);                                                 
	mThumbInnerShadow = Color(40, 41, 54);
	mThumbInnerShadow2 = Color(34, 35, 46);                             
	mThumbOuterShadow = Color(30, 28, 34);
	mThumbOuterShadow2 = Color(22, 19, 21);                             
	mBackgroundColor = Color(152, 149, 188);
	mViewport = Rect();
	mThumbX = 0;
	mThumbY = 0;
	mThumbWidth = mWidth;
	mThumbHeight = mHeight;
	mScrollValue = 0;
	mScrollRange = 0;
	mDoFinger = true;
	mButtonNoDraw = true;
}

LawnScrollbarWidget::~LawnScrollbarWidget() {
	
}

void LawnScrollbarWidget::Update()
{
    Widget::Update();
    mScrollRange = pvz_max(0.0f, mScrollRange);
    mScrollValue = pvz_max(0.0f, pvz_min(mScrollValue, mScrollRange));
    Point aOrigin = GetAbsPos();
    const bool aVertical = mScrollbarMode == ScrollbarMode::VERTICAL;
    const float aTrackLength = pvz_max(0, aVertical ? mHeight : mWidth);
    const float aViewLength = pvz_max(0, aVertical ? mViewport.mHeight : mViewport.mWidth);
    const float aTotalLength = aViewLength + mScrollRange;
    const float aThumbLength = aTotalLength > 0 ? aViewLength / aTotalLength * aTrackLength : aTrackLength;
    const float aTravel = aTrackLength - aThumbLength;
    if (mWidgetManager && isThumbDown() && aTravel > 0 && mScrollRange > 0)
    {
        const float aMouse = aVertical ? mWidgetManager->mLastMouseY - aOrigin.mY : mWidgetManager->mLastMouseX - aOrigin.mX;
        mScrollValue = pvz_max(0.0f, pvz_min((aMouse - aThumbLength / 2) / aTravel * mScrollRange, mScrollRange));
    }
    const float aThumbPosition = mScrollRange > 0 ? mScrollValue / mScrollRange * aTravel : 0;
    mThumbX = aVertical ? 0 : aThumbPosition;
    mThumbY = aVertical ? aThumbPosition : 0;
    mThumbWidth = aVertical ? mWidth : aThumbLength;
    mThumbHeight = aVertical ? aThumbLength : mHeight;
}

void LawnScrollbarWidget::Draw(Graphics* g) {
	if (mButtonNoDraw)
		return;

	g->SetColorizeImages(true);

	DrawThumb(g);

	if (isThumbOver()) {
		g->PushState();
		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		g->mColor.mAlpha = 96;
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		DrawThumb(g);
		g->PopState();
	}

	DrawScrollBackground(g);
	
	g->SetColorizeImages(false);
}

bool LawnScrollbarWidget::isThumbOver() {
	return !mDisabled && mIsOver;
}

bool LawnScrollbarWidget::isThumbDown() {
	return !mDisabled && mIsDown;
}

void LawnScrollbarWidget::DrawThumb(Graphics* g) {
	int aAlpha = g->mColor.mAlpha;

	g->PushState();

	g->SetColor(mThumbColor);        
	g->mColor.mAlpha = aAlpha;
	g->DrawLine(mThumbX, mThumbY, mThumbX+mThumbWidth-2, mThumbY);                            
	g->DrawLine(mThumbX, mThumbY+1, mThumbX, mThumbY+mThumbHeight-2);                             
	g->FillRect(mThumbX+2, mThumbY+2, mThumbWidth-4, mThumbHeight-3);                        

	g->SetColor(mThumbInnerHighlight);                   
	g->mColor.mAlpha = aAlpha;
	g->DrawLine(mThumbX+2, mThumbY+1, mThumbX + mThumbWidth-3, mThumbY+1);                                  
	g->DrawLine(mThumbX+1, mThumbY+2, mThumbX+1, mThumbY+mThumbHeight-3);                                   

	g->SetColor(mThumbInnerHighlight2);                                  
	g->mColor.mAlpha = aAlpha;
	g->FillRect(mThumbX+1, mThumbY+1, 1, 1);                        

	g->SetColor(mThumbInnerHighlightShadow);                                    
	g->mColor.mAlpha = aAlpha;
	g->FillRect(mThumbWidth-2, mThumbY+1, 1, 1);                                       
	g->FillRect(mThumbX+1, mThumbY+mThumbHeight-2, 1, 1);                                         

	g->SetColor(mThumbInnerShadow);                
	g->mColor.mAlpha = aAlpha;
	g->DrawLine(mThumbX+mThumbWidth-2, mThumbY+1, mThumbX+mThumbWidth-2, mThumbY+mThumbHeight-3);                      
	g->DrawLine(mThumbX+1, mThumbY+mThumbHeight-2, mThumbX+mThumbWidth-3, mThumbY+mThumbHeight-2);                        

	g->SetColor(mThumbInnerShadow2);                                    
	g->mColor.mAlpha = aAlpha;
	g->FillRect(mThumbX+mThumbWidth-2, mThumbY+mThumbHeight-2, 1, 1);                             
	
	g->SetColor(mThumbOuterShadow);           
	g->mColor.mAlpha = aAlpha;
	g->DrawLine(mThumbX+mThumbWidth-1, mThumbY, mThumbX+mThumbWidth-1, mThumbY+mThumbHeight-2);                           
	g->DrawLine(mThumbX, mThumbY+mThumbHeight-1, mThumbX+mThumbWidth-2, mThumbY+mThumbHeight-1);                            

	g->SetColor(mThumbOuterShadow2);                             
	g->mColor.mAlpha = aAlpha;
	g->FillRect(mThumbX+mThumbWidth-1, mThumbY+mThumbHeight-1, 1, 1);                            

	g->PopState();
}

void LawnScrollbarWidget::DrawScrollBackground(Graphics* g) {
	int aAlpha = g->mColor.mAlpha;
	g->PushState();
	g->SetColor(mBackgroundColor);
	g->mColor.mAlpha = aAlpha;
	if (mScrollbarMode == ScrollbarMode::VERTICAL)
	{
		g->FillRect(0, 0, mThumbWidth, mThumbY);
		g->FillRect(0, mThumbY+mThumbHeight, mThumbWidth, mHeight-mThumbY-mThumbHeight);
	}
	else if (mScrollbarMode == ScrollbarMode::HORIZONTAL)
	{
		g->FillRect(0, 0, mThumbX, mThumbHeight);
		g->FillRect(mThumbX+mThumbWidth, 0, mWidth-mThumbX-mThumbWidth, mThumbHeight);
	}
	g->PopState();
}