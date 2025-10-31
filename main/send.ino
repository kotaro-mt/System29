// Non-blocking, atomic framed send
// Frame layout (9 bytes):
// 0: 'H'
// 1: dist high
// 2: dist low
// 3: angle*10 high
// 4: angle*10 low
// 5: mode high
// 6: mode low
// 7: color
// 8: terminator '\n'

const uint8_t FRAME_LEN = 9;

void sendData(){
    int angle_s = (int)(angle * 10.0f); // existing scaling
    uint16_t u_dist  = (uint16_t)dist;
    uint16_t u_angle = (uint16_t)angle_s;
    uint16_t u_mode  = (uint16_t)mode;
    uint8_t u_color  = (uint8_t)color;

    uint8_t buf[FRAME_LEN];
    buf[0] = 'H';
    buf[1] = (u_dist >> 8) & 0xFF;
    buf[2] = u_dist & 0xFF;
    buf[3] = (u_angle >> 8) & 0xFF;
    buf[4] = u_angle & 0xFF;
    buf[5] = (u_mode >> 8) & 0xFF;
    buf[6] = u_mode & 0xFF;
    buf[7] = u_color;
    buf[8] = '\n';

    // Avoid blocking: only write if TX buffer has enough space
    // (availableForWrite() returns # bytes that can be written without blocking)
#if defined(ARDUINO) && defined(Serial)
    if (Serial.availableForWrite() >= FRAME_LEN) {
        Serial.write(buf, FRAME_LEN);
    } else {
        // Skip this frame to avoid blocking; next loop will try again
    }
#else
    // Fallback (shouldn't happen on Arduino)
    Serial.write(buf, FRAME_LEN);
#endif

}