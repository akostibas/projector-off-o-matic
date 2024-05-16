/*
 * Based on looking a few presses of the power button, it seems like we can
 * key off of a pattern like this:
 *  1. HIGH:  7 - 10ms
 *  2.  LOW:  8 - 11ms
 *  3. HIGH:  5 -  7ms
 *  4.  LOW: 40 - 50ms
 *  5. HIGH:  7 - 10ms
 * 
 * There are a lot of other state changes between these, so we need to ignore
 * those.
 * 
 * The total pattern seems to take less than 100ms, but we should give a bit of
 * a buffer.
 */

long dpoTicks = 0;
int dpoPreviousIRInput = 0;

// We'll track our state in this variable as we try to find the 5 patterns
// above
int dpoState = 0;

bool detectPowerOff() {
  int irInput = digitalRead(IR_INPUT_PIN);

  // On state change, we want to see how many dpoTicks we were in the previous
  // state.
  if (irInput != dpoPreviousIRInput) {
    if (dpoState == 0 && irInput == 1 && dpoTicks >= 7 && dpoTicks <= 10) {
      dpoState = 1;
      debug("State 1");
    } else if (dpoState == 1 && irInput == 0 && dpoTicks >= 4 && dpoTicks <= 6) {
      dpoState = 2;
      debug("State 2");
    } else if (dpoState == 2 && irInput == 0 && dpoTicks >= 4 && dpoTicks <= 7) {
      dpoState = 3;
      debug("State 3");
    } else if (dpoState == 3 && irInput == 0 && dpoTicks >= 35 && dpoTicks <= 50) {
      dpoState = 4;
      debug("State 4");
    } else if (dpoState == 4 && irInput == 1 && dpoTicks >= 7 && dpoTicks <= 10) {
      debug("State 5");
      dpoTicks = 0;
      dpoState = 0;
      return true;
    }

    dpoTicks = 0;
  }

  // Reset state after 150ms; the pattern doesn't take this long
  if (dpoTicks > 150 && dpoState != 0) {
    dpoState = 0;
    debug("Resetting state");

    blinkDotStar(0xFF00FF, 250);
  }

  dpoPreviousIRInput = irInput;
  dpoTicks++;

  return false;
}