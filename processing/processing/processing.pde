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
  
  port1=new Serial(this,"COM4",9600); //Serialクラスのインスタンスを生成
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
  // determine which port triggered this event
  if(p == port1)
    zumo_id = 1;
  else if(p == port2)
    zumo_id = 2;
  else if(p == port3)
    zumo_id = 3;

  // process available bytes; use header-synchronization and unsigned conversion
  while (p.available() > 0) {
    int b = p.read();
    if (b < 0) break;
    int ub = b & 0xFF;
    if (ub != 'H') {
      // discard until header
      continue;
    }

    // need 8 more bytes after header (dist hi/lo, angle hi/lo, mode hi/lo, color, term)
    if (p.available() < 8) {
      // not enough data yet; wait for next serialEvent
      break;
    }

    int high_d = p.read() & 0xFF;
    int low_d  = p.read() & 0xFF;
    int high_a = p.read() & 0xFF;
    int low_a  = p.read() & 0xFF;
    int high_m = p.read() & 0xFF;
    int low_m  = p.read() & 0xFF;
    int col    = p.read() & 0xFF;
    int term   = p.read() & 0xFF; // expected '\n'

    if (term != '\n') {
      // mismatch terminator: skip this frame
      continue;
    }

    int dist = (high_d << 8) | low_d;
    int temp_angle = (high_a << 8) | low_a;
    float angle_val = temp_angle / 10.0;
    int mode_val = (high_m << 8) | low_m;

    if (zumo_id == 1) {
      col1 = col;
      dist1 = dist;
      mode1 = mode_val;
      angle1 = angle_val;
      println(angle1);
    } else if (zumo_id == 2) {
      col2 = col;
      dist2 = dist;
      mode2 = mode_val;
      angle2 = angle_val;
      println(dist2, angle2, mode2, col2);
    } else if (zumo_id == 3) {
      col3 = col;
      dist3 = dist;
      mode3 = mode_val;
      angle3 = angle_val;
      println(dist3, angle3, mode3, col3);
    }
  }

}
