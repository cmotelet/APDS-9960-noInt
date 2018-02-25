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
int8_t * p_tmp_val;

//-----------------------------------------------------------------------------
void print_gesture_config()
{
 Serial.println("--------------------------------");
 Serial.print("a+/q- GGAIN:");
 Serial.println(apds.getGestureGain(),DEC);
 Serial.print("z+/s- GLDRIVE:");
 Serial.println(apds.getGestureLEDDrive(),DEC);
 Serial.print("e+/d- LED_BOOST:");
 Serial.println(apds.getLEDBoost(),DEC);
 Serial.print("r+/f- GWTIME:");
 Serial.println(apds.getGestureWaitTime(),DEC);
 Serial.print("t+/g- GPENTH:");
 Serial.println(apds.getGestureEnterThresh(),DEC);
 Serial.print("y+/h- GEXTH:");
 Serial.println(apds.getGestureExitThresh(),DEC);
 Serial.print("u+/j- GPLEN:");
 Serial.println(apds.getGestureGPLEN(),DEC);
 Serial.print("i+/k- GPULSE:");
 Serial.println(apds.getGestureGPULSE(),DEC);
 Serial.print("7+/4- UpOffset:");
 gesture_param_value = apds.getGestureUpOffset();
 p_tmp_val = (int8_t *) &gesture_param_value;
 Serial.println(*p_tmp_val,DEC);
 Serial.print("8+/5- DownOffset:");
 gesture_param_value = apds.getGestureDownOffset();
 p_tmp_val = (int8_t *) &gesture_param_value;
 Serial.println(*p_tmp_val,DEC);
 Serial.print("9+/6- LeftOffset:");
 gesture_param_value = apds.getGestureLeftOffset();
 p_tmp_val = (int8_t *) &gesture_param_value;
 Serial.println(*p_tmp_val,DEC);
 Serial.print("2+/0- RightOffset:");
 gesture_param_value = apds.getGestureRightOffset();
 p_tmp_val = (int8_t *) &gesture_param_value;
 Serial.println(*p_tmp_val,DEC);
// Serial.print("z+/s- :");
// Serial.println(apds.(),DEC);
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
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}
//-----------------------------------------------------------------------------
void loop()
{
    if ( apds.isGestureAvailable() )
	{
		int16_t gesture = apds.readGesture();

		digitalWrite(LED_BUILTIN, HIGH);
		if ( gesture > 0)
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
		} else {
			if ( gesture == 0 ) Serial.println(" NONE");
			else Serial.println(" ERROR");
		}
		digitalWrite(LED_BUILTIN, LOW);
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
			case 't': // GPENTH +
				gesture_param_value = apds.getGestureEnterThresh();
				if (gesture_param_value < 255) gesture_param_value++;
				if (apds.setGestureEnterThresh(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureEnterThresh: failure !");
				break;
			case 'g': // GPENTH -
				gesture_param_value = apds.getGestureEnterThresh();
				if (gesture_param_value > 0) gesture_param_value--;
				if (apds.setGestureEnterThresh(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureEnterThresh: failure !");
				break;
			case 'y': // GEXTH +
				gesture_param_value = apds.getGestureExitThresh();
				if (gesture_param_value < 255) gesture_param_value++;
				if (apds.setGestureExitThresh(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureExitThresh: failure !");
				break;
			case 'h': // GEXTH -
				gesture_param_value = apds.getGestureExitThresh();
				if (gesture_param_value > 0) gesture_param_value--;
				if (apds.setGestureExitThresh(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureExitThresh: failure !");
				break;
			case 'u': // GPLEN +
				gesture_param_value = apds.getGestureGPLEN();
				if (gesture_param_value < 3) gesture_param_value++;
				if (apds.setGestureGPLEN(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGPLEN: failure !");
				break;
			case 'j': // GPLEN -
				gesture_param_value = apds.getGestureGPLEN();
				if (gesture_param_value > 0) gesture_param_value--;
				if (apds.setGestureGPLEN(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGPLEN: failure !");
				break;
			case 'i': // GPULSE +
				gesture_param_value = apds.getGestureGPULSE();
				if (gesture_param_value < 63) gesture_param_value++;
				if (apds.setGestureGPULSE(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGPULSE: failure !");
				break;
			case 'k': // GPULSE -
				gesture_param_value = apds.getGestureGPULSE();
				if (gesture_param_value > 0) gesture_param_value--;
				if (apds.setGestureGPULSE(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureGPULSE: failure !");
				break;
			case '7': // UpOffset +
				gesture_param_value = apds.getGestureUpOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val < 127) (*p_tmp_val)++;
				if (apds.setGestureUpOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureUpOffset: failure !");
				break;
			case '4': // UpOffset -
				gesture_param_value = apds.getGestureUpOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val > -127) (*p_tmp_val)--;
				if (apds.setGestureUpOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureUpOffset: failure !");
				break;
			case '8': // DownOffset +
				gesture_param_value = apds.getGestureDownOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val < 127) (*p_tmp_val)++;
				if (apds.setGestureDownOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureDownOffset: failure !");
				break;
			case '5': // DownOffset -
				gesture_param_value = apds.getGestureDownOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val > -127) (*p_tmp_val)--;
				if (apds.setGestureDownOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureDownOffset: failure !");
				break;
			case '9': // LeftOffset +
				gesture_param_value = apds.getGestureLeftOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val < 127) (*p_tmp_val)++;
				if (apds.setGestureLeftOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureLeftOffset: failure !");
				break;
			case '6': // LeftOffset -
				gesture_param_value = apds.getGestureLeftOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val > -127) (*p_tmp_val)--;
				if (apds.setGestureLeftOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureLeftOffset: failure !");
				break;
			case '2': // RightOffset +
				gesture_param_value = apds.getGestureRightOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val < 127) (*p_tmp_val)++;
				if (apds.setGestureRightOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureRightOffset: failure !");
				break;
			case '0': // RightOffset -
				gesture_param_value = apds.getGestureRightOffset();
				p_tmp_val = (int8_t *)&gesture_param_value;
				if (*p_tmp_val > -127) (*p_tmp_val)--;
				if (apds.setGestureRightOffset(gesture_param_value))
				{
					print_gesture_config();
				} else Serial.println("setGestureRightOffset: failure !");
				break;
			case '?':
				print_gesture_config();
				break;
				
		}
	}
	delay(10);
}
