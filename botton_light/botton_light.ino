
int y = 3;
int r = 5;
int b = 6;
int pin1 = A0;
int pin2 = 12;
int pin3 = 11;
int pin4 = 10;
int light1 = 100;
int light2 = 100;
int light3 = 100;

void setup() {
  // put your setup code here, to run once:

    pinMode(y, OUTPUT);
    pinMode(r,OUTPUT);
    pinMode(b,OUTPUT);
    pinMode(pin1,INPUT);
    pinMode(pin2,INPUT);
    pinMode(pin3,INPUT);
    pinMode(pin4,INPUT);
         //该端口需要选择有#号标识的数字口
     analogWrite(y,light1);
     analogWrite(r,light2);
     analogWrite(b,light3);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;//定义变量
  int h;
  int j;
  int k;
  while(1)
  {
    delay(180);
    i=digitalRead(pin4);//读取模拟5口电压值
    h=digitalRead(pin3);
    j=digitalRead(pin2);
    k=digitalRead(pin1);
  if (i == HIGH) {
  //判断黄色
    if (h == HIGH) {
    // turn LED
    light1=light1-10;                         //每次累加值为5
    if (light1<=0) {
      light1=0;
    }        
    analogWrite(y,light1);
   
    //判断红色
    }
    if (j == HIGH) {
    // turn LED
    light2=light2-10;                         //每次累加值为5
    if (light2<=0) {
      light2=0;
    }        
    analogWrite(r,light2);
    
    //判断红色
    }
     if (k == HIGH) {
    // turn LED
    light3=light3-10;                         //每次累加值为5
    if (light3<=0) {
      light3=0;
    }        
    analogWrite(b,light3);
    
    //判断红色
   }
  }
  else{
  if (h == HIGH) {
    // turn LED
    light1=light1+10;                         //每次累加值为5
    if (light1>=255) {
      light1=255;
    }        
    analogWrite(y,light1);
    
    //判断红色
    }
    if (j == HIGH) {
    // turn LED
    light2=light2+10;                         //每次累加值为5
    if (light2>=255) {
      light2=255;
    }        
    analogWrite(r,light2);
    
    //判断蓝色
    }
    if (k == HIGH) {
    // turn LED
    light3=light3+10;                         //每次累加值为5
    if (light3>=255) {
      light3=255;
    }        
    analogWrite(b,light3);
 
  }
  }
}
}

