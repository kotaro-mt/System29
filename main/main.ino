#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h>       // 通信のヘッダーファイル
#include <LSM303.h>     // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

// インスタンス定義
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

<<<<<<< HEAD
const int id=1; //processsing通信用
int mode = 0;                    // 動作モード
=======
// 変数定義
int mode = 0; // 動作モード
>>>>>>> abbd962124ea21e1606760353b7a67ffb17c1338
int climb_mode = 0;
int motorL = 0, motorR = 0; // モーター速度（初期化）
float red,green,blue; // RGB値
int dist=0; // オブジェクトまでの距離
float angle=0;  //向いている方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0; // 色判定用変数
uint8_t role = -1; // 役割判定用変数
enum Color {WHITE, BLACK, RED, BLUE }; // 色の定義
enum Role {FORWARD, BACKWARD,CLIMB}; // 役割の定義(敵陣,自陣,山)
float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
float ax = 0, ay = 0, az = 0; // 加速度センサーの値
float goalAngle = 0.0f;// ゴール方向

//=====================
// 共通関数用変数
//=====================
// 判定変数
unsigned long checkStartTime; // 判定処理の計測開始時刻
int prevDist;  // 前回計測した距離
int changeCount; // 距離変化回数カウント

// 色動作用タイマー（旧 c_time/c_active を整理）
unsigned long c_time = 0;
bool isColorAction = false;   // 色動作中フラグ
int modeBeforeColor = 0;      // 色動作前の mode を保存

float vx = 0.0, vy = 0.0; // 速度（cm/s）

// かそくどので使います
float ax_offset = 0, ay_offset = 0, az_offset = 0;
float axmap = 0, aymap = 0, azmap = 0; // map関数による丸め込み

// color_move の参照渡し版
//bool color_move(uint8_t detectedColor, unsigned long &refTime);
//bool rotateToAngle(float targetAngle, float tolerance=5.0f);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

  role = ClassifyRole(); // ロール分類
  mode = 0;

  //カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  // 地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

//   button.waitForButton();
//   offset();
  
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
  else if(role == CLIMB)
  {
    goalAngle -= 180.0f;
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

  // センサ読み取り
  getRGB(red, green, blue); // RGB値の取得
  getAcc(ax, ay, az);       // 加速度の取得

  dist = distance();        // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得
  color = classifyColor();  // 色の分類

  timeNow = millis();

  if (timeNow - timePrev > 500)
  {
    timePrev = timeNow;
    //sendData();
    // Serial.print(x);
    // Serial.print(',');
    // Serial.println(y);
    // Serial.print(ax);
    // Serial.print(',');
    // Serial.println(ay);
    // Serial.print(vx);
    // Serial.print(',');
    // Serial.println(vy);
  }

  // 色割込み判定（どのモードからでも安全に遷移
  // 色動作が発生していない状態で、黒/赤/青を検出したら mode=99 に遷移
  if (!isColorAction && (color == BLACK || color == RED || color == BLUE)) {
    isColorAction = true;
    modeBeforeColor = mode;
    c_time = millis();
    Serial.print("[COLOR] detected -> entering COLOR_ACTION. color=");
    Serial.println(color);
    mode = 99; // 色専用モード
  }

  switch (mode)
  {
  case 0: // 初期化処理
    climb_mode=0;
    motorL = motorR = 0;
    // place(); //位置情報の初期設定
    //Serial.println(role);
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
<<<<<<< HEAD
<<<<<<< HEAD
    forward_robot();
=======
    //place();
    //forward_robot();
>>>>>>> abbd962124ea21e1606760353b7a67ffb17c1338
=======
    // place();
    forward_robot();
>>>>>>> dcd7e884b6ce3eb7f42507adf224f9589d963189
    break;

  case 2:
    // 山登りモード
    if (MountClimb())
      mode = 3;
    break;

  case 3:
    // 探索モード
<<<<<<< HEAD
    if (search())
    {
      mode = 4;
    }
    break;
=======
    search();
<<<<<<< HEAD
      break;
>>>>>>> abbd962124ea21e1606760353b7a67ffb17c1338
=======
    break;
>>>>>>> dcd7e884b6ce3eb7f42507adf224f9589d963189

  case 4:
    // 宝物を見つけて取りに行く
    {
      int res = catchObject();
      if (res == 1)
      {
        mode = 5;
      }
      else if (res == 2)
      {
        mode = 3;
      }
    }
    break;
      
  case 5:
    // ゴールに運ぶ
    goal();
<<<<<<< HEAD
    mode = 3;
    break;

  case 7: // 緊急対応
    motorL = motorR = 0;
=======
    break;

  case 6: // 緊急対応
    // 必要なら記述
    break;

<<<<<<< HEAD
>>>>>>> abbd962124ea21e1606760353b7a67ffb17c1338
=======
  case 99: // 色動作専用モード（割込み）
    {
      bool finished = color_move(color, c_time);
      if (finished) {
        Serial.println("[COLOR] action finished -> restoring previous mode");
        isColorAction = false;
        mode = modeBeforeColor;
      }
    }
    break;

  default:
    // 未定義のmodeは安全停止
    motorL = motorR = 0;
>>>>>>> dcd7e884b6ce3eb7f42507adf224f9589d963189
    break;
  }

  // モーター出力の反映（各モードが motorL/motorR を設定する想定）
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);
}

//===================
// 共通関数群
//===================
// 角度回転専用関数
bool rotateToAngle(float targetAngle, float tolerance) {
  float diff = relativeHeading(angle, targetAngle);
  if (abs(diff) <= tolerance) {
    motorL = motorR = 0;
    return true;
  }
  if (diff > 0) { motorL = 150; motorR = -150; }
  else { motorL = -150; motorR = 150; }
  return false;
}

// 色処理
bool color_move(uint8_t detectedColor, unsigned long &refTime) {
  static bool rotating = false;
  static float targetAngle = 0;
  unsigned long t = millis() - refTime;

  switch (detectedColor) {
    case BLACK:
      if (t < 300) {
        motorL = motorR = -200;
      } else if (t < 1300) {
        motorL = 200;
        motorR = -200;
      } else {
          if (!rotating) {
              targetAngle = angle + 90.0f;
              if (targetAngle >= 360.0f) targetAngle -= 360.0f;
              rotating = true;
          }
          if (rotateToAngle(targetAngle, 5.0f)) {
              rotating = false;
              return true;
          }
      }
      break;

    case RED:
    case BLUE:
      if (t < 300) {
        motorL = motorR = 200;
      } else if (t < 1600) {
          motorL = motorR = -150;
      } else {
          if (!rotating) {
              targetAngle = angle + 180.0f;
              if (targetAngle >= 360.0f) targetAngle -= 360.0f;
              rotating = true;
          }
          if (rotateToAngle(targetAngle, 5.0f)) {
              rotating = false;
              return true;
          }
      }
      break;

    default:
      motorL = motorR = 0;
      return true;
  }

  return false;
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
      // 割込みの色は中央で処理するため、ここは通常の接近処理
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
  }
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
    goalMode = 0;
    break;
  }
  return 0;
}

// 役割の分類
int ClassifyRole()
{ 
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
