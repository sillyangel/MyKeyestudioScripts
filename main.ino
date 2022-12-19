//**********************************************************************************
/*  
 * Filename    : main
 * Description : main
 * Auther      : sillyangel
*/
#include <Wire.h>
#include "DHT.h"
#define DHTPIN 17
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C mylcd(0x27,16,2);
#include <ESP32_Servo.h>
Servo myservo;
#include <Wire.h>
#include "MFRC522_I2C.h"
// IIC pins default to GPIO21 and GPIO22 of ESP32
// 0x28 is the i2c address of SDA, if doesn't match，please check your address with i2c.
MFRC522 mfrc522(0x28);   // create MFRC522.
#define servoPin  13
#define btnPin 16
boolean btnFlag = 0;

String password = "";
void setup() {
  Serial.begin(9600);           // initialize and PC's serial communication
  mylcd.init();
  mylcd.backlight();
  Wire.begin();                   // initialize I2C
  mfrc522.PCD_Init();             // initialize MFRC522
  ShowReaderDetails();            // display PCD - MFRC522 read carder
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
  myservo.attach(servoPin);
  pinMode(btnPin, INPUT);
  mylcd.setCursor(0, 0);
  mylcd.print("Card");
  dht.begin();
}

void loop() {
  // 
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    password = "";
    if(btnFlag == 1)
    {
      boolean btnVal = digitalRead(btnPin);
      if(btnVal == 0)  //Swipe the card to open the door and click button 1 to close the door
      {
        mylcd.clear();
        Serial.println("close");
        mylcd.setCursor(0, 0);
        mylcd.clear();
        mylcd.print("GoodBye!");
        myservo.write(0);
        btnFlag = 0;
        delay(1000);
        mylcd.clear();
      }
    }
    return;
  }
  
  // select one of door cards. UID and SAK are mfrc522.uid.
  
  // save UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(mfrc522.uid.uidByte[i]);
    password = password + String(mfrc522.uid.uidByte[i]);
  }
  if(password == "1161603391")  //The card number is correct, open the door 1161603391
  {
    Serial.println("open");
    mylcd.setCursor(0, 0);
    mylcd.clear();
    mylcd.print("Welcome Angel");
    myservo.write(180);
    password = "";
    btnFlag = 1;
    delay(1000);
    TempAndHumin();
  }
  else   //The card number is wrong，LCD displays error
  {
    password = "";
    mylcd.setCursor(0, 0);
    mylcd.print("Wrong Card.");
    delay(1500);
    mylcd.clear();
  }
  //Serial.println(password);
}

void ShowReaderDetails() {
  //  attain the MFRC522 software
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // when returning to 0x00 or 0xFF, may fail to transmit communication signals
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}


void TempAndHumin() {
  Serial.println("TempAndHumin is Online/Executing Correctly");
  while (btnFlag == 1) {
    float h = dht.readHumidity();
    float t = dht.readTemperature(true);
    mylcd.clear();
    Serial.print("RH:");
    Serial.print(h); //The integral part of humidity, DHT [1] is the fractional part
    Serial.print("%  ");
    Serial.print("Temp:");
    Serial.print(t); //The integral part of temperature, DHT [3] is the fractional part
    Serial.println("F");
    
    mylcd.setCursor(0, 0);
    mylcd.print("T = ");
    mylcd.print(t);
    mylcd.setCursor(0, 1);
    mylcd.print("H = ");
    mylcd.print(h]);
    //mylcd.clear();
    delay(1000);
 }
}
//**********************************************************************************
