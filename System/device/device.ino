#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
SoftwareSerial xbee(A4, A5); // RX, TX
int Contrast=75;
int k =0;int m =0;





#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void change_keys(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;
  uint8_t trailer[16] = { 0xA1, 0xA1, 0xA1, 0xA3, 0xA4, 0xA5, 0xFF, 0x07, 0x80, 0x00,0xA1, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
  uint8_t keyc[6] = {0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF};
  
  Serial.println("Tap Your NFC Card");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
    if (success) 
    {
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(uid, uidLength);
      Serial.println("");
      
      if (uidLength != 4)
      {
        Serial.println("Ooops ... this doesn't seem to be a Mifare Classic card!"); 
        return;
      }
      
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

   for(int block = 11; block < 20; block = block + 4){
    Serial.println(block);
      success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, block, 0, keyc);
    
   if (!success){
      Serial.println("Authentication Failed");
      return;
      }
          
    Serial.println("Authentication succeeded");

    success = nfc.mifareclassic_WriteDataBlock(block,trailer);
     
    if (success)
    {
      Serial.println("Key Change Success");
      Serial.println("");      
    }
    else
    {
      Serial.println("Key Change Failed");
    }
    
    }
  
  }

}

String readrfid(){
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
      char id_str[16]; 
      char id_str_xbee[16];
      sprintf(id_str, "%02d%02d%02d%02d", uid[0], uid[1], uid[2],uid[3]);
      sprintf(id_str_xbee, "#%02d%02d%02d%02d^", uid[0], uid[1], uid[2],uid[3]);
      
      Serial.print("Card id number ");
      Serial.println(id_str);
      Serial.print("Xbee card id number ");
      Serial.println(id_str_xbee);
      beepSuc();


      
      xbee.flush();
      delay(1000);
      xbee.write(id_str_xbee);

      Serial.println("send data to XBEE");
      
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Card Num:");
      lcd.setCursor(3,1);
      lcd.print(id_str);
      
     
      
      delay(3000);
 
    
    
      
      return id_str;
    }
    Serial.println("read done");
    return "error";
  }
  delay(1000);
  return "error";
  

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


    
    

  }
  
  delay(2000);

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


  void beepSuc(){

  tone(13, 2000,100);
  delay(100);
  tone(13, 6000,200);
  delay(100);
  tone(13, 2000,100);
  }




void beepOne(){//read

  tone(13, 2000,1000);
  }




void beepTwo(){//write

  tone(13, 3000,500);
  delay(500);
  tone(13, 570,500);
  }


void coutdown(void){
  
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
 
  }
  

void setup()
{
  analogWrite(A0,0);
  analogWrite(A1,0);
  analogWrite(6,Contrast);
  Serial.begin(115200);
  pinMode(13,OUTPUT);

  nfc.begin();
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
  lcd.begin(16,2);
  
  lcd.setCursor(0,0);
  lcd.print("Welcome to UCSC");
    Serial.println( "Arduino started receiving bytes via XBee" );

    // Set the data rate for the SoftwareSerial port.
    xbee.begin(9600);
    delay(1000);
   
   
}

void loop()  {
 coutdown();
//change_keys();

  delay(1000);
char mode=' ';
  mode=modeSelect();



String url="";
char tmp=' ';
  if(xbee.available() && xbee.read() == '#'){
     while(tmp!='^'){
        if (xbee.available())
          {
            if(tmp!=' '){
              url.concat(tmp);
              }
               tmp= xbee.read();
          }
       }

    }

 Serial.println(url);
 if(mode=='r'){
      lcd.clear();
      lcd.print("Read mode");
       delay(1500);
        lcd.clear();
      lcd.print("Insert a card");
       delay(2000);
       
      String id = readrfid();
           
      Serial.println(id);
     }

  
 if(mode=='w'){ 
  lcd.clear();
   lcd.print("Write mode");
   delay(1500);
   lcd.clear();
   lcd.print("Connecting.....");
    delay(2000);
    lcd.clear();
       lcd.print("To server.....");

    delay(3000);
    
    if(url.substring(0,4) == "1331"){
      lcd.clear();
      lcd.print("Ready to write");
      url=url.substring(4,url.length());
      Serial.println(url);
      writerfid(url);   

    

    }
    
     
 }

   

}
