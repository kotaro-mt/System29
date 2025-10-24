import processing.serial.*;

Serial port1;
Serial port2;
Serial port3;

int high_d,low_d,dist;
int high_a,low_a,angle;
int high_m,low_m,mode;
int col;


void setup(){
  size(1200,800); //幅1200px,高さ800pxのウインドウを生成
  port1=new Serial(this,"COM9",9600); //Serialクラスのインスタンスを生成
  port1.clear();
  port1.bufferUntil(0x0d);
}

void draw(){
    
}

void serialEvent(Serial p){
  if(p.available() >= 8){
    if(p.read() == 'H')
    {
        println("OK");
        high_d = p.read();
        low_d = p.read();
        high_a = p.read();
        low_a = p.read();
        high_m = p.read();
        low_m = p.read();
        col = p.read();
        
        dist = (high_d << 8 ) + low_d;
        angle = (high_a << 8 ) + low_a;
        mode = (high_m << 8 ) + low_m;

        if( 32767 < dist )
           dist-= 65536; // 65536 = 10^16
        if( 32767 < angle )
           angle-= 65536; // 65536 = 10^16  
        if( 32767 < mode )
           mode-= 65536; // 65536 = 10^16
           
         angle/=100;
         
         println(dist,angle,mode,col);
    }
  }
}
