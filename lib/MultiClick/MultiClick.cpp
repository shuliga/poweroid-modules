#include "MultiClick.h"
#include <Arduino.h>

McEvent MultiClick::checkButton() {
    McEvent event = NOTHING;
// Read the state of the button
    buttonVal = digitalRead(buttonPin);
// Button pressed down
    if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce) {
        downTime = millis();
        ignoreUp = false;
        waitForUp = false;
        singleOK = true;
        holdEventPast = false;
        longHoldEventPast = false;
        DConUp = (millis() - upTime) < DCgap && DConUp == false && DCwaiting == true;
        DCwaiting = false;
        event = PRESSED;
    }
// Button released
    else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce) {
        event = RELEASED;
        if (not ignoreUp) {
            upTime = millis();
            if (DConUp == false) DCwaiting = true;
            else {
                event = DOUBLE_CLICK;
                DConUp = false;
                DCwaiting = false;
                singleOK = false;
            }
        }
    }
// Test for normal click event: DCgap expired
    if (buttonVal == HIGH && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true) {
        event = CLICK;
        DCwaiting = false;
    }
// Test for hold
    if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
// Trigger "normal" hold
        if (not holdEventPast) {
            event = HOLD;
            waitForUp = true;
            ignoreUp = true;
            DConUp = false;
            DCwaiting = false;
//downTime = millis();
            holdEventPast = true;
        }
// Trigger "long" hold
        if ((millis() - downTime) >= longHoldTime) {
            if (not longHoldEventPast) {
                event = LONG_HOLD;
                longHoldEventPast = true;
            }
        }
    }
    buttonLast = buttonVal;
    return event;
}