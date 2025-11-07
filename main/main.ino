#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h>       // 通信のヘッダーファイル
#include <LSM303.h>     // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

<<<<<<< HEAD
// インスタンス定義
=======
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

<<<<<<< HEAD
// 変数定義
int mode = 0; // 動作モード
int motorL, motorR; // モーター速度
float red,green,blue; // RGB値
int dist=0; // オブジェクトまでの距離
float angle=0;  //向いている方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0; // 色判定用変数
uint8_t role = -1; // 役割判定用変数
enum Color {WHITE, BLACK, RED, BLUE }; // 色の定義
enum Role {FORWARD, BACKWARD,CLIMB}; // 役割の定義(敵陣,自陣,山)
float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
float ax, ay, az; // 加速度センサーの値

//=====================
// 共通関数用変数
//=====================
// 判定変数
unsigned long checkStartTime; // 判定処理の計測開始時刻
int prevDist;  // 前回計測した距離
int changeCount; // 距離変化回数カウント

// 色動作用タイマー
unsigned long c_time = 0;
bool c_active = false;

// 初期設定(キャリブレーション3回)
void setup() {
=======
int mode = 0;                    // 動作モード
int climb_mode = 0;
int motorL, motorR;              // モーター速度
float red, green, blue;          // RGB値
int dist = 0;                    // オブジェクトまでの距離
float angle = 0;                 // 向いている方角
float goalAngle = 0;             // ゴール方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0;               // 色判定用変数
uint8_t role = -1;               // 役割判定用変数
enum Color
{
  WHITE,
  BLACK,
  RED,
  BLUE
}; // 色の定義
enum Role
{
  FORWARD,
  BACKWARD,
  CLIMB
}; // 役割の定義
float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
<<<<<<< HEAD
float ax, ay, az; // 加速度センサーの値
float vx = 0.0, vy = 0.0; // 速度（m/s）

//速度の最小値最大値を求めるよ
float xMv = 0;
float xmv = 100;
float yMv = 0;
float ymv = 100;

// かそくどので使います
float ax_offset = 0, ay_offset = 0, az_offset = 0;
=======
float ax, ay, az;       // 加速度センサーの値
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3

void setup()
{
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

<<<<<<< HEAD
  role = ClassifyRole(); // ロール分類
  mode = 0;

  //カラーセンサーのキャリブレーション
=======
  // カラーセンサーのキャリブレーション
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
  button.waitForButton();
  CalibrationColorSensor();

  // 地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  button.waitForButton();
<<<<<<< HEAD
<<<<<<< HEAD
  offset();
  
  button.waitForButton();
=======
  angle = averageHeading(); // 向いている方角の取得
  role = ClassifyRole();
>>>>>>> 2c81d7cd5810e883fc471a9cfa40efa6bfd9ae7b
=======
  goalAngle = angle = averageHeading(); // 向いている方角の取得(初期方角)
  role = ClassifyRole();                // ロール分類

  // 目標方角の設定
  if (role == FORWARD)
  {
    goalAngle += 90.0f;
    if (goalAngle >= 360.0f)
      goalAngle -= 360.0f;
  }
  else if (role == BACKWARD)
  {
    goalAngle -= 90.0f;
    if (goalAngle < 0.0f)
      goalAngle += 360.0f;
  }
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
  // 初回送信時間の設定
  timePrev = millis();

  // ボタン入力をプルアップで有効化（Zumo ボタンが LOW 押下の想定）
  pinMode(ZUMO_BUTTON, INPUT_PULLUP);
}

void loop()
{

  // ボタン押下で初期化処理
  if (digitalRead(ZUMO_BUTTON) == LOW)
  { // 押された
    delay(40);
    if (digitalRead(ZUMO_BUTTON) == LOW)
    {
      reinitializeAll();
    }
  }

  getRGB(red, green, blue); // RGB値の取得
  getAcc(ax, ay, az);       // 加速度の取得

  dist = distance();        // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得
  color = classifyColor();  // 色の分類

  timeNow = millis();

  if (timeNow - timePrev > 500)
  {
    timePrev = timeNow;
<<<<<<< HEAD
    //sendData();
    //Serial.print(x);
    //Serial.print(',');
    //Serial.println(y);
    //Serial.print(ax);
    //Serial.print(',');
    //Serial.print(ay);
    //Serial.print(',');
    //Serial.println(az);
    Serial.print(vx);
    Serial.print(',');
    Serial.println(vy);
  }

 switch (mode) {
    case 0: //初期化処理
      motorL = motorR = 0;
      if(role == FORWARD){
        mode = 1;
      }
      else if(role == CLIMB){
        mode = 2;
      }
      else{
        mode = 3;
      }
      break;
    case 1:
      // 敵陣ロボットの移動      
      forward_robot();
      break;
=======
    // sendData();
  }

  switch (mode)
  {
  case 0: // 初期化処理
    climb_mode=0;
    motorL = motorR = 0;
    Serial.println(role);
    if (role == FORWARD)
    {
      mode = 1;
    }
    else if (role == CLIMB)
    {
      mode = 2;
    }
    else
    {
      mode = 3;
    }
    break;
  case 1:
    // 敵陣ロボットの移動
    break;
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3

  case 2:
    // 山登りモード
    if (MountClimb())
      mode = 3;
    break;

  case 3:
    // 探索モード
    if (search())
    {
      mode = 4;
      motorL = motorR = 0;
<<<<<<< HEAD
      // roleを文字列で表示
      Serial.print("[ROLE] 現在の役割：");
      if(role == FORWARD){
        Serial.println("FORWARD");
        mode = 1;
      }
      else if(role == CLIMB){
        Serial.println("CLIMB");
        mode = 2;
      }
      else{
        Serial.println("BACKWARD");
        mode = 3;
      }
      break;
    case 1:
      // 敵陣ロボットの移動
      break;
=======
    }
    break;
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3

  case 4:
    // 宝物を見つけて取りに行く
    if (catchObject() == 1)
    {
      mode = 6;
    }
    else if (catchObject() == 2)
    {
      mode = 3;
    }
    break;

<<<<<<< HEAD
    case 3:
      // 探索モード
      Back();
      break;
=======
  case 5:
    // 宝物を見つけて投げ飛ばす
    break;
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3

  case 6:
    // ゴールに運ぶ
    break;

  case 7: // 緊急対応

    break;
  }

  // モーター出力の反映（各モードが motorL/motorR を設定する想定）
  //motorR = 200;
  //motorL = 217;
  motorR = motorL = 0;
  place();
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);
}

//===================
// 共通関数群
//===================

// 探索についての関数
int search()
{
  static int searchMode = 0;
  static unsigned long timePrev1 = 0;
  static unsigned long timeNow1 = 0;

  timeNow1 = millis();
  if (timePrev1 == 0)
    timePrev1 = timeNow1;

  switch (searchMode)
  {
  case 0: // 直進
    if (color == WHITE)
    {
      motorL = motorR = 200;
      if (timeNow1 - timePrev1 > 2000)
      {
        timePrev1 = timeNow1;
        searchMode = 2;
      }
    }
    else
    {
      timePrev1 = timeNow1;
      searchMode = 1;
    }
    break;
  case 1: // 後退
    motorL = motorR = -200;
    if (timeNow1 - timePrev1 > 2000)
    {
      timePrev1 = timeNow1;
      searchMode = 0;
    }
    break;
  case 2: // 回転
    motorR = 150;
    motorL = -150;
    if (dist < 30)
    {
      searchMode = 0;
      return 1;
    }
    if (timeNow1 - timePrev1 > 2000)
    {
      timePrev1 = timeNow1;
      searchMode = 0;
    }
    break;
  }
  return 0;
}

int catchObject()
{ // 宝物を見つけて取りに行く　0:継続 1:完了 2:失敗（敵機感知）
  static int catchMode = 0;
  static unsigned long timePrev2 = 0;
  unsigned long timeNow2 = millis();
  static float avgDist = 0.0f;

  if (timePrev2 == 0) timePrev2 = timeNow2;

  switch (catchMode)
  {
    case 0: // 待機：距離を平滑化して静止かどうか判定
      motorL = motorR = 0;
      // LPF 更新（前回値を保持しつつ更新）
      avgDist = 0.8f * avgDist + 0.2f * (float)dist;
      if (timeNow2 - timePrev2 > 1000) {
        timePrev2 = timeNow2;
        // 敵機でない（ほぼ静止）なら接近モードへ、そうでなければ敵判定
        if (fabs(avgDist - (float)dist) < 3.0f) {
          catchMode = 1;
        } else {
          return 2; // 敵機判定（失敗）
        }
      }
      break;

    case 1: // 接近して保持
      motorL = motorR = 150;
      if (dist < 5) {
        catchMode = 2;
      }
      break;

    case 2: // 完了
      motorL = motorR = 0;
      catchMode = 0;
      return 1;
  }
  return 0;
}

<<<<<<< HEAD
// 🔹 色に応じた動作処理
void color_move(uint8_t color, unsigned long &refTime) {
  if (!c_active) {
    c_time = millis();
    c_active = true;
  }

  unsigned long elapsedColorTime = millis() - c_time;

  switch (color) {
    case BLACK:
      // 後退 → 回転 → 復帰
      if (elapsedColorTime < 300) {
        motorL = motorR = -150; // 0.3秒後退
      } else if (elapsedColorTime < 1300) {
        motorL = 200; motorR = -200; // 1秒回転
      } else {
        mode = 1;
        c_active = false;
      }
      break;

    case RED:
    case BLUE:
      // 前進 → 後退 → 半回転
      if (elapsedColorTime < 1000) {
        motorL = motorR = 200;   // 前進1秒
      } else if (elapsedColorTime < 1500) {
        motorL = motorR = -150;  // 後退0.5秒
      } else if (elapsedColorTime < 2300) {
        motorL = -200; motorR = 200; // 半回転0.8秒
      } else {
        mode = 1;
        c_active = false;
      }
      break;

    default:
      motorL = motorR = 200;
      c_active = false;
      break;
  }
}

// 🔹 静止判定関数（距離センサーの微小変化を無視）
bool Check(int currentDist, unsigned long &checkStartTime, int &prevDist, int &changeCount,
           unsigned long checkDuration = 500, int changeThreshold = 2) {
  static bool firstCall = true;

  if (firstCall) {
    checkStartTime = millis(); // 判定開始
    prevDist = currentDist; // 初期距離
    changeCount = 0; // 変化回数リセット
    firstCall = false;
  }

  // 判定期間中はモーター停止しつつ待つ
  motorL = motorR = 0;

  //unsigned long elapsed = millis() - cheakStartTime;

  // 判定期間中の動作
  if (millis() - checkStartTime < checkDuration) {
    // 変化閾値を緩める（例: ±5cm未満はノイズとして無視）
    if (abs(currentDist - prevDist) >= 5) {
      changeCount++;
      prevDist = currentDist;
    }
    return false;
  } else {
    firstCall = true;
    // 距離変化がほとんどなければ「静止」とみなす
    return (changeCount < changeThreshold);
=======
int goal()
{ // ゴールに運ぶ
  static int goalMode = 0;
  switch (goalMode)
  {
  case 0: // 回転
    motorL = 150;
    motorR = -150;
    if (relativeHeading(angle, goalAngle) < 5.0 && relativeHeading(angle, goalAngle) > -5.0) // 目標方角に到達
      goalMode = 1;

    break;
  case 1: // 前進
    motorL = motorR = 200;
    if (color == BLUE || color == RED)
    { // ゴール到達
      motorL = motorR = 0;
      goalMode = 0;
      return 1;
    }
    break;
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
  }
  return 0;
}

<<<<<<< HEAD

// 役割の分類
int ClassifyRole() { 
  if (angle >= 60 && angle < 130) {
    role = FORWARD;
  }
  else if (angle > 230 && angle < 320) {
    role = BACKWARD;
  }
  else {
    role = CLIMB;
=======
int ClassifyRole()
{ // 役割の分類
  if (230 < angle && angle < 320)
  {
    return FORWARD;
  }
  else if (angle >= 60 && angle < 130)
  {
    return BACKWARD;
  }
  else
  {
    return CLIMB;
>>>>>>> 30f13d59fad76ed28d0fc6be0f1edc9c4a99ebf3
  }
  return role;
}


// 再初期化関数：ボタン押下時に呼ぶ（キャリブレーションも行う）
void reinitializeAll()
{
  // 即時モーター停止
  motorL = 0;
  motorR = 0;
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);

  Serial.println("[INFO] reinitializeAll: stopping motors and resetting sensors/state");

  // センサー類の再セットアップ（ハード初期化）
  initWaveSensor();
  setupCompass();

  // 状態変数リセット
  dist = 0;
  angle = 0;
  color = 0;
  role = -1;
  mode=0;
  timePrev = millis();

  // カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  // 地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  // 最初に向いている方向を取得
  button.waitForButton();
  angle = averageHeading(); // 向いている方角の取得
  role = ClassifyRole();
}
