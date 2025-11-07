#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating
#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M の値 ： 地磁気センサーのスケールを +/-2.5 ガウスに設定
#define CRA_REG_M_220HZ 0x1C // CRA_REG_M の値 ： 地磁気センサのアップデートレートを 220 Hz に設定

LSM303 compass;
float mx, my, mz; // 地磁気センサーの値
// 最初の方向を記録する変数
float initialHeading = 0.0; 

void setupCompass() // 地磁気センサーのセットアップ
{
  compass.init();
  compass.enableDefault();
  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // 地磁気センサーのスケールを +/-2.5 ガウスに設定
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ); // 地磁気センサのアップデートレートを 220 Hz に設定

  delay(1000);

  // 最初の方向（初期方位）を記録する
  initialHeading = averageHeading();  // 初期方位を記録
}

void  calibrationCompass()
{
  unsigned int index;
  int motorL, motorR;

  LSM303::vector<int16_t> running_min = {
    32767, 32767, 32767
  }
  , running_max = {
    -32767, -32767, -32767
  };

  motorL = 217;
  motorR = -200;
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);

  for (index = 0; index < CALIBRATION_SAMPLES; index ++)
  {
    // 地磁気センサ（LSM303）から磁気ベクトルを読み取る
    compass.read();

    running_min.x = min(running_min.x, compass.m.x);
    running_min.y = min(running_min.y, compass.m.y);

    running_max.x = max(running_max.x, compass.m.x);
    running_max.y = max(running_max.y, compass.m.y);

    delay(50);
  }

  motorL = 0;
  motorR = 0;
  motors.setLeftSpeed(motorL);
  motors.setRightSpeed(motorR);

  // Set calibrated values to compass.m_max and compass.m_min
  compass.m_max.x = running_max.x;
  compass.m_max.y = running_max.y;
  compass.m_min.x = running_min.x;
  compass.m_min.y = running_min.y;
}

void getAcc(float& ax, float& ay, float& az)
{
  compass.read();
  ax = compass.a.x;
  ay = compass.a.y;
  az = compass.a.z;
}

void CalibrationCompassManual()
{
  compass.m_min.x = 0;
  compass.m_min.y = 0;
  compass.m_max.x = 0;
  compass.m_max.y = 0;
}

template <typename T> float heading(LSM303::vector<T> v)
{
  float x_scaled =  2.0 * (float)(v.x - compass.m_min.x) / ( compass.m_max.x - compass.m_min.x) - 1.0;
  float y_scaled =  2.0 * (float)(v.y - compass.m_min.y) / (compass.m_max.y - compass.m_min.y) - 1.0;

  float angle = atan2(y_scaled, x_scaled) * 180 / M_PI;
  if (angle < 0)
    angle += 360;
  return angle;
}

// Yields the angle difference in degrees between two headings
float relativeHeading(float heading_from, float heading_to)
{
  float relative_heading = heading_to - heading_from;

  // constrain to -180 to 180 degree range
  if (relative_heading > 180)
    relative_heading -= 360;
  if (relative_heading < -180)
    relative_heading += 360;

  return relative_heading;
}

// Average 10 vectors to get a better measurement and help smooth out
// the motors' magnetic interference.
float averageHeading()
{
  LSM303::vector<int32_t> avg = {
    0, 0, 0
  };

  for (int i = 0; i < 10; i ++)
  {
    compass.read();

    avg.x += compass.m.x;
    avg.y += compass.m.y;
  }
  avg.x /= 10.0;
  avg.y /= 10.0;

  // avg is the average measure of the magnetic vector.
  return heading(avg);
}

// float averageHeadingLP()
// {
//   static LSM303::vector<int32_t> avg = {
//     0, 0, 0
//   };

//   compass.read();
//   avg.x = 0.2 * compass.m.x + 0.8 * avg.x;
//   avg.y = 0.2 * compass.m.y + 0.8 * avg.y ;

//   return heading(avg);
// }

float offset() {
  const int samples = 100;
  long sum_x = 0, sum_y = 0, sum_z = 0;
  for (int i = 0; i < samples; i++) {
    compass.read();
    sum_x += compass.a.x;
    sum_y += compass.a.y;
    sum_z += compass.a.z;
    delay(10);
  }
  ax_offset = sum_x / (float)samples;
  ay_offset = sum_y / (float)samples;
  az_offset = (sum_z / (float)samples) - 16384.0;
}
