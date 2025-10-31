// unsigned long f_time;
// float f_angle;
// int erea;

// void forward_robot() {
//  // 敵陣用ロボのロール
//  if (role == FORWARD) {
//    switch (mode) {
//      case 0: // 初期状態
// //        if (time - millis() > 3) { // 仮おき
// //          mode = 1; // 探索モードへ移行
// //        }
//        place(); // 初期位置の設定に必要
//        mode = 1;
//        f_time = millis();
//        break;

//      case 1: // 探索：直進
//       if (millis() - f_time < 3000){
//         colorR = motorL = 200;
//       } else {
//         mode = 2; // 探索：調査へ移行
//       }
      
//       if (color == WHITE) {
//       } else {
//         mode = 3; // 後退
//         f_time = millis(); // 後退するために時間を保存
//         f_angle = angle; // 直前まで向いていた方向を保存
//       }
//        break;

//      case 2: // 探索：調査
//       motorR = -200; // 時計回りに回転
//       motorL = 200;

//       if (5 < dist < 30) { // 5cm~30cm未満の時発見
//         f_time = millis(); // 正面にオブジェクトが来るように調整
//         mode = 4;
//       }
//       break;

//      case 3:
//       if (millies() - f_time > 1000) { // 2秒間後退し回転するように設定
//         motorL = motorR = -200;
//       } else {
//         if (180.0 < f_angle < 360.0) { // 左側の黒からの後退
//           motorR = -200;
//           motorL = 200;
//           if (0 < f_angle - 90 < 20) {
//             mode = 1;
//           }
//         } else { // 右側の黒からの後退
//           motorR = 200;
//           motorL = -200;
//           if (0 < f_angle + 90 < 20) {
//             mode = 1;
//           }
//         }
//       }
//       break;

//      case 4: // 捕獲
//       motorR = motorL = 200;
//       if (dist < 5) {
//         mode = 5; // 投げるならモード100へ移行 
//       }

//      case 5: // お持ち帰り
//       if     
//      case 100: // 投げる
//        motorR = 400;
//        motorL = -400;

//        if (10 < dist) {
//          mode = 1;
//        }
//        break;
//    }
//  }

//  serch();
//  color_move();

//  motorR = motorL = 200;

// }

// void erea() {
// }