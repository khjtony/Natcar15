  
// Example by Tom Igoe

import processing.serial.*;

Serial myPort;    // The serial port
volatile int totalcounter=0;

volatile int index=0;
int buffersize= 128*2;
volatile byte[] inBuffer = new byte[buffersize*2];
int data_width=128;
int data_height=0xff;
int middle=data_width*2;
float[] camera_FIR={0.0022082,-0,-0,0,0.014026,-0,-0,0,0.048987,-0,-0,0,0.31509,0.5,0.31509,0,-0,-0,0.048987,0,-0,-0,0.014026,0,-0,-0,0.0022082};
byte threshould=0x40;


//filters:
void convolve(byte Signal[/* SignalLen */], int SignalLen,
              float Kernel[/* KernelLen */], int KernelLen,
              byte Result[/* SignalLen + KernelLen - 1 */])
{
  int n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    int kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += (byte)((float)Signal[k] * (float)Kernel[n - k]);
    }
    
  }
}
//convolve

void Gfilter(byte input[], byte output[], int len){
//using 5th order of Yanghui's Tri for coefficient 1 4 6 4 1
  float[] FIR=camera_FIR;
  convolve(input, len,
           FIR, 27,
           output);

}

void Mfilter(byte input[], byte output[], int len){
  int i=0;
  byte zip[] = new byte[3] ;
  for (i=1;i<len-1;i++){
    zip[0]=input[i-1];
    zip[1]=input[i];
    zip[2]=input[i+1];
    zip=sort(zip);
    output[i]=zip[1];
  }
}

void Cfilter(byte input[], byte output[], int len){
  float[] correlation={0x80,0x80,0x80,0x80,0x20,0x20,0x20,0x20,0x80,0x80,0x80,0x80};
  convolve(input, len,
           correlation, 12,
           output);

}
int camera_edge_detect(byte buffer[]){
    //drop 7 pixels on the both ends
  int i=0;
  int diff=0;
  int thereshold=0x50;
  int max_slope=0;
  int max_pos=0;
  for (i=15;i<128-15;i++){
    diff = buffer[i]- buffer[i+5];
    if (diff>thereshold){
      if (diff>max_slope){
        max_pos=i+2;
        max_slope=diff;
      }
    }
  }
  return max_pos;

}

void Bfilter(byte input[], byte output[], int len){
  int i=0;
  byte zip[] = new byte[3] ;
  for (i=1;i<len-1;i++){
    output[i]=(byte)(int(inBuffer[i])>threshould? 0xff:0) ;
  }
}


void setup() {
  size(data_width*4,data_height*2);
  // Make your own font. It's fun!
  // List all the available serial ports:
  println(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[1], 115200);
  for (int i=0;i>buffersize;i++){
    inBuffer[i]=0x00;
  }

}




void draw() {
  // Twiddle your thumbs
  background(255);
  byte outBuffer[] = new byte[buffersize+40];
  byte postBuffer[] = new byte[buffersize+40];
  strokeWeight(0); //Thinker
  

  line(0,data_height*2-0xff,data_width*4,data_height*2-0xff);
  line(0,data_height*2-0xff/2,data_width*4,data_height*2-0xff/2);
  line(0,data_height*2-2*0x40,data_width*4,data_height*2-2*0x40);
  
  //Mfilter(inBuffer,outBuffer, buffersize);
  //Gfilter(inBuffer,outBuffer, buffersize);
  //Bfilter(inBuffer,outBuffer, buffersize);
  //Cfilter(inBuffer,postBuffer, buffersize);
  //Mfilter(inBuffer,outBuffer, buffersize);
  
  for (int i=6;i<buffersize-6;i++){     
      line(2*(i),data_height*2-int(outBuffer[i]),2*(i+1),data_height*2-int(outBuffer[i+1]));
  }
  for (int i=6;i<buffersize-6;i++){  
      line(2*(i),data_height-int(inBuffer[i]),2*(i+1),data_height-int(inBuffer[i+1]));

  }
  line(0,data_height-0x40,data_width*4,data_height-0x40);
  int x=camera_edge_detect(outBuffer);
  line(2*x,0,2*x,data_height*2);
  delay(10);
}

void serialEvent(Serial myPort) { 
  int item;
  item=myPort.read();
  if ((item)==0x00){
    index=0;
//    inBuffer[index]=(byte)myPort.read();
  }
   else if((item)==0x01){
    index=128;
//    inBuffer[index]=(byte)myPort.read();
    }else if (index<127*2){
      index++;
      inBuffer[index]=(byte)item;
      
    }
} 



