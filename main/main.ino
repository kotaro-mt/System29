#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h> // 通信のヘッダーファイル
#include <LSM303.h> // LSM303ライブラリのヘッダーファイル


ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

int mode = 0; // 動作モード
int motorL, motorR; // モーター速度
float red,green,blue; // RGB値
int dist; // オブジェクトまでの距離
float angle;  //向いている方角

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
}

void loop() {

  getRGB(red, green, blue); // RGB値の取得
  dist = distance(); // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得


  Serial.println("R:" + String(red) + " G:" + String(green) + " B:" + String(blue));
  Serial.println("Distance:" + String(dist) + "cm");
  Serial.println("Angle:" + String(angle) + "deg");
  delay(500);

}

// 探索についての関数(敵陣用ロボ)
void serch() {

}