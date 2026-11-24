#ifndef __SCROLLBUTTONWIDGET_H__
#define __SCROLLBUTTONWIDGET_H__

#include "ButtonWidget.h"

namespace Sexy 
{
	
class ButtonListener;

class ScrollbuttonWidget : public ButtonWidget 
{
public:
	bool				mHorizontal;

	                                                                                
	                                      
	int					mType; 

public:
	ScrollbuttonWidget(int theId, ButtonListener *theButtonListener, int theType = 0);
	virtual ~ScrollbuttonWidget();

	void				Draw(Graphics *g);
};

}

#endif                           
