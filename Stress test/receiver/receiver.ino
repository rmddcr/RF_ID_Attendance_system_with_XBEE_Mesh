#include <SoftwareSerial.h>

SoftwareSerial xbee(A4, A5); // RX, TX


void setup()
{
    Serial.begin(9600);

    Serial.println( "Arduino started receiving bytes via XBee" );

    //Set the data rate for the SoftwareSerial port
    xbee.begin(9600);
    
    delay( 1000 );
   
    
}

void loop() {
Serial.println(xbee.readString());


}
