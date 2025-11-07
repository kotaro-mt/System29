/*
山登りの関数
//1を返すと成功、0を返すと相手に山頂のオブジェクトを取られた
*/

float avg_ax=0;
float avg_mx=0;
float max_ax=0;
int speed=0;

int MountClimb(){ 

float diff=0;
int count=0; //カウント用
getAcc(ax, ay, az); // 加速度センサーの値を取得

avg_ax=0.9*avg_ax+0.1*ax; // 低域通過フィルタ
avg_mx=0.9*avg_mx+0.1*mxScaled(); // 低域通過フィルタ

Serial.print("山登りモード：");
Serial.println(climb_mode);

switch(climb_mode){
  case 0: //山登り開始
    speed=200;
    if(avg_ax> 5000){ //傾きが一定以上になったら
      climb_mode=1; //山頂到達判定へ
    }
    break;
  case 1: //山頂到達判定
    speed = 200;
    diff = -0.02*my+50; // P-制御 少し斜めを向くようにする（+10は調整用）
    if (avg_mx>0.5){ //山の反対側に行ったら
        // climb_mode = 2;
        // diff=0;
    }
    break;
  case 2: //山頂方向に回転
    motorL = 150;
    motorR = -150;
    if(max_ax <= avg_ax){ //山頂方向に向いたら
      max_ax = avg_ax;
      count = 0;
    }else{
      count++;
      if(count > 3){ //安定するまで少し待つ
        climb_mode = 3;
        count = 0;
      }
    }
    break;
  case 3: //物体を取りに行く
    speed = 200;
    diff = -0.02*ay;
    if(avg_ax<500){

    }
    break;
  case 4: //山降り開始
    speed = 200;
    if(avg_ax>-5000){ //山を降りきったら
      climb_mode = 5;
    }
    break;
  case 5: //山降り完了
    speed = 200;
    diff = -0.02*ay;
    if(avg_ax> -500){ //平地に戻ったら
      return 1; //成功
    }
    break;

}
motorL = speed + diff;
motorR = speed - diff;
return 0; //継続
}
