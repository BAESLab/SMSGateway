#include <LGATT.h>
#include <LGATTClient.h>
#include <LFlash.h>
#include <LStorage.h>
#include <LGSM.h>
#include "SMSGateway.h"

SMSGateway s;
bool flagAlert = false;
int numberAlert = 0;
unsigned int timeOut = 0;

void setup() {
  
    delay(3000);
    Serial.begin(115200);
    s.begin();
}

void loop() {

   int member = s.scanBLE();

   if(!s.checkLossAlert(member, -90)){
      Serial.println("+++++++++++ Alert ++++++++++++");
      flagAlert = true;
   }
     
   if(s.isSIM()){
       if(flagAlert){
           if(timeOut == 0){
              if(s.sendSMS("Out Of Area.")){
                Serial.println("Send SMS complete.");
              }
           }    
            delay(1); 
            timeOut++;
            if(timeOut >= 60000){ // Time out sent SMS 60 second.
              flagAlert = false;
              timeOut = 0;
            }
       }
   }
}
