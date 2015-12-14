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
 
   This project was later modified by Nick Felker, Elijah Neville, and Mike McCaffrey
   The system now uses a variety of sensors and writes them to a predefined CSV file
 */
#include "SPI.h" 
#include "pfatfs.h"

#define cs_pin      6             // chip select pin P1.4 
#define read_buffer 128             // size (in bytes) of read buffer

//GO HERE TO SET CONFIGURATION VARIABLES
int WRITE_SPEED = 1000; //How many milliseconds should be used between writes? (1000Hz is the max thereotical speed, but it's more like 25Hz with write delays)
String filename = "LOG.CSV"; //Make sure the file is preallocated with enough data to store all the logs you want. (See http://43oh.com/2013/12/interfacing-the-launchpad-to-an-sd-card-a-walkthrough/ )
//In Windows' CMD, you can execute the command `fsutil file createnew <filename> 1048576` to create a new file with a specific number of bytes
boolean stopExecutionIfSDError = false; //If your wiring is incorrect,
//or you remove the SD card during progam execution, the system will
//start to fail and will print errors. If you set this to true,
//the program will completely stop working instead of continuing to fail
boolean continueIncrementingIfSDError = true; //If there is a problem, do you want the system to pause incrementing time points, or continue and leave those data as undefined? 
//It depends on whether you want blank data points with the correct time scale (true)
//Or if you want each data point to have its own weight (false)

//Here are some system variables
unsigned short int bw, br;
char buffer[read_buffer];
int rc; //Return code for File I/O. 0 means okay, else means a specific error

char buf[64];
uint32_t counter = 0;
uint32_t AccStringLength = 0;
uint32_t lastWrite = 0;  
boolean SDError = false; //Used for asynchronous operations, can inform if there's a problem with the SD card

void setup() {
    Serial.begin(4800);                // initialize the serial terminal
    Serial.println("Loading...");
    lastWrite = millis(); //We keep storing the lastWrite time for asynchronous SD writing
    analogReference(INTERNAL1V5);
    analogRead(TEMPSENSOR);           // first reading usually wrong
    FatFs.begin(cs_pin, 3);              // initialize FatFS library calls
    Serial.println("**********\r\n MSP430 Temperature Logger \n\r**********\n\r\n\r");
}
void loop() {
  if(millis() - lastWrite > WRITE_SPEED) { //Write only so often, but check in a non-blocking way
    lastWrite = millis();
    write();
  }
}
         
/* Print dying message. This may stop program execution. */    
void die(int pff_err) {
  Serial.print("Failed with rc=");
  Serial.print(pff_err,DEC);
  SDError = true;
  if(stopExecutionIfSDError) {
    for (;;) ; //Endless loop that prevents the system from returning to normal operation
  } else {    
    Serial.println(" but we keep going"); 
    delay(50);
    //Try to reset the FatFs object
    FatFs.begin(cs_pin, 3); 
    delay(50);
  }
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/
void write() {
  //Grab our sensors data
  //GO HERE TO DECLARE YOUR SENSORS
   uint32_t temp = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16; //Formula to temp sensor voltage to Celcius
   uint32_t barometer = ((uint32_t) 3); //We can read from all kinds of sensors here
   Serial.println();
   Serial.println("Opening log file to write data.");
   delay(20);
   
   SDError = false; //We'll be optimisitc and then change if there's still a problem
   rc = FatFs.open(filename.c_str()); //Open a file at the given filename
   if (rc) die(rc); //We may get an SD card error here. Let's not try to write if we cannot.

   if(!SDError || continueIncrementingIfSDError) {
     delay(20); //Delays are placed so the MSP430 has enough time to properly open the file
     bw=0;
     //GO HERE IF YOU WANT TO CHANGE THE FILE OUTPUT
     //To insert a long, use  %lu (lowercase L), %s for a string of characters
     sprintf(buf, "%lu,%lu.%lu,%lu\r\n", counter, temp/10, temp%10, barometer); //Write a CSV row to a buffer
     counter++; //Increment the index of the CSV file
     uint8_t StringLength =  strlen(buf); //Grab the buffer
     Serial.println(buf);        
  
     rc = FatFs.lseek(AccStringLength); //Append to your file instead of overwriting
     if (rc) die(rc);
     AccStringLength =  AccStringLength + 512; 
     rc = FatFs.write(buf, StringLength,&bw); //Start writing the buffer
     if (rc) die(rc);
     rc = FatFs.write(0, 0, &bw);  //Finalize write
     if (rc) die(rc);
     rc = FatFs.close();  //Close file
          if (rc) die(rc);
   }
}


