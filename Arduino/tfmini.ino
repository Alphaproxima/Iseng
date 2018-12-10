#include <SoftwareSerial.h>
#include "TFMini.h"

SoftwareSerial Serial1(10, 11); //RX TX arduino uno
TFMini tfmini;

void setup()
{
  // Initialized Serial Baud rate
  Serial.begin(115200);
  while(!Serial)
  {
    Serial.println("Initialization...");
  }

  // Initialize Serial1 Baud rate
  Serial1.begin(TFMINI_BAUDRATE);

  // 
  tfmini.begin(&Serial1);
}

void loop()
{
  uint16_t distance = tfmini.getDistance();
  uint16_t strength = tfmini.getRecentSignalStrength();

  Serial.println(distance);

  delay(25);
}
