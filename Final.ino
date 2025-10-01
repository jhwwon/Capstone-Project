#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 8 
#define DHTTYPE DHT11 
int btn = 6; 
int bodySen = 7; 
int soundSen = 9; 


int hour = 0;
int minute = 0;
int sec = 0;
unsigned long m=0; // 시간증가 계산용
unsigned long t2=0; // 시간증가 계산용

int cnt = 0; // 버튼 횟수 변수 정의
int soundCnt = 0; // 센서 값 변수 정의
int bodyCnt = 0;
int array_t[20] = {0,};
int array_h[20] = {0,};
int index_t=0;
int index_h=0;

int mark_sleepTime = 0; //수면시간 
int mark_sound = 0;     //사운드 센서 
int mark_body = 0;      //인체감지 센서
int mark_move = 0;      //움직임 발생 변수
int mark_t = 0;         //온도
int mark_h = 0;         //습도
int t;
int h;

LiquidCrystal_I2C lcd(0x27,16,2); 
DHT dht(DHTPIN, DHTTYPE); 

void setup()
{
  Serial.begin(9600);
  dht.begin(); 
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);

  h = dht.readHumidity(); 
  t = dht.readTemperature(); 

  pinMode(btn, INPUT); 
  pinMode(soundSen, INPUT);
  pinMode(bodySen, INPUT);
}

void loop()
{
  if (m==0 && digitalRead(btn)) { // 버튼 입력 및 횟수 카운트, 채터링 해결
    lcd.clear();
    m=millis();
    cnt++;

// cnt
// 1 : 시작. 시간증가, 온습도, 코골이, 움직임 측정 및 출력
// 2 : 정지. 시간정지, 온습도, 코골이, 움직임 출력
// 0 : 초기화. 시간 0, 온습도, 코골이, 움직임 등 모두 0으로

  if (cnt > 2) {
    cnt=0;
  }
  }
  else if (m > 0 && millis() > m+300) {
    m=0;
  }

// 초기화
  if (cnt==0) {
    init_fun();
  }

// 시작
  else if (cnt==1) {
    start_setup();
}
// 정지 및 결과
  else if (cnt==2) {
  }

// 측정중
  if (cnt==1) {
    measure();
  }
// 결과
  else if (cnt==2) {
    result();
  }
}

void init_fun(){ //초기화
  sec=0;
  minute=0;
  hour=0;
  soundCnt = 0;
  bodyCnt = 0;

  mark_sleepTime = 0;
  mark_sound = 0;
  mark_body = 0;
  mark_move = 0;
  mark_t = 0;
  mark_h = 0;

  for (int i=0;i < 20;i++) {
    array_t[i] = -1;
    array_h[i] = -1;
  }
}

void start_setup(){ //시간 처리 및 측정
  if (millis() > t2+1000) {
    t2=millis();
    sec++;

    if (sec >= 60) {
      sec=0;
      minute++;

      if (minute >= 60) {
        minute=0;
        hour++;

        if (hour >= 100) {
          hour=0;
        }

        h = dht.readHumidity();
        t = dht.readTemperature();

        array_t[index_t] = t;
        array_h[index_h] = h;

        index_t++;
        index_h++;
      }
    }
  }

  if (digitalRead(soundSen)==HIGH) {
    soundCnt++;
  }
  if (digitalRead(bodySen)==HIGH) {
    bodyCnt++;
  }  
}

void measure(){  //측정 진행중일때 처리할 내용
  // 시간출력
  lcd.setCursor(0,0);
  Serial.print("Time : ");
  if (hour < 10) {
    Serial.print("0");
    lcd.print("0");
  }
  Serial.print(hour);
  lcd.print(hour);
  Serial.print(":");
  lcd.print(":");

  if (minute < 10) {
    Serial.print("0");
    lcd.print("0");
  }
  Serial.print(minute);
  lcd.print(minute);
  Serial.print(":");
  lcd.print(":");

  if (sec < 10) {
    Serial.print("0");
    lcd.print("0");
  }
  Serial.print(sec);
  lcd.print(sec);
  Serial.print("\t");

  Serial.print("T : ");
  Serial.print(t);
  Serial.print("\t");
  Serial.print("H : ");
  Serial.print(h);
  Serial.print("\t");
  Serial.print("S : ");
  Serial.print(soundCnt);
  Serial.print("\t");
  Serial.print("M : ");
  Serial.print(bodyCnt);
  Serial.println("");

  lcd.setCursor(0,1);
  lcd.print("T");
  lcd.print(t);
  lcd.print(" ");
  lcd.print("H");
  lcd.print(h);
  lcd.print(" ");
  lcd.print("S");
  lcd.print(soundCnt);
  lcd.print(" ");
  lcd.print("M");
  lcd.print(bodyCnt);
}

void result() { //측정 결과 표시
  int sleepTime = (sec + minute*60 + hour*3600) / 3600;
  int sum_t=0;
  int sum_h=0;

  for (int i=0;i < index_t;i++) {
    sum_t += array_t[i];
  }
  for (int i=0;i < index_h;i++) {
    sum_h += array_h[i];
  }

  mark_t = sum_t / index_t;
  mark_h = sum_h / index_h;

// 점수계산
// 수면시간
  if (sleepTime >= 7 && sleepTime <= 9) {
    mark_sleepTime=2;
  }
  else if (sleepTime < 6 || sleepTime > 10){
    mark_sleepTime=0;
  }
  else {
    mark_sleepTime=1;
  }

// 사운드
  if (soundCnt <= 60) {
    mark_sound=2;
  }
  else if (soundCnt > 80) {
    mark_sound=0;
  }
  else {
    mark_sound=1;
  }

// 인체감지
  if (bodyCnt <= 5) {
    mark_body=2;
  }
  else if (bodyCnt > 10) {
    mark_body=0;
  }
  else {
    mark_body=1;
  }

// 온도
  if (mark_t >= 16 && mark_t <= 20) {
    mark_t=2;
  }
  else if (mark_t < 10 || mark_t > 25) {
    mark_t=0;
  }
  else {
    mark_t=1;
  }

// 습도
  if (mark_h >= 40 && mark_h <= 60) {
    mark_h=2;
  }
  else if (mark_h < 20 || mark_h > 80) {
    mark_h=0;
  }
  else {
    mark_h=1;
  }

  Serial.print("T : ");
  Serial.print(mark_t);
  Serial.print("\t");
  Serial.print("H : ");
  Serial.print(mark_h);
  Serial.print("\t");
  Serial.print("S : ");
  Serial.print(mark_sound);
  Serial.print("\t");
  Serial.print("M : ");
  Serial.print(mark_body);
  Serial.print("TS : ");
  Serial.print(mark_sleepTime);
  Serial.println("");

  lcd.setCursor(0,0);
  lcd.print("T");
  lcd.print(mark_t);
  lcd.print(" ");
  lcd.print("H");
  lcd.print(mark_h);
  lcd.print(" ");
  lcd.print("S");
  lcd.print(mark_sound);
  lcd.print(" ");
  lcd.print("M");
  lcd.print(mark_body);
  lcd.print(" ");
  lcd.print("TS");
  lcd.print(mark_sleepTime);

  lcd.setCursor(0,1);
  lcd.print("Total: ");
  lcd.print( (mark_t + mark_h + mark_sound + mark_body + mark_sleepTime) );
  lcd.print(" Aver: ");
  lcd.print( (mark_t + mark_h + mark_sound + mark_body + mark_sleepTime)/5 );
}
