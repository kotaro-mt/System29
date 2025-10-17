import processing.serial.*;

Serial port1;
Serial port2;
Serial port3;

int high_d,low_d,dist;
int high_a,low_a,angle;
int col;


void setup(){
  size(1200,800); //幅1200px,高さ800pxのウインドウを生成
  port1=new Serial(this,"COM7",9600); //Serialクラスのインスタンスを生成
  port1.clear();
  port1.bufferUntil(0x0d);
  println("start");
}

void draw(){
    
}

void SerialEvent(Serial p){
  if(p.available() >= 6){
    if(p.read() == 'H')
    {
        println("OK");
        high_d = p.read();
        low_d = p.read();
        high_a = p.read();
        low_d = p.read();
        col = p.read();
        
        dist = (high_d << 8 ) + low_d;
        angle = (high_a << 8 ) + low_a;
        if( 32767 < dist )
           dist-= 65536; // 65536 = 10^16
        if( 32767 < angle )
           angle-= 65536; // 65536 = 10^16  
        if( 32767 < col )
           col-= 65536; // 65536 = 10^16
           
         angle/=100;
         
         println(dist,angle,col);
    }
  }
}
