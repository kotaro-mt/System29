void sendData(){
  // ハンドシェイク：'H'を受信したら送信
  if(Serial.read()=='H'){
    Serial.println(String(dist) + "," + String(angle) + "," + String(mode) + "," + String(color) + "," + String(ax) + "," + String(ay) + "," + String(az));
    //delay(100); // 送信頻度調整（XBeeのバッファ溢れ防止）
  } 
}