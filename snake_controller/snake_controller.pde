import processing.serial.*;

Serial myPort;  // Create object from Serial class
char value;

void setup()
{
  size(200,200);
  String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200);
}

void draw() {
  if (keyPressed) {
      if(key == 10){  //ENTER
        value = '1';
      } else if(key == 97){ //a
        value = '2';
      } else if(key == 115){ //s
        value = '3';
      } else if(key == 100){ //d
        value = '4';
      } else if(key == 119){ //w
        value = '5';
      } else if(key == 8){ //ESC
        value = '6';
      }
      myPort.write(value);
      delay(200);
    }
}
