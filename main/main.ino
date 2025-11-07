#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h> // 通信のヘッダーファイル
#include <LSM303.h> // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル


ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

int mode = 0; // 動作モード
int motorL, motorR; // モーター速度
float red,green,blue; // RGB値
int dist=0; // オブジェクトまでの距離
float angle=0;  //向いている方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0; // 色判定用変数
uint8_t role = -1; // 役割判定用変数
enum Color {WHITE, BLACK, RED, BLUE }; // 色の定義
enum Role {FORWARD, BACKWARD,CLIMB}; // 役割の定義
float x = 0.0, y = 0.0; // マップに対するXY座標(赤のラインの左側を原点とする)
float ax, ay, az; // 加速度センサーの値


void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

  role = ClassifyRole(); // ロール分類

  //カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  //地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  button.waitForButton();
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
      break;

    case 2:
      // 山登りモード
      MountClimb();
      break;

    case 3:
      // 探索モード
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

int ClassifyRole() { // 役割の分類
  if(230 < angle && angle < 320){
    role = FORWARD;
  }
  else if(angle >= 60 && angle < 130){
    role = BACKWARD;
  }
  else{
    role = CLIMB;
  }
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
