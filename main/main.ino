#include <ZumoMotors.h> // モーターライブラリのヘッダーファイル
#include <Pushbutton.h> // プッシュボタンプッシュボタンライブラリのヘッダーファイル
#include <Wire.h>       // 通信のヘッダーファイル
#include <LSM303.h>     // LSM303ライブラリのヘッダーファイル
#include <ZumoBuzzer.h> // ブザーライブラリのヘッダーファイル

// インスタンス定義
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

const int id=1; //processsing通信用
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
unsigned long b_time;

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
bool isColorAction = false;   // 色動作中フラグ
int modeBeforeColor = 0;      // 色動作前の mode を保存

// フラグ：goal 中に色割込みが入ったか
bool fromGoal = false;
// 黒フェーズ用
int blackPhase = 0;
unsigned long blackPhaseTime = 0;
bool blackPhaseStarted = false;
float blackStartAngle = 0.0f;     // 黒検出時の角度固定
float blackTargetAngle = 0.0f;

// 赤フェーズ用
int redPhase = 0;
unsigned long redPhaseTime = 0;
bool redPhaseStarted = false;
float redStartAngle = 0.0f;     // 黒検出時の角度固定
float redTargetAngle = 0.0f;

// 青フェーズ用
int bluePhase = 0;
unsigned long bluePhaseTime = 0;
bool bluePhaseStarted = false;
float blueStartAngle = 0.0f;     // 黒検出時の角度固定
float blueTargetAngle = 0.0f;

// 回転方向保持（1＝左,-1＝右）
int rotateDir = 0;

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

  // 目標方角の設定(各自設定する必要あり)
  // 目標方角の設定（修正：条件と演算を正しく）
  /*if (role == FORWARD) {
    goalAngle += 90.0f;
    if (goalAngle >= 360.0f) goalAngle -= 360.0f;
  } else if (role == BACKWARD) {
    goalAngle -= 90.0f;
    if (goalAngle < 0.0f) goalAngle += 360.0f;
  } else if (role == CLIMB) {
    goalAngle -= 180.0f;
    if (goalAngle < 0.0f) goalAngle += 360.0f;
  }*/
        
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

  getRGB(red, green, blue); // RGB値の取得
  getAcc(ax, ay, az);       // 加速度の取得

  dist = distance();        // オブジェクトまでの距離の取得
  angle = averageHeading(); // 向いている方角の取得
  color = classifyColor();  // 色の分類

  timeNow = millis();

  if (timeNow - timePrev > 500)
  {
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

  // 色割込み判定（どのモードからでも安全に遷移)
  // 色動作が発生していない状態で,黒/赤/青を検出した場合mode=99 に遷移
  if (!isColorAction && (color == BLACK || color == RED || color == BLUE)) {
    isColorAction = true;
    modeBeforeColor = mode;
    c_time = millis();
    colorBeforeAction = color; // 検出色を保存
    Serial.print("[COLOR] 検出 -> COLOR_ACTIONを実行. color=");
    Serial.println(color);
    mode = 99; // 色専用モード
  }

  switch (mode)
  {
  case 0: // 初期化処理
    climb_mode=0;
    // place(); //位置情報の初期設定
    Serial.println(role);
    // roleを文字列で表示
    Serial.print("[ROLE] 現在の役割：");
    if(role == FORWARD){
      Serial.println("FORWARD");
      goalAngle += 90.0f;
    if (goalAngle >= 360.0f)
      goalAngle -= 360.0f;
      mode = 1;
    }
    else if(role == CLIMB){
      Serial.println("CLIMB");
      goalAngle -= 180.0f;
    if (goalAngle < 0.0f)
      goalAngle += 360.0f;
      mode = 2;
    }
    else{
      Serial.println("BACKWARD");
      goalAngle -= 90.0f;
    if (goalAngle < 0.0f)
      goalAngle += 360.0f;
      mode = 3;
    }
    break;
    
  case 1:
    // 敵陣ロボットの移動
    //place();
    forward_robot();
    break;

  case 2:
    // 山登りモード
    if (MountClimb())
      mode = 3;
    break;

  case 3:
    Serial.println("探索");
    // 探索モード
    if(search())
      mode=4;
    break;

  case 4:
    Serial.println("接近・取得");
    // 宝物を見つけて取りに行く
    if (catchObject() == 1)
    {
      Serial.println("取得完了");
      mode = 5;
    }
    else if (catchObject() == 2)
    {
      Serial.println("喪失");
      mode = 3;
    }
    break;
      
  case 5:
    Serial.println("運搬中");
    // ゴールに運ぶ
    if(goal()){
      mode=3;
    }
    break;

  case 6: // 緊急対応
  Serial.println("緊急対応");
    // 必要なら記述
    motorL = motorR = 0;
    break;

  case 99: // 色動作専用モード（割込み）
    {
      bool finished = color_move(colorBeforeAction);
      if (finished) {
        Serial.println("[COLOR] 終了 -> 前のモードに戻る");
        isColorAction = false;
        
        // フェーズ初期化
        blackPhase = redPhase = bluePhase = 0;
        blackPhaseStarted = redPhaseStarted = bluePhaseStarted = false;
        rotateDir = 0; // 回転方向リセット    
        // goal 中に割り込まれていたら探索へ戻す
        if (fromGoal) {
          fromGoal = false;
          mode = 3;
        } else {
          mode = modeBeforeColor;  // 元のモードに戻す
        }
        }
    }
    break;

  default:
    // 未定義のmodeは安全停止
    motorL = motorR = 0;
    break;


    motors.setLeftSpeed(motorL);
    motors.setRightSpeed(motorR);
  }
}

//===================
// 共通関数群
//===================

// 角度回転専用関数（安定判定付き）
// targetAngle: 目標角度, tolerance: 許容誤差（度）
bool rotateToAngle(float targetAngle, float tolerance)
{
    float diff = relativeHeading(angle, targetAngle);

    // 到達判定
    if (fabs(diff) <= tolerance) {
        motorL = motorR = 0;
        rotateDir = 0; // 回転方向リセット
        return true;
    }

    // 回転方向決定（初回のみ）
    if (rotateDir == 0) {
        rotateDir = (diff > 0.0f) ? 1 : -1;
    }
    int dir = rotateDir;

    // P制御（減速付き）
    const float Kp = 4.0f;
    int speed = fabs(diff) * Kp;
    if (speed < 100) speed = 100;
    if (speed > 250) speed = 250;

    // 実際の回転
    if (dir > 0) {   // 左回り
        motorL = -speed;
        motorR =  speed;
    } else {         // 右回り
        motorL =  speed;
        motorR = -speed;
    }

    return false; // まだ到達していない
}

// 色処理
bool color_move(uint8_t detectedColor) {
    unsigned long t = millis();

    switch (detectedColor) {
        case BLACK:
            switch (blackPhase) {

                case 0: // 後退
                    if (!blackPhaseStarted) {
                        blackPhaseTime = t;
                        blackPhaseStarted = true;
                        Serial.println("黒フェーズ0開始");
                    }

                    Serial.println("後退[黒]");
                    motorL = motorR = -200;

                    if (t - blackPhaseTime >= 400) {
                        blackPhase++;
                        blackPhaseStarted = false;  // 次のフェーズ用に必ずリセット
                    }
                    break;

                case 1: // 回転
                    if (!blackPhaseStarted) {
                        Serial.println("★ 黒フェーズ1開始（角度設定）");
                        blackStartAngle = angle;
                        blackTargetAngle = blackStartAngle + 120.0f;
                        if (blackTargetAngle >= 360.0f) blackTargetAngle -= 360.0f;
                        blackPhaseStarted = true;
                    }

                    Serial.print("回転[黒] angle=");
                    Serial.println(angle);
                    if (rotateToAngle(blackTargetAngle, 5.0f)) {
                        blackPhase++;
                        blackPhaseStarted = false; // リセット
                    }
                    break;

                case 2: // 完了
                    Serial.println("終了[黒]");
                    blackPhase = 0;
                    blackPhaseStarted = false;
                    rotateDir = 0;
                    return true;
            }
            break;

        case RED:
            switch (redPhase) {
                case 0: // 前進
                if (!redPhaseStarted) {
                        redPhaseTime = t;
                        redPhaseStarted = true;
                    }
                    Serial.println("前進[赤]");
                    motorL = motorR = 200;
                    if (t - redPhaseTime >= 300) {
                        redPhase++;
                        redPhaseStarted = false;
                    }
                    break;
                case 1: // 後退
                    Serial.println("後退[赤]");
                    if (!redPhaseStarted) {
                      redPhaseTime = t; // 保険的に再セット
                      redPhaseStarted = true;
                     }
                    motorL = motorR = -200;
                    if (t - redPhaseTime >= 900) {
                        redPhase++;
                        redPhaseStarted = false;
                    }
                    break;
                case 2: // 回転
                    Serial.println("回転[赤]");
                    if (!redPhaseStarted) {
                        redStartAngle = angle;
                        redTargetAngle = redStartAngle + 180.0f;
                        if (redTargetAngle >= 360.0f) redTargetAngle -= 360.0f;
                        redPhaseStarted = true;
                        rotateDir = 0;
                    }
                    Serial.print("回転[赤] angle=");
                    Serial.println(angle);
                    if (rotateToAngle(redTargetAngle, 5.0f)) {
                        redPhase = 0;
                        redPhaseStarted = false;
                        rotateDir = 0;
                        return true;
                    }
                    break;
            }
            break;

        case BLUE:
            switch (bluePhase) {
                case 0: // 前進
                if (!bluePhaseStarted) {
                        bluePhaseTime = t;
                        bluePhaseStarted = true;
                    }
                    Serial.println("前進[青]");
                    motorL = motorR = 200;
                    if (t - bluePhaseTime >= 300) {
                        bluePhase++;
                        bluePhaseStarted = false;
                    }
                    break;
                case 1: // 後退
                    Serial.println("後退[青]");
                    if (!bluePhaseStarted) {
                      bluePhaseTime = t;
                      bluePhaseStarted = true;
                     }
                    motorL = motorR = -200;
                    if (t - bluePhaseTime >= 900) {
                        bluePhase++;
                        bluePhaseStarted = false;
                    }
                    break;
                case 2: // 回転
                    Serial.println("回転[青]");
                    if (!bluePhaseStarted) {
                        blueStartAngle = angle;
                        blueTargetAngle = blueStartAngle + 180.0f;
                        if (blueTargetAngle >= 360.0f) blueTargetAngle -= 360.0f;
                        bluePhaseStarted = true;
                        rotateDir = 0;
                    }
                    Serial.print("回転[青] angle=");
                    Serial.println(angle);
                    if (rotateToAngle(blueTargetAngle, 5.0f)) {
                        bluePhase = 0;
                        bluePhaseStarted = false;
                        rotateDir = 0;
                        return true;
                    }
                    break;
            }
            break;

        default: // WHITE 等
            motorL = motorR = 0;
            return true;
    }

    return false; // 完了していない
}

// 探索についての関数
int search()
{
  static int searchMode = 0;
  static unsigned long timePrev1 = 0;
  unsigned long timeNow1 = millis();

  if (timePrev1 == 0) timePrev1 = timeNow1;
  switch (searchMode)
  {
  case 0: // 直進
  Serial.println("探索[直進]");
    motorL = motorR = 200;
      if (timeNow1 - timePrev1 > 1000)
      {
        timePrev1 = timeNow1;
        searchMode = 2;
      }
    if (color != WHITE)
    {
      timePrev1 = timeNow1;
      searchMode = 1;
    }
    break;
  case 1: // 後退
  Serial.println("探索[後退]");
    motorL = motorR = -200;
    if (timeNow1 - timePrev1 > 500)
    {
      timePrev1 = timeNow1;
      searchMode = 0;
    }
    break;
  case 2: // 回転
  Serial.println("探索[回転]");
    motorR = -200;
    motorL = 200;
    if (dist < 30&&dist!=0)
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
      motorL = motorR = 150;
      if (dist < 5 && dist != 0) {
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
    return BACKWARD;
  }
  else if (angle >= 60 && angle < 130)
  {
    return FORWARD;
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

  Serial.println("[INFO] reinitializeAll: モーター停止センサの状態をリセット");

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
