/****************************************************************
ColorSensor.ino

Tests the color and ambient light sensing abilities of the 
APDS-9960. Configures APDS-9960 over I2C and polls the sensor for
ambient light and color levels, which are displayed over the 
serial console.

Distributed as-is; no warranty is given.
****************************************************************/

#include <APDS9960.h>

// Global Variables
APDS9960 apds;

uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

//-----------------------------------------------------------------------------
void setup()
{
  // Initialize Serial port
  Serial.begin(115200);
  while( !Serial); delay(100);
  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("   APDS-9960 - ColorSensor      ");
  Serial.println("--------------------------------");
  Serial.println();
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  
  // Wait for initialization and calibration to finish
  delay(500);
}
//-----------------------------------------------------------------------------
void loop()
{
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
    Serial.print("Ambient: ");
    Serial.print(ambient_light);
    Serial.print(" Red: ");
    Serial.print(red_light);
    Serial.print(" Green: ");
    Serial.print(green_light);
    Serial.print(" Blue: ");
    Serial.println(blue_light);
  }
  
  // Wait 1 second before next reading
  delay(1000);
}