#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h>       // 通信のヘッダーファイル
#include <LSM303.h>     // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

// インスタンス定義
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

const int id=2; //processsing通信用
int mode = 0;                    // 動作モード
int climb_mode = 0;
int motorL, motorR; // モーター速度
float red,green,blue; // RGB値
int dist=0; // オブジェクトまでの距離
float angle=0;  //向いている方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0; // 色判定用変数
uint8_t colorBeforeAction = 0; // 色割込み
uint8_t role = -1; // 役割判定用変数
enum Color {WHITE, BLACK, RED, BLUE }; // 色の定義
enum Role {FORWARD, BACKWARD,CLIMB}; // 役割の定義(敵陣,自陣,山)
float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
float ax = 0, ay = 0, az = 0; // 加速度センサーの値
float goalAngle = 0.0f;// ゴール方向
float avg_ax = 0;
float distPrev = 0;
boolean isBLUE=false;

//　敵陣ロボ用
unsigned long f_time;
float f_angle;
int f_mode = 101;
int erea;
int cup_count = 0;

//=====================
// 共通関数用変数
//=====================
// 判定変数
unsigned long checkStartTime; // 判定処理の計測開始時刻
int prevDist;  // 前回計測した距離
int changeCount; // 距離変化回数カウント
bool targetAngleRecorded = false;  // 角度が記録済みかどうか

// 色動作用タイマー
unsigned long c_time = 0;
bool isColorAction = false; // 色動作中フラグ 
int modeBeforeColor = 0; // 色動作前の mode を保存

float vx = 0.0, vy = 0.0; // 速度（cm/s）

// 加速度で使います
float ax_offset = 0, ay_offset = 0, az_offset = 0;
float axmap = 0, aymap = 0, azmap = 0; // map関数による丸め込み

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

  // 初期方角を取得
  button.waitForButton();
  goalAngle = angle = averageHeading(); // 向いている方角の取得(初期方角)
  role = ClassifyRole();                // ロール分類
  Serial.println(String(id) + "," + String(dist) + "," + String(angle) + "," + String(mode) + "," + String(color));
        
  // 初回送信時間の設定時間の設定
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
  avg_ax = 0.9 * avg_ax + 0.1 * ax; // 低域通過フィルタ

  dist = distance();        // オブジェクトまでの距離の取得
  Serial.println(dist);
  angle = averageHeading(); // 向いている方角の取得
  color = classifyColor();  // 色の分類

  timeNow = millis();

  // ハンドシェイク通信のため、常時sendDataを呼び出す（sendData内で受信待ちを行う）
  sendData();

  switch (mode)
  {
  case 0: // 初期化処理
    climb_mode=0;
    // place(); //位置情報の初期設定
    //Serial.println(role);
    // roleを文字列で表示
    //Serial.print("[ROLE] 現在の役割：");
    if(role == FORWARD){
      //Serial.println("FORWARD");
      goalAngle += 90.0f;
    if (goalAngle >= 360.0f)
      goalAngle -= 360.0f;
      mode = 1;
    }
    else if(role == CLIMB){
      //Serial.println("CLIMB");
      goalAngle -= 180.0f;
    if (goalAngle < 0.0f)
      goalAngle += 360.0f;
      mode = 2;
    }
    else{
      //Serial.println("BACKWARD");
      goalAngle -= 90.0f;
    if (goalAngle < 0.0f)
      goalAngle += 360.0f;
      mode = 3;
    }
    break;
    
  case 1:
    // 敵陣ロボットの移動
    forward_robot();
    break;

  case 2:
    // 山登りモード
    if(isBLUE){
      if (MountClimb_B())
      mode = 3;
    }else{
      if (MountClimb_R())
      mode = 3;
    }
    
    break;

  case 3:
    //Serial.println("探索");
    if(role != FORWARD) {
      if(search()) mode=4;
    } else {
      forward_robot();
    }
    break;

  case 4:
    //Serial.println("接近・取得");
    // 宝物を見つけて取りに行く
    if(role != FORWARD) {
      int res = catchObject();
      if (res == 1)
      {
        //Serial.println("取得完了");
        mode = 5;
      }
      else if (res == 2)
      {
        //Serial.println("喪失");
        mode = 3;
      }
    } else {
      forward_robot();
    }
    break;
      
  case 5:
    //Serial.println("運搬中");
    // ゴールに運ぶ
    if (role != FORWARD) {
      if(goal()){
        mode=3;
      } 
    } else {
      forward_robot();
    }
  
    break;

  case 6: // 緊急対応
  //Serial.println("緊急対応");
    // 必要なら記述
    motorL = motorR = 0;
    break;

  default:
    // 未定義のmodeは安全停止
    motorL = motorR = 0;
    break;


    motors.setLeftSpeed(motorL);
    motors.setRightSpeed(motorR);
  }
  
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
  unsigned long timeNow1 = millis();

  if (timePrev1 == 0) timePrev1 = timeNow1;
  switch (searchMode)
  {
  case 0://直進
    motorL = motorR = 200;
      if (timeNow1 - timePrev1 > 1000)
      {
        timePrev1 = timeNow1;
        searchMode = 1;
      }
      break;
  case 1://回転
    motorR = -180;
    motorL = 180;
    if (dist < 40&&dist!=0)
    { 
      if(distPrev!=0&&dist>distPrev){
        distPrev=0;
        searchMode = 2;
        return 1;
      }
      distPrev = dist;
      
    }
    if (timeNow1 - timePrev1 > 300)
    {
      timePrev1 = timeNow1;
      searchMode = 2;
    }
    break;
  
  case 2: // 直進
    motorL = motorR = 200;
    if (timeNow1 - timePrev1 > 1700)
      {
        timePrev1 = timeNow1;
        searchMode = 4;
      }
    if (color != WHITE)
    {
      timePrev1 = timeNow1;
      searchMode = 5;
    }
    if(avg_ax>3000){ //段差検知
      timePrev1 = timeNow1;
      searchMode = 3;
    }
    break;
    
  case 3: // 後退
    motorL = motorR = -200;
    if (timeNow1 - timePrev1 > 1000)
    {
      timePrev1 = timeNow1;
      searchMode = 4;
    }
    if (color != WHITE)
    {
      timePrev1 = timeNow1;
      searchMode = 5;
    }
    break;
  case 4: // 回転
    motorR = -180;
    motorL = 180;
    if (dist < 40&&dist!=0)
    {
      if(distPrev!=0&&dist>distPrev){
        distPrev=0;
        searchMode = 2;
        return 1;
      }
      distPrev = dist;
      return 1;
    }
    if (timeNow1 - timePrev1 > 573)
    {
      timePrev1 = timeNow1;
      searchMode = 2;
    }
    break;
  case 5:// 黒赤青
  motorL = motorR = -200;
  if (timeNow1 - timePrev1 > 500)
    {
      timePrev1 = timeNow1;
      searchMode = 6;
    }
    break;
  case 6://黒赤青
  motorL = 200;
  motorR = -200;
  if (timeNow1 - timePrev1 > 600)
    {
      timePrev1 = timeNow1;
      searchMode = 2;
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
          catchMode = 2;
        } else {
          return 2; // 敵機判定（失敗）
        }
      }
      break;

    /*case 1:
      motorL = motorR = -200;
      if (timeNow2 - timePrev2 > 300) {
        timePrev2 = timeNow2;
        catchMode = 2;
      }
      break;*/

    case 2: // 接近して保持
    //Serial.print("接近中");
      if (dist < 5 && dist != 0) {
        catchMode = 3;
      }
      else if(dist > 80){
        return 2; // 見失う
      }
      break;

    case 3: // 完了
    //Serial.println("終了");
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
  static unsigned long timePrev_g=0;
  static unsigned long timeNow_g=0;
  switch (goalMode)
  {
  case 0: // 回転
    // ゴール方向に最短で向かうように
    if(relativeHeading(angle, goalAngle) > 0.0)
    {
      motorL = 180;
      motorR = -180;
    }
    else
    {
      motorL = -180;
      motorR = 180;
    }
    if (relativeHeading(angle, goalAngle) < 5.0 && relativeHeading(angle, goalAngle) > -5.0) // 目標方角に到達
      goalMode = 1;
    break;

  case 1: // 前進
    motorL = motorR = 200;
    if(color==RED||color==BLUE){
      goalMode = 2;
      timePrev_g=millis();
    }
    break;
  case 2:
    motorR=motorL=-200;
    timeNow_g=millis();
    if(timeNow_g-timePrev_g>1000){
      goalMode=0;
      return 1;
    }
    break;
  }
  return 0;
}

// 役割の分類
int ClassifyRole()
{ 
  if (230 < angle && angle < 320)
  {
    return BACKWARD;
  }
  else if (angle >= 60 && angle < 130)
  {
    return FORWARD;
  }
  else
  {
    if(angle>=320||angle<60){
      isBLUE=false;
    }else{
      isBLUE=true;
    }
    return CLIMB;
  }
  return role;
}


// 再初期化関数：ボタン押下時に呼ぶ（キャリブレーションも行う）
/*void reinitializeAll()
{
  // // 即時モーター停止
  // motorL = 0;
  // motorR = 0;
  // motors.setLeftSpeed(0);
  // motors.setRightSpeed(0);

  // Serial.println("[INFO] reinitializeAll: モーター停止センサの状態をリセット");

  // // センサー類の再セットアップ（ハード初期化）
  // initWaveSensor();
  // setupCompass();

  // // 状態変数リセット
  // dist = 0;
  // angle = 0;
  // color = 0;
  // role = -1;
  // mode=0;
  // timePrev = millis();

  // // カラーセンサーのキャリブレーション
  // button.waitForButton();
  // CalibrationColorSensor();

  // // 地磁気センサのキャリブレーション
  // button.waitForButton();
  // calibrationCompass();

  // // 最初に向いている方向を取得
  // button.waitForButton();
  // angle = averageHeading(); // 向いている方角の取得
  // role = ClassifyRole();
  mode=3;
}*/
void reinitializeAll()
{
  motorL = motorR = 0;
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);

  // 状態変数リセット
  mode = 0;
  climb_mode = 0;
  avg_ax = 0;

  // 時間リセット
  timePrev = millis();

  // 角度再取得
  float sum = 0.0f;
  for (int i = 0; i < 10; i++) {
    sum += averageHeading();
    delay(5);
  }
  angle = sum / 10.0f;

  role = ClassifyRole();
  if(role==CLIMB){
   mode=3;
  }
}
