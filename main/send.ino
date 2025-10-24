int high_d, low_d;
int high_a, low_a;
int high_m, low_m;

void sendData(){
    float angle_s;
    angle_s=angle*100;

    high_d = dist >> 8; // highにvalの上位8ビットを代入
    low_d = dist & 255; // lowにvalの下位8ビット
    high_a = ((int)angle_s) >> 8; // highにvalの上位8ビットを代入
    low_a = ((int)angle_s) & 255; // lowにvalの下
    high_m = mode >> 8;
    low_m = mode & 255;

    Serial.write('H');
    Serial.write(high_d);
    Serial.write(low_d);
    Serial.write(high_a);
    Serial.write(low_a);
    Serial.write(high_m);
    Serial.write(low_m);
    Serial.write(color);

    Serial.println("OK");

}