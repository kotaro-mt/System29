#include <LSM303.h> // LSM303ライブラリのヘッダーファイル

#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M の値 ： 地磁気センサーのスケールを +/-2.5 ガウスに設定
#define CRA_REG_M_220HZ 0x1C // CRA_REG_M の値 ： 地磁気センサのアップデートレートを 220 Hz に設定

LSM303 compass;
int mx, my, mz; // 地磁気センサーの値
int ax, ay, az; // 加速度センサーの値

void setupCompass() // 地磁気センサーのセットアップ
{
  compass.init();
  compass.enableDefault();
  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // 地磁気センサーのスケールを +/-2.5 ガウスに設定
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ); // 地磁気センサのアップデートレートを 220 Hz に設定

  delay(1000);
}

void getCompass() // 地磁気センサーの値の取得
{
  compass.read();
    mx = compass.m.x;
    my = compass.m.y;
    mz = compass.m.z;
    ax = compass.a.x;
    ay = compass.a.y;
    az = compass.a.z;
}