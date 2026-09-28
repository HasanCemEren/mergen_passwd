/*
 * gRoundedButton.h
 *
 * A gGUIButton drawn with rounded corners and a hover inversion: at rest it is
 * filled (fillcol) with fillcol-contrasting text (textcol); on hover it swaps to
 * the background color (hoverfillcol) with the resting fill color as its text.
 * Presentation only - it reuses the base button's geometry and event handling.
 */

#ifndef GROUNDEDBUTTON_H_
#define GROUNDEDBUTTON_H_

#include "gGUIButton.h"
#include "gColor.h"


class gRoundedButton : public gGUIButton {
public:
	gRoundedButton();

	void setColors(gColor fill, gColor text, gColor hoverFill, gColor hoverText);
	void setCornerRadius(int radius);

	// Keep the drawn button the width of its laid-out cell, on first layout and on
	// every window resize.
	void set(gBaseApp* root, gBaseGUIObject* topParentGUIObject, gBaseGUIObject* parentGUIObject, int parentSlotLineNo, int parentSlotColumnNo, int x, int y, int w, int h) override;

	void draw() override;

private:
	gColor fillcol;
	gColor textcol;
	gColor hoverfillcol;
	gColor hovertextcol;
	int radius;
};

#endif /* GROUNDEDBUTTON_H_ */
