//int dist1,dist2,dist3; // 距離
//float angle1,angle2,angle3; // 地磁気センサの値
//int mode1,mode2,mode3; // ロボットのモード
//int col1,col2,col3;
//int zumo_id = 0; // ロボットの番号

int f_time;
float f = 1, map_f = 1.8, map_robot_f = 0.1; // 縮尺系
float robot1_x = 100, robot1_y = 100;

 ////左上を１右上を２左下を３右下を４としている
 //int ox1 = 0, oy1 = 0; // 左上のマスの原点
 //int ox2 = width, oy2 = 0; // 右上のマスの原点
 //int ox3 = 0, oy3 = height / 2; // 左下のマスの原点
 //int ox4 = width / 2, oy4 = height / 2; //右下のマスの原点

void setup() {
 size(1000, 700); // 中心(250, 175), 大きさ(500, 350)
 f_time = millis();
}

void draw() {
 background(0);
 window_squ(); // 画面を4分割
 draw_robot(325, 175, angle1, f); // ロボット1の回転の中心点(325, 175)とする
 draw_robot(325 + width / 2, 175, angle2, f); // ロボット2の回転の中心点(325, 175)とする
 draw_robot(325, 175 + height / 2, angle3, f); // ロボット3の回転の中心点(325, 175)とする
 map_draw(width / 2, height / 2, map_f);
 Text(20, 30);
  
 // 回転確認用
 if (millis() - f_time > 10) {
   f_time = millis();
   //angle1++;
   //angle2--;
 }
}

void window_squ() {
 strokeWeight(2); // 線の太さ
 stroke(255, 255, 255); //線の色（白）
 line(width / 2, 0, width / 2, height);
 line(0, height / 2, width, height / 2);
 noFill();
}
void draw_robot(float x, float y, float angle, float f) {
 strokeWeight(1);
 // zumorobotの周りの円
 float line_num = 5 * f; // 四方に配置されている線の長さ
 float radius = 150 * f; // 円の半径
 translate(x, y); // 原点位置移動
 rotate(radians(angle + 90)); // 回転
 stroke(255);
 ellipse(0, 0, radius * 2, radius * 2); // 円を作図
 line(0, radius - line_num, 0, radius + line_num); // 下
 line(0, -radius - line_num, 0, -radius + line_num); // 上
 line(radius - line_num, 0, radius + line_num, 0); // 右
 line(-radius - line_num, 0, -radius + line_num, 0); // 左
  
 // zumorobotの作図
 // Aruduino本体
 float aru_y = 70 * f;
 translate(0, aru_y);
 float aru_w = 80 * f, aru_h = 80 * f; // aumo本体の縦横幅
 int aru_R = 32, aru_G = 178, aru_B = 170; // aruduinoの色
 rectMode(CENTER); // 図形の中心を原点と指定
 stroke(aru_R, aru_G, aru_B); // Aruduinoの色
 fill(aru_R, aru_G, aru_B); // Aruduinoの色
 rect(0, 0, aru_w, aru_h);
 noFill();
  
 // キャタピラー
 strokeWeight(3);
 float tire_w = 20 * f, tire_h = aru_h; // キャタピラーの大きさ
 stroke(112,128,144);
 rect(-(aru_w / 2) - tire_w, 0, tire_w, tire_h); // 左キャタピラー
 rect((aru_w / 2) + tire_w, 0, tire_w, tire_h); // 右キャタピラー
 for (float i = -(tire_h / 2); i <= tire_h / 2; i = i + 10) {
   rect(-(aru_w / 2) - tire_w, i, tire_w, 0); // 左の模様
   rect((aru_w / 2) + tire_w, i, tire_w, 0); // 右の模様
 }
  
 // クワガタのところ
 float k1_w = 20 * f, k1_h = 15 * f;
 float k2_w = aru_w * f, k2_h = 15 * f;
 float k3_w = 15 * f, k3_h = 40 * f;
 float k4_w = k3_w, k4_h = 40 * f;
 float k5_w = 25 * f, k5_h = 30 * f;
 int k_R = 248, k_G = 180, k_B = 0;
 strokeWeight(1);
 stroke(k_R, k_G, k_B);
 fill(k_R, k_G, k_B);
 rect(-(aru_w / 2) + (k1_w / 2) + 8, -(aru_h / 2) - 5 - (k1_h / 2), // 小さい四角
      k1_w, k1_h);
 rect((aru_w / 2) - (k1_w / 2) - 8, -(aru_h / 2) - 5 - (k1_h / 2),
       k1_w, k1_h);
 rect(0, -(aru_h / 2) - k1_h - 5 - (k2_h / 2), k2_w, k2_h);
 quad(-(aru_w / 2), -(aru_h / 2) - 5 - k1_h, 
      -(aru_w / 2) + k3_w * 1.5, -(aru_h / 2) - 5 - k1_h, 
      -(aru_w / 2) - k3_w / 2, -(aru_h / 2) - 5 - k1_h - k3_h, 
      -(aru_w / 2) - k3_w * 1.5,  -(aru_h / 2) - 5 - k1_h - k3_h);
 quad((aru_w / 2), (-(aru_h / 2) - 5 - k1_h), 
      -(-(aru_w / 2) + k3_w * 1.5), (-(aru_h / 2) - 5 - k1_h), 
      -(-(aru_w / 2) - k3_w / 2), (-(aru_h / 2) - 5 - k1_h - k3_h), 
      -(-(aru_w / 2) - k3_w * 1.5),  (-(aru_h / 2) - 5 - k1_h - k3_h));
 rectMode(CORNER);
 rect(-(aru_w / 2) - k4_w * 1.5, -(aru_h / 2) - 5 - k1_h - k3_h - k4_h, k3_w, k4_h);
 rect((aru_w / 2) + k4_w / 2, -(aru_h / 2) - 5 - k1_h - k3_h - k4_h, k3_w, k4_h);
 triangle(-(aru_w / 2) - k4_w * 1.5, -(aru_h / 2) - 5 - k1_h - k3_h - k4_h,
          -(aru_w / 2) - k4_w * 1.5, -(aru_h / 2) - 5 - k1_h - k3_h - k4_h - k5_h,
          -(aru_w / 2) - k4_w * 1.5 + k5_w, -(aru_h / 2) - 5 - k1_h - k3_h - k4_h);
 triangle(-(-(aru_w / 2) - k4_w * 1.5), -(aru_h / 2) - 5 - k1_h - k3_h - k4_h,
          -(-(aru_w / 2) - k4_w * 1.5), -(aru_h / 2) - 5 - k1_h - k3_h - k4_h - k5_h,
          -(-(aru_w / 2) - k4_w * 1.5 + k5_w), -(aru_h / 2) - 5 - k1_h - k3_h - k4_h);
  
 translate(0, -aru_y);
 rotate(radians(-angle - 90));
 translate(-x, -y);
 noFill();
}

void Text(float x, float y) {
 strokeWeight(10); // 文字の太さ
 textSize(25); // 文字のサイズ
 fill(255, 255, 255); // 文字の色
  
 // robot1のテキスト
 text("robot1", x, y);
 text("mode = " + mode1, x, y + 30);
 text("dist = " + dist1, x, y + 60);
 text("angle = " + angle1, x, y + 90);
 text("color = " + col1, x, y + 120);
  
 // robot2のテキスト
 text("robot2", width / 2 + x, y);
 text("mode = " + mode2, width / 2 + x, y + 30);
 text("dist = " + dist2, width / 2 + x, y + 60);
 text("angle = " + angle2, width / 2 + x, y + 90);
 text("color = " + col2, width / 2 + x, y + 120);
  
 // robot3のテキスト
 text("robot3", x, height / 2 + y);
 text("mode = " + mode3, x, height / 2 + y + 30);
 text("dist = " + dist3, x, height / 2 + y + 60);
 text("angle = " + angle3, x, height / 2 + y + 90);
 text("color = " + col1, x, height / 2 + y + 120);
  
 noFill(); // 色の初期化
}

void map_draw(float x, float y, float f) {
 rectMode(CENTER);
 float line_w = 5 * f, gap = 15 * f; // 線の幅、赤線の隙間
 float map_w = 240 * f, map_h = 150 * f;
 float mout_radius = 70 * f;
 translate(x, y);
 stroke(255);
 strokeWeight(line_w);
 rect(width / 4, height / 4, map_w, map_h); // 大枠の形
 stroke(255, 0, 0);
 line(width / 4 - map_w / 2, height / 4 - map_h / 2 + gap,
      width / 4 - map_w / 2, height / 4 + map_h / 2 - gap); // 赤線
 stroke(0, 0, 255);
 line(width / 4 + map_w / 2, height / 4 - map_h / 2 + gap,
      width / 4 + map_w / 2, height / 4 + map_h / 2 - gap); // 青線
  
 strokeWeight(1);
 stroke(255);
 ellipse(width / 4, height / 4, mout_radius, mout_radius); // 山
  
 // マップ上のロボットの描画
 draw_robot((robot1_x + width / 4 - map_w / 2) * f, (robot1_y + height / 4 - map_h / 2) * f, angle1, map_robot_f);
 translate(-x, -y);
 noFill();
}

void north(float x, float y, float angle, float f) {
}
