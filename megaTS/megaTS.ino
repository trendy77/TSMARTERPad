/**
 * \file
 *			 ESP8266 RESTful Thingspeak example
 * \author
 *			 BeeGee
 */


void setup() {
	Serial.begin(115200);
  Serial3.begin(115200);
	
	Serial.println("");
	Serial.println("EL-Client starting!");
}

float tempValue = 99.5;
float humidValue = 99.5;



void loop() {
	
		// Generate a fake value starting from 100 going up to 300
		
		tempValue = tempValue + 0.5;
		if (tempValue == 300) {
			tempValue = 100;
		}
		String tempValString = String(tempValue);
		const char *tempValChar = tempValString.c_str();
		String humidValString = String(humidValue);
    const char *humidValChar = humidValString.c_str();
    
  // Reserve a buffer for sending the data
		char path_data[BUFLEN];
		// Copy the path and API key into the buffer
		sprintf(path_data, "%s", "/update?api_key=");
		sprintf(path_data + strlen(path_data), "%s", api_key);
		
		// Copy the field number and value into the buffer
		// If you have more than one field to update,
		// repeat and change field1 to field2, field3, ...
		sprintf(path_data + strlen(path_data), "%s", "&field1=");
		sprintf(path_data + strlen(path_data), "%s", tempValChar);
		
    sprintf(path_data + strlen(path_data), "%s", "&field2=");
    sprintf(path_data + strlen(path_data), "%s", humidValChar);

    
		// Send PUT request to thingspeak.com
		rest.post(path_data,"");	

		// Reserve a buffer for the response from Thingspeak
		char response[BUFLEN];
		// Clear the buffer
		memset(response, 0, BUFLEN);
		// Wait for response from Thingspeak
		uint16_t code = rest.waitResponse(response, BUFLEN-1);
		// Check the response from Thingspeak
		if(code == HTTP_STATUS_OK){
			Serial.println("Thingspeak: POST successful:");
			Serial.print("Response: ");
			Serial.println(response);
		} else {
			Serial.print("Thingspeak: POST failed with error ");
			Serial.println(code);
			Serial.print("Response: ");
			Serial.println(response);
		}
		// Send next data in 30 seconds
		delay(30000);
	}
}
