/*unsigned long b_time;
float b_angle;
int b_mode = 0;
bool isChecking = false; // グローバル変数
bool targetAngleRecorded = false;  // 角度が記録済みかどうか

void Back() {
  if (role == BACKWARD) {
    switch (b_mode) {
      case 0: // INIT
        Serial.println("[Back] INIT");
        b_time = millis();
        b_mode = 1;
        break;

      case 1: // FORWARD：直進
        Serial.println("[Back] FORWARD");
        motorL = motorR = 200;

        if (millis() - b_time >= 1000) { // 1秒後に探索へ
          b_mode = 2;
          b_time = millis();
          Serial.println("[Back] → SEARCHへ移行");
        }

        if (color == BLACK || color == RED || color == BLUE) {
          b_mode = 7; // 色反応処理
          b_time = millis();
          Serial.println("[Back] 色検出 → color_moveへ");
        }
        break;

      case 2: // SEARCH：探索（1秒だけ旋回）
        if (millis() - b_time >= 1000) { // 1秒経過したら再直進
          b_mode = 1;
          b_time = millis();
          Serial.println("[Back] SEARCH終了 → 再FORWARD");
        } else {
          motorL = -150;  // 左旋回
          motorR = 150;
        }

        // 途中で物体を発見した場合
        if (dist > 0) {
          if (!targetAngleRecorded) {     // まだ角度を記録していなければ
            b_angle = angle;             // 現在角度を記録
            targetAngleRecorded = true;
            Serial.print("[Back] 物体発見 → 角度記録: ");
            Serial.println(b_angle);
          }
          b_mode = 3;                      // CHECKへ
          b_time = millis();
          Serial.println("[Back] → CHECKへ");
        }
        break;

      case 3: // CHECK：静止判定
          Serial.println("[Back] CHECK開始");

        if (Check(dist, checkStartTime, prevDist, changeCount, 500, 3)) {
          b_mode = 4;
          b_time = millis();
          Serial.println("[Back] 静止確認 → HITへ");
        } else if (millis() - checkStartTime >= 500) {
          b_mode = 2;
          b_time = millis();
          Serial.println("[Back] 動いている → SEARCHへ戻る");
        }
        break;

      case 4: // HIT：発見・接近
        Serial.println("[Back] HIT");
        motorL = motorR = 200;
        if (dist <= 5) {
          b_mode = 5;
          b_time = millis();
          Serial.println("[Back] 十分接近 → RETURNへ");
        }
        break;

         case 5: // RETURN：角度を使って反転
      {
        // 対象を見失った場合 → 再探索
        if (dist == 0) {
          b_mode = 2;
          b_time = millis();
          targetAngleRecorded = false; // 角度リセット
          Serial.println("[Back] RETURN中に対象ロスト → SEARCHへ戻る");
          break;
        }

        // 目標角度 = b_angle + 137（±180範囲に正規化）
        float target = b_angle + 137.0;
        if (target >= 360.0) target -= 360.0;
        else if (target < 0.0) target += 360.0;

        float diff = angle - target;
        if (diff > 180.0) diff -= 360.0;
        if (diff < -180.0) diff += 360.0;

        Serial.print("[Back] RETURN diff=");
        Serial.println(diff);

        if (fabs(diff) > 10.0) { // ±10°以内まで許容
          if (diff > 0) {
            motorL = -150;
            motorR = 150;
          } else {
            motorL = 150;
            motorR = -150;
          }
        } else {
          motorL = motorR = 0;
          b_mode = 6;
          b_time = millis();
          targetAngleRecorded = false; // 次回に備えてリセット
          Serial.println("[Back] 反転完了 → STRAIGHTへ");
        }
      }
      break;

      case 6: // STRAIGHT：復帰直進
        Serial.println("[Back] STRAIGHT");
        motorL = motorR = 200;
        if (color == BLACK || color == RED || color == BLUE) {
          b_mode = 7;
          b_time = millis();
          Serial.println("[Back] 色検出 → color_moveへ");
        }
        break;

      case 7: // 色検出処理（共通関数で実行）
        Serial.println("[Back] color_move実行");
        //color_move(color, b_time);
        break;
    }
  }
}*/
