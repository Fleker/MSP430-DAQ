#include <pfatfs.h>
#include <pffconf.h>

/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
   Ported to Energia by calinp
   Copy the file LOG.txt from this file's location to the root of the `
   SD-Card.

   12/21/2013
   Modified by bluehash(43oh) to log temperature data from the MSP430G2553
   internal temperature sensor and log it to a file on the SD-Card
   References:
   http://forum.43oh.com/topic/3209-energia-library-petit-fatfs-sd-card-library/
 */

 
#include "SPI.h" 
#include "pfatfs.h"

#define cs_pin      6             // chip select pin P1.4 
#define read_buffer 128             // size (in bytes) of read buffer 
#define LOG_DELAY   5000          // 5000ms -> 5sec

unsigned short int bw, br;//, i;
char buffer[read_buffer];
int rc;
DIR dir;				/* Directory object */
FILINFO fno;			/* File information object */

uint32_t ui32_ReadTemp = 0;
uint32_t ui32_Barometer = 0;
uint8_t StringLength = 0;
char buf[30];
uint32_t counter = 0;
uint32_t AccStringLength = 0;
uint32_t lastWrite = 0;
int WRITE_SPEED = 1000; //Write once for second / once per 1000 ms

void setup() {
    Serial.begin(4800);                // initialize the serial terminal
    Serial.println("setup()");
    lastWrite = millis(); //We keep storing the lastWrite time for asynchronous SD writing
    setup2();
}
void loop() {
  Serial.println("Hi eli");
  if(millis() - lastWrite > WRITE_SPEED) {
    lastWrite = millis();
    write();
  }
  delay(500); 
}
void setup2()
{
   pinMode(PUSH2, INPUT_PULLUP);
   Serial.println("Reference repeated");
   analogReference(INTERNAL1V5);
   analogRead(TEMPSENSOR);           // first reading usually wrong
   FatFs.begin(cs_pin, 3);              // initialize FatFS library calls
   Serial.print("**********\r\n MSP430 Temperature Logger \n\r**********\n\r\n\r");
}
         
/* Stop with dying message */    
void die(int pff_err) {
  Serial.print("Failed with rc=");
  Serial.print(pff_err,DEC);
  Serial.println(" but we keep going");
}
void die2 ( int pff_err	)
{
   Serial.println();
   Serial.print("Failed with rc=");
   Serial.print(pff_err,DEC);
   for (;;) ;
}

void printDec(uint32_t ui) 
{
   Serial.print(ui/10, DEC);
   Serial.print(".");
   Serial.print(ui%10, DEC);
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/
void write() {
  //Grab our sensors data
   ui32_ReadTemp = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16;
   ui32_Barometer = ((uint32_t) 3);
   Serial.print(ui32_ReadTemp);

   Serial.println();
   Serial.println("Opening log file to write temperature(LOG.txt).");
   delay(100);
   
   rc = FatFs.open("LOG.TXT");
   if (rc) die(rc);

   delay(100);
   bw=0;
   //sprintf( buf, "%lu Current temperature is %lu.%lu\r\n", counter, ui32_ReadTemp/10, ui32_ReadTemp%10 );
   //buf = String(counter) + "," + String(ui32_ReadTemp/10)+ "." + String(ui32_ReadTemp%10) + ",0\r\n";
   //sprintf(buf, "%1u,%1u.%1u,%1u\r\n", counter, ui32_ReadTemp/10, ui32_ReadTemp%10, 0);
   sprintf(buf, "%lu,%lu.%lu,%lu\r\n", counter, ui32_ReadTemp/10, ui32_ReadTemp%10, ui32_Barometer);
   counter++;
   StringLength =  strlen(buf);
   Serial.println(buf);        

   //Serial.println(StringLength, DEC);
   //Serial.println(AccStringLength, DEC);

   rc = FatFs.lseek(AccStringLength);
   if (rc) die(rc);
   AccStringLength =  AccStringLength + 512;
   rc = FatFs.write(buf, StringLength,&bw);
   if (rc) die(rc);
   rc = FatFs.write(0, 0, &bw);  //Finalize write
   if (rc) die(rc);
   rc = FatFs.close();  //Close file
        if (rc) die(rc);

   // Log delay
   //delay(LOG_DELAY);
}


