#include <Stepper.h>
#include <DHT11.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2); // 0x3F I2C 주소를 가지고 있는 16x2 LCD객체를 생성합니다.

DHT11 dht11(2);
const int stepvalue=2048;
Stepper stepper(stepvalue, 10, 8, 9, 7); // stepper객체 설정, 연결 핀 : IN4, IN2, IN3, IN1  

int rain_state=0,gas_state=0,tem_state=0; //센서 상태
int now_state=0; // 창문이 닫혀있으면 0, 열려있으면 1
int rain_value, sum_rain =0;
int gas_value, sum_gas=0;
float tem_value, sum_tem=0;
float humi, temp;
int buzzer = 3;
int buttonApin = 5;
int buttonBpin = 4;

void setup() {
  Serial.begin(9600); // 시리얼 통신 시작
  stepper.setSpeed(17); // 모터 속도
  lcd.init(); //I2C LCD를 초기화
  lcd.backlight(); //I2C LCD의 백라이트 on
  pinMode (buzzer, OUTPUT);
  pinMode (buttonApin, INPUT_PULLUP);
  pinMode (buttonBpin, INPUT_PULLUP);
}

void loop() {
  
  if(digitalRead(buttonApin) == LOW){
    if(now_state ==0){
      Serial.println("스위치로 창문열림");
      for(int i =0; i<3; i++){
        stepper.step(-stepvalue); //창문열림
    }      
      now_state = 1;
    }
  }
  
  if(digitalRead(buttonBpin) == LOW){ 
    if(now_state ==1){
      Serial.println("스위치로 창문닫힘");
      for(int i =0; i<3; i++){
        stepper.step(stepvalue); //창문닫기              
      }
      now_state = 0;  
    }  
  } 
  for(int i=0; i<3; i++){
    rain_value = analogRead(A0); // 빗방울 데이터 읽기
    gas_value = analogRead(A1); // 가스 데이터 읽기
    dht11.read(humi, temp); //온도 데이터 읽기
    tem_value = temp;
    
    sum_rain += rain_value;
    sum_gas += gas_value;
    sum_tem += tem_value;
    delay(1000);
  }
  rain_value = sum_rain/5; //5초간의 데이터 평균 값 
  gas_value = sum_gas/5;
  tem_value = sum_tem/5;

  lcd.setCursor(0,0);
  lcd.print("Today: ");
  lcd.print(" Rain ");
  
  lcd.setCursor(0,1);
  lcd.print("Now: ");
  lcd.print(tem_value);
  lcd.print("'C");
      
  Serial.println("");
  Serial.println("센서 감지");
  Serial.print("rain - ");
  Serial.println(rain_value);
  Serial.print("gas - ");
  Serial.println(gas_value);
  Serial.print("tem - ");
  Serial.println(tem_value);

  if(rain_value < 500){
    Serial.println("빗물이 감지 됐습니다.");
    rain_state = 1; // 비가온다.
  }
  if(gas_value > 170){
    Serial.println("가스누출이 감지 됐습니다.");
    gas_state = 1; // 가스농도 상승
    tone(buzzer, 200, 1000);
  }
  if(tem_value > 15){
    Serial.println("온도상승");
    tem_state = 1; // 실내온도 상승
  }
  
  if(now_state ==1 && rain_state ==1){ //창문이 열려있고 비가 올경우
    for(int i =0; i<3; i++){
      stepper.step(stepvalue); //창문닫기
    }
    Serial.println("창문닫힘");
    now_state = 0; 
  }  
     
  if(now_state ==0 && rain_state ==0 && (gas_state ==1 || tem_state ==1)){ //창문이 닫혀있고 온도상승,가스누출 경우 (비가 오면 열지 않음)
    for(int i =0; i<3; i++){
      stepper.step(-stepvalue); //창문열기
     }
    Serial.println("창문열림");
    now_state = 1;
}
    //Serial.println(now_state);
    
  // 변수 값 초기화
  rain_state=0;
  gas_state=0;
  tem_state=0;
  sum_rain = 0;
  sum_gas = 0;
  sum_tem = 0;
 
}
