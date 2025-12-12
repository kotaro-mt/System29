// /*
// 山登りの関数
// //1を返すと成功、0を返すと相手に山頂のオブジェクトを取られた
// */

// float avg_mx = 0;
// float max_ax = 0;
// int speed = 0;
// unsigned long timePrev_c = 0;
// unsigned long timeNow_c = 0;

// int MountClimb()
// {

//   float diff = 0;
//   int count = 0;      // カウント用
//   getAcc(ax, ay, az); // 加速度センサーの値を取得

//   avg_ax = 0.9 * avg_ax + 0.1 * ax; // 低域通過フィルタ

//   // Serial.println(climb_mode);
//   avg_mx = 0.9 * avg_mx + 0.1 * mxScaled(); // 低域通過フィルタ
//   timeNow_c = millis();

//   switch (climb_mode)
//   {
//   case 0: // 山登り開始
//     speed = 250;
//     if (avg_ax > 5000)
//     {                 // 傾きが一定以上になったら
//       climb_mode = 1; // 山頂到達判定へ
//     }
//     break;
//   case 1: // 山頂到達判定
//   {
//     // 目標 ay（センサ単位）に近づける制御
//     const float target_ay = 3000.0f; // 目標値（実機に合わせて変更）
//     const float Kp_ay = 0.02f;        // 比例ゲイン（チューニング）

//     static float avg_ay = 0.0f;   // ay のローパス用蓄積
//     static float prevDiff = 0.0f; // diff 平滑化用

//     // ay を平滑化してノイズを抑える
//     avg_ay = 0.9f * avg_ay + 0.1f * ay;

//     // Serial.println(avg_ay);

//     // 誤差に比例して舵を決定
//     float error = target_ay - avg_ay;
//     float steer = Kp_ay * error;
//     // diff をローパスして滑らかに
//     diff = 0.8f * prevDiff + 0.2f * steer;
//     prevDiff = diff;

//     // 例: 磁気方向が所定に向いたら次のモードへ（条件は実機で調整）
//     if (angle < 145)
//     {
//       // Serial.println(avg_mx);
//       climb_mode = 2;
//       diff = 0;
//       timePrev_c = timeNow_c;
//     }
//   }
//   break;
//   case 2: // 山頂方向に回転
//     speed = -200;
//     diff = -0.02 * ay;
//     if (timeNow_c - timePrev_c > 500)
//     {
//       climb_mode = 3;
//       timePrev_c = timeNow_c;
//     }
//     break;
//   case 3: // 物体を取りに行く
//     speed = 220;
//     diff = -0.02 * ay;
//     ;
//     if (timeNow_c - timePrev_c > 1500)
//     {
//       if (ax < 2000 || (dist > 0 && dist < 5))
//       {
//         climb_mode = 4;
//         timePrev_c = timeNow_c;
//       }
//     }
//     break;
//   case 4: // 物体取得準備
//     speed = 0;
//     diff = -400;
//     if (timeNow_c - timePrev_c > 100)
//     {
//       climb_mode = 5;
//     }
//     break;
//   case 5: // 物体検知
//     speed = 200;
//     diff = 0;
//     if (avg_ax < -1000)
//     { // 山を降り始めたら
//       climb_mode = 6;
//     }

//     break;
//   case 6: // 山降り開始
//     speed = 200;
//     diff = -0.02 * ay;
//     if (avg_ax > -500)
//     {           // 平地に戻ったら
//       return 1; // 成功
//     }
//     break;
      case 100:
          speed =(-150);
          timeNow_c=millis();
          if(timeNow_c-timePrev_c>1000){
            return 1;
          
        }
//   }

  
  // if(color!=WHITE){
  //   climb_mode=100;
  //   timePrev_c=millis();
  // }

//   motorL = speed + diff;
//   motorR = speed - diff;
//   return 0; // 継続
// }

/*
山登りの関数
//1を返すと成功、0を返すと相手に山頂のオブジェクトを取られた
*/

float avg_mx = 0;
float max_ax = 0;
int speed = 0;
unsigned long timePrev_c = 0;
unsigned long timeNow_c = 0;

int MountClimb()
{

  float diff = 0;
  int count = 0;      // カウント用
  getAcc(ax, ay, az); // 加速度センサーの値を取得

  // Serial.println(climb_mode);
  avg_mx = 0.9 * avg_mx + 0.1 * mxScaled(); // 低域通過フィルタ
  timeNow_c = millis();

  switch (climb_mode)
  {
  case 0: // 山登り開始
    speed = 250;
    diff=(-10);
    if (avg_ax > 5000)
    {                 // 傾きが一定以上になったら
      climb_mode = 1; // 山頂到達判定へ
      diff=0;
    }
    break;
  case 1: // 山頂到達判定
  {
    // 目標 ay（センサ単位）に近づける制御
    const float target_ay = -3000.0f; // 目標値（実機に合わせて変更）
    const float Kp_ay = 0.02f;        // 比例ゲイン（チューニング）

    static float avg_ay = 0.0f;   // ay のローパス用蓄積
    static float prevDiff = 0.0f; // diff 平滑化用

    // ay を平滑化してノイズを抑える
    avg_ay = 0.9f * avg_ay + 0.1f * ay;

    // Serial.println(avg_ay);

    // 誤差に比例して舵を決定
    float error = target_ay - avg_ay;
    float steer = Kp_ay * error;
    // diff をローパスして滑らかに
    diff = 0.8f * prevDiff + 0.2f * steer;
    prevDiff = diff;

    // 例: 磁気方向が所定に向いたら次のモードへ（条件は実機で調整）
    if (angle > 60&&angle<120)
    {
      Serial.println(avg_mx);
      climb_mode = 2;
      diff = 0;
      timePrev_c = timeNow_c;
    }
  }
  break;
  case 2: // 山頂方向に回転
    speed = -200;
    diff = -0.02 * ay;
    if (timeNow_c - timePrev_c > 500)
    {
      climb_mode = 3;
      timePrev_c = timeNow_c;
    }
    break;
  case 3: // 物体を取りに行く
    speed = 220;
    diff = -0.02 * ay;
    ;
    if (timeNow_c - timePrev_c > 1500)
    {
      if (ax < 2000 || (dist > 0 && dist < 5))
      {
        climb_mode = 4;
        timePrev_c = timeNow_c;
      }
    }
    break;
  case 4: // 物体取得準備
    speed = 0;
    diff = 400;
    if (timeNow_c - timePrev_c > 100)
    {
      climb_mode = 5;
    }
    break;
  case 5: // 物体検知
    speed = 200;
    diff = 0;
    if (avg_ax < -1000)
    { // 山を降り始めたら
      climb_mode = 6;
    }

    break;
  case 6: // 山降り開始
    speed = 200;
    diff = -0.02 * ay;
    if (avg_ax > -500)
    {           // 平地に戻ったら
      return 1; // 成功
    }
    break;
  case 100:
    speed =(-150);
    timeNow_c=millis();
    if(timeNow_c-timePrev_c>1000){
      return 1;
    }
  }
  
  if(color!=WHITE){
    climb_mode=100;
    timePrev_c=millis();
  }
  
  motorL = speed + diff;
  motorR = speed - diff;
  return 0; // 継続
}
