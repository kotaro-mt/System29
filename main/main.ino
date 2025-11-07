#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h> // 通信のヘッダーファイル
#include <LSM303.h> // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

// インスタンス定義
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

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
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

  role = ClassifyRole(); // ロール分類
  mode = 0;

  //カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  //地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  button.waitForButton();
  angle = averageHeading(); // 向いている方角の取得
  role = ClassifyRole();
  // 初回送信時間の設定
  timePrev = millis();
}

void loop() {

  // ボタン押下で初期化処理
  if (digitalRead(ZUMO_BUTTON) == LOW) { // 押された
    delay(40); 
    if (digitalRead(ZUMO_BUTTON) == LOW) {
      reinitializeAll();
    }
  }
  
  getRGB(red, green, blue); // RGB値の取得
  getAcc(ax, ay, az); // 加速度の取得

  dist = distance(); // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得
  color=classifyColor(); // 色の分類
  
  timeNow = millis();
  

  if(timeNow - timePrev > 500){
    timePrev = timeNow;
    //sendData();

  }

 switch (mode) {
    case 0: //初期化処理
      motorL = motorR = 0;
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

    case 2:
      // 山登りモード
      MountClimb();
      break;

    case 3:
      // 探索モード
      Back();
      break;

    case 4:
      // 宝物を見つけて取りに行く
      break;

    case 5:
      // 宝物を見つけて投げ飛ばす
      break;

    case 6:
      // ゴールに運ぶ
      break;

    case 7: //緊急対応
      
      break;
  }

  // モーター出力の反映（各モードが motorL/motorR を設定する想定）
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);
}

//===================
// 共通関数群
//===================

// 探索についての関数
void serch() {
  unsigned long timeNow1; // 微調整用の時間

  if (5 < dist && dist < 30) {
    if (millis() - timeNow1 > 2){ // 正面にオブジェクトが来るようにするための時間調整
      if (role == FORWARD){ // 敵陣用ロボ
        motorL = motorR = 150; // 接近
        if (dist < 5) { // 距離が5センチ未満になったらキャッチとして判定
          mode = 2;
        }
      }
    }
  } else {
    motorR = 150;
    motorL = -150;
    timeNow1 = millis();
  }
}

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
  }
}


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
  }
  return role;
}


// 再初期化関数：ボタン押下時に呼ぶ（キャリブレーションも行う）
void reinitializeAll() {
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
  timePrev = millis();

    // カラーセンサーのキャリブレーション（ユーザー操作を待つ）
    button.waitForButton();
    CalibrationColorSensor();


    button.waitForButton();
    calibrationCompass();

    button.waitForButton();
    angle = averageHeading(); // 向いている方角の取得
    role = ClassifyRole();
 
}
