#ifndef __ZOMBATARSCREEN_H__
#define __ZOMBATARSCREEN_H__

#include "../../ConstEnums.h"
#include <vector>
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "../Zombie.h"
#include "GameButton.h"
#include "../../SexyAppFramework/SDL3Image.h"

#define NUM_SKIN_COLOR_PALLETES 12
#define NUM_COLOR_PALLETES 17
#define NUM_CLOTHES 12

class LawnApp;

using namespace Sexy;

enum ZombatarCategory {
	ZombatarCategory_Skin,
	ZombatarCategory_Hairs,
	ZombatarCategory_FacialHair,
	ZombatarCategory_Tidbits,
	ZombatarCategory_EyeWears,
	ZombatarCategory_Clothes,
	ZombatarCategory_Accessories,
	ZombatarCategory_Hats,
	ZombatarCategory_Backdrops,
	MAX_ZOMBATAR_CATEGORIES
};

class ZombatarDefinition {
public:
	ZombatarItem mZombatarItem;
	Image** mOutlineImage;
	Image** mImage;
	ZombatarCategory mCategory;
	int mColumn;
	int mRow;
	Color* mColorGroup;
};
extern ZombatarDefinition gZombatarDefinitions[NUM_ZOMBATAR_ITEMS];
ZombatarDefinition& GetZombatarDefinition(ZombatarItem theItem);

struct ZombatarAppearance
{
	int mCurSkinPallete;
	int mCurHair;
	int mCurHairPallete;
	int mCurFacialHair;
	int mCurFacialHairPallete;
	int mCurBackground;
	int mCurBackgroundPallete;
	int mCurTidbit;
	int mCurTidbitPallete;
	int mCurEyeWear;
	int mCurEyeWearPallete;
	int mCurClothe;
	int mCurAccessory;
	int mCurAccessoryPallete;
	int mCurHat;
	int mCurHatPallete;
};

void ApplyRandomZombatar(Zombie& zombie);

class ZombatarWidget : public Widget, public ButtonListener, public ZombatarAppearance 
{
private:
	enum {
		ZombatarScreen_Back,
		ZombatarScreen_Finish,
		ZombatarScreen_Skin,
		ZombatarScreen_Hair,
		ZombatarScreen_FacialHair,
		ZombatarScreen_Tidbits,
		ZombatarScreen_EyeWear,
		ZombatarScreen_Clothes,
		ZombatarScreen_Acessory,
		ZombatarScreen_Hats,
		ZombatarScreen_Backdrops,
		ZombatarScreen_ClearPick,
		ZombatarScreen_ClearPallete,
		ZombatarScreen_Palletes,
		ZombatarScreen_Items = ZombatarScreen_Palletes + NUM_COLOR_PALLETES + 1,
		ZombatarScreen_Confirm = ZombatarScreen_Items + NUM_ZOMBATAR_ITEMS,
		ZombatarScreen_Cancel,
		ZombatarScreen_New,
		ZombatarScreen_Delete,
		ZombatarScreen_Previous,
		ZombatarScreen_Next,
	};
	enum class View { Editor, Warning, Gallery };
	View mView = View::Editor;
	NewLawnButton* mFinishFlowButtons[6]{};
	void SetView(View view);
	int mSavedIndex = 0;
	std::vector<ZombatarAppearance> mSavedZombatars;
	void ShowSavedZombatar(int index);
	void DrawFinishView(Graphics* g);

public:

	LawnApp* mApp; 
	NewLawnButton* mBackButton;
	NewLawnButton* mFinishButton;
	NewLawnButton* mSkinButton;
	NewLawnButton* mHairButton;
	NewLawnButton* mFacialHairButton;
	NewLawnButton* mTidbitsButton;
	NewLawnButton* mEyeWearButton;
	NewLawnButton* mClothesButton;
	NewLawnButton* mAccessoryButton;
	NewLawnButton* mHatsButton;
	NewLawnButton* mBackdropsButton;
	NewLawnButton* mClearPickButton;
	NewLawnButton* mClearPalleteButton;
	ZombatarCategory mCurCategory;
	NewLawnButton* mZombatarItems[NUM_ZOMBATAR_ITEMS];
	NewLawnButton* mColorPalletes[NUM_COLOR_PALLETES];

	Zombie* mZombie;
	int mFacialHairPage = 0;


public:
	ZombatarWidget(LawnApp* theApp);
	virtual ~ZombatarWidget();
	virtual void                Update();
	virtual void                Draw(Graphics* g);
	virtual void                AddedToManager(WidgetManager* theWidgetManager);
	virtual void                RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void                OrderInManagerChanged();
	virtual void                ButtonMouseEnter(int theId);
	virtual void                ButtonDepress(int theId);
	virtual void                ButtonPress(int theId, int theClickCount);
	void						ResetZombatar(bool loadSaved = true);
	void MouseUp(int x, int y, int theClickCount) override;
	bool SaveZombatar();
	

	void						SetupZombie();
	void						SetCategory(ZombatarCategory theCategory);
	void						SetZombatarRef(int* theTarget, int theValue);
	void						DrawColorPalletes(Graphics* g);
	void						DrawZombiePortrait(Graphics* g, const Rect& bounds = Rect(596, 116, 180, 180));
	void						DrawPortraitFacialHair(Graphics* g, const Rect& bounds);
	void						DrawZombieAvatar(Graphics* g);
	void						DrawZombatarItems(Graphics* g);
	void						DrawZombatarItem(Graphics* g, NewLawnButton* button, ZombatarItem theItem, int* theTargetItem, ZombatarDefinition* aDef);
	void						DrawClearItem(Graphics* g, NewLawnButton* button, int* theTargetItem);
	void						UpdatePalletes();
	void						UpdateItems();
	void						UpdateZombieAvatar();
	void						GetOutlineOffset(ZombatarItem theItem, float* offsetX, float* offsetY);
	void						GetZombatarItemOffset(ZombatarItem theItem, float* offsetX, float* offsetY);
	void						GetZombatarItemScale(ZombatarItem theItem, float* scaleX, float* scaleY);
	void						GetZombatarPortraitOffset(ZombatarItem theItem, float* offsetX, float* offsetY);
	void						DrawClearPallete(Graphics* g, NewLawnButton* button, int* theTargetPallete);
	void						CreateZombatarClothes();
};

extern Color gZombatarSkinPalletes[NUM_SKIN_COLOR_PALLETES];
extern Color gZombatarDimPalletes[NUM_COLOR_PALLETES];
extern Color gZombatarBrightPalletes[NUM_COLOR_PALLETES];
extern MemoryImage* gZombatarClothes[NUM_CLOTHES];

void DisposeZombatarClothesCache();
#endif