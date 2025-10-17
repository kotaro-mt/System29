const int trig = 2; //trigピンをaruduinoの2番ピンへ
const int echo = 4; //echoピンをaruduinoの4番ピンへ

int distance() {
  unsigned long interval; //echoのパルス幅（μs）
  int dist;               //距離（cm）

  digitalWrite(trig, HIGH); //10μsのパスるを超音波センサのTrigピンに出力
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  interval = pulseIn(esho, HIGH, 23071);
  dist = (0.61 * 25 + 331.5) * interval / 10000 / 2;   //距離(cm)に変換

  // 移動平均フィルタの実施
  buffer[index] = dist;
  index = (index + 1) % n;

  int sum = 0;
  for(int i = 0; i < n; i++) {
    sum += buffer[i];
  }
  int avg = sum / n;
  return avg;

  delay(60);                     //trigがHIGHになる間隔を60ms以上空ける（超音波センサの仕様）

  return avg;
}