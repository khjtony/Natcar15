import processing.serial.*;

Serial myPort;

int x=128*2;
int y=0;

void setup(){
  size(256,0xff);
  myPort= new Serial(this,Serial.list()[1],115200);
}


void draw(){
  background(204);
  byte[] inBuffer = new byte[40];
  while (myPort.available()>0){
    myPort.readBytes(inBuffer);
    if (inBuffer != null) {
      String myString = new String(inBuffer);
      println(myString);
  }

}
}
