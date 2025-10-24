#define PI 3.141592653589793 // パイの値
//200の場合
//横5秒ぐらい（150）
//縦9秒ぐらい（240）

int robot_speed = 30; //一旦ロボットの速さを30cm/sと仮定
unsigned long time_dist = millis(); // 動いた距離計測用

void place() {
  int x, y; // マップに対するXY座標(赤のラインの左側を原点とする)
  float rad;
  
  if (mode = 1){ //
    if (role == FORWARD) { // 仮です
      x = 0;
      y = 15;
    } else if (role == CLIMB) {
      x = 75;
      y = 0;
    } else { // BACKWARD
      x = 150;
      y = 15;
    }
  }

  // -----10/24追加-----

  if(mode = 1) { // 停止中ではマップ移動無し(モードで選択)
    x += 0;
    y += 0;
  } else {
      rad = angle / (180 / PI); // ラジアンの値
      x += 30 * cos(rad) * (millis() - time_dist); // x座標の移動
      y += 30 * sin(rad) * (millis() - time_dist); // y座標の移動
  }

  // 色による位置の修正(四隅のところは判定甘いかも)
  if (color == BLUE) {
    y = 240;
  } if else (color == RED) {
    y = 0;
  } if else (color == BLACK && angle < 180) {
    x = 0;
  } if else (color == BLACK && angle > 180) {
    x = 150;
  }
}
