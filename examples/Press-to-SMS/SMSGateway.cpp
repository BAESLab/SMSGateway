#include "SMSGateway.h"


SMSGateway::SMSGateway(void)
{
    countT  = 0;
    countPN = 0;
    timeInterval = 0;
    mtimeInterval = 0;
    str1 = "time_interval";
    str2 = "phone_number";
}

SMSGateway::~SMSGateway(void)
{
   
}

bool SMSGateway::begin()
{
  
    pinMode(LED_POWER, OUTPUT);
    pinMode(LED_SIGNAL, OUTPUT);
    
    digitalWrite(LED_POWER, HIGH);
    digitalWrite(LED_SIGNAL, LOW);
    
     // GATT central begin to start
    if (c.begin(myUUID))
    {
        #ifdef _DEBUG
        Serial.println("LGATT Success to register.");
        #endif
    }
    else
    {
        #ifdef _DEBUG
        Serial.println("LATT Failed to register.");
        #endif
        delay(0xffffffff);
    }
      LFlash.begin();
      myFile = LFlash.open("Config.txt", FILE_WRITE);
      getConfig(timeIntervalString, &countT, GET_TIME_INTERVAL);
      
      #ifdef _DEBUG
      Serial.print("countT: ");
      Serial.println(countT);
      #endif
      
      timeInterval = timeIntervalString[0].toInt(); // ==> to Unit 1S
      Serial.print("timeInterval: ");
      Serial.println(timeInterval);
  
      myFile = LFlash.open("Config.txt", FILE_WRITE);
      getConfig(phoneNuber, &countPN, GET_PHONE_NUBER);
      
      #ifdef _DEBUG
      Serial.print("countPN: ");
      Serial.println(countPN);
      #endif
      
      isSIM();
}

bool SMSGateway::getConfig(String strConfig[], byte *len, byte mode){
  
  String stringOne = "";
  int lashListItem = 0;
  int indexItem = 0;
  char bufRead[256] = "";
  char *pBuf;
  
     pBuf = &bufRead[0];
      
     if (myFile){
       
        #ifdef _DEBUG
        Serial.println("Config.txt:");
        #endif
        
        myFile.seek(0);
        // read from the file until there's nothing else in it:
        while (myFile.available()){      
         *pBuf++ = myFile.read();
        }
        stringOne = String(bufRead);
         
       switch(mode){
         
           case GET_TIME_INTERVAL:
               lashListItem  = stringOne.indexOf(str1);
               indexItem = lashListItem + SIZE_OF_STR_TIME_INTERVAL + 1;
               pBuf = &bufRead[indexItem];
                do{
                  strConfig[0] = stringOne.substring(indexItem, indexItem + *len);
                  *len += 1;
               }while(*pBuf++ && (*pBuf != '\n'));
               *len -= 1;
           break;
           
           case GET_PHONE_NUBER:
            
              lashListItem = stringOne.indexOf(str2);
              indexItem = lashListItem + SIZE_OF_STR_PHONE_NUMBER + 1;
             
              pBuf = &bufRead[indexItem - 1];
              
              do{
                  if(*pBuf == '=' || *pBuf == ','){
                     strConfig[*len] = stringOne.substring(indexItem, indexItem + 10);
                     indexItem += SIZE_OF_PHONE_NUMBER + 1;
                     *len += 1;
                  }
              }while(*pBuf++);
              
              #ifdef _DEBUG
              for(int i = 0; i < *len; i++){
                  Serial.print("Phone number1: ");
                  Serial.println(phoneNuber[i]);
              }
              #endif
              
           break;
       }
        // close the file:
        myFile.close();
    } else {
      
        #ifdef _DEBUG
        Serial.println("error opening Config.txt");
        #endif
    }
    
  return true;
}


int SMSGateway::scanBLE()
{ 
     return c.scan(1);  //Time out 1S.
}


int SMSGateway::buttonsPressed(int num)
{
  LGATTDeviceInfo info = {0};
   typeBLEInfo mInfo;
   int numberAlert = 0;
   for (int i = 0; i < num; i++)
   {
      c.getScanResult(i, info);
      
      #ifdef _DEBUG_INFO
      Serial.printf("[LGATTC ino]dev address : [%x:%x:%x:%x:%x:%x] rssi [%d]", 
      info.bd_addr.addr[5], info.bd_addr.addr[4], info.bd_addr.addr[3], info.bd_addr.addr[2], info.bd_addr.addr[1], info.bd_addr.addr[0],
      info.rssi);
      Serial.println();
      #endif
 
      int lenAdvertData = info.eir[8];
      memcpy(&mInfo, &info.eir[9], lenAdvertData - 1);
      memcpy(&mInfo.nameBLE, &info.eir[9 + lenAdvertData + 2], 12); 
       
      if(memcmp(myUuid, mInfo.uuid, 16 ) == 0){
               
          if(mInfo.minor[1] == 0x12 || mInfo.minor[1] == 0x11){
             memcpy(&BLEInfo[numberAlert], &mInfo, 32);
             numberAlert++;
             
             #ifdef _DEBUG_INFO
             Serial.println("\n<<<<<< Alert >>>>>>\n");
             #endif
             
          }else if(mInfo.minor[1] == 0x02){
            
             #ifdef _DEBUG_INFO
             Serial.println("\n<<<<<< Normal >>>>>>\n");
             #endif
             
          }
      }
   }
  return numberAlert;
}


bool SMSGateway::isLoss(int num, int rssi)
{
  static bool fristScand = false;
   LGATTDeviceInfo info = {0};
   typeBLEInfo mInfo;
   int numberAlert = 0;
     for (int i = 0; i < num; i++){
          c.getScanResult(i, info);
          
          #ifdef _DEBUG_INFO
          Serial.printf("[LGATTC ino]dev address : [%x:%x:%x:%x:%x:%x] rssi [%d]", 
          info.bd_addr.addr[5], info.bd_addr.addr[4], info.bd_addr.addr[3], info.bd_addr.addr[2], info.bd_addr.addr[1], info.bd_addr.addr[0],
          info.rssi);
          Serial.println();
          #endif
  
          int lenAdvertData = info.eir[8];
          memcpy(&mInfo, &info.eir[9], lenAdvertData - 1);
          memcpy(&mInfo.nameBLE, &info.eir[9 + lenAdvertData + 2], 12); 
            
          if(memcmp(myUuid, mInfo.uuid, 16 ) == 0){
              fristScand = true;
              memcpy(&BLEInfo[0], &mInfo, 32);
              
              if(info.rssi > rssi){         
                mtimeInterval = 0;
              }else{
                mtimeInterval++;
                delay(1);
              }
          }       
      } 
 
     
     if(numberAlert == 0 && fristScand == true){
       mtimeInterval++;
       delay(1); 
     }
     
     if(mtimeInterval >= timeInterval){  
       
        #ifdef _DEBUG_INFO
        Serial.println("\n<<<<<< Alert >>>>>>\n");
        #endif
        
        return false;
     }else{
       
        #ifdef _DEBUG_INFO
        Serial.println("\n<<<<<< Normal >>>>>>\n");
        #endif
        
     }
     
     #ifdef _DEBUG
     Serial.print("mtimeInterval: ");
     Serial.println(mtimeInterval);
     #endif
     
    return true;
}


bool SMSGateway::sendSMS(int numberAlert, const char message[])
{

  char buf[SIZE_OF_PHONE_NUMBER];
  
  for(int i = 0; i < numberAlert; i++){
    for(int j = 0; j < countPN; j++){  
      phoneNuber[j].toCharArray(buf, SIZE_OF_PHONE_NUMBER + 1);
      
      #ifdef _DEBUG
      Serial.printf("%s %s",&BLEInfo[i].nameBLE[0], message);
      #endif
      
      LSMS.beginSMS(buf);
      LSMS.printf("%s %s",&BLEInfo[i].nameBLE[0], message);
      LSMS.endSMS();
 
    }
  }
  
  
 return true; 
}

bool SMSGateway::sendSMS(const char message[])
{
  char buf[SIZE_OF_PHONE_NUMBER];

    for(int j = 0; j < countPN; j++){   
      phoneNuber[j].toCharArray(buf, SIZE_OF_PHONE_NUMBER + 1);
      
      #ifdef _DEBUG
      Serial.printf("%s %s",&BLEInfo[0].nameBLE[0], message);
      #endif
      
      LSMS.beginSMS(buf);
      LSMS.printf("%s %s",&BLEInfo[0].nameBLE[0], message);
      LSMS.endSMS();
 
    }

 return true; 
}


bool SMSGateway::isSIM() //Check status SIM.
{
    if(!LSMS.ready()){
      
        #ifdef _DEBUG_INFO
        Serial.println("\n!!!!!!!!! SIM not ready. !!!!!!!!!!!\n");
        delay(1000);
        #endif
        
        return false;
    }
  return true;
}


