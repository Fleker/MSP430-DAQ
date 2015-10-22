import processing.serial.*;

String COM_PORT = "COM15";

Serial port;
float brightness = 0;
PrintWriter output;
boolean writing;
boolean setup;
int time;
String temperature;

float pulsing = 1;

void setup() {
  size(640,480);
  output = createWriter("temperatures.csv");
  setup = true;
}

void draw() {
  background(writing?0:brightness,writing?brightness*3:0,0);
  if(setup) {
    brightness += pulsing;
    if(brightness > 32)
      pulsing = -0.5;
    else if(brightness <= 0)
      pulsing = 1;
  } else if(writing) {
     brightness = 0;
  }
  
  if(writing) {
     textSize(16);
     text("Writing serial data", 10, 30);
     text(temperature, 10,50);
     text("Press ENTER to stop recording", 10, 130);
  } else if(setup) {
     textSize(16);
     text("Waiting for MSP430...", 10, 30);
     text("Press SHIFT to start recording", 10, 50);
  }
  textSize(10);
//  text("Writing to <sketch>/temperatures.csv", 400, 468);
  text("Writing to <MSP430-DAQ>/WinFirefly/Firefly/temperatures.csv", 300, 468);
}

void serialEvent (Serial port) {
  temperature = port.readString();
  writing = true;
  output.print(temperature);
}
void keyPressed() {
  if(keyCode == ENTER || keyCode == RETURN) {  
    writing = false;
    output.flush();
    output.close();
    exit();
  } else if(keyCode == SHIFT) {
    setup = false;
    port = new Serial(this, COM_PORT, 4800); 
    // select the COM port shown bellow in the energia IDE 
    port.bufferUntil('\n');
    output = createWriter("temperatures.csv");
  }
}