 /*
 * gCanvas.h
 *
 *  Created on: May 6, 2020
 *      Author: Noyan Culum
 */

#ifndef GCANVAS_H_
#define GCANVAS_H_

#include "gBaseCanvas.h"
#include "gApp.h"
#include "gGUIFrame.h"
#include "gGUISizer.h"
#include "gGUISizerSpace.h"
#include "gGUITextbox.h"
#include "gGUIText.h"
#include "gColor.h"
#include "gFont.h"
#include "gRoundedButton.h"
#include "gRoundedSizer.h"
#include "gThemedCheckbox.h"
#include "passwordgenerator.h"
#include "bruteforceengine.h"
#include <vector>


// GUI-only shell: a white page with a single green rounded card that holds the
// input, a collapsible rounded charset panel and rounded buttons with hover
// inversion. No strength/brute-force logic lives here yet - the Test / Generate
// buttons and the checkboxes are wired to empty handlers for you to fill in.
class gCanvas : public gBaseCanvas {
public:
	gCanvas(gApp* root);
	virtual ~gCanvas();

	void setup();
	void update();

	void onGuiEvent(int guiObjectId, int eventType, std::string value1 = "", std::string value2 = "");
	void windowResized(int w, int h);

	void showNotify();
	void hideNotify();

private:
	void rebuildLayout();

	// selectedSets returns the map keys for every checked charset box, in a fixed
	// order. These names are what passwordgenerator matches against its map.
	std::vector<std::string> selectedSets();

	// charsetForTarget infers the alphabet to brute-force from the characters the
	// target actually uses, so Test Strength needs no checkbox selection.
	std::string charsetForTarget(const std::string& target);

	// threadCount returns how many worker threads to run (from the hardware).
	int threadCount();

	gApp* root;
	gGUIFrame mainframe;

	// White page (mainsizer) with a centered green card.
	gGUISizer mainsizer;
	gRoundedSizer cardsizer;

	// Header.
	gGUIText titlelabel;
	gGUIText subtitlelabel;

	// Input sits inside a white rounded panel.
	gRoundedSizer inputpanel;
	gGUITextbox passwordbox;

	// Collapsible charset panel (a rounded white dropdown of checkboxes).
	gRoundedButton charsettoggle;
	gRoundedSizer charsetpanel;
	gThemedCheckbox lowercasebox;
	gThemedCheckbox uppercasebox;
	gThemedCheckbox digitsbox;
	gThemedCheckbox symbolsbox;
	bool charsetpanelopen;

	// Action buttons.
	gGUISizer buttonrow;
	gRoundedButton teststrengthbutton;
	gRoundedButton generatebutton;

	// Character set matching / generation (the unordered_map / "hash").
	passwordgenerator generator;

	// The multithreaded brute-force benchmark behind the Test button.
	bruteforceengine engine;

	// Last text pushed to the labels. update() only calls setText when the string
	// actually changes: rebuilding a gGUIText every frame races with the render
	// thread and drops glyphs.
	std::string lastresult;
	std::string lastprogress;

	// Output.
	gGUIText resulttext;
	gGUIText progresstext;

	// Equal spacers above and below the content block center it in the card.
	gGUISizerSpace topspace;
	gGUISizerSpace bottomspace;

	// Fonts (engine default is 11px, too small).
	gFont fonttitle;
	gFont fontbody;
	gFont fontbutton;

	// Palette.
	gColor colpage;
	gColor colcard;
	gColor colwhite;
	gColor colinktitle;
	gColor colinksubtle;
	gColor colink;
	gColor colbtnfill;
	gColor colbtntext;
	gColor colbtnhoverfill;
	gColor colbtnhovertext;
	gColor colcheck;
	gColor colcheckborder;
};

#endif /* GCANVAS_H_ */
