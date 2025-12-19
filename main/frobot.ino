// // motorR = 200
// //motorL = 217

void forward_robot() {
// 敵陣用ロボのロール
 if (role == FORWARD) {
   switch (f_mode) {
     case 101: // 敵陣まで移動：赤陣地の場合
       //Serial.println(f_mode);
       f_angle = angle; // 最初の方向を記録
       f_mode = 0;
       mode = 3;
       break;

     // 自陣が青の場合
     case 0:
       //Serial.println(f_mode);
       if (angle < 225) { //敵陣方向に回転
         f_mode = 1;
         f_time = millis();
       } else {
         motorR = 200;
         motorL = -217;
       }
       break;

     // 自陣が赤の場合
     /*case 0:
       if (40 > angle) {
        f_mode = 1;
        f_time = millis();
       } else {
        motorR = 200;
        motorL = -217;
       }*/

     case 1: // 敵陣まで直進
       //Serial.println(f_mode);
       motorR = 340;
       motorL = 400;
       if(millis() - f_time > 2700) { // 秒経過したら探索へ移行
        f_mode = 2;
        f_time = millis();
        mode = 3;
       }
       if (color == BLACK || color == BLUE || color == RED) {
        //f_mode = 99; // 色判定
        //f_angle = angle;
        //f_time = millis();
       }
       break;

     case 2: // 探索-回転
       //Serial.println(f_mode);
       if (millis() - f_time >= 1500) { // 1.5秒経過したら再直進
         f_mode = 3; // 探索-直進
         f_time = millis();
       } else {
         motorL = -217;  // 左旋回
         motorR = 200;
       }
       // 途中で物体を発見した場合
       if (0 < dist && dist < 50) {
         if (!targetAngleRecorded) {     // まだ角度を記録していなければ
           f_angle = angle;             // 現在角度を記録
           targetAngleRecorded = true;
         }
         f_mode = 4;                      // CHECKへ
         f_time = millis();
       }
       break;

     case 3: // 探索-直進
       //Serial.println(f_mode);
       motorR = 400;
       motorL = 400;
         if (millis() - f_time >= 2000) { // 1秒後に探索へ
           f_mode = 2;
           f_time = millis();
           mode = 3;
         }

         if (color == BLACK || color == RED || color == BLUE) {
           f_mode = 99; // 色反応処理
           f_time = millis();
           f_angle = angle;
         }
       break;
     
     case 4: // CHECK 静止反転
       //Serial.println(f_mode);
       if (Check(dist, checkStartTime, prevDist, changeCount, 500, 3)) {
         f_mode = 5;
         f_time = millis();
         mode = 4;
       } else if (millis() - checkStartTime >= 500) {
         f_mode = 2;
         f_time = millis();
         mode = 3;
       }
       break;

     case 5: // キャッチ
       //Serial.println(f_mode);
         motorL = 320;
         motorR = 300;
         if (dist <= 5) {
           f_mode = 6; // 運搬
           f_angle = angle; // 角度を記録（どっちの壁に運ぶのが楽か算出するため）
           f_time = millis();
           mode = 5;
         }

         if (cup_count == 2) {
          f_mode = 7;
          f_time = millis();
          f_angle = angle;
         }

         if (color == BLACK || color == RED || color == BLUE) {
          f_mode = 99; // 色判定
          f_time = millis();
          f_angle = angle;
         }
         break;
     
     case 6: //運搬-回転
       //Serial.println(f_mode);
       if (dist > 10) { // オブジェクトが離れたら再探索
        f_mode = 2;
        mode = 3;
       }
       
       if (80 < angle && angle < 100) { // 黒の壁を向いたら直進
         motorR = 200; motorL = 217;
         if (color == BLACK) { // 黒を認識したら
           f_time = millis();
           f_mode = 100; // 外に出す処理
         }
       } else { // 黒の壁を向くまで回転
         motorR = -200; motorL = 217;
       }
       break;

     case 99: // 色による判定
       //Serial.println(f_mode);
       if (millis() - f_time < 1200) {
        motorR = -200;
        motorL = -217;
       } else {
        f_mode = 3;
        f_time = millis();
        f_angle = angle;
       }
       break;

     case 100: // 投げるor押し出し
       //Serial.println(f_mode);
       if(millis() - f_time < 200) { // 押し出し直進
         motorR = 400; motorL = 400;
       } else if (200 <= millis() - f_time && millis() - f_time < 1200){
         motorR = -400; motorL = -400; // 後退
         f_angle = angle;
       } else {
         if (60 < abs(angle - f_angle)) {
           f_mode = 3; // 探索-直進へ移行
           mode = 3;
           cup_count++; //カップ数に関係
         } else { // 適当に回転
           motorR = -400;
           motorL = 400;
         }
       }
       break;

     // case 6: // 運搬-自陣ver
     //   if (160 < angle < 200) { // 運搬
     //     motorR = 200;  motorL = 217;
     //   } else { // 自陣のほうを向く
     //     motorR = 200;  motorL = -217;
     //     if (color == RED) { // 赤を踏んだら押し出し
     //       f_time = millis();
     //       f_mode = 7;
     //     }
     //   }
     //   break;

     // case 7:
     //   if (millis() - f_time < 1000) {
     //       motorR = 200;  motorL = 217;
     //   } else {
     //     motorR = 200;  motorL = -217;
     //     if (0 < f_angle - angle < 20) {
     //       f_mode = 3;
     //     }
     //   }
   }
 }
}
