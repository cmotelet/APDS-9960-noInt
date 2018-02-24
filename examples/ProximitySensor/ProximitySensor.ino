/****************************************************************
ProximityTest.ino

Tests the proximity sensing abilities of the APDS-9960.
Configures the APDS-9960 over I2C and polls for the distance to
the object nearest the sensor.

****************************************************************/

#include <APDS9960.h>

// Global Variables
APDS9960 apds;

uint8_t proximity_data = 0;

//-----------------------------------------------------------------------------
void setup()
{
  // Initialize Serial port
  Serial.begin(9600);
  while( !Serial); delay(100);
  Serial.println();
  Serial.println(F("------------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ProximitySensor"));
  Serial.println(F("------------------------------------"));

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete.\n"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!\n"));
  }

  Serial.print(F("Detected ID: 0x")); Serial.println(apds.getID(), HEX);

  // Adjust the Proximity sensor gain
  if ( !apds.setProximityGain(PGAIN_2X) ) {
    Serial.println(F("Something went wrong trying to set PGAIN"));
  }

  // Start running the APDS-9960 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
    Serial.println(F("Proximity sensor is now running...\n"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }
}
//-----------------------------------------------------------------------------
void loop()
{
  // Read the proximity value
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println("Error reading proximity value");
  } else {
    Serial.print("Proximity: ");
    Serial.println(proximity_data);
  }

  // Wait before next reading
  delay(100);
}