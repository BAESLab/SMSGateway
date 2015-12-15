#ifndef _SMSGATEWAY_H_
#define _SMSGATEWAY_H_

#include <LGATT.h>
#include <LGATTClient.h>
#include <LFlash.h>
#include <LStorage.h>
#include <LGSM.h>

#define SIZE_OF_STR_TIME_INTERVAL 13
#define SIZE_OF_STR_PHONE_NUMBER  12
#define SIZE_OF_PHONE_NUMBER 10
#define MAX_PHONE_NUMBER 20
#define MAX_DEVICE_BLE_INFO 10

//#define _DEBUG
#define _DEBUG_INFO


enum mode{
  GET_TIME_INTERVAL = 0,
  GET_PHONE_NUBER
};

typedef struct {
  byte uuid[16];
  byte major[2];
  byte minor[2];
  byte nameBLE[12];
}typeBLEInfo;

// create a uuid for app, this uuid id is to identify a client
static LGATTUUID myUUID("B4B4B4B4-B4B4-B4B4-B4B4-B4B4B4B4B4B4");
static byte myUuid[16] = {0x4A, 0x03, 0x03, 0x19, 0x00, 0x00, 0x11, 0xA0, 0x01, 0x06, 0x42, 0x41, 0x45, 0x53, 0x00, 0x00};

class SMSGateway
{
private:
  LGATTClient c;
  LFile myFile;	
  
public:
  SMSGateway(void);
  ~SMSGateway(void); 
  bool begin();
	
private:
 typeBLEInfo BLEInfo[MAX_DEVICE_BLE_INFO];

 String str1 = "time_interval";
 String str2 = "phone_number";
 String phoneNuber[MAX_PHONE_NUMBER];
 String timeIntervalString[2];
 unsigned long timeInterval;
 unsigned long mtimeInterval;
 byte countT;
 byte countPN;

 
public:
   int scanBLE();
   int checkPressAlert(int num);
   bool checkLossAlert(int num, int rssi);
   bool sendSMS(int numberAlert, const char message[]);
   bool sendSMS(const char message[]);
   bool isSIM();
   
private:
  bool getConfig(String strConfig[], byte *len, byte mode);
	
};

#endif

