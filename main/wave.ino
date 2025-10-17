const int trig = 2; //trigピンをaruduinoの2番ピンへ
const int echo = 4; //echoピンをaruduinoの4番ピンへ

void initWaveSensor() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

int distance() {
  unsigned long interval; //echoのパルス幅（μs）
  int dist;               //距離（cm）

  digitalWrite(trig, HIGH); //10μsのパスるを超音波センサのTrigピンに出力
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  interval = pulseIn(echo, HIGH, 23071);
  dist = (0.61 * 25 + 331.5) * interval / 10000 / 2;   //距離(cm)に変換
          
  return dist;
}