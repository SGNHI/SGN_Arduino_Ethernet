#ifndef SGNHI
#define SGNHI

#include "SPI.h"
#include "Ethernet.h"
#if ARDUINO < 100
  #include "WProgram.h"
#else
    #include "Arduino.h"
#endif

#define SERVER "sgnhi.org"

#define atInt 1
#define atFloat 2
#define atDouble 3
#define atLong 4

#define OK 0 // 서버에 데이터 전송.
#define WAIT 1 // 서버에 전송한지 대기시간이 지나지 않음, 좀더 뒤에 전송 요망.
#define ERROR 2 // 웹사이트에 접속 실패.

#define MACHTYPE(v,t) (t == atInt?*(int*)v:(t == atFloat?*(float*)v:(t == atDouble?*(double*)v:(t == atLong?*(long*)v:0))))

#define REST 600000

//#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x) Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif



class dotori {
public:
	dotori(char *sencode);
	void printcode();
	void set(int val);
	void set(float val);
	void set(long val);
	void set(double val);
	//int request(void *val,int type);
	//
	//vvoid setvalue(void *val);
private:
	int argType;
	int chk = 42;
	char* senCode;
	uint32_t value;
friend class sgnDev;
};




//장치 메인 객체 사용자가 추가할 필요 x
class sgnDev {
public:
	void init(char *id,char *devCode,IPAddress local_ip);
	int send(dotori mdotori,...);
	void setRest(unsigned long rest);
	void setmac(byte a,byte b,byte c,byte d,byte e,byte f);
private:

	//일괄 업로드 기능 필요.
	char* ID;
	char* devCode;
	byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	unsigned long sTime;
	int state = 0;
	unsigned long restTime = REST;
	IPAddress addr;
	void init();


};



extern sgnDev dev;

extern EthernetClient client;


#endif
