//200の場合
//横5秒ぐらい（150）
//縦9秒ぐらい（240）

unsigned long prevTime = 0;
float ax_g = 0, ay_g = 0, az_g = 0;

void place() {
  unsigned long now = millis();
  float dt = (now - prevTime) / 1000.0; // 秒に変換
  prevTime = now;
  float rad;
  
  // if (mode = 0){ //
  //   if (role == FORWARD) { // 仮です
  //     x = 0;
  //     y = 15;
  //   } else if (role == CLIMB) {
  //     x = 75;
  //     y = 0;
  //   } else { // BACKWARD
  //     x = 150;
  //     y = 15;
  //   }
  // } else {

    // フィルタ
    getAcc(ax, ay, az);
    map(ax,-32768,32767,-128,127);
    axmap = ax/256;
    map(ay,-32768,32767,-128,127);
    aymap = ay/256; 

    axmap = 0.1 * (axmap - ax_offset) + 0.9 * axmap;
    aymap = 0.1 * (aymap - ay_offset) + 0.9 * aymap;
  
    // 必要なら cm/s^2 単位に変換
    ax_g = axmap * 9.80665 * 100;
    ay_g = aymap * 9.80665 * 100;
  
    // 現在の速度
    vx = ax_g * dt;
    vy = ay_g * dt;

    if (abs(vx) < 1.0) vx = 0; // 止まっている場合の処理
    if (abs(vy) < 1.0) vy = 0;

    if(abs(vx) > 100) vx = 0; // 外れ値の処理
    if(abs(vy) > 100) vy = 0;
    
    // 加速度を実装した場合の位置の特定
    x += vx * dt; // x座標の移動
    y += vy * dt; // y座標の移動
  
    // 色による位置の修正(四隅のところは判定甘いかも)
    // if (color == BLUE) {
    //   y = 240;
    // } else if (color == RED) {
    //   y = 0;
    // } else if(color == BLACK && angle < 180) {
    //   x = 0;
    // } else if(color == BLACK && angle > 180) {
    //   x = 150;
    // } else { 
    // }
    //}
}
