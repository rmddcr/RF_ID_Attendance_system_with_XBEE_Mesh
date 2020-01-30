#include <SoftwareSerial.h>

SoftwareSerial xbee(A4, A5); // RX, TX
char c = 'A';
int  pingPong = 1;

void setup()
{
    Serial.begin(115200);

    Serial.println( "Arduino started sending bytes via XBee" );

    //Set the data rate for the SoftwareSerial port
    xbee.begin(9600);
    
    delay( 1000 );
   
    
}

void loop() {
     xbee.flush();
     xbee.write( "#1331ucsc.cmb.ac.lk^" );
     Serial.println("#1331ucsc.cmb.ac.lk^");

String c="";
char tmp=' ';
  if(xbee.available() && xbee.read() == '#'){
 while(tmp!='^'){
    
    if (xbee.available())
    {

     
      if(tmp!=' '){
      c.concat(tmp);
      }
      
      tmp= xbee.read();
      
     
    }
  }

  }




 if(c!=""){ 
  Serial.println();
  Serial.print("RFid Card no is : ");
     Serial.print(c);
     Serial.println();
     
 }
    
   
   


    //--- Switch LED on Arduino board for every character sent---
    if ( pingPong == 0 )
        digitalWrite(13, LOW);
    else
        digitalWrite(13, HIGH);
    pingPong = 1 - pingPong;
    
    delay( 1000 );
    
}
