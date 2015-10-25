#include <VirtualWire.h>
#include <DataCoder.h>
#include <LiquidCrystal.h>


double percent =0 ;
const int rx_pin = 11;
const int relay_pin = 12;
const int baudRate = 1000;
const int TankHeight = 75;
String mode = "OFF";
float PreSensorValue =  0;
bool flgFirst = 0;
unsigned long previousMillis=0;
float previousLevel=0;
bool isFault=0;
bool isFirstON=0;
float value =0;

LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

void setup()   {

  delay(100);
  SetupRFDataRxnLink(rx_pin, baudRate);
  pinMode(relay_pin,OUTPUT);
  digitalWrite(relay_pin,HIGH);
  lcd.begin(16, 2);
  mode = "OFF";
  Serial.begin(19200);
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  union RFData inDataSeq;//
  float inArray[RF_DATA_ARRAY_SIZE];

  if(RFLinkDataAvailable(buf, &buflen))
  {
     for(int i =0; i< buflen; i++)
     {
        inDataSeq.s[i] = buf[i];
     }
     
     DecodeRFData(inArray, inDataSeq);
 
     if(!flgFirst)
     {
      PreSensorValue = inArray[0];
      flgFirst = 1;
      //Serial.print("Init Pre. Sesnse Value: ");
      //Serial.println(PreSensorValue);
     }
 
     if((abs(PreSensorValue - inArray[0]) > 1))
     {
        //Serial.println("************Logic***********");
        //Serial.print("abs(PreSensorValue - inArray[0]) > 1): ");
        //Serial.println(abs(PreSensorValue - inArray[0]));

        //Serial.print("abs(PreSensorValue - inArray[0]) < 1): ");
        //Serial.println(abs(PreSensorValue - inArray[0]));
        //Serial.println("********End Logic************");
        
        //Serial.println("***************************");
        //Serial.print("Init Pre. Sesnse Value: ");
        //Serial.println(PreSensorValue);
        //Serial.print("Eror Read Value: ");
        //Serial.println(inArray[0]);
        inArray[0] = PreSensorValue;
        //Serial.print("Value Correction To: ");
        //Serial.println(PreSensorValue);
        //Serial.println("***************************");
     }
     else
     {
      PreSensorValue = inArray[0];
      value = inArray[0];
      //Serial.print("Value Normal: ");
      //Serial.println(PreSensorValue);
      //Serial.print("Time mills: ");
      //Serial.print(millis());
     }


     if(mode == "ON")
     {
      if((millis()-previousMillis)>=300000)
      {
       if(previousLevel==inArray[0])
       {
         digitalWrite(relay_pin,HIGH);
         mode = "OFF";
         isFault=1;
         lcd.clear();
         lcd.setCursor(0, 1);
         lcd.print("Error In motor.");
       }
       else
       {
         previousLevel=inArray[0];
         previousMillis = millis();
       }
      }
     }
     if(isFault) return;
     //Convert to CM to Feet
     float feet = ((TankHeight - inArray[0]) * 0.032808);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Lvl:");
     if(feet > 1.7)
     lcd.print("Full");
     else if (feet >= 1.5 && feet <= 1.7)
     lcd.print("Med.");
     else
     lcd.print("Low");
     
     lcd.print(" Mo.:");
     lcd.print(mode);
     
     lcd.setCursor(0, 1);
     lcd.print((TankHeight - inArray[0]) * 0.032808);
     lcd.print(" Ft "); 
     percent = ((TankHeight - inArray[0])/TankHeight) * 100;
     lcd.print(percent);
     lcd.print("%");
     
     if(value <= 0 || value >= 80)
     {
        digitalWrite(relay_pin,HIGH);
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Error in device.");
        mode = "OFF";
        isFirstON=0;
     }
     else if(value < 9) // Motor OFF
     {
      digitalWrite(relay_pin,HIGH);
      mode = "OFF";
      isFirstON=0;
     }
     else if(value > 30) // Motor ON
     {
      if(!isFault)
      {
       digitalWrite(relay_pin,LOW);
       mode = "ON";
       if(!isFirstON)
       {
        previousMillis= millis();
        previousLevel = inArray[0];
        isFirstON=1;
       }
      }
     }
  }
   delay(7000);
}

