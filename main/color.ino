#include <Adafruit_TCS34725.h> // カラーセンサライブラリのヘッダーファイル

#define COLORNUM 4 // 判定する色の数
// 松永
int colorValue[COLORNUM][3] = {
  {255, 255, 255},   // 白色の値
  {0,0,0}, //黒色の値
  {120, 30, 31},   // 赤色の値
  {20, 40, 80}    // 青色の値
};

// 喜多
/* int colorValue[COLORNUM][3] = {
   {255, 255, 255},   // 白色の値
   {0,0,0}, //黒色の値
   {122, 22, 26},   // 赤色の値
   {10, 30, 70}    // 青色の値
 };*/


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

unsigned int r_min, g_min, b_min; 
unsigned int r_max, g_max, b_max;

void  CalibrationColorSensor() // カラーセンサーのキャリブレーション
{
  unsigned long timeInit;
  unsigned int r, g, b, clr;

  tcs.begin(); // カラーセンサーのsetup
  
<<<<<<< HEAD
  motors.setSpeeds(100, 100); 
=======
  motors.setSpeeds(60, 60);
  //motors.setSpeeds(100, 100); // 喜多用
>>>>>>> d1c2bd5c2b25b1af75486c28627ebf4b2172faa2

  r_min = 30000;
  g_min = 30000;
  b_min = 30000;
  r_max = 0;
  g_max = 0;
  b_max = 0;

  timeInit = millis();

  while (1) 
  {
    tcs.getRawData(&r, &g, &b, &clr); // rowdataの取得
  
    if (r < r_min) r_min = r;
    if (g < g_min) g_min = g;
    if (b < b_min) b_min = b;
    if (r > r_max) r_max = r;
    if (g > g_max) g_max = g;
    if (b > b_max) b_max = b;

    if (millis() - timeInit > 2000)
      break;
  }
  
  motors.setSpeeds(0, 0);
}

void getRGB(float& r0, float& g0, float& b0)  // RGB値の取得
{
  unsigned int r, g, b, clr;

  tcs.getRawData(&r, &g, &b, &clr); // rowdataの取得
 
  r0 = map(r, r_min, r_max, 0, 255);
  g0 = map(g, g_min, g_max, 0, 255);
  b0 = map(b, b_min, b_max, 0, 255);

  if (r0 < 0.0) r0 = 0.0;
  if (r0 > 255.0) r0 = 255.0;
  if (g0 < 0.0) g0 = 0.0;
  if (g0 > 255.0) g0 = 255.0;
  if (b0 < 0.0) b0 = 0.0;
  if (b0 > 255.0) b0 = 255.0;

}

uint8_t classifyColor(){ //
  int minDistColor=-1; //最短距離の色
  float minDist=1000; //最短距離 
  for(int i=0;i<COLORNUM;i++){
    float dist = sqrt(pow(red - colorValue[i][0], 2) + pow(green - colorValue[i][1], 2) + pow(blue - colorValue[i][2], 2));
    if(dist < minDist){
      minDist = dist;
      minDistColor = i;
    } 
  }
  return minDistColor; //最短距離の色を返す
}

void linetrace_P(){
  static float lightMin =0; // 各自で設定
  static float lightMax =255; // 各自で設定
  static float speed = 100; // パラメーター
  static float Kp = 2.0; // パラメーター
  float lightNow;
  float speedDiff;

  lightNow = (red + green + blue) / 3.0;
  if (lightNow > (lightMin + lightMax) / 2.0) // 右回転
    speedDiff = Kp * speed;
  else // 左回転
    speedDiff = -Kp * speed;

  motorL = speed - speedDiff;
  motorR = speed + speedDiff;
}
