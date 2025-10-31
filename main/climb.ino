/*
山登りの関数
//0を返すと成功、１を返すと相手に山頂のオブジェクトを取られた
*/

float ax, ay, az; // 加速度センサーの値
float avg_ax=0;
int speed=0;

int MountClimb(){ 

static int climb_mode = 0;
float diff=0;
getAcc(ax, ay, az); // 加速度センサーの値を取得

avg_ax=0.9*avg_ax+0.1*ax; // 低域通過フィルタ

switch(climb_mode){
  case 0: //山登り開始
    speed=200;
    if(avg_ax< 500){ //傾きが一定以上になったら
      climb_mode=1; //山頂到達判定へ
    }
    break;
  case 1: //山頂到達判定
    speed = 200;
    diff = -0.02*ay; // P-制御 最急勾配方向へ向く
    if (abs(avg_ax)<1500) // 登頂したら
        climb_mode = 2;
    break;
  case 2: //山頂の物体を検出
    diff=0;
    speed=0;
    if(dist>20){ //物体を取られたら
      return 0; //失敗を返す
    }else{
        return 1; //成功を返す
    }
    break;
  case 3: //物体を取りに行く

    break;
}
motorL = speed + diff;
motorR = speed - diff;
}