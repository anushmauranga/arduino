#include <DataCoder.h>
#include <VirtualWire.h>

const int transmit_pin = 11;
#define trigPin 13
#define echoPin 12

const int led_pin = 13;
const int baudRate = 1000;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  SetupRFDataTxnLink(transmit_pin, baudRate);

}

void loop()
{
  float outArray[1];
  long duration, distance;
  
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  
  outArray[0] = distance;
  union RFData outDataSeq;
  EncodeRFData(outArray, outDataSeq); 
  TransmitRFData(outDataSeq);  
  delay(1000);
}


