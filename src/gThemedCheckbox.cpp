/*
 * gThemedCheckbox.cpp
 */

#include "gThemedCheckbox.h"
#include "gRenderer.h"

gThemedCheckbox::gThemedCheckbox() {
	boxcol = gColor(1.0f, 1.0f, 1.0f);
	checkedcol = gColor(0.42f, 0.55f, 0.24f);
	bordercol = gColor(0.62f, 0.68f, 0.55f);
	tickcol = gColor(1.0f, 1.0f, 1.0f);
	textcol = gColor(0.22f, 0.30f, 0.15f);
}

void gThemedCheckbox::setThemeColors(gColor box, gColor checked, gColor border, gColor tick, gColor text) {
	boxcol = box;
	checkedcol = checked;
	bordercol = border;
	tickcol = tick;
	textcol = text;
}

void gThemedCheckbox::draw() {
	gColor oldcolor = *renderer->getColor();
	int r = 5;
	renderer->setColor(ischecked ? checkedcol : boxcol);
	gDrawRoundedRectangle(left, top, buttonw, buttonh, r, true);
	renderer->setColor(bordercol);
	gDrawRoundedRectangle(left, top, buttonw, buttonh, r, false);

	if(ischecked) {
		renderer->setColor(tickcol);
		int x1 = left + (int)(buttonw * 0.22f);
		int y1 = top + (int)(buttonh * 0.52f);
		int x2 = left + (int)(buttonw * 0.42f);
		int y2 = top + (int)(buttonh * 0.72f);
		int x3 = left + (int)(buttonw * 0.78f);
		int y3 = top + (int)(buttonh * 0.28f);
		// Draw the check a few times with a 1px offset so it reads as bold.
		for(int o = 0; o < 2; o++) {
			gDrawLine(x1, y1 + o, x2, y2 + o);
			gDrawLine(x2, y2 + o, x3, y3 + o);
		}
	}

	if(istextvisible) {
		renderer->setColor(textcol);
		getFont()->drawText(title, left + buttonw + 8, top - 2 + (buttonh + titleh) / 2 - 1);
	}
	renderer->setColor(oldcolor);
}
