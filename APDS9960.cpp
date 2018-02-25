/**
 * APDS-9960.h
 *
 * This library interfaces the Avago APDS9960 over I2C.
 *
 * Inspired from https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor/tree/master/Libraries
 */

#include <Arduino.h>
#include <Wire.h>
#include "APDS9960.h"

// Setup of HW registers
bool APDS9960::init()
{
    // Initialize I2C
    Wire.begin();

    // disable all features
    if( !setMode(ALL, OFF) ) {
        return false;
    }

    // Set default values for ambient light and proximity registers
    if( !wireWriteDataByte(APDS9960_ATIME, DEFAULT_ATIME) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_WTIME, DEFAULT_WTIME) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_PPULSE, DEFAULT_PROX_PPULSE) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG1, DEFAULT_CONFIG1) ) {
        return false;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return false;
    }
    if( !setProxIntLowThresh(DEFAULT_PILT) ) {
        return false;
    }
    if( !setProxIntHighThresh(DEFAULT_PIHT) ) {
        return false;
    }
    if( !setLightIntLowThreshold(DEFAULT_AILT) ) {
        return false;
    }
    if( !setLightIntHighThreshold(DEFAULT_AIHT) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_PERS, DEFAULT_PERS) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG2, DEFAULT_CONFIG2) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG3, DEFAULT_CONFIG3) ) {
        return false;
    }

    // Set default values for gesture sense registers
    if( !setGestureEnterThresh(DEFAULT_GPENTH) ) {
        return false;
    }
    if( !setGestureExitThresh(DEFAULT_GEXTH) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GCONF1, DEFAULT_GCONF1) ) {
        return false;
    }
    if( !setGestureGain(DEFAULT_GGAIN) ) {
        return false;
    }
    if( !setGestureLEDDrive(DEFAULT_GLDRIVE) ) {
        return false;
    }
    if( !setGestureWaitTime(DEFAULT_GWTIME) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_U, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_D, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_L, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_R, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GCONF3, DEFAULT_GCONF3) ) {
        return false;
    }
    if( !setGestureIntEnable(DEFAULT_GIEN) ) {
        return false;
    }

	resetGestureParameters();
    return true;
}

uint8_t APDS9960::getID()
{
    uint8_t id = 0;
    // Read ID register
    wireReadDataByte(APDS9960_ID, id);
    return id;
}

/*******************************************************************************
 * Public methods for controlling the APDS-9960
 ******************************************************************************/

/**
 * @brief Reads and returns the contents of the ENABLE register
 *
 * @return Contents of the ENABLE register. 0xFF if error.
 */
uint8_t APDS9960::getMode()
{
    uint8_t enable_value;

    /* Read current ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, enable_value) ) {
        return ERROR;
    }

    return enable_value;
}

/**
 * @brief Enables or disables a feature in the APDS-9960
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 * @return True if operation success. False otherwise.
 */
bool APDS9960::setMode(uint8_t mode, uint8_t enable)
{
    /* Read current ENABLE register */
    uint8_t reg_val = getMode();
    if( reg_val == ERROR ) {
        return false;
    }

    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( mode >= 0 && mode <= 6 ) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }

    /* Write value back to ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, reg_val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Starts the light (R/G/B/Ambient) sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware interrupt on high or low light
 * @return True if sensor enabled correctly. False on error.
 */
bool APDS9960::enableLightSensor(bool interrupts)
{
    /* Set default gain, interrupts, enable power, and enable sensor */
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return false;
    }
    if( interrupts ) {
        if( !setAmbientLightIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setAmbientLightIntEnable(0) ) {
            return false;
        }
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(AMBIENT_LIGHT, 1) ) {
        return false;
    }

    return true;
}

/**
 * @brief Ends the light sensor on the APDS-9960
 *
 * @return True if sensor disabled correctly. False on error.
 */
bool APDS9960::disableLightSensor()
{
    if( !setAmbientLightIntEnable(0) ) {
        return false;
    }
    if( !setMode(AMBIENT_LIGHT, 0) ) {
        return false;
    }

    return true;
}

/**
 * @brief Starts the proximity sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on proximity
 * @return True if sensor enabled correctly. False on error.
 */
bool APDS9960::enableProximitySensor(bool interrupts)
{
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( interrupts ) {
        if( !setProximityIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setProximityIntEnable(0) ) {
            return false;
        }
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }

    return true;
}

/**
 * @brief Ends the proximity sensor on the APDS-9960
 *
 * @return True if sensor disabled correctly. False on error.
 */
bool APDS9960::disableProximitySensor()
{
	if( !setProximityIntEnable(0) ) {
		return false;
	}
	if( !setMode(PROXIMITY, 0) ) {
		return false;
	}

	return true;
}

/**
 * @brief Starts the gesture recognition engine on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on gesture
 * @return True if engine enabled correctly. False on error.
 */
bool APDS9960::enableGestureSensor(bool interrupts)
{
    /* Enable gesture mode
       Set ENABLE to 0 (power off)
       Set WTIME to 0xFF ???
	   Set APDS9960_GCONF2 to DEFAULT_GWTIME|DEFAULT_GLDRIVE|DEFAULT_GGAIN
	   Set APDS9960_GPULSE to DEFAULT_GPULSE
       Set AUX to DEFAULT_GLED_BOOST
       Enable PON, WEN, PEN, GEN in ENABLE 
    */
    resetGestureParameters();
//    if( !wireWriteDataByte(APDS9960_WTIME, 0xFF) ) {
//        return false;
//    }
    if( !wireWriteDataByte(APDS9960_GCONF2, DEFAULT_GWTIME|DEFAULT_GLDRIVE|DEFAULT_GGAIN) ) {
        return false;
    }
    if( !wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE) ) {
        return false;
    }
    if( !setLEDBoost(DEFAULT_GLED_BOOST) ) {
        return false;
    }
    if( interrupts ) {
        if( !setGestureIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setGestureIntEnable(0) ) {
            return false;
        }
    }
    if( !setGestureMode(1) ) {
        return false;
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(WAIT, 1) ) {
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }
    if( !setMode(GESTURE, 1) ) {
        return false;
    }

    return true;
}

/**
 * @brief Ends the gesture recognition engine on the APDS-9960
 *
 * @return True if engine disabled correctly. False on error.
 */
bool APDS9960::disableGestureSensor()
{
    resetGestureParameters();
    if( !setGestureIntEnable(0) ) {
        return false;
    }
    if( !setGestureMode(0) ) {
        return false;
    }
    if( !setMode(GESTURE, 0) ) {
        return false;
    }

    return true;
}

/**
 * @brief Determines if there is a gesture available for reading
 *
 * @return True if gesture available. False otherwise.
 */
bool APDS9960::isGestureAvailable()
{
    uint8_t val;

    // Read value from GSTATUS register
    if( !wireReadDataByte(APDS9960_GSTATUS, val) ) {
        return ERROR;
    }

    // Return true/false based on GVALID bit
    if( val & APDS9960_GVALID) {
        return true;
    } else {
        return false;
    }
}

gesture_record_t fifo_buf[32];
#if DEBUG
gesture_record_t rec_data[MAX_RECORDS];
#endif
/**
 * @brief Processes a gesture event and returns best guessed gesture
 *
 * @return Number corresponding to gesture. -1 on error.
 */
int16_t APDS9960::readGesture()
{
    /* Make sure that power and gesture is on and data is valid */
    if( !isGestureAvailable() || !(getMode() & 0b01000001) ) {
        return FLAG_NONE;
    }

    // Keep looping as long as gesture data is valid
    while(1)
	{
		uint8_t gstatus;
        /* Get the contents of the STATUS register. Is data still valid? */
        if( !wireReadDataByte(APDS9960_GSTATUS, gstatus) ) {
            return FLAG_ERROR;
        }

        // If we have valid data, read in FIFO
        if( (gstatus & APDS9960_GVALID) )
		{
            // Read the current FIFO level
			uint8_t fifo_level;
            if( !wireReadDataByte(APDS9960_GFLVL, fifo_level) ) {
                return FLAG_ERROR;
            }
#if DEBUG
            Serial.print("> FIFO Level: "); Serial.println(fifo_level);
#endif

            if ( fifo_level==0 ) continue; // no data read and to process

			if ( fifo_level>8 ) fifo_level = 8; // limit to 32 records
			
            /* Read the FIFO into our data buffer */
			int bytes_read = wireReadDataBlock( APDS9960_GFIFO_U,
												(uint8_t*)fifo_buf,
												(fifo_level * 4));
#if DEBUG
            Serial.print("Bytes read: "); Serial.println(bytes_read);
#endif
			if ( bytes_read<0 )	return FLAG_ERROR; // something went wrong

            if ( bytes_read<4 ) continue; // not enough data to process

			// check if already too many data processed
			if ( gesture_data_.total_records>=MAX_RECORDS )
			{
#if DEBUG
				Serial.println("<<< MAX_RECORDS >>>");
#endif
				break;
			}

			gesture_data_.current_records = bytes_read/4;
			// limit the data to available buffer lenght
			if ( gesture_data_.current_records>(MAX_RECORDS-gesture_data_.total_records) )
				gesture_data_.current_records = (MAX_RECORDS-gesture_data_.total_records);

#if DEBUG
			Serial.print("FIFO Dump:");
			for (uint8_t i = 0; i < bytes_read; i++ )
			{
				Serial.print(" ");
				Serial.print(((uint8_t*)fifo_buf)[i]);
				if ( (i&3)==3 ) Serial.write(',');
			}
			Serial.write('\n');
			// copy fifo buffer to record
			
			for (uint8_t i=0; i<gesture_data_.current_records; i++)
			{
				uint8_t j = gesture_data_.total_records+i;
				rec_data[j].u_data = fifo_buf[i].u_data;
				rec_data[j].d_data = fifo_buf[i].d_data;
				rec_data[j].l_data = fifo_buf[i].l_data;
				rec_data[j].r_data = fifo_buf[i].r_data;
			}
#endif
			gesture_data_.total_records += gesture_data_.current_records;
			// Process gesture data.
			processGestureData();
        }
		else break;
        // Wait some time to collect next batch of FIFO data
        delay(FIFO_PAUSE_TIME);
	}
	// Determine best guessed gesture and clean up
	decodeGesture();
	int16_t motion = gesture_motion_;
	resetGestureParameters();
	return motion;
}

/**
 * Turn the APDS-9960 on
 *
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::enablePower()
{
    if( !setMode(POWER, 1) ) {
        return false;
    }

    return true;
}

/**
 * Turn the APDS-9960 off
 *
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::disablePower()
{
    if( !setMode(POWER, 0) ) {
        return false;
    }

    return true;
}

/*******************************************************************************
 * Ambient light and color sensor controls
 ******************************************************************************/

/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::readAmbientLight(uint16_t &val)
{
    uint8_t val_byte;
    val = 0;

    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_CDATAL, val_byte) ) {
        return false;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_CDATAH, val_byte) ) {
        return false;
    }
    val = val + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::readRedLight(uint16_t &val)
{
    uint8_t val_byte;
    val = 0;

    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_RDATAL, val_byte) ) {
        return false;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_RDATAH, val_byte) ) {
        return false;
    }
    val = val + ((uint16_t)val_byte << 8);

    return true;
}
 
/**
 * @brief Reads the green light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::readGreenLight(uint16_t &val)
{
    uint8_t val_byte;
    val = 0;

    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_GDATAL, val_byte) ) {
        return false;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_GDATAH, val_byte) ) {
        return false;
    }
    val = val + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::readBlueLight(uint16_t &val)
{
    uint8_t val_byte;
    val = 0;

    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_BDATAL, val_byte) ) {
        return false;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_BDATAH, val_byte) ) {
        return false;
    }
    val = val + ((uint16_t)val_byte << 8);

    return true;
}

/*******************************************************************************
 * Proximity sensor controls
 ******************************************************************************/

/**
 * @brief Reads the proximity level as an 8-bit value
 *
 * @param[out] val value of the proximity sensor.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::readProximity(uint8_t &val)
{
    val = 0;

    /* Read value from proximity data register */
    if( !wireReadDataByte(APDS9960_PDATA, val) ) {
        return false;
    }

    return true;
}

/*******************************************************************************
 * High-level gesture controls
 ******************************************************************************/

/**
 * @brief Resets all the parameters in the gesture data member
 */
void APDS9960::resetGestureParameters()
{
    gesture_data_.delta_ud = 0;
    gesture_data_.delta_lr = 0;
    gesture_data_.sum_udlr  = 0;
    gesture_data_.prev_udlr  = 0;
    gesture_data_.delta_udlr = 0;
//    gesture_data_.delta_ud_var = 0;
//    gesture_data_.delta_lr_var = 0;
    gesture_data_.dir_up    = 0;
    gesture_data_.dir_down  = 0;
    gesture_data_.dir_left  = 0;
    gesture_data_.dir_right = 0;
    gesture_data_.current_records = 0;
    gesture_data_.total_records = 0;

//    gesture_near_count_ = 0;
//    gesture_far_count_ = 0;

//    gesture_state_ = 0;
    gesture_motion_ = FLAG_NONE;
}

/**
 * @brief Processes the raw gesture data to determine swipe direction
 *
 * @return True if near or far state seen. False otherwise.
 */
bool APDS9960::processGestureData()
{
    /* Check to make sure our data isn't out of bounds */
	// the cummulated value should be limited in order to avoid overflow
	// collect the UD and LR delta from all records over time

	for(uint8_t i = 0; i < gesture_data_.current_records; i++ )
	{
		int16_t delta_ud = (fifo_buf[i].u_data - fifo_buf[i].d_data);
		//gesture_data_.delta_ud_var += abs(gesture_data_.delta_ud - delta_ud);
		gesture_data_.delta_ud = delta_ud;
		if ( delta_ud>=DELTA_MIN )
		{
			gesture_data_.dir_up += delta_ud;
			if ( !(gesture_motion_&(FLAG_UP|FLAG_DOWN)) &&
				gesture_data_.dir_up>THRESHOLD_MIN && gesture_data_.dir_down>THRESHOLD_MIN )
			{
				gesture_motion_ |= FLAG_DOWN;
#if DEBUG
				Serial.print("FLAG_DOWN Set at count:");Serial.println((gesture_data_.total_records -gesture_data_.current_records +i),DEC);
#endif
			}
		}
		else if ( delta_ud<=(-DELTA_MIN) )
		{
			gesture_data_.dir_down += -delta_ud;
			if ( !(gesture_motion_&(FLAG_UP|FLAG_DOWN)) &&
				gesture_data_.dir_up>THRESHOLD_MIN && gesture_data_.dir_down>THRESHOLD_MIN )
			{
				gesture_motion_ |= FLAG_UP;
#if DEBUG
				Serial.print("FLAG_UP Set at count:");Serial.println((gesture_data_.total_records -gesture_data_.current_records +i),DEC);
#endif
			}
		}

		int16_t delta_lr = (fifo_buf[i].l_data - fifo_buf[i].r_data);
		//gesture_data_.delta_lr_var += abs(gesture_data_.delta_lr - delta_lr);
		gesture_data_.delta_lr = delta_lr;
		if ( delta_lr>=DELTA_MIN )
		{
			gesture_data_.dir_left += delta_lr;
			if ( !(gesture_motion_&(FLAG_LEFT|FLAG_RIGHT)) && 
				gesture_data_.dir_left>THRESHOLD_MIN && gesture_data_.dir_right>THRESHOLD_MIN )
			{
				gesture_motion_ |= FLAG_RIGHT;
#if DEBUG
				Serial.print("FLAG_RIGHT Set at count:");Serial.println((gesture_data_.total_records -gesture_data_.current_records +i),DEC);
#endif
			}
		}
		else if ( delta_lr<(-DELTA_MIN) )
		{
			gesture_data_.dir_right += -delta_lr;
			if ( !(gesture_motion_&(FLAG_LEFT|FLAG_RIGHT)) && 
				gesture_data_.dir_left>THRESHOLD_MIN && gesture_data_.dir_right>THRESHOLD_MIN )
			{
				gesture_motion_ |= FLAG_LEFT;
#if DEBUG
				Serial.print("FLAG_LEFT Set at count:");Serial.println((gesture_data_.total_records -gesture_data_.current_records +i),DEC);
#endif
			}
		}
		// collect NEAR/FAR data
		// discard very first sample
		if ( i==0 && gesture_data_.total_records==gesture_data_.current_records ) continue;
		// current global value
		uint16_t crt_udlr = (fifo_buf[i].u_data + fifo_buf[i].d_data + fifo_buf[i].l_data + fifo_buf[i].r_data)/4;
		if ( gesture_data_.sum_udlr==0 ) // first value
			gesture_data_.delta_udlr = 0;
		else
			gesture_data_.delta_udlr += gesture_data_.prev_udlr - crt_udlr;

		gesture_data_.sum_udlr += crt_udlr;
		gesture_data_.prev_udlr = crt_udlr;
	}

#if DEBUG
    Serial.print("Total records: "); Serial.println(gesture_data_.total_records);
    Serial.print("Diff counts: U: "); Serial.print(gesture_data_.dir_up);
    Serial.print(", D: "); Serial.print(gesture_data_.dir_down);
    Serial.print(", L: "); Serial.print(gesture_data_.dir_left);
    Serial.print(", R: "); Serial.println(gesture_data_.dir_right);
	Serial.print("Cumulative count: "); Serial.print(gesture_data_.sum_udlr);
	Serial.print(", delta: "); Serial.println(gesture_data_.delta_udlr);
//	Serial.print("Deltas: UD: "); Serial.print(gesture_data_.delta_ud);
//	Serial.print(", LR: "); Serial.println(gesture_data_.delta_lr);
//  Serial.print("Deltas vars: UD: "); Serial.print(gesture_data_.delta_ud_var);
//  Serial.print(", LR: "); Serial.println(gesture_data_.delta_lr_var);
//	Serial.print("Sign changes UD: "); Serial.print(gesture_data_.sign_change_ud);
//	Serial.print(", LR: "); Serial.println(gesture_data_.sign_change_lr);
#endif

    return false;
}

/**
 * @brief Determines swipe direction or near/far state
 *
 * @return True if any event detected. False otherwise.
 */
void APDS9960::decodeGesture()
{
    // Determine NEAR/FAR position
	if ( !gesture_motion_&(FLAG_UP|FLAG_DOWN|FLAG_RIGHT|FLAG_LEFT) &&
		gesture_data_.total_records>(MAX_RECORDS/2) ) // no horizontal motion detected
	{
		uint32_t global_count = gesture_data_.sum_udlr/(gesture_data_.total_records-1);///(4*MAX_RECORDS);
		if ( global_count < 180 )
			gesture_motion_ |= FLAG_FAR;
		else
			gesture_motion_ |= FLAG_NEAR;

		if ( gesture_data_.delta_udlr < -80 )
			gesture_motion_ |= FLAG_APPROACH;
		else if ( gesture_data_.delta_udlr > 80 )
			gesture_motion_ |= FLAG_DEPART;
	}

#if DEBUG
    Serial.println("-----------------------------------------------------------------");
	Serial.print("Motion: 0x"); Serial.print(gesture_motion_, HEX);
	Serial.print(" >>>>>");
	if ( (gesture_motion_&FLAG_UP) ) Serial.print(" UP");
	else if ( (gesture_motion_&FLAG_DOWN) ) Serial.print(" DOWN");
	else Serial.print(" ---");

	if ( gesture_motion_&FLAG_LEFT ) Serial.print(" LEFT");
	else if ( gesture_motion_&FLAG_RIGHT ) Serial.print(" RIGHT");
	else Serial.print(" ---");

	if ( gesture_motion_&FLAG_DEPART ) Serial.print(" DEPARTING");
	else if ( gesture_motion_&FLAG_APPROACH ) Serial.print(" APPROACHING");
	else Serial.print(" ---");

	if ( gesture_motion_&FLAG_FAR ) Serial.print(" FAR");
	else if ( gesture_motion_&FLAG_NEAR ) Serial.print(" NEAR");
	else Serial.print(" ---");
	Serial.write('\n'); // NL
	// print the records
	for (int16_t i=250; i>=0; i-=10)
	{
		if (i<100) Serial.write(' ');
		if (i<10) Serial.write(' ');
		Serial.print(i);
		Serial.write(':');
		for (uint8_t j = 0; j<gesture_data_.total_records; j++)
		{
			uint8_t marker = 0;
			if ( i<=rec_data[j].u_data && (i+10)>rec_data[j].u_data) marker |= FLAG_UP;
			if ( i<=rec_data[j].d_data && (i+10)>rec_data[j].d_data) marker |= FLAG_DOWN;
			if ( i<=rec_data[j].l_data && (i+10)>rec_data[j].l_data) marker |= FLAG_LEFT;
			if ( i<=rec_data[j].r_data && (i+10)>rec_data[j].r_data) marker |= FLAG_RIGHT;
			if (marker&FLAG_UP) Serial.write('u');
			else Serial.write(' ');
			if (marker&FLAG_DOWN) Serial.write('d');
			else Serial.write(' ');
			if (marker&FLAG_LEFT) Serial.write('l');
			else Serial.write(' ');
			if (marker&FLAG_RIGHT) Serial.write('r');
			else Serial.write(' ');
		}
		Serial.write('\n');
	}
	Serial.print("----");
	for (uint8_t i=0; i<gesture_data_.total_records; i++)
	{
		Serial.print(i,DEC);
		if (i<10) Serial.print(' ');
		Serial.print("  ");
	}
	Serial.println();
#endif
}

/*******************************************************************************
 * Getters and setters for register values
 ******************************************************************************/

/**
 * @brief Returns the lower threshold for proximity detection
 *
 * @return lower threshold
 */
uint8_t APDS9960::getProxIntLowThresh()
{
    uint8_t val;

    /* Read value from PILT register */
    if( !wireReadDataByte(APDS9960_PILT, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProxIntLowThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_PILT, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Returns the high threshold for proximity detection
 *
 * @return high threshold
 */
uint8_t APDS9960::getProxIntHighThresh()
{
    uint8_t val;

    /* Read value from PIHT register */
    if( !wireReadDataByte(APDS9960_PIHT, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProxIntHighThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_PIHT, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Returns LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the value of the LED drive strength. 0xFF on failure.
 */
uint8_t APDS9960::getLEDDrive()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return ERROR;
    }

    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value (0-3) for the LED drive strength
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 6;
    val &= 0b00111111;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Returns receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the value of the proximity gain. 0xFF on failure.
 */
uint8_t APDS9960::getProximityGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return ERROR;
    }

    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProximityGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Returns receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @return the value of the ALS gain. 0xFF on failure.
 */
uint8_t APDS9960::getAmbientLightGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return ERROR;
    }

    /* Shift and mask out ADRIVE bits */
    val &= 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setAmbientLightGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    val &= 0b11111100;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Get the current LED boost value
 * 
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @return The LED boost value. 0xFF on failure.
 */
uint8_t APDS9960::getLEDBoost()
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    if( !wireReadDataByte(APDS9960_CONFIG2, val) ) {
        return ERROR;
    }

    /* Shift and mask out LED_BOOST bits */
    val = (val >> 4) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED current boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @param[in] drive the value (0-3) for current boost (100-300%)
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setLEDBoost(uint8_t boost)
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    if( !wireReadDataByte(APDS9960_CONFIG2, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;

    /* Write register value back into CONFIG2 register */
    if( !wireWriteDataByte(APDS9960_CONFIG2, val) ) {
        return false;
    }

    return true;
}    

/**
 * @brief Gets proximity gain compensation enable
 *
 * @return 1 if compensation is enabled. 0 if not. 0xFF on error.
 */
uint8_t APDS9960::getProxGainCompEnable()
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, val) ) {
        return ERROR;
    }

    /* Shift and mask out PCMP bits */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Sets the proximity gain compensation enable
 *
 * @param[in] enable 1 to enable compensation. 0 to disable compensation.
 * @return True if operation successful. False otherwise.
 */
 bool APDS9960::setProxGainCompEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into CONFIG3 register */
    if( !wireWriteDataByte(APDS9960_CONFIG3, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the current mask for enabled/disabled proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @return Current proximity mask for photodiodes. 0xFF on error.
 */
uint8_t APDS9960::getProxPhotoMask()
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, val) ) {
        return ERROR;
    }

    /* Mask out photodiode enable mask bits */
    val &= 0b00001111;

    return val;
}

/**
 * @brief Sets the mask for enabling/disabling proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @param[in] mask 4-bit mask value
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProxPhotoMask(uint8_t mask)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    mask &= 0b00001111;
    val &= 0b11110000;
    val |= mask;

    /* Write register value back into CONFIG3 register */
    if( !wireWriteDataByte(APDS9960_CONFIG3, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the entry proximity threshold for gesture sensing
 *
 * @return Current entry proximity threshold.
 */
uint8_t APDS9960::getGestureEnterThresh()
{
    uint8_t val;

    /* Read value from GPENTH register */
    if( !wireReadDataByte(APDS9960_GPENTH, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the entry proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to start gesture mode
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureEnterThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_GPENTH, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the exit proximity threshold for gesture sensing
 *
 * @return Current exit proximity threshold.
 */
uint8_t APDS9960::getGestureExitThresh()
{
    uint8_t val;

    /* Read value from GEXTH register */
    if( !wireReadDataByte(APDS9960_GEXTH, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the exit proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to end gesture mode
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureExitThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_GEXTH, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the Gesture UP Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 * @return Current Gesture UP Offset Register.
 */
uint8_t APDS9960::getGestureUpOffset()
{
    uint8_t val;

    /* Read value from Gesture UP Offset register */
    if( !wireReadDataByte(APDS9960_GOFFSET_U, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the Gesture UP Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 *
 * @param[in] Gesture UP Offset Register
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureUpOffset(uint8_t offset)
{
    if( !wireWriteDataByte(APDS9960_GOFFSET_U, offset) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the Gesture DOWN Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 * @return Current Gesture DOWN Offset Register.
 */
uint8_t APDS9960::getGestureDownOffset()
{
    uint8_t val;

    /* Read value from Gesture DOWN Offset register */
    if( !wireReadDataByte(APDS9960_GOFFSET_D, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the Gesture DOWN Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 *
 * @param[in] Gesture DOWN Offset Register
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureDownOffset(uint8_t offset)
{
    if( !wireWriteDataByte(APDS9960_GOFFSET_D, offset) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the Gesture LEFT Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 * @return Current Gesture LEFT Offset Register.
 */
uint8_t APDS9960::getGestureLeftOffset()
{
    uint8_t val;

    /* Read value from Gesture LEFT Offset register */
    if( !wireReadDataByte(APDS9960_GOFFSET_L, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the Gesture LEFT Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 *
 * @param[in] Gesture LEFT Offset Register
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureLeftOffset(uint8_t offset)
{
    if( !wireWriteDataByte(APDS9960_GOFFSET_L, offset) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the Gesture RIGHT Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 * @return Current Gesture RIGHT Offset Register.
 */
uint8_t APDS9960::getGestureRightOffset()
{
    uint8_t val;

    /* Read value from Gesture RIGHT Offset register */
    if( !wireReadDataByte(APDS9960_GOFFSET_R, val) ) {
        val = 0;
    }

    return val;
}

/**
 * @brief Sets the Gesture RIGHT Offset Register
 *
 * FIELD VALUE Offset Correction Factor
 * 01111111				 127
 *      ...				 ...
 * 00000001				   1
 * 00000000				   0 
 * 10000001				  -1 
 *      ...				 ...
 * 11111111				-127
 *
 * @param[in] Gesture RIGHT Offset Register
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureRightOffset(uint8_t offset)
{
    if( !wireWriteDataByte(APDS9960_GOFFSET_R, offset) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the current photodiode gain. 0xFF on error.
 */
uint8_t APDS9960::getGestureGain()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return ERROR;
    }

    /* Shift and mask out GGAIN bits */
    val = (val >> 5) & 0b00000011;

    return val;
}

/**
 * @brief Sets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] gain the value for the photodiode gain
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureGain(uint8_t gain)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    gain &= 0b00000011;
    gain = gain << 5;
    val &= 0b10011111;
    val |= gain;

    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the drive current of the LED during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the LED drive current value. 0xFF on error.
 */
uint8_t APDS9960::getGestureLEDDrive()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return ERROR;
    }

    /* Shift and mask out GLDRIVE bits */
    val = (val >> 3) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED drive current during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value for the LED drive current
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 3;
    val &= 0b11100111;
    val |= drive;

    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets GPLEN
 *
 * Value    GPLEN
 *   0          4 us
 *   1          8 us
 *   2         16 us
 *   3         32 us
 *
 * @return the GPLEN current value. 0xFF on error.
 */
uint8_t APDS9960::getGestureGPLEN()
{
    uint8_t val;

    /* Read value from GPULSE register */
    if( !wireReadDataByte(APDS9960_GPULSE, val) ) {
        return ERROR;
    }

    /* Shift and mask out GPLEN bits */
    val = (val >> 6) & 0b00000011;

    return val;
}

/**
 * @brief Sets GPLEN
 *
 * Value    GPLEN
 *   0          4 us
 *   1          8 us
 *   2         16 us
 *   3         32 us
 *
 * @param[in] the value for the GPLEN
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureGPLEN(uint8_t gplen)
{
    uint8_t val;

    /* Read value from GPULSE register */
    if( !wireReadDataByte(APDS9960_GPULSE, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    gplen &= 0b00000011;
    gplen = gplen << 6;
    val &= 0b00111111;	
    val |= gplen;

    /* Write register value back into GPULSE register */
    if( !wireWriteDataByte(APDS9960_GPULSE, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets GPULSE
 *
 * Value    GPULSE
 *   0          1
 * ...        ...
 *  63         64
 *
 * @return the GPULSE current value. 0xFF on error.
 */
uint8_t APDS9960::getGestureGPULSE()
{
    uint8_t val;

    /* Read value from GPULSE register */
    if( !wireReadDataByte(APDS9960_GPULSE, val) ) {
        return ERROR;
    }

    /* Shift and mask out GPULSE bits */
    val &= 0b00111111;

    return val;
}

/**
 * @brief Sets GPULSE
 *
 * Value    GPLEN
 * Value    GPULSE
 *   0          1
 * ...        ...
 *  63         64
 *
 * @param[in] the value for the GPULSE
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureGPULSE(uint8_t gpulse)
{
    uint8_t val;

    /* Read value from GPULSE register */
    if( !wireReadDataByte(APDS9960_GPULSE, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    gpulse &= 0b00111111;
    val &= 0b11000000;	
    val |= gpulse;

    /* Write register value back into GPULSE register */
    if( !wireWriteDataByte(APDS9960_GPULSE, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @return the current wait time between gestures. 0xFF on error.
 */
uint8_t APDS9960::getGestureWaitTime()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return ERROR;
    }

    /* Mask out GWTIME bits */
    val &= 0b00000111;

    return val;
}

/**
 * @brief Sets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @param[in] the value for the wait time
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureWaitTime(uint8_t time)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    time &= 0b00000111;
    val &= 0b11111000;
    val |= time;

    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the low threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::getLightIntLowThreshold(uint16_t &threshold)
{
    uint8_t val_byte;
    threshold = 0;

    /* Read value from ambient light low threshold, low byte register */
    if( !wireReadDataByte(APDS9960_AILTL, val_byte) ) {
        return false;
    }
    threshold = val_byte;

    /* Read value from ambient light low threshold, high byte register */
    if( !wireReadDataByte(APDS9960_AILTH, val_byte) ) {
        return false;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Sets the low threshold for ambient light interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setLightIntLowThreshold(uint16_t threshold)
{
    /* Break 16-bit threshold into 2 8-bit values */
    uint8_t val_low = threshold & 0x00FF;
    uint8_t val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    if( !wireWriteDataByte(APDS9960_AILTL, val_low) ) {
        return false;
    }

    /* Write high byte */
    if( !wireWriteDataByte(APDS9960_AILTH, val_high) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the high threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::getLightIntHighThreshold(uint16_t &threshold)
{
    uint8_t val_byte;
    threshold = 0;

    /* Read value from ambient light high threshold, low byte register */
    if( !wireReadDataByte(APDS9960_AIHTL, val_byte) ) {
        return false;
    }
    threshold = val_byte;

    /* Read value from ambient light high threshold, high byte register */
    if( !wireReadDataByte(APDS9960_AIHTH, val_byte) ) {
        return false;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Sets the high threshold for ambient light interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setLightIntHighThreshold(uint16_t threshold)
{
    /* Break 16-bit threshold into 2 8-bit values */
    uint8_t val_low = threshold & 0x00FF;
    uint8_t val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    if( !wireWriteDataByte(APDS9960_AIHTL, val_low) ) {
        return false;
    }

    /* Write high byte */
    if( !wireWriteDataByte(APDS9960_AIHTH, val_high) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets the low threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::getProximityIntLowThreshold(uint8_t &threshold)
{
    threshold = 0;

    /* Read value from proximity low threshold register */
    if( !wireReadDataByte(APDS9960_PILT, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Sets the low threshold for proximity interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProximityIntLowThreshold(uint8_t threshold)
{
    /* Write threshold value to register */
    if( !wireWriteDataByte(APDS9960_PILT, threshold) ) {
        return false;
    }

    return true;
}
    
/**
 * @brief Gets the high threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::getProximityIntHighThreshold(uint8_t &threshold)
{
    threshold = 0;

    /* Read value from proximity low threshold register */
    if( !wireReadDataByte(APDS9960_PIHT, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Sets the high threshold for proximity interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProximityIntHighThreshold(uint8_t threshold)
{
    /* Write threshold value to register */
    if( !wireWriteDataByte(APDS9960_PIHT, threshold) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets if ambient light interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960::getAmbientLightIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, val) ) {
        return ERROR;
    }

    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0b00000001;

    return val;
}

/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 4;
    val &= 0b11101111;
    val |= enable;

    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets if proximity interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960::getProximityIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, val) ) {
        return ERROR;
    }

    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Turns proximity interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setProximityIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Gets if gesture interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960::getGestureIntEnable()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, val) ) {
        return ERROR;
    }

    /* Shift and mask out GIEN bit */
    val = (val >> 1) & 0b00000001;

    return val;
}

/**
 * @brief Turns gesture-related interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, val) ) {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;

    /* Write register value back into GCONF4 register */
    if( !wireWriteDataByte(APDS9960_GCONF4, val) ) {
        return false;
    }

    return true;
}

/**
 * @brief Clears the ambient light interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
bool APDS9960::clearAmbientLightInt()
{
    uint8_t throwaway;
    if( !wireReadDataByte(APDS9960_AICLEAR, throwaway) ) {
        return false;
    }

    return true;
}

/**
 * @brief Clears the proximity interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
bool APDS9960::clearProximityInt()
{
    uint8_t throwaway;
    if( !wireReadDataByte(APDS9960_PICLEAR, throwaway) ) {
        return false;
    }

    return true;
}

/**
 * @brief Tells if the gesture state machine is currently running
 *
 * @return 1 if gesture state machine is running, 0 if not. 0xFF on error.
 */
uint8_t APDS9960::getGestureMode()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, val) ) {
        return ERROR;
    }

    /* Mask out GMODE bit */
    val &= 0b00000001;

    return val;
}

/**
 * @brief Tells the state machine to either enter or exit gesture state machine
 *
 * @param[in] mode 1 to enter gesture state machine, 0 to exit.
 * @return True if operation successful. False otherwise.
 */
bool APDS9960::setGestureMode(uint8_t mode)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;
    
    /* Write register value back into GCONF4 register */
    if( !wireWriteDataByte(APDS9960_GCONF4, val) ) {
        return false;
    }

    return true;
}

/*******************************************************************************
 * Raw I2C Reads and Writes
 ******************************************************************************/

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
bool APDS9960::wireWriteByte(uint8_t val)
{
    Wire.beginTransmission(APDS9960_I2C_ADDR);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }

    return true;
}

/**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
bool APDS9960::wireWriteDataByte(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(APDS9960_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }

    return true;
}

/**
 * @brief Writes a block (array) of bytes to the I2C device and register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val pointer to the beginning of the data byte array
 * @param[in] len the length (in bytes) of the data to write
 * @return True if successful write operation. False otherwise.
 */
bool APDS9960::wireWriteDataBlock(uint8_t reg, 
                                        uint8_t *val, 
                                        unsigned int len)
{
    Wire.beginTransmission(APDS9960_I2C_ADDR);
    Wire.write(reg);
    for(uint8_t i = 0; i < len; i++) {
        Wire.beginTransmission(val[i]);
    }
    if( Wire.endTransmission() != 0 ) {
        return false;
    }

    return true;
}

/**
 * @brief Reads a single byte from the I2C device and specified register
 *
 * @param[in] reg the register to read from
 * @param[out] the value returned from the register
 * @return True if successful read operation. False otherwise.
 */
bool APDS9960::wireReadDataByte(uint8_t reg, uint8_t &val)
{
    /* Indicate which register we want to read from */
    if (!wireWriteByte(reg)) {
        return false;
    }

    /* Read from register */
    Wire.requestFrom(APDS9960_I2C_ADDR, 1);
    while (Wire.available()) {
        val = Wire.read();
    }

    return true;
}

/**
 * @brief Reads a block (array) of bytes from the I2C device and register
 *
 * @param[in] reg the register to read from
 * @param[out] val pointer to the beginning of the data
 * @param[in] len number of bytes to read
 * @return Number of bytes read. -1 on read error.
 */
int APDS9960::wireReadDataBlock(uint8_t reg, 
                                        uint8_t *val, 
                                        unsigned int len)
{
    /* Indicate which register we want to read from */
    if (!wireWriteByte(reg)) {
        return -1;
    }

    /* Read block data */
    Wire.requestFrom(APDS9960_I2C_ADDR, len);
    uint8_t i = 0;
    while (Wire.available()) {
        if (i >= len) {
            return -1;
        }
        val[i] = Wire.read();
        i++;
    }

    return i;
}