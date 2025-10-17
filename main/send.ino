int high_d, low_d;
int high_a, low_a;

void sendData(){
    angle*=100;

    high_d = dist >> 8; // highにvalの上位8ビットを代入
    low_d = dist & 255; // lowにvalの下位8ビット
    high_a = ((int)angle) >> 8; // highにvalの上位8ビットを代入
    low_a = ((int)angle) & 255; // lowにvalの下
    color = color/4;

    Serial.write('H');
    Serial.write(high_d);
    Serial.write(low_d);
    Serial.write(high_a);
    Serial.write(low_a);
    Serial.write(color);

}