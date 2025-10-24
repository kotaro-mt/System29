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
int dist; // オブジェクトまでの距離
float angle;  //向いている方角
unsigned long timeNow, timePrev; // 時間計測用変数
uint8_t color = 0; // 色判定用変数
uint8_t role = -1; // 役割判定用変数
enum Color { WHITE,BLACK, RED, BLUE }; // 色の定義
enum Role {FORWARD, BACKWARD,CLIMB}; // 役割の定義


void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  initWaveSensor();

  //カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  //地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass();

  // 初回送信時間の設定
  timePrev = millis();
}

void loop() {
  getRGB(red, green, blue); // RGB値の取得

  dist = distance(); // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得
  color=classifyColor(); // 色の分類
  
  timeNow = millis();
  

  if(timeNow - timePrev > 500){
    timePrev = timeNow;
    sendData();

    

    // Serial.println("R:" + String(red) + " G:" + String(green) + " B:" + String(blue));
    // Serial.println("Distance:" + String(dist) + "cm");
    // Serial.println("Angle:" + String(angle) + "deg");

  }

}

// 探索についての関数
void serch() {
  unsigned long timeNow1; // 微調整用の時間

  if (5 < dist && dist < 30) {
    if (millis() - timeNow1 > 2){
      motorL = motorR = 0;
    }
  } else {
    motorR = 150;
    motorL = -150;
    timeNow1 = millis();
  }
  
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);
}

void ClassifyRole() { // 役割の分類
  if(angle < 180){
    role = FORWARD;
  }
  else if(angle >= 180){
    role = BACKWARD;
  }
  else{
    role = CLIMB;
  }
}

void mountClimb() { // 登攀動作

  
}
