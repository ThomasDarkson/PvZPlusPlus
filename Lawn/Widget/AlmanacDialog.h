#ifndef __ALMANACDIALOG_H__
#define __ALMANACDIALOG_H__

#include "LawnDialog.h"
#include "GameButton.h"
#include "../../ConstEnums.h"
#include "ScrollbarWidget.h"

#define NUM_ALMANAC_SEEDS NUM_SEEDS_IN_CHOOSER
#define NUM_ALMANAC_ZOMBIES 26

constexpr const float			ALMANAC_PLANT_POSITION_X		= 578.0f;
constexpr const float			ALMANAC_PLANT_POSITION_Y		= 140.0f;
constexpr const float			ALMANAC_ZOMBIE_POSITION_X		= 559.0f;
constexpr const float			ALMANAC_ZOMBIE_POSITION_Y		= 175.0f;
constexpr const int				ALMANAC_INDEXPLANT_POSITION_X	= 167;
constexpr const int				ALMANAC_INDEXPLANT_POSITION_Y	= 255;
constexpr const float			ALMANAC_INDEXZOMBIE_POSITION_X	= 535.0f;
constexpr const float			ALMANAC_INDEXZOMBIE_POSITION_Y	= 215.0f;

class Plant;
class Zombie;
class LawnApp;
class GameButton;
class Reanimation;
class AlmanacDialog : public LawnDialog
{
    ContentPageSelector mContentPages;
    std::size_t mCustomPlantCount = 0;
    std::size_t mCustomZombieCount = 0;
    void DrawEntryText(Graphics* g, const SexyString& text, const Rect& bounds, DrawStringJustification align);
private:
	enum
	{
		ALMANAC_BUTTON_CLOSE = 0,
		ALMANAC_BUTTON_PLANT = 1,
		ALMANAC_BUTTON_ZOMBIE = 2,
		ALMANAC_BUTTON_INDEX = 3
	};

public:
	LawnApp*					mApp;					        
	GameButton*					mCloseButton;			        
	GameButton*					mIndexButton;			        
	GameButton*					mPlantButton;			        
	GameButton*					mZombieButton;			        
	AlmanacPage					mOpenPage;				        
	Reanimation*				mReanim[4];				        
	SeedType					mSelectedSeed;			        
	ZombieType					mSelectedZombie;		        
	Plant*						mPlant;					        
	Zombie*						mZombie;				        
	Zombie*						mZombiePerfTest[400];	        
	vector<Zombie*>				mFollowersList;	
	LawnScrollbarWidget*			mScrollbar;
	float						mScrollAmount;
	
public:
	void SetCustomEntryCounts(std::size_t plants, std::size_t zombies);
	AlmanacDialog(LawnApp* theApp);
	virtual ~AlmanacDialog();

	void						ClearPlantsAndZombies();
	virtual void				RemovedFromManager(WidgetManager* theWidgetManager);
	void						SetupPlant();
	void						SetupZombie();
	void						SetPage(AlmanacPage thePage);
	virtual void				Update();
	void						DrawIndex(Graphics* g);
	void						DrawPlants(Graphics* g);
	void						DrawZombies(Graphics* g);
	virtual void				Draw(Graphics* g);
	void						GetSeedPosition(SeedType theSeedType, int& x, int& y);
	SeedType					SeedHitTest(int x, int y);
	           bool				ZombieHasSilhouette(ZombieType theZombieType);
	bool						ZombieIsShown(ZombieType theZombieType);
	bool						ZombieHasDescription(ZombieType theZombieType);
	void						GetZombiePosition(ZombieType theZombieType, int& x, int& y);
	ZombieType					ZombieHitTest(int x, int y);
	virtual void				MouseUp(int x, int y, int theClickCount);
	virtual void				MouseDown(int x, int y, int theClickCount);
	virtual void				KeyChar(char theChar) {  }

	static ZombieType			GetZombieType(int theIndex);
	           void				ShowPlant(SeedType theSeedType);
	           void				ShowZombie(ZombieType theZombieType);

	virtual void				MouseWheel(int theDelta);
	virtual void				AddedToManager(WidgetManager* theWidgetManager);
	virtual void				OrderInManagerChanged();
};
extern bool gZombieDefeated[NUM_ZOMBIE_TYPES];

           void					AlmanacInitForPlayer();
           void					AlmanacPlayerDefeatedZombie(ZombieType theZombieType);

#endif
