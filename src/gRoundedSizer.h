/*
 * gRoundedSizer.h
 *
 * A gGUISizer whose background is a filled rounded rectangle instead of the
 * default sharp fill. Used for the green app card and the white sub-panels
 * (input field, charset dropdown). Presentation only: layout and events come
 * straight from gGUISizer.
 */

#ifndef GROUNDEDSIZER_H_
#define GROUNDEDSIZER_H_

#include "gGUISizer.h"
#include "gColor.h"


class gRoundedSizer : public gGUISizer {
public:
	gRoundedSizer();

	void setPanel(gColor color, int radius);

	void draw() override;

private:
	gColor panelcolor;
	int panelradius;
	bool haspanel;
};

#endif /* GROUNDEDSIZER_H_ */
