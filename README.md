# MSP430-DAQ
Record data to MSP430 flash and automatically download to CSV

<img src='http://i138.photobucket.com/albums/q267/Bantoregion/logo_transparent_zpsqhwpfeiu.png'>

## Requirements
There are two pieces of software:
* MSP430 code that sends data over USB-Serial using the standard `Serial.print` commands
* Simple application written in <a href='processing.org'>Processing</a> that can read from serial and writes to a file

## Firefly
The original application of this program was to act as a data acquisition system for the MSP430. 
First it is added to a circuit on a rocket. There's plenty of data to track. However it cannot be
reliably sent back to the base station without sophisticated RF equipment. Instead, this data will
be cached on the device. Later, after retrieval, this data needs to be downloaded to a computer
for further analysis. This should be as easy as possible, hence the need for a dedicated application.

## Customizing
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

## Hardware Notes
* The USB-serial connection is currently being done with the MSP430G2553 on the TI Launchpad MSP-EXP430G2 Rev 1.5.
* All of the jumpers on J3 are crossing the RXD/TXD divide

## MSP430Flash
The "MSP430Flash" program uses the <a href='https://github.com/energia/Energia/tree/master/hardware/msp430/libraries/MspFlash'>MspFlash library for Energia</a> which will saves information to the device's flash so that data does not have to be read in realtime.

## Demo
This crude video demonstrates the workflow.

https://www.youtube.com/watch?v=9Qaa0txHvrs
