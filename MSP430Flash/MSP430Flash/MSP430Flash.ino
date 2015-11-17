#include "MspFlash.h"
#include <Energia.h>
#include <msp430.h>

// Two options to use for flash: One of the info flash segments, or a part of the program memory
// either define a bit of constant program memory, and pass a pointer to the start of a segment to the flash functions,

//*** Option 2: use one of the 64 byte info segments, uncomment this line. Use SEGMENT_B, SEGMENT_C or SEGMENT_D (each 64 bytes, 192 bytes in total)
#define flash SEGMENT_D
//

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly: 
  if ( Serial.available() )
  {
    switch ( Serial.read() )
    { 
      case 'e': doErase(); break;  
      case 'r': doRead(); break; 
      case 'w': doWrite(); break; 
      case 10:
      case 13: break;
      default: doHelp();
    }
  }  
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
 Flash.write(flash, (unsigned char*) "Hello World!" ,13); 
 Serial.println("Done.");
}

void doHelp()
{
  int div = (F_CPU/400000L) & 63; 
  Serial.println("flash test: e, r, w");
  Serial.println(flash);
  Serial.println(F_CPU);
  Serial.println(div); 
}
