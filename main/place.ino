#define PI 3.141592653589793 // パイの値
//200の場合
//横5秒ぐらい（150）
//縦9秒ぐらい（240）

unsigned long time_speed; // 動いた距離計測用
void place() {
  int x, y; // マップに対するXY座標(赤の左側を原点とする)
  
  if (role == FORWARD) {
    x = 0;
    y = 15;
  } else if (role == CLIMB) {
    x = 75;
    y = 0;
  } else {
    x = 150;
    y = 15;
  }
}
