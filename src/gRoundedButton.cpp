/*
 * gRoundedButton.cpp
 */

#include "gRoundedButton.h"
#include "gRenderer.h"

gRoundedButton::gRoundedButton() {
	radius = 14;
	fillcol = gColor(0.30f, 0.42f, 0.16f);
	textcol = gColor(1.0f, 1.0f, 1.0f);
	hoverfillcol = gColor(1.0f, 1.0f, 1.0f);
	hovertextcol = gColor(0.30f, 0.42f, 0.16f);
	enableBackgroundFill(true);
	setContentCentered(true);
}

void gRoundedButton::setColors(gColor fill, gColor text, gColor hoverFill, gColor hoverText) {
	fillcol = fill;
	textcol = text;
	hoverfillcol = hoverFill;
	hovertextcol = hoverText;
}

void gRoundedButton::setCornerRadius(int radius) {
	this->radius = radius;
}

void gRoundedButton::set(gBaseApp* root, gBaseGUIObject* topParentGUIObject, gBaseGUIObject* parentGUIObject, int parentSlotLineNo, int parentSlotColumnNo, int x, int y, int w, int h) {
	gGUIButton::set(root, topParentGUIObject, parentGUIObject, parentSlotLineNo, parentSlotColumnNo, x, y, w, h);
	// Fill the cell width; height stays whatever setButtonh() configured.
	buttonw = width;
}

void gRoundedButton::draw() {
	gColor oldcolor = *renderer->getColor();
	int drawleft = getButtonDrawLeft();
	int drawtop = getButtonDrawTop();
	int press = ispressed ? 1 : 0;
	gColor bg = ishover ? hoverfillcol : fillcol;
	gColor tcol = ishover ? hovertextcol : textcol;

	renderer->setColor(bg);
	gDrawRoundedRectangle(drawleft, drawtop + press, buttonw, buttonh, radius, true);
	if(ishover) {
		// A thin outline keeps the hovered (background-filled) button defined.
		renderer->setColor(fillcol);
		gDrawRoundedRectangle(drawleft, drawtop + press, buttonw, buttonh, radius, false);
	}
	if(istextvisible) {
		renderer->setColor(tcol);
		resetTitlePosition();
		getFont()->drawText(title, drawleft + tx, drawtop + buttonh - ty + press);
	}
	renderer->setColor(oldcolor);
}
