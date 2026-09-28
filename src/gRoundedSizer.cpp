/*
 * gRoundedSizer.cpp
 */

#include "gRoundedSizer.h"
#include "gRenderer.h"

gRoundedSizer::gRoundedSizer() {
	panelradius = 0;
	haspanel = false;
	panelcolor = gColor(1.0f, 1.0f, 1.0f);
}

void gRoundedSizer::setPanel(gColor color, int radius) {
	panelcolor = color;
	panelradius = radius;
	haspanel = true;
}

void gRoundedSizer::draw() {
	if(haspanel) {
		gColor oldcolor = *renderer->getColor();
		renderer->setColor(panelcolor);
		gDrawRoundedRectangle(left, top, width, height, panelradius, true);
		renderer->setColor(oldcolor);
	}
	// Draw the child controls (mirrors gGUISizer::draw, minus the sharp fill and
	// the borders we don't want).
	for(int line = 0; line < getLineNum(); line++) {
		for(int column = 0; column < getColumnNum(); column++) {
			gGUIControl* control = getControl(line, column);
			if(control != nullptr && control->isEnabled() && control->isVisible()) {
				control->draw();
			}
		}
	}
}
