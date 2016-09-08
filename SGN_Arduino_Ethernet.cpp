#include "SGN_Arduino_Ethernet.h"

dotori::dotori(char *sencode){
	senCode = sencode;
}

void dotori::printcode(){
	DEBUG_PRINT(senCode);
}

//입력 변수형별 전송 데이터 타입 정리.
void dotori::set(int val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atInt;
}

void dotori::set(float val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atFloat;
}

void dotori::set(long val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atLong;
}

void dotori::set(double val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atDouble;
}



void sgnDev::init(char *id,char *devcode,IPAddress local_ip){
	addr = local_ip;
	init();
	devCode = devcode;
	ID = id;
	DEBUG_PRINT(devCode);

	delay(1000);

	DEBUG_PRINT("connecting....");
}
void sgnDev::init(){
	if(Ethernet.begin(mac) == 0){
		DEBUG_PRINT("fail using dhcp");
		Ethernet.begin(mac,addr);
	}
}

void sgnDev::setRest(unsigned long rest){
	restTime = rest < REST? REST:rest;
}


void sgnDev::setmac(byte a,byte b,byte c,byte d,byte e,byte f){
	mac[0] = a;
	mac[1] = b;
	mac[2] = c;
	mac[3] = d;
	mac[4] = e;
	mac[5] = f;
}

int sgnDev::mail(char *subject,char *text){
	if (client.connect(SERVER, 80)) {//서버에 연결됨을 확인.
		DEBUG_PRINT("connected");
		//서버에 데이터 전송
		client.print("GET /iot/iot_up.php?");
		client.print("uid=");client.print(ID);
		client.print("&dc=");client.print(devCode);
		client.print("$ms=");client.print(subject);
		client.print("$mt=");client.print(text);
		client.print(" HTTP/1.0\r\n");
		client.print("Host:sgnhi.org \r\n");
		client.print("User-Agent: sgnhi\r\n");
		client.print("Connection: close\r\n");
		client.println();
		client.stop();
	}
	else {//서버와의 열결 실패를 확인.
		/*
		연결실패의 원인은 다양하다.
		정말 서버가 없거나,
		공유기에서 IP 할당을 끝낼때,
		(대기시간이 긴경우 자동으로 IP할당이 끊기고 이로인해 서버에 접속을 못함)
		간단한 해결법은 그냥 다시한번 공유기에 IP할당을 요청하는 것이다.
		처음에 사용한 init 함수를 통해서..
		*/
		Serial.println(client.status());
		client.stop();
		DEBUG_PRINT("connection failed");
		DEBUG_PRINT("try to begin");
		init();// 빠른 init
		state = 0;
		return ERROR;
	}
	return OK;
}

int sgnDev::send(dotori mdotori, ...){//iot_up 소스코드 수정해야함 -> 수정완료.

	//return 1;
	//send value code 아래쪽 부터.
	unsigned long now = millis();
	if(state != 0){//state는 데이터 전송 성공 유무를 확인하기 위함이다. 실패하면 바로 다시 보내야 되니까..
		if(now <= sTime){
			unsigned long lastTime = 0xffffffff - sTime;
			if((lastTime + now < restTime)){
				return WAIT;
			}
		}else if(now - sTime < restTime){
			return WAIT;
		}
	}

	if (client.connect(SERVER, 80)) {//서버에 연결됨을 확인.
		DEBUG_PRINT("connected");
		//서버에 데이터 전송
		client.print("GET /iot/iot_up.php?");
		client.print("uid=");client.print(ID);
		client.print("&dc=");client.print(devCode);
		int cnt = 0;
		va_list vl;
		va_start(vl,mdotori);
		for(dotori m = mdotori;m.chk == 42;m= va_arg(vl,dotori)){
			uint32_t value = m.value;
			void * vo = &m.value;
			client.print("&sc");client.print(cnt);client.print("=");
			client.print(m.senCode);
			client.print("&sv");client.print(cnt);client.print("=");
			client.print(MACHTYPE(vo,m.argType));
			cnt++;
		}
		va_end(vl);

		client.print(" HTTP/1.0\r\n");
		client.print("Host:sgnhi.org \r\n");
		client.print("User-Agent: sgnhi\r\n");
		client.print("Connection: close\r\n");
		client.println();

		String req = "";
		unsigned long lastRead = millis();
		state = 0;
		while(!client.available() && (millis() - lastRead < 1000));
		while(client.available()){
			req += (char) client.read();
			if(req.endsWith("SGNHI0")){
				state = 1;
				DEBUG_PRINT("SGNHI0");
			}
			if(req.endsWith("SGNHI1")){
				state = 0;
				DEBUG_PRINT("SGNHI1");
			}
			lastRead = millis();
			//Serial.print((char)client.read());
		}
		client.stop();
		sTime = now;

	}
	else {//서버와의 열결 실패를 확인.
		/*
		연결실패의 원인은 다양하다.
		정말 서버가 없거나,
		공유기에서 IP 할당을 끝낼때,
		(대기시간이 긴경우 자동으로 IP할당이 끊기고 이로인해 서버에 접속을 못함)
		간단한 해결법은 그냥 다시한번 공유기에 IP할당을 요청하는 것이다.
		처음에 사용한 init 함수를 통해서..
		*/
		Serial.println(client.status());
		client.stop();
		DEBUG_PRINT("connection failed");
		DEBUG_PRINT("try to begin");
		init();// 빠른 init
		state = 0;
		return ERROR;
	}
	return OK;
}

sgnDev dev;

EthernetClient client;
