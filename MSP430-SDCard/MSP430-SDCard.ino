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
//#include <SPI.h>
//#include "pfatfs.h"
#include <Adafruit_BMP280.h>
//#include "bmp280.h"

#define cs_pin      6             // chip select pin P1.4 
#define read_buffer 128             // size (in bytes) of read buffer

#define GREEN 10 
#define RED 11
#define BLUE 12

//GO HERE TO SET CONFIGURATION VARIABLES
//int WRITE_SPEED = 50; //How many milliseconds should be used between writes? (1000Hz is the max thereotical speed, but it's more like 25Hz with write delays)
//String filename = "LOG.CSV"; //Make sure the file is preallocated with enough data to store all the logs you want. (See http://43oh.com/2013/12/interfacing-the-launchpad-to-an-sd-card-a-walkthrough/ )
//In Windows' CMD, you can execute the command `fsutil file createnew <filename> 1048576` to create a new file with a specific number of bytes
//boolean stopExecutionIfSDError = false; //If your wiring is incorrect,
//or you remove the SD card during progam execution, the system will
//start to fail and will print errors. If you set this to true,
//the program will completely stop working instead of continuing to fail
//boolean continueIncrementingIfSDError = true; //If there is a problem, do you want the system to pause incrementing time points, or continue and leave those data as undefined? 
//It depends on whether you want blank data points with the correct time scale (true)
//Or if you want each data point to have its own weight (false)

//Here are some system variables
unsigned short int bw, br;
char buffer[read_buffer];
int rc; //Return code for File I/O. 0 means okay, else means a specific error

char buf[64];
uint16_t counter = 0;
uint16_t AccStringLength = 0;
uint16_t lastWrite = 0;  
boolean SDError = false; //Used for asynchronous operations, can inform if there's a problem with the SD card

//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
Adafruit_BMP280 bme(10, 11, 12,  13);

void setup() {
    //Serial.begin(4800);                // initialize the serial terminal
    //Serial.println("Loading...");
    lastWrite = millis(); //We keep storing the lastWrite time for asynchronous SD writing
    analogReference(INTERNAL1V5);
    //analogRead(TEMPSENSOR);           // first reading usually wrong
    FatFs.begin(cs_pin, 3);              // initialize FatFS library calls
    //Serial.println("**********\r\n MSP430 Temperature Logger \n\r**********\n\r\n\r");
   
    if (!bme.begin()) {  
    //  Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
    }
    
      // put your setup code here, to run once:
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(RED, OUTPUT);
    
    digitalWrite(GREEN,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(BLUE,LOW);
}
void loop() {
  if(millis() - lastWrite > 50) { //Write only so often, but check in a non-blocking way
    lastWrite = millis();
    write();
  }
  LED_STUFF();
}
         
/* Print dying message. This may stop program execution. */    
void die(int pff_err) {
  //Serial.print("Failed with rc=");
  //Serial.print(pff_err,DEC);
  SDError = true;
     
    //Serial.println(" but we keep going"); 
    delay(50);
    //Try to reset the FatFs object
    FatFs.begin(cs_pin, 3); 
    delay(50);
  
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/
void write() {
  //Grab our sensors data
  //GO HERE TO DECLARE YOUR SENSORS
   //uint32_t temp = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16; //Formula to temp sensor voltage to Celcius
   uint8_t temp2 = bme.readTemperature();
   uint8_t pressure = bme.readPressure();
   //uint32_t altitude = bme.readAltitude(1013.25); NO ALTITUDE (pow is missing??) but use the algorithm l8r
   //uint32_t temp2 = ((uint32_t) 3);
   //uint32_t pressure = ((uint32_t) 4);
   //uint32_t altitude = ((uint32_t) 5);
   //Serial.println();
   //Serial.println("Opening log file to write data.");
   delay(20);
   
   SDError = false; //We'll be optimisitc and then change if there's still a problem
   rc = FatFs.open("LOG.CSV"); //Open a file at the given filename
   if (rc) die(rc); //We may get an SD card error here. Let's not try to write if we cannot.

   //if(!SDError) {
     delay(20); //Delays are placed so the MSP430 has enough time to properly open the file
     bw=0;
     //GO HERE IF YOU WANT TO CHANGE THE FILE OUTPUT
     //To insert a long, use  %lu (lowercase L), %s for a string of characters
     //sprintf(buf, "%lu,%lu.%lu,%lu,%lu,%lu\r\n", counter, temp/10, temp%10, temp2, pressure, altitude); //Write a CSV row to a buffer
     sprintf(buf, "%lu,%lu,%lu\r\n", counter, temp2, pressure); //Write a CSV row to a buffer
     
     counter++; //Increment the index of the CSV file
     uint8_t StringLength =  strlen(buf); //Grab the buffer
     //Serial.println(buf);        
  
     rc = FatFs.lseek(AccStringLength); //Append to your file instead of overwriting
     if (rc) die(rc);
     AccStringLength =  AccStringLength + 512; 
     rc = FatFs.write(buf, StringLength,&bw); //Start writing the buffer
     if (rc) die(rc);
     rc = FatFs.write(0, 0, &bw);  //Finalize write
     if (rc) die(rc);
     rc = FatFs.close();  //Close file
          if (rc) die(rc);
   //}
}

void LED_STUFF(){
  
  int PULSE_MILS = 50;
  int COLOR = 0;
    
  if (millis()%500 > PULSE_MILS){
    COLOR++;
    if (COLOR%3 == 0){
      digitalWrite(RED,HIGH);
      digitalWrite(BLUE,LOW);
      digitalWrite(GREEN,LOW);
    }
    else if (COLOR%3 == 1){
      digitalWrite(RED,LOW);
      digitalWrite(BLUE,HIGH);
      digitalWrite(GREEN,LOW);
    }
    else {
      digitalWrite(RED,LOW);
      digitalWrite(BLUE,LOW);
      digitalWrite(GREEN,HIGH);
    }
  }
  else{
      digitalWrite(RED,LOW);
      digitalWrite(BLUE,LOW);
      digitalWrite(GREEN,LOW);
    }
  }

/*#define SPI_BUFFER_LEN 5
#define BUFFER_LENGTH	0xFF
#define	MASK_DATA	0x80
#define REGISTER_MASK	0x7F
struct bmp280_t bmp280;
s8 SPI_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bmp280 the following structure parameter can be accessed
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *--------------------------------------------------------------------------*

	bmp280.bus_write = BMP280_SPI_bus_write;
	bmp280.bus_read = BMP280_SPI_bus_read;
	bmp280.delay_msec = BMP280_delay_msek;

        BMP_SPI_bus_write(0xF5,

	return BMP280_INIT_VALUE;
}
s8  BMP280_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError=BMP280_INIT_VALUE;
	u8 array[SPI_BUFFER_LEN]={BUFFER_LENGTH};
	u8 stringpos;
	/*	For the SPI mode only 7 bits of register addresses are used.
	The MSB of register address is declared the bit what functionality it is
	read/write (read as 1/write as BMP280_INIT_VALUE)*
	array[BMP280_INIT_VALUE] = reg_addr|MASK_DATA;/*read routine is initiated register address is mask with 0x80*/
	/*
	* Please take the below function as your reference for
	* read the data using SPI communication
	* " IERROR = SPI_READ_WRITE_STRING(ARRAY, ARRAY, CNT+1)"
	* add your SPI read function here
	* iError is an return value of SPI read function
	* Please select your valid return value
	* In the driver SUCCESS defined as BMP280_INIT_VALUE
    * and FAILURE defined as -1
	* Note :
	* This is a full duplex operation,
	* The first read data is discarded, for that extra write operation
	* have to be initiated. For that cnt+1 operation done in the SPI read
	* and write string function
	* For more information please refer data sheet SPI communication:
	*
	for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos+BMP280_ONE_U8X];
	}
	return (s8)iError;
}

/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 *
s8  BMP280_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP280_INIT_VALUE;
	u8 array[SPI_BUFFER_LEN * BMP280_TWO_U8X];
	u8 stringpos = BMP280_INIT_VALUE;
	for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++) {
		/* the operation of (reg_addr++)&0x7F done: because it ensure the
		   BMP280_INIT_VALUE and 1 of the given value
		   It is done only for 8bit operation*
		array[stringpos * BMP280_TWO_U8X] = (reg_addr++) & REGISTER_MASK;
		array[stringpos * BMP280_TWO_U8X + BMP280_ONE_U8X] = *(reg_data + stringpos);
	}
	/* Please take the below function as your reference
	 * for write the data using SPI communication
	 * add your SPI write function here.
	 * "IERROR = SPI_WRITE_STRING(ARRAY, CNT*2)"
	 * iError is an return value of SPI write function
	 * Please select your valid return value
	 * In the driver SUCCESS defined as BMP280_INIT_VALUE
     * and FAILURE defined as -1 
	 *
	return (s8)iError;
}
*/
