#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
SoftwareSerial xbee(A4, A5); // RX, TX

#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

int Contrast=75;

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);




void readrfid(){
  Serial.println("Initializing reading ....");
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;                        
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    
    if (uidLength == 4)
    { 
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      Serial.print("Card id number ");
      
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Card Num:");
      lcd.setCursor(3,1);
      lcd.print(cardid);
      
      Serial.println(cardid);
      String card_id_as_string = String(cardid);

      Serial.println("send data to XBEE");
      delay(1000);
 
    

      beepSuc();
    }
    Serial.println("");
  }
  delay(2000);

  }


void writerfid(const String& input){

  String target_url = input;
  int str_len = target_url.length() + 1;       
  char url[str_len];
  target_url.toCharArray(url,str_len);

  
  uint8_t ndefprefix1 = NDEF_URIPREFIX_HTTP_WWWDOT;
 
  const char * email = "mail@example.com";
  uint8_t ndefprefix2 = NDEF_URIPREFIX_MAILTO;
  
  const char * phn = "+1 212 555 1212";
  uint8_t ndefprefix3 = NDEF_URIPREFIX_TEL;
   
  Serial.println("Initializing Writing ....");
  
  uint8_t success;                          
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;                        
  bool authenticated = false;               

  uint8_t keya[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
  uint8_t keyb[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
  
  Serial.println("Waiting for an ISO14443A Card ...");
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) 
  {
    success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, 4, 0, keyb);
    if (!success)
    {
      Serial.println("Not in NDEF format");
      return;
    }
    
    Serial.println("Authentication succeeded");

    Serial.println("Updating sector 1...");
    success = nfc.mifareclassic_WriteNDEFURI(1, ndefprefix1, url);
    delay(100);
    if (success)
    {
      Serial.println("data written succeeded");
      Serial.println("");
            
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Writing Success");

      beepSuc();
    }
    else
    {
      Serial.println("data written failed");
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Writing Failed");
    }


    
    
//    Serial.println("Updating sector 2...");
//    success = nfc.mifareclassic_WriteNDEFURI(2, ndefprefix2, email);
//    delay(100);
//    if (success)
//    {
//      Serial.println("data written succeeded");
//      Serial.println("");      
//    }
//    else
//    {
//      Serial.println("data written failed");
//    }
//
//
//    
//    
//    Serial.println("Updating sector 3...");
//    success = nfc.mifareclassic_WriteNDEFURI(3, ndefprefix3, phn);
//    delay(100);
//    if (success)
//    {
//      Serial.println("data written succeeded");
//      Serial.println("");      
//    }
//    else
//    {
//      Serial.println("data written failed");
//    }
  }
  
  delay(2000);

  }

void beepOne(){//read

  tone(13, 2000,1000);
//     digitalWrite(13,HIGH);
//    delay(500);
//    digitalWrite(13,LOW);
//    delay(500);
  }

void beepTwo(){//write

  tone(13, 3000,500);
  delay(500);
  tone(13, 570,500);

  
//    digitalWrite(13,HIGH);
//    delay(500);
//    digitalWrite(13,LOW);
//    delay(500);
//
//    digitalWrite(13,HIGH);
//    delay(500);
//    digitalWrite(13,LOW);
//    delay(500);
  }


void beepSuc(){

  tone(13, 2000,100);
  delay(100);
  tone(13, 6000,200);
  delay(100);
  tone(13, 2000,100);
  
//  digitalWrite(13,HIGH);
//    delay(100);
//    digitalWrite(13,LOW);
//    delay(100);
//
//    digitalWrite(13,HIGH);
//    delay(100);
//    digitalWrite(13,LOW);
//    delay(100);
//
//    digitalWrite(13,HIGH);
//    delay(100);
//    digitalWrite(13,LOW);
//    delay(100);
  
  }

void beepFailed(){
  digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(13,LOW);
    delay(500);

    digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(13,LOW);
    delay(100);

    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
  
  }

char modeSelect(){
  int rVal=analogRead(A0);
  int wVal=analogRead(A1);
  
  if(rVal>wVal){
    return 'r';
  }
  else{
    return 'w';
    }
  
  
  }




void setup(void) {
 analogWrite(A0,0);
 analogWrite(A1,0);
 analogWrite(6,Contrast);
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  lcd.begin(16,2);
  nfc.begin();
  lcd.setCursor(0,0);
  lcd.print("Welcome to UCSC");
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
  delay(3000);
  
}


void loop(void) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select the mode");
  lcd.setCursor(0, 1);
  lcd.print("7 # # # # # # #");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("6 # # # # # #   ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("5 # # # # #     ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("4 # # # #       ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("3 # # #         ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("2 # #           ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("1 #             ");
  delay(1000);
  
//  Serial.println("Enter 'r' for reading mode ");
//  Serial.println("Enter 'w' for writing mode ");
  char incomm=' ';
  //incomm = Serial.read();
//  delay(5000);
  incomm=modeSelect();
  if(incomm=='r'){
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Reading Mode");
    Serial.println("Switching to Card Reading Mode");
    
    beepOne();
   


    
    
    readrfid();
    }
  else if (incomm=='w'){
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Writing Mode");
    Serial.println("Switching to Card Writing Mode");

    beepTwo();
    Serial.println("Switching to Card Writing Mode");
    
    Serial.flush();
    delay(1000);
   
    String url1 ="ucs.cmb.ac.lk";
   

         
    writerfid(url1);
    }
}
