#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//Defining Joystick 1 and Joystick 2 connections
const int J1S = 2;
const int J1X = 0;
const int J1Y = 1;
const int J2S = 3;
const int J2X = 2;
const int J2Y = 3;

String stringj1x="0";
String stringj1y="0";
String stringj2x="0";
String stringj2y="0";
char stringhex[6]="0a00;";
char receivedControls[6]="00a0;";


RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


void setup(void)
{
  //Start serial communication and print info
  Serial.begin(57600);
  printf_begin();
  Serial.println("-- RF_transmitter - Transmitter Radio Board --");

  radio.begin();
  radio.setRetries(15,15);

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();
  radio.printDetails();
}

void loop(void)
{
  
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    int time = analogRead(J1X);
    time=map(time,0,1022,0,15);
    stringj1x=String(time,HEX);
    time = analogRead(J1Y);
    time=map(time,0,1022,0,15);
    stringj1y=String(time,HEX);
    time = analogRead(J2X);
    time=map(time,0,1022,0,15);
    stringj2x=String(time,HEX);
    time = analogRead(J2Y);
    time=map(time,0,1022,0,15);
    stringj2y=String(time,HEX);
    
    stringhex[0]=stringj1x[0];
    stringhex[1]=stringj1y[0];
    stringhex[2]=stringj2x[0];
    stringhex[3]=stringj2y[0];
    stringhex[4]=';';

    Serial.print("Posicoes: ");
    Serial.print(stringhex);
    bool ok = radio.write( &stringhex, sizeof(stringhex));
    
    if (ok) 
      Serial.print(" OK! ");
      
    else
      Serial.print(" FALHOU! ");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    if ( timeout )
      Serial.println("Tempo expirado.");

    else
    {
      // Grab the response, compare, and send to debugging spew
      
      radio.read( &receivedControls, sizeof(receivedControls));
      
      Serial.print("Resposta: ");
      Serial.println(receivedControls);
    }

    // Try again 1s later
    delay(1000);
  

}
