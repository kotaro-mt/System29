#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h> // 通信のヘッダーファイル

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

int mode = 0; // 動作モード
int motorL, motorR; // モーター速度
float red,green,blue; // RGB値

void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupCompass();

  //カラーセンサーのキャリブレーション
  button.waitForButton();
  CalibrationColorSensor();

  //地磁気センサのキャリブレーション
  button.waitForButton();
  calibrationCompass()
}

void loop() {

  getRGB(red, green, blue); // RGB値の取得
  Serial.println("R:" + String(red) + " G:" + String(green) + " B:" + String(blue));

}
