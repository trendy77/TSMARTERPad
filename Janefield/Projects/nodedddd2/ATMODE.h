 /*  
   *   
   Serial3.begin(115200);
Serial3.println("AT");     
inputString.reserve(200);  
    Serial.println("Ready");
 // pinMode(13,OUTPUT);   
  delay(500);   
//Serial3.println("AT+CIPMUX=1");   
//delay(2000);   
Serial3.println("AT+CIPSERVER=1,5000"); // setup TCP server
delay(2000);   

Serial3.println("AT+CIPSTO=3600");   delay(2000); 
Serial3.println("AT+CWAUTOCONN=1");delay(2000); 
Serial3.println("AT+CIPMODE=1");delay(2000); 
Serial3.println("AT+CIPSTA_DEF=\"10.0.77.31\",\"10.0.77.100\",\"255.255.255.0\""); delay(2000); 
Serial3.println("AT+CWJAP_DEF=\"Northern Frontier Interwebs\",\"num4jkha8nnk\"");delay(2000); 
*/
