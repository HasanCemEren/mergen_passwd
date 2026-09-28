/*
 * gThemedCheckbox.h
 *
 * A gGUICheckbox drawn with a rounded box and a drawn checkmark in theme colors
 * (the stock checkbox hardcodes a blue square). Presentation only: the checked
 * state and click handling come from gGUICheckbox.
 */

#ifndef GTHEMEDCHECKBOX_H_
#define GTHEMEDCHECKBOX_H_

#include "gGUICheckbox.h"
#include "gColor.h"


class gThemedCheckbox : public gGUICheckbox {
public:
	gThemedCheckbox();

	void setThemeColors(gColor box, gColor checked, gColor border, gColor tick, gColor text);

	void draw() override;

private:
	gColor boxcol;
	gColor checkedcol;
	gColor bordercol;
	gColor tickcol;
	gColor textcol;
};

#endif /* GTHEMEDCHECKBOX_H_ */
