/****************************************************************

Tests the gesture sensing abilities of the APDS-9960.
With tunning parameters
IMPORTANT: The APDS-9960 can only accept 3.3V!
 
****************************************************************/

#include <APDS9960.h>

// Global Variables
APDS9960 apds;
int incomingByte = 0;   // for incoming serial data
uint8_t gesture_param_value;
//-----------------------------------------------------------------------------
void blink()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//-----------------------------------------------------------------------------
void print_gesture_config()
{
 Serial.println("--------------------------------");
 Serial.printl("a+/q- GGAIN:");
 Serial.println(apds.getGestureGain(),HEX);
 Serial.printl("z+/s- GLDRIVE:");
 Serial.println(apds.getGestureLEDDrive(),HEX);
 Serial.printl("e+/d- LED_BOOST:");
 Serial.println(apds.getLEDBoost(),HEX);
// Serial.printl("z+/s- :");
// Serial.println(apds.(),HEX);
 Serial.println("--------------------------------");
 	
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
	
	if (Serial.available() > 0) {
		incomingByte = Serial.read();
		switch (incomingByte)
		{
			case 'a': // GGAIN +
				gesture_param_value = apds.getGestureGain();
				switch (gesture_param_value)
				{
					case GGAIN_1X:
						gesture_param_value = GGAIN_2X;
						break;
					case GGAIN_2X:
						gesture_param_value = GGAIN_4X;
						break;
					case GGAIN_4X:
						gesture_param_value = GGAIN_8X;
						break;
				}
				if (apds.setGestureGain(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGain: failure !");
				break;
			case 'q': // GGAIN -
				gesture_param_value = apds.getGestureGain();
				switch (gesture_param_value)
				{
					case GGAIN_8X:
						gesture_param_value = GGAIN_4X;
						break;
					case GGAIN_4X:
						gesture_param_value = GGAIN_2X;
						break;
					case GGAIN_2X:
						gesture_param_value = GGAIN_1X;
						break;
				}
				if (apds.setGestureGain(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGain: failure !");
				break;
			case 'z': // GLDRIVE +
				gesture_param_value = apds.getGestureLEDDrive();
				switch (gesture_param_value)
				{
					case LED_DRIVE_12_5MA:
						gesture_param_value = LED_DRIVE_25MA;
						break;
					case LED_DRIVE_25MA:
						gesture_param_value = LED_DRIVE_50MA;
						break;
					case LED_DRIVE_50MA:
						gesture_param_value = LED_DRIVE_100MA;
						break;
				}
				if (apds.setGestureLEDDrive(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureLEDDrive: failure !");
				break;
			case 's': // GLDRIVE -
				gesture_param_value = apds.getGestureLEDDrive();
				switch (gesture_param_value)
				{
					case LED_DRIVE_100MA:
						gesture_param_value = LED_DRIVE_50MA;
						break;
					case LED_DRIVE_50MA:
						gesture_param_value = LED_DRIVE_25MA;
						break;
					case LED_DRIVE_25MA:
						gesture_param_value = LED_DRIVE_12_5MA;
						break;
				}
				if (apds.setGestureLEDDrive(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureLEDDrive: failure !");
				break;
			case 'e': // LED_BOOST +
				gesture_param_value = apds.getLEDBoost();
				switch (gesture_param_value)
				{
					case LED_BOOST_100:
						gesture_param_value = LED_BOOST_150;
						break;
					case LED_BOOST_150:
						gesture_param_value = LED_BOOST_200;
						break;
					case LED_BOOST_200:
						gesture_param_value = LED_BOOST_300;
						break;
				}
				if (apds.setLEDBoost(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setLEDBoost: failure !");
				break;
			case 'd': // LED_BOOST -
				gesture_param_value = apds.getLEDBoost();
				switch (gesture_param_value)
				{
					case LED_BOOST_300:
						gesture_param_value = LED_BOOST_200;
						break;
					case LED_BOOST_200:
						gesture_param_value = LED_BOOST_150;
						break;
					case LED_BOOST_150:
						gesture_param_value = LED_BOOST_100;
						break;
				}
				if (apds.setLEDBoost(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setLEDBoost: failure !");
				break;
/*				
			case 'r': // GWTIME +
				gesture_param_value = apds.getGestureWaitTime();
				switch (gesture_param_value)
				{
					case GWTIME_0MS:
						gesture_param_value = GWTIME_2_8MS;
						break;
					case GWTIME_2_8MS:
						gesture_param_value = GWTIME_5_6MS;
						break;
					case GWTIME_5_6MS:
						gesture_param_value = GWTIME_8_4MS;
						break;
					case GWTIME_8_4MS:
						gesture_param_value = GWTIME_14_0MS;
						break;
					case GWTIME_14_0MS:
						gesture_param_value = GWTIME_22_4MS;
						break;
					case GWTIME_22_4MS:
						gesture_param_value = GWTIME_30_8MS;
						break;
					case GWTIME_30_8MS:
						gesture_param_value = GWTIME_39_2MS;
						break;
				}
				if (apds.setGestureWaitTime(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureWaitTime: failure !");
				break;
			case 'f': // GWTIME -
				gesture_param_value = apds.getGestureWaitTime();
				switch (gesture_param_value)
				{
					case GWTIME_2_8MS:
						gesture_param_value = GWTIME_0MS;
						break;
					case GWTIME_5_6MS:
						gesture_param_value = GWTIME_2_8MS;
						break;
					case GWTIME_8_4MS:
						gesture_param_value = GWTIME_5_6MS;
						break;
					case GWTIME_14_0MS:
						gesture_param_value = GWTIME_8_4MS;
						break;
					case GWTIME_22_4MS:
						gesture_param_value = GWTIME_14_0MS;
						break;
					case GWTIME_30_8MS:
						gesture_param_value = GWTIME_22_4MS;
						break;
					case GWTIME_39_2MS:
						gesture_param_value = GWTIME_30_8MS;
						break;
				}
				if (apds.setGestureWaitTime(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureWaitTime: failure !");
				break;
*/
			case '?':
				print_gesture_config();
				break;
				
		}
	}
	delay(10);
}
