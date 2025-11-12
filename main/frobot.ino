// // motorR = 200;
// //motorL = 217

// unsigned long f_time;
// float f_angle;
// int f_mode = -1;
// int erea;

// void forward_robot() {
//  // 敵陣用ロボのロール
//  if (role == FORWARD) {
//    switch (f_mode) {
//      case -1: // 敵陣まで移動：赤陣地の場合
//       f_angle = angle; // 最初の方向を記録
//       f_mode = 1;
//       break;
    
//      case 0:
//       if (80 < angle - f_angle < 100) { //敵陣方向に回転
//         motorR = -200;
//         motorL = 217;
//       } else {
//         f_mode = 1;
//         f_time = millis();
//       }
//       break;

//      case 1: // 敵陣まで直進
//       motorR = 200;
//       motorL = 217;
//       if(milli() - f_time > 6000) { // 6秒経過したら探索へ移行
//         mode = 3; // 探索へ移行
//         f_time = millis();
//       }
//       break;

//      case 2: // 探索-回転
//       if (millis() - f_time >= 1000) { // 1秒経過したら再直進
//         f_mode = 3; // 探索-直進
//         b_time = millis();
//       } else {
//         motorL = -150;  // 左旋回
//         motorR = 150;
//       }
//         // 途中で物体を発見した場合
//         if (dist > 0) {
//           if (!targetAngleRecorded) {     // まだ角度を記録していなければ
//             f_angle = angle;             // 現在角度を記録
//             targetAngleRecorded = true;
//           }
//           f_mode = 4;                      // CHECKへ
//           f_time = millis();
//         }
//       break;

//     case 3: // 探索-直進
//       motorR = 200;
//       motorL = 217;
//         if (millis() - f_time >= 1000) { // 1秒後に探索へ
//           f_mode = 2;
//           f_time = millis();
//         }

//         if (color == BLACK || color == RED || color == BLUE) {
//           f_mode = 7; // 色反応処理
//           f_time = millis();
//         }
//       break;
    
//     case 4: // CHECK
//       if (Check(dist, checkStartTime, prevDist, changeCount, 500, 3)) {
//         f_mode = 5;
//         f_time = millis();
//       } else if (millis() - checkStartTime >= 500) {
//         f_mode = 2;
//         f_time = millis();
//       }
//       break;

//     case 99:
//       color_move(color, f_time);
//       break;

//      case 100: // 投げる
//       if ( f_angle)
//        motorR = 400;
//        motorL = 400;

//        if (color == BLACK) {
//          f_mode = 1;
//        }
//        break;
//    }
//  }
// }
