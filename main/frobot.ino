unsigned long f_time;
float f_angle;
int erea;

void forward_robot() {
 // 敵陣用ロボのロール
 if (role == FORWARD) {
   switch (mode) {
     case -1: // 敵陣まで移動
      if (abs(angle - f_angle) < 10) {
        motorR = -200;
        motorL = 200;
      } else {
        
      }
     case 0: // 初期状態
//        if (time - millis() > 3) { // 仮おき
//          mode = 1; // 探索モードへ移行
//        }
      //  place(); // 初期位置の設定に必要
       mode = 1;
       f_time = millis();
       f_angle = angle;
       break;

     case 1: // 探索：直進
      if (millis() - f_time < 2000){
        motorR = motorL = 200;
      } else {
        mode = 2; // 探索：調査へ移行
      }
      
      if (color == WHITE) {
      } else {
        mode = 3; // 後退
        f_time = millis(); // 後退するために時間を保存
        f_angle = angle; // 直前まで向いていた方向を保存
      }
       break;

     case 2: // 探索：調査
      motorR = -200; // 時計回りに回転
      motorL = 200;

      if (5 < dist < 30) { // 5cm~30cm未満の時発見
        f_time = millis(); // 正面にオブジェクトが来るように調整
        mode = 4;
      }
      break;

     case 3:
      if (180.0 < f_angle < 360.0) { // 左側の黒からの後退
        motorR = -200;
        motorL = 200;
        if (0 < f_angle - 90 < 20) {
          mode = 1;
        }
      } else { // 右側の黒からの後退
        motorR = 200;
        motorL = -200;
        if (0 < f_angle + 90 < 20) {
          mode = 1;  
        }
      }
      break;

     case 4: // 捕獲
      motorR = motorL = 200;
      if (dist < 5) {
        mode = 5; // 投げるならモード100へ移行
        //f_angle = angle; //捕まえた時の方向を記録
      }
      break;

     case 5: // お持ち帰り：回転
       motorR = motorL = 0;
       break;

     case 100: // 投げる
      if ( f_angle)
       motorR = 400;
       motorL = 400;

       if (color == BLACK) {
         mode = 1;
       }
       break;
   }
 }
}

// 色の判定による挙動の関数
// void color_move() {
//   if (color == BLACK) {
//     motorR = motorL = 0;
//   } else if (color == BLUE) {
//     // 押出の処理
//   } else if (color == RED) {
//     // 押出の処理
//   } else { // 白色の時
//     motorR = motorL = 150;
//   }
// }