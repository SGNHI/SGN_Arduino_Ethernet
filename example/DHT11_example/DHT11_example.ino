#include <SGN_Arduino_Ethernet.h>//스근하이 이더넷 라이브러리 입니다.
#include <DHT.h>//adafruit의 DHT11라이브러리입니다.
#define DHTTYPE DHT11   // DHT 11

char ID[] = "사용자 ID";//사용자의 ID를 입력합니다.
char DEVICE_CODE[] = "장치코드";//장치 코드를 입력해주도록 합니다.

IPAddress IP(192, 168, 0, 177);//IP주소를 입력해줍니다.
DHT dht(2, DHTTYPE);//사용할 DHT센서의 핀을 지정합니다.(2번 핀을 사용했습니다.)
dotori Tsensor("온도센서 코드");//사용할 센서를 선언해줍니다.
dotori Hsensor("습도센서 코드");

void setup() {
  Serial.begin(9600);
  dht.begin();
  //dev.setmac(0xC0, 0x00, 0x01, 0x0F, 0xF1, 0xCE);//맥어드레스 변경은 충돌이 없을시 생략 가능합니다.
  dev.init(ID,DEVICE_CODE,IP);//장치의 사용을 시작합니다.
  dev.setRest((unsigned long)1000 * 60 * 30);
}

void loop() {
  Hsensor.set(dht.readHumidity());//읽어들인 습도센서의 값을 저장합니다.
  Tsensor.set(dht.readTemperature());//읽어들인 온도센서의 값을 저장합니다.
  Serial.println(dev.send(Hsensor,Tsensor) == OK?"OK":"NO");//값의 전송이 성공되었는지 확인합니다.
  delay(1000);
}
