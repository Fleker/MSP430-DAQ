#include "MspFlash.h"
#include <Energia.h>
#include <msp430.h>

// Two options to use for flash: One of the info flash segments, or a part of the program memory
// either define a bit of constant program memory, and pass a pointer to the start of a segment to the flash functions,

//*** Option 2: use one of the 64 byte info segments, uncomment this line. Use SEGMENT_B, SEGMENT_C or SEGMENT_D (each 64 bytes, 192 bytes in total)
#define flash SEGMENT_D
int flashOffset = 0;
//
int FIREFLY = 1;
unsigned char cr = 13;
unsigned char* carriageReturn = &cr;

#define IR 5
#define RED 6
#define GREEN 7
#define BLUE 8

bool RGB;

enum color{Red, Green, Blue};
color RGBColor;

int lastFlashWrite;


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(4800);
  Serial.println("Welcome");
  pinMode(IR, OUTPUT);
	pinMode(RED,OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);

	RGBColor = Red;
	RGB = false;
  lastFlashWrite = millis();  
}

void loop() {
  // put your main code here, to run repeatedly: 
  //Serial.println("1234");
  if(FIREFLY == 1) {
    if(millis() - lastFlashWrite > 1000 && millis() > 60000) {
      doWrite();
      lastFlashWrite = millis();
    }
  } else {
    if ( Serial.available() ) {
      Serial.println("...");
      switch ( Serial.read() )
      { 
        case 'e': doErase(); break;  
        case 'r': doRead(); break; 
        case 'w': doWrite(); break; 
        case 10:
        case 13: break;
        default: doHelp();
      }
    } else {
       //Serial.println("Not available"); 
    }
  }
  ledStuff();
}
void ledStuff() {
   if (millis() % 1000 < 200) // Flash LED for 1/5 second
		RGB = true;
	else RGB = false;

	if (millis() % 66 == 0){ // Split flash between 3 colors
		if (RGBColor == Red)
			RGBColor = Blue;
		else if (RGBColor == Blue)
			RGBColor = Green;
		else if (RGBColor == Green)
			RGBColor = Red;
	}

	if (RGB){ // Control LED pins
		switch (RGBColor){
		case Red: digitalWrite(RED, LOW);
                                 digitalWrite(GREEN,HIGH);
                                 digitalWrite(BLUE,HIGH);
		case Green: digitalWrite(RED, HIGH);
                                    digitalWrite(GREEN,LOW);
                                    digitalWrite(BLUE,HIGH);
		case Blue: digitalWrite(RED, HIGH);
                                  digitalWrite(GREEN,HIGH);
                                  digitalWrite(BLUE,LOW);
		}
	}
        else {
         digitalWrite(RED, HIGH);
         digitalWrite(GREEN,HIGH);
         digitalWrite(BLUE,HIGH); 
        }
        
	/*if (millis()%1000 > 900) // flash IR 
		digitalWrite(IR, LOW);
	else digitalWrite(IR, HIGH);*/
 
}

void doRead()
{
  unsigned char p = 0;
  int i=0;
  Serial.println("Read:");
  do
  {
    Flash.read(flash+i, &p, 1);
    Serial.write(p);
    Serial.print(":");    
    Serial.println(p);
  } while ( p && (i++ < 16) );
  Serial.println(".");
}


void doErase()
{
 Serial.println("Erase"); 
 Flash.erase(flash); 
 Serial.println("Done."); 
}

void doWrite()
{
 Serial.println("Write");
  uint32_t temp = analogRead(TEMPSENSOR);
   Serial.println(temp);
 //String data = String(flashOffset)+","+String(kelvin);
 String data = String(temp);
 Serial.println(data);
 for(int c=0; c<data.length(); c++) {
   //unsigned char* charizard[32];
   //data.toCharArray(charizard, data.length());
   //Flash.write(flash+flashOffset, (unsigned char*) charizard, data.length()); 
  unsigned char a = data.charAt(c);   
  unsigned char* charizard = &a;
  Serial.print("..>");
  Serial.write(a);
  Serial.print(":");
  Serial.println(a);
   Flash.write(flash+flashOffset, charizard, 1);
   flashOffset++;
 }
 //Flash.write(flash+flashOffset, carriageReturn, 1); //Write a NL char
 //flashOffset++;
 Serial.println("Done.");
}

void doHelp()
{
  int div = (F_CPU/400000L) & 63; 
  Serial.println("flash test: e, r, w");
  //Serial.println(flash);
  Serial.println(F_CPU);
  Serial.println(div); 
}
