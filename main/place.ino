// #define PI 3.141592653589793 // パイの値
// //200の場合
// //横5秒ぐらい（150）
// //縦9秒ぐらい（240）

// float robo_v = 30.0; //一旦ロボットの速さを30cm/sと仮定
// unsigned long time_dist = millis(); // 動いた距離計測用


// // float vx = 0.0, vy = 0.0; // 速度（m/s）
// // float px = 0.0, py = 0.0; // 位置（m)
// // unsigned long prevTime = 0;

// void place() {
//   // unsigned long now = millis();
//   // float dt = (now - prevTime) / 1000.0; // 秒に変換
//   // prevTime = now;
//   float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
//   float rad;
  
//   if (mode = 1){ //
//     if (role == FORWARD) { // 仮です
//       x = 0;
//       y = 15;
//     } else if (role == CLIMB) {
//       x = 75;
//       y = 0;
//     } else { // BACKWARD
//       x = 150;
//       y = 15;
//     }
//   }

//   // -----10/24追加-----

//   // // 必要なら m/s^2 単位に変換
//   // float ax_g = ax / 16384.0 * 9.80665;
//   // float ay_g = ay / 16384.0 * 9.80665;
//   // float az_g = az / 16384.0 * 9.80665;
//   // // ノイズ除去（静止時の微小値をゼロに）
//   // if (abs(ax) < 0.05) ax = 0;
//   // if (abs(ay) < 0.05) ay = 0;

//   // // 速度の積分（v = v + a * dt）
//   // vx += ax * dt;
//   // vy += ay * dt;


//   if(mode = 1) { // 停止中ではマップ移動無し(モードで選択)
//     x += 0;
//     y += 0;
//   } else {
//       rad = angle / (180 / PI); // ラジアンの値
//       x += robo_v * cos(rad) * (millis() - time_dist) / 1000.0; // x座標の移動
//       y += robo_v * sin(rad) * (millis() - time_dist) / 1000.0; // y座標の移動
//   }

//   // 加速度を実装した場合の位置の特定
//   // x += vx * cos(rad) * (millis() - time_dist) / 1000.0; // x座標の移動
//   // y += vx * sin(rad) * (millis() - time_dist) / 1000.0; // y座標の移動

//   // 色による位置の修正(四隅のところは判定甘いかも)
//   if (color == BLUE) {
//     y = 240;
//   } if else (color == RED) {
//     y = 0;
//   } if else (color == BLACK && angle < 180) {
//     x = 0;
//   } if else (color == BLACK && angle > 180) {
//     x = 150;
//   }
// }
