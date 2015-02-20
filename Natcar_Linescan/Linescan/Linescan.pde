  
// Example by Tom Igoe

import processing.serial.*;

Serial myPort;    // The serial port
PFont myFont;     // The display font
String inString;  // Input string from serial port
int lf = 10;      // ASCII linefeed
int buffersize= 128;
byte[] inBuffer = new byte[buffersize];

void setup() {
  size(600,600);
  // Make your own font. It's fun!
  // List all the available serial ports:
  println(Serial.list());
  // Open the port you are using at the rate you want:
 
  myPort.buffer(buffersize);

}

void draw() {
  // Twiddle your thumbs
  background(255);
  
//  myPort.readBytes(inBuffer);
//  for (int i=0;i<buffersize-130;i++){
//    
//    if ((int(inBuffer[i])== 0)){
////          println("YES!");
//        for (int j=i+3;j<i+3+128;j++){
//          strokeWeight(4);  // Thicker
//         
//          
//          line(2*(j-3-i),600-2*int(inBuffer[j]),2*(j-3+1-i),600-2*int(inBuffer[j+1]));
//          //H line 600-2*128
//          line(0,600-2*128,600,600-2*128);
//          //H line 600-2*255
//          line(0,600-2*255,600,600-2*255);
//          //H line 600-2*40
//          line(0,600-2*60,600,600-2*60);
//        
//        }
//        return;
//        }
//        
//}
  while (int(myPort.read()) != 0x00){
  }
  strokeWeight(4); //Thinker
  myPort.readBytes(inBuffer);
  //H line 600-2*128
          line(0,600-2*128,600,600-2*128);
//          //H line 600-2*255
          line(0,600-2*255,600,600-2*255);
//          //H line 600-2*40
          line(0,600-2*60,600,600-2*60);
  for (int i=0;i<buffersize-1;i++){        
      line(3*(i),600-2*int(inBuffer[i]),3*(i+1),600-2*int(inBuffer[i+1]));
  }
  myPort.clear();
  return;
}



