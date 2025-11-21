#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h>       // 通信のヘッダーファイル
#include <LSM303.h>     // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

const int id=1; //processsing通信用
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
float ax, ay, az;       // 加速度センサーの値

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

  // カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  // 地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  button.waitForButton();
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
    forward_robot();
    break;

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
    }
    break;

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

  case 5:
    // 宝物を見つけて投げ飛ばす
    break;

  case 6:
    // ゴールに運ぶ
    goal();
    mode = 3;
    break;

  case 7: // 緊急対応
    motorL = motorR = 0;
    break;
  }

  // モーター出力の反映（各モードが motorL/motorR を設定する想定）
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);
}

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
      motorL = motorR = 100;
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
  }
  return 0;
}

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
  }
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
