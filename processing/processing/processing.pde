import processing.serial.*;

Serial port1;
Serial port2;
Serial port3;

int dist1,dist2,dist3;
float angle1,angle2,angle3;
int mode1,mode2,mode3;
int col1,col2,col3;

int zumo_id = 0;


void setup(){
  size(400,400); //幅1200px,高さ800pxのウインドウを生成
  
  port1=new Serial(this,"COM3",9600); //Serialクラスのインスタンスを生成
  port1.clear();
  port1.bufferUntil('\n');
  
  //port2=new Serial(this,"COM10",9600); //Serialクラスのインスタンスを生成
  //port2.clear();
  //port2.bufferUntil('\n');
  
  //port3=new Serial(this,"COM11",9600); //Serialクラスのインスタンスを生成
  //port3.clear();
  //port3.bufferUntil('\n');
}

void draw(){
  background(0);
}

void serialEvent(Serial p){
  if(p.available()>=8){
      if(p.read() == 'H'){
        int high_d = p.read();
        int low_d  = p.read();
        int high_a = p.read();
        int low_a  = p.read();
        int high_m = p.read();
        int low_m  = p.read();
        
        if(zumo_id==1){
        col1        = p.read();
        dist1 = (int)((high_d << 8) + low_d);
        mode1 = (int)((high_m << 8) + low_m);
        int temp_angle = (int)((high_a << 8) + low_a);
  
        //// 3. 浮動小数点に戻す
        angle1 = (int)temp_angle / 10;
        println(dist1,angle1,mode1,col1);
        }else if(zumo_id==2){
        col2        = p.read();
        dist2 = (int)((high_d << 8) + low_d);
        mode2 = (int)((high_m << 8) + low_m);
        int temp_angle = (int)((high_a << 8) + low_a);
  
        //// 3. 浮動小数点に戻す
        angle2 = (int)temp_angle / 10;
        println(dist2,angle2,mode2,col2);
        }else if(zumo_id==3){
        col3        = p.read();
        dist3 = (int)((high_d << 8) + low_d);
        mode3 = (int)((high_m << 8) + low_m);
        int temp_angle = (int)((high_a << 8) + low_a);
  
        //// 3. 浮動小数点に戻す
        angle3 = (int)temp_angle / 10;
        println(dist3,angle3,mode3,col3);
        }
      }
   }
   
      if(p == port1)
        zumo_id = 1;
       else if(p == port2)
        zumo_id = 2;
       else if(p == port3)
        zumo_id = 3;
           
}
