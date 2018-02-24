/****************************************************************

Tests the gesture sensing abilities of the APDS-9960.

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
****************************************************************/

#include <APDS9960.h>

// Global Variables
APDS9960 apds;

//-----------------------------------------------------------------------------
void blink()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//-----------------------------------------------------------------------------
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn off
  // Initialize Serial port
  Serial.begin(115200);
  while( !Serial); delay(100);
  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("  APDS9960 - GestureTest demo   ");
  Serial.println("--------------------------------");
  Serial.println();
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() )
  {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }

  Serial.print(F("Detected ID: 0x")); Serial.println(apds.getID(), HEX);

  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}
//-----------------------------------------------------------------------------
void loop()
{
    if ( apds.isGestureAvailable() )
	{
		uint8_t gesture = apds.readGesture();

		if ( gesture )
		{
#if DEBUG
			Serial.println("********************************");
#endif
			Serial.print("Detected gesture:");

			if ( gesture&FLAG_UP )          Serial.print(" UP");
			else if ( gesture&FLAG_DOWN )   Serial.print(" DOWN");

			if ( gesture&FLAG_LEFT )        Serial.print(" LEFT");
			else if ( gesture&FLAG_RIGHT )  Serial.print(" RIGHT");

			if ( gesture&FLAG_NEAR )        Serial.print(" NEAR");
			else if ( gesture&FLAG_FAR )    Serial.print(" FAR");

			if ( gesture&FLAG_APPROACH )    Serial.print(" APPROACHING");
			else if ( gesture&FLAG_DEPART ) Serial.print(" DEPARTING");

			Serial.println();
#if DEBUG
			Serial.println("********************************");
#endif
		}

		blink(); delay(100); blink(); delay(100);
	}
	delay(10);
}
