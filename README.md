# MSP430-DAQ
Record data with an MSP430 (using either Flash or an SD Card) and automatically download to CSV

<img src='http://i138.photobucket.com/albums/q267/Bantoregion/logo_transparent_zpsqhwpfeiu.png'>

## Firefly
The original application of this program was to act as a data acquisition system for the MSP430. 

First it is added to a circuit on a rocket. There's plenty of data to track. However it cannot be reliably sent back to the base station without sophisticated RF equipment. Instead, this data will be cached on the device. Later, after retrieval, this data needs to be downloaded to a computer for further analysis. This should be as easy as possible, hence the need for a dedicated application.

There are two revisions of Firefly. The first uses internal Flash, which was found to be inadequate for our needs and somewhat buggy. Therefore, the second uses an external micro SD card in order to easily retrieve data.

## Using Flash
### Requirements
There are two pieces of software:
* MSP430 code that sends data over USB-Serial using the standard `Serial.print` commands
* Simple application written in <a href='processing.org'>Processing</a> that can read from serial and writes to a file

### Customizing
If you want to use this for your own projects, go ahead! Regardless of the data you want to record, there's only a few spots 
you need to change it:

### MSP430
On the MSP430, you need to modify your `Serial.println` line to print out the data that you want.

### Laptop
On your laptop, you need to modify the `serialEvent` method, the `writing` condition in the `draw` method, and
probably `output = createWriter("temperatures.csv");` in the `keyPressed` method. Additionally you probably want to rename
the String `temperature`.

### CSV
This system is designed to output to the `CSV` (comma-separated values) format. This is a relatively simple format:

    COLUMN_1, COLUMN_2, COLUMN 3
    DATA_1A,  DATA_2A,  DATA_3A
    DATA_2A,  DATA_3A,  DATA_4A

Each column is comma-separated, and each row is new line-separated. Keep in mind that you're writing raw data, so any
improper new lines or commas will mess up your data.

### Hardware Notes
* The USB-serial connection is currently being done with the MSP430G2553 on the TI Launchpad MSP-EXP430G2 Rev 1.5.
* All of the jumpers on J3 are crossing the RXD/TXD divide

### MSP430Flash
The "MSP430Flash" program uses the <a href='https://github.com/energia/Energia/tree/master/hardware/msp430/libraries/MspFlash'>MspFlash library for Energia</a> which will saves information to the device's flash so that data does not have to be read in realtime.

### Demo
This crude video demonstrates the workflow.

https://www.youtube.com/watch?v=9Qaa0txHvrs

## Using SD Card
The SD card software allows the system to asynchronously write any sort of string based data to a file on an SD card. If the SD card is pulled out while the program is running, the system will do one of two things (depending on configuration parameters).

Note that the MSP430 will begin writing data from scratch each time it starts. Pressing reset will do the same thing. It will continue to clear pre-existing data.

Also of note is the fact that the file must be pre-allocated with the amount of storage you want it to contain. If the file is only 1024 bytes long, only two log entries will be recorded (each log is 512 bytes). In Windows, you can create a file of any size using the command:

`fsutil file createnew LOG.TXT 1048576`

In this example, I'm creating the file `LOG.TXT` and it has a size of 1MB. See <a href='http://43oh.com/2013/12/interfacing-the-launchpad-to-an-sd-card-a-walkthrough/'>this page to learn more.</a>

This program comes with some error handling that you can change with configuration variables. You can easily take out the SD card at any point and plug into a computer. The action that the MSP430 will take depends.

* Stop altogether (the program will not do anything else until it resets)
* Continue checking for the SD card to be returned (and once it is fixed, it will continue to write)
    * Another configuration variable specifies whether the counter should continue to increment if there is an error

### Requirements
The only software necessary on will be the MSP430 code, which should be modified based on your DAQ parameters (error handling, write speed, etc.)

However, you will need additional hardware support for this system to work. 

### Hardware
The MSP430 can interface with an SD card using the SPI (Serial Peripheral Interface) protocol. The pinouts are shown below for both a Micro SD or a standard size SD card.

| MSP430 | Micro SD | Standard SD |
| :---   | :---     | :---        |
| CS 1.4 | Pin 2    | Pin 1       |
| Din 1.7| Pin 3    | Pin 2       |
| CLK 1.5| Pin 5    | Pin 5       |
| Dout 1.6|Pin 7    | Pin 6       |
| VCC    | Pin 4    | Pin 4       |
| GND    | Pin 6    | Pins 3 & 6  |

### Customization
There are a few key areas where you can modify code for your needs without affecting the rest of the system.

#### Write Speed
You can change the frequency of a write by changing the following variable.

`int WRITE_SPEED = 1000` 

It is the minimum number of milliseconds between each write.

#### Filename
You can change the filename accessed by changing the following variable.

`String filename = "LOG.CSV";`

#### Error Handling
As the MSP430 is continuing to run its DAQ program, you pull out the SD card. What should the MSP430 do?

* Stop processing altogether
    * `stopExecutionIfSDError = true`
* Keep checking if the SD card is returned and then resume processing AND don't increment the counter in the meantime
    * `stopExecutionIfSDError = false`
    * `continueIncrementingIfSDError = false`
* Keep checking if the SD card is returned and then resume processing WHILE incrementing the counter in the meantime
    * `stopExecutionIfSDError = false`
    * `continueIncrementingIfSDError = true`
    
#### Sensor Inputs
Go to the `write()` method of the program. At the top of this method, you should create new variables for all of your sensors. Use methods like `analogRead` to get the current values.

Next, go to the `sprintf` function. We'll be creating a formatted string that will be saved to the variable `buf`. The format that you specify is what will be saved to a row in the file.

You can learn more about the intracacies of `sprintf` <a href='www.cplusplus.com/reference/cstdio/sprintf/'>here,</a> but here's a few quick pointers.

    sprintf(buf, "%lu,%lu.%lu,%lu\r\n", counter, temp/10, temp%10, barometer);
    
The percent, L, and u (`%lu`) mean that we'll be putting in that spot an unsigned long. If you wanted to insert a string, you could use `%s`.

Keep in mind that the `CSV` file format requires each value in a row to be separated by a comma. And a newline `\r\n` (or `\n\r`) must be at the end.

Now when you run it, the end result will be an actual `CSV` that can be opened up in Microsoft Excel or any other spreadsheet tool.