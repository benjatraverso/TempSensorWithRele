#include <OneWire.h> 
#include <DallasTemperature.h>
OneWire oneWire(2); 
DallasTemperature sensors(&oneWire);
// constants
const float SENSOR_CALIBRATION_VALUE = 0.004882814;
const int DESIRED_TEMP = 17;

const byte GREEN_LED = 9;
const byte RED_LED = 8;
const byte BLUE_LED = 7;
const byte RELE = 4;
//const byte TEMP_SENSOR = A0;

const int LED_BLINK_DELAY = 1000;
const int TEMP_CHECK_DELAY = 2000;

int giState;

enum states
{
	cool = 0,
	hot,
	error
};

void setup(void)
{
	#ifdef TEST
		Serial.begin(9600);
	#endif
	
	state = idle;
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(RELE, OUTPUT);
	sensors.begin();
}

void loop(void)
{
	switch(giState)
	{
		case cool:
		{
			doIdle(); //make sleep until shit gets hot
			state = hot;
			break;
		}
		case hot:
		{
			if(!doCool())//cool down until it chills
			{
				//came out from cooling with no success
				state = error;
			}
			else
			{
				//cooling succeeded
				state = cool;
			}
			break;
		}
		case error:
		{
			doError(); //blink on red
			break;
		}
		default:
		{
			break;
		}
	}
}

void doCool(void)
{
	#ifdef test
		Serial.println("Idle");
	#endif
	digitalWrite(GREEN_LED, HIGH);
	float temp = readTemp();
	while( DESIRED_TEMP > temp ) //check we are in desired temp
	{
		light("green");
		if(DESIRED_TEMP < temp)
		{
			// shut EVERYTHING down for a while and check again after a quick nap
			// TODO: sleep mode for at least 10min and check again, only blink blue each 8 sec to let know still alive
		}
		
		//check temperature again
		temp = readTemp();
		
		#ifdef test
			_T("Cool. Temp now",temp);
		#endif
	}
}

int doCooling(void)
{
	#ifdef test
		Serial.println("Cooling");
	#endif
	
	light("blue");//we got everything up and running
	while( DESIRED_TEMP < temp ) //check we are hot
	{
		// stay here for as long as it takes to reach the desired temp
		digitalwrite(RELE, high);
		// TODO: implement time count to deside it's been too long cooling
		delay(60*1000);
		temp = readTemp();
		#ifdef test
			_T("Cooling. Temp now",temp);
		#endif
	}

	return 0;
}

void doError(void)
{
	#ifdef test
		_T("Error",0);
	#endif
	while(1)
	{
		light("red");
		// TODO:go to deep sleep again for N times
		giState = hot;
	}
}

float readTemp(void)
{
	sensors.requestTemperatures();
	return sensors.getTempCByIndex(0);
}

void _T(String text, float value)
{
	Serial.print(text + " :");
	Serial.println(value);
}
