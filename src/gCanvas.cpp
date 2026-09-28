/*
* gCanvas.cpp
*
*  Created on: May 6, 2020
*      Author: Noyan Culum
*/


#include "gCanvas.h"
#include <thread>

// Length of a generated password.
static const int generatedlength = 12;


gCanvas::gCanvas(gApp* root) : gBaseCanvas(root) {
	this->root = root;
	charsetpanelopen = false;
	// Palette: white page, green rounded card, deep-green buttons that invert to
	// white on hover.
	colpage = gColor(1.0f, 1.0f, 1.0f);
	colcard = gColor(0.55f, 0.69f, 0.37f);
	colwhite = gColor(1.0f, 1.0f, 1.0f);
	colinktitle = gColor(1.0f, 1.0f, 1.0f);
	colinksubtle = gColor(0.91f, 0.96f, 0.84f);
	colink = gColor(0.20f, 0.28f, 0.13f);
	colbtnfill = gColor(0.28f, 0.40f, 0.16f);
	colbtntext = gColor(1.0f, 1.0f, 1.0f);
	colbtnhoverfill = gColor(1.0f, 1.0f, 1.0f);
	colbtnhovertext = gColor(0.28f, 0.40f, 0.16f);
	colcheck = gColor(0.34f, 0.47f, 0.19f);
	colcheckborder = gColor(0.62f, 0.68f, 0.55f);
}

gCanvas::~gCanvas() {
	engine.stop();
}

void gCanvas::setup() {
	fonttitle.loadFont("FreeSansBold.ttf", 32);
	fontbody.loadFont("FreeSans.ttf", 16);
	fontbutton.loadFont("FreeSansBold.ttf", 16);

	// Pre-warm every glyph now, on the main thread (which owns the GL context).
	// Result/progress text is rebuilt from the update thread at runtime; if a
	// glyph were first rasterized there it would corrupt, so load them all here.
	std::string warm = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .,-_/:()[]!@#$%^&*+=?\"";
	fonttitle.getStringWidth(warm);
	fontbody.getStringWidth(warm);
	fontbutton.getStringWidth(warm);

	root->getGUIManager()->registerFrameForResizing(&mainframe);
	root->getGUIManager()->setCurrentFrame(&mainframe);
	mainframe.setSizer(&mainsizer);

	// Header.
	titlelabel.setFont(&fonttitle);
	titlelabel.setTextAlignment(gGUIText::TEXTALIGNMENT_CENTER);
	titlelabel.setTextColor(colinktitle);
	titlelabel.setText("Password Strength Benchmark");
	subtitlelabel.setFont(&fontbody);
	subtitlelabel.setTextAlignment(gGUIText::TEXTALIGNMENT_CENTER);
	subtitlelabel.setTextColor(colinksubtle);
	subtitlelabel.setText("Measure how long a brute-force search would take.");

	// Input inside a white rounded panel. Configure the textbox BEFORE adding it
	// so the panel's vertical centering uses the right content height.
	passwordbox.setFont(&fontbody);
	passwordbox.enableBackground(false);
	passwordbox.setHintText("Type a password to benchmark");
	inputpanel.setPanel(colwhite, 12);
	inputpanel.enableBorders(false);
	inputpanel.setSize(1, 1);
	inputpanel.setSlotPadding(16, 0);
	inputpanel.setAlignContentVertically(true);
	inputpanel.setControl(0, 0, &passwordbox);

	// Collapsible charset panel: a rounded white dropdown of checkboxes. They are
	// checkboxes (not radios) so the sets combine. Configure and size the boxes
	// BEFORE adding them so the panel's vertical centering is correct.
	lowercasebox.setFont(&fontbody);
	uppercasebox.setFont(&fontbody);
	digitsbox.setFont(&fontbody);
	symbolsbox.setFont(&fontbody);
	lowercasebox.setThemeColors(colwhite, colcheck, colcheckborder, colwhite, colink);
	uppercasebox.setThemeColors(colwhite, colcheck, colcheckborder, colwhite, colink);
	digitsbox.setThemeColors(colwhite, colcheck, colcheckborder, colwhite, colink);
	symbolsbox.setThemeColors(colwhite, colcheck, colcheckborder, colwhite, colink);
	lowercasebox.setTitle("a - z");
	uppercasebox.setTitle("A - Z");
	digitsbox.setTitle("0 - 9");
	symbolsbox.setTitle("! @ # ?");
	lowercasebox.setChecked(true);
	uppercasebox.setChecked(true);
	digitsbox.setChecked(true);
	symbolsbox.setChecked(false);
	lowercasebox.setSize(22, 22);
	uppercasebox.setSize(22, 22);
	digitsbox.setSize(22, 22);
	symbolsbox.setSize(22, 22);
	charsetpanel.setPanel(colwhite, 14);
	charsetpanel.enableBorders(false);
	charsetpanel.setSize(1, 4);
	charsetpanel.setSlotPadding(20, 0);
	charsetpanel.setAlignContentVertically(true);
	charsetpanel.setControl(0, 0, &lowercasebox);
	charsetpanel.setControl(0, 1, &uppercasebox);
	charsetpanel.setControl(0, 2, &digitsbox);
	charsetpanel.setControl(0, 3, &symbolsbox);

	// Toggle + action buttons (rounded, hover inversion).
	charsettoggle.setFont(&fontbutton);
	charsettoggle.setColors(colbtnfill, colbtntext, colbtnhoverfill, colbtnhovertext);
	charsettoggle.setCornerRadius(14);
	charsettoggle.setButtonh(42);

	buttonrow.enableBorders(false);
	buttonrow.enableBackgroundFill(false);
	buttonrow.setSize(1, 2);
	buttonrow.setSlotPadding(10, 0);
	buttonrow.setControl(0, 0, &teststrengthbutton);
	buttonrow.setControl(0, 1, &generatebutton);
	teststrengthbutton.setFont(&fontbutton);
	generatebutton.setFont(&fontbutton);
	teststrengthbutton.setColors(colbtnfill, colbtntext, colbtnhoverfill, colbtnhovertext);
	generatebutton.setColors(colbtnfill, colbtntext, colbtnhoverfill, colbtnhovertext);
	teststrengthbutton.setCornerRadius(16);
	generatebutton.setCornerRadius(16);
	teststrengthbutton.setButtonh(44);
	generatebutton.setButtonh(44);
	teststrengthbutton.setTitle("Test Strength");
	generatebutton.setTitle("Generate & Test");

	// Output.
	resulttext.setFont(&fontbody);
	progresstext.setFont(&fontbody);
	resulttext.setTextAlignment(gGUIText::TEXTALIGNMENT_CENTER);
	progresstext.setTextAlignment(gGUIText::TEXTALIGNMENT_CENTER);
	resulttext.setTextColor(colinktitle);
	progresstext.setTextColor(colinksubtle);
	resulttext.setText("Pick your character sets and press Test Strength.");
	progresstext.setText("Idle.");

	rebuildLayout();
}

void gCanvas::rebuildLayout() {
	// Outer: white page with a centered green card. Three rows / three columns
	// leave a white margin all around the card.
	mainsizer.setSize(3, 3);
	mainsizer.enableBorders(false);
	mainsizer.enableBackgroundFill(true);
	mainsizer.setBackgroundColor(colpage);
	static float outerrows[3] = {0.06f, 0.88f, 0.06f};
	static float outercols[3] = {0.14f, 0.72f, 0.14f};
	mainsizer.setLineProportions(outerrows);
	mainsizer.setColumnProportions(outercols);
	mainsizer.setControl(1, 1, &cardsizer);

	// Card: a top and bottom spacer of equal size center the content block, and
	// the content rows all share the same proportion so the gaps are even.
	// Proportions must be set BEFORE the controls (setLineProportions does not
	// reposition existing controls).
	cardsizer.setPanel(colcard, 28);
	int rows = charsetpanelopen ? 10 : 9;
	cardsizer.setSize(rows, 1);
	cardsizer.enableBorders(false);
	// Vertical slot padding keeps a consistent gap between every row, so the
	// charset panel never touches the buttons below it when it opens.
	cardsizer.setSlotPadding(40, 12);

	if(charsetpanelopen) {
		static float openprs[10] = {0.16f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.16f};
		cardsizer.setLineProportions(openprs);
	} else {
		static float closedprs[9] = {0.2025f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.085f, 0.2025f};
		cardsizer.setLineProportions(closedprs);
	}

	charsettoggle.setTitle(charsetpanelopen ? "Character sets  –" : "Character sets  +");

	int row = 0;
	cardsizer.setControl(row++, 0, &topspace);
	cardsizer.setControl(row++, 0, &titlelabel);
	cardsizer.setControl(row++, 0, &subtitlelabel);
	cardsizer.setControl(row++, 0, &inputpanel);
	cardsizer.setControl(row++, 0, &charsettoggle);
	if(charsetpanelopen) {
		cardsizer.setControl(row++, 0, &charsetpanel);
	}
	cardsizer.setControl(row++, 0, &buttonrow);
	cardsizer.setControl(row++, 0, &resulttext);
	cardsizer.setControl(row++, 0, &progresstext);
	cardsizer.setControl(row, 0, &bottomspace);
}

void gCanvas::update() {
	// Read the engine's state every frame without blocking, and mirror it into
	// the result and progress labels.
	testresult r = engine.snapshot();
	if(r.started) {
		if(r.summary != lastresult) {
			resulttext.setText(r.summary);
			lastresult = r.summary;
		}
		std::string prog = gToStr(r.attempts) + " tries  -  " + gToStr((long long)r.guesspersec) + " /sec";
		if(prog != lastprogress) {
			progresstext.setText(prog);
			lastprogress = prog;
		}
	}
}

void gCanvas::onGuiEvent(int guiObjectId, int eventType, std::string value1, std::string value2) {
	if(guiObjectId == charsettoggle.getId() && eventType == G_GUIEVENT_BUTTONRELEASED) {
		charsetpanelopen = !charsetpanelopen;
		rebuildLayout();
		return;
	}
	if(guiObjectId == teststrengthbutton.getId() && eventType == G_GUIEVENT_BUTTONRELEASED) {
		std::string target = passwordbox.getText();
		if(target.empty()) {
			resulttext.setText("Type a password first.");
			return;
		}
		// Test whatever was typed: the alphabet is inferred from the password's
		// own characters, no checkbox selection needed.
		std::string charset = charsetForTarget(target);
		engine.startTest(target, charset, threadCount());
		return;
	}
	if(guiObjectId == generatebutton.getId() && eventType == G_GUIEVENT_BUTTONRELEASED) {
		std::vector<std::string> sets = selectedSets();
		if(sets.empty()) {
			resulttext.setText("Select at least one character set.");
			return;
		}
		// Generate a password from the selected sets, show it, then test it
		// against the same charset.
		std::string password = generator.generate(sets, generatedlength);
		passwordbox.setText(password);
		std::string charset = generator.buildPool(sets);
		engine.startTest(password, charset, threadCount());
		return;
	}
}

std::string gCanvas::charsetForTarget(const std::string& target) {
	bool haslower = false, hasupper = false, hasdigit = false, hassymbol = false;
	for(char c : target) {
		if(c >= 'a' && c <= 'z') {
			haslower = true;
		} else if(c >= 'A' && c <= 'Z') {
			hasupper = true;
		} else if(c >= '0' && c <= '9') {
			hasdigit = true;
		} else {
			hassymbol = true;
		}
	}
	std::vector<std::string> names;
	if(haslower) {
		names.push_back("lower");
	}
	if(hasupper) {
		names.push_back("upper");
	}
	if(hasdigit) {
		names.push_back("digits");
	}
	if(hassymbol) {
		names.push_back("symbols");
	}
	std::string pool = generator.buildPool(names);
	// Make sure every character of the target is reachable, even a symbol that is
	// not in our standard symbol set.
	for(char c : target) {
		if(pool.find(c) == std::string::npos) {
			pool += c;
		}
	}
	return pool;
}

int gCanvas::threadCount() {
	int tc = (int)std::thread::hardware_concurrency();
	return tc < 1 ? 4 : tc;
}

std::vector<std::string> gCanvas::selectedSets() {
	// Each key here must match a key in passwordgenerator's map.
	std::vector<std::string> names;
	if(lowercasebox.isChecked()) {
		names.push_back("lower");
	}
	if(uppercasebox.isChecked()) {
		names.push_back("upper");
	}
	if(digitsbox.isChecked()) {
		names.push_back("digits");
	}
	if(symbolsbox.isChecked()) {
		names.push_back("symbols");
	}
	return names;
}

void gCanvas::windowResized(int w, int h) {
	// Buttons refit themselves in gRoundedButton::set() on relayout; nothing to do.
}

void gCanvas::showNotify() {
}

void gCanvas::hideNotify() {
}
