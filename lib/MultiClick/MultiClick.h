/*
MULTI-CLICK: One Button, Multiple Events

Oct 12, 2009
Run checkButton() to retrieve a button event:
Click
Double-Click
Hold
Long Hold
*/

#ifndef _MULTI_CLICK_H
#define _MULTI_CLICK_H

#include <Arduino.h>

enum McEvent{
	NOTHING, PRESSED, CLICK, DOUBLE_CLICK, RELEASED, HOLD, LONG_HOLD
};

struct MultiClick {

	int buttonPin;

	// Button timing variables
	int debounce = 20; // ms debounce period to prevent flickering when pressing or releasing the button
	int DCgap = 250; // max ms between clicks for a double click event
	int holdTime = 1200; // ms hold period: how long to wait for press+hold event
	int longHoldTime = 5400; // ms long hold period: how long to wait for press+hold event
	
	// Other button variables
	boolean buttonVal = HIGH; // value read from button
	boolean buttonLast = HIGH; // buffered value of the button's previous state
	boolean DCwaiting = false; // whether we're waiting for a double click (down)
	boolean DConUp = false; // whether to register a double click on next release, or whether to wait and click
	boolean singleOK = true; // whether it's OK to do a single click
	long downTime = -1; // time the button was pressed down
	long upTime = -1; // time the button was released
	boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
	boolean waitForUp = false; // when held, whether to wait for the up event
	boolean holdEventPast = false; // whether or not the hold event happened already
	boolean longHoldEventPast = false;// whether or not the long hold event happened already

	MultiClick(int _b_pin): buttonPin(_b_pin){
		pinMode(_b_pin, INPUT_PULLUP);
	}

	McEvent checkButton();

};

#endif