#include <LGATT.h>
#include <LGATTClient.h>
#include <LFlash.h>
#include <LStorage.h>
#include <LGSM.h>
#include "SMSGateway.h"

#define SMS_TIMEOUT 30

SMSGateway gateway;
bool smsFlag = false;
int numberOfButtonsPressed = 0;
unsigned int timeOut = 0;

void setup() {
  
    delay(3000);
    Serial.begin(115200);
    gateway.begin();
}

void loop() {

   int member = gateway.scanBLE();

   if(!gateway.isLoss(member, -90)){
      Serial.println("+++++++++++ Alert ++++++++++++");
      smsFlag = true;
   }
     
   if(gateway.isSIM()){
       if(smsFlag){
           if(timeOut == 0){
              if(gateway.sendSMS("Out Of Area.")){
                Serial.println("Send SMS complete.");
              }
           }    

            timeOut++;
            if(timeOut >= SMS_TIMEOUT){ // Time out sent SMS 30 second.
              smsFlag = false;
              timeOut = 0;
            }
       }
   }
}
