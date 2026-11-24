#ifndef __IMITATERDIALOG_H__
#define __IMITATERDIALOG_H__

#include "LawnDialog.h"
#include "GameButton.h"
#include <vector>

class ToolTipWidget;
class ImitaterDialog : public LawnDialog
{
public:
    ToolTipWidget*          mToolTip;
    SeedType                mToolTipSeed;

private:
    ContentPageSelector     mPages;
    std::vector<SeedType>   mCustomSeeds;
    SeedType                SeedAtSlot(std::size_t slot) const;
    int                     SeedSlotCount() const;

public:
    ImitaterDialog();
    virtual ~ImitaterDialog();

    SeedType                SeedHitTest(int x, int y);
    void                    UpdateCursor();
    virtual void            Update();
               void         GetSeedPosition(int theIndex, int& x, int& y);
    virtual void            Draw(Graphics* g);
    void                    ShowToolTip();
               void         RemoveToolTip();
    virtual void            MouseDown(int x, int y, int theClickCount);
    virtual void            MouseUp(int x, int y, int theClickCount);
};

#endif