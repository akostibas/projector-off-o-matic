/*
 * I need to figure out what IR pattern the remote is sending when the Power button is pressed.
 * This will print each state change, along with how long we linger in that state.
 */

long sicCount = 0;
int sicPreviousState = 0;

void seeIrCodes() {
  int irState = digitalRead(IR_INPUT_PIN);

  if (irState != sicPreviousState) {
    if (sicCount > 5) {
      Serial.print(irState);
      Serial.print(" - ticks: ");
      Serial.println(sicCount);
      sicCount = 0;
    }
  }
  
  sicCount++;

  sicPreviousState = irState;
}