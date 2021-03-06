#pragma config(UART_Usage, UART1, uartUserControl, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartUserControl, baudRate19200, IOPins, None, None)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)
#define port uartOne

unsigned char get() {
	while (true) {
		int out=getChar(port);
		if (out!=-1) {
			return out;
		}
	}
}

//#define put(x) sendChar(port,x)

void put(unsigned char x) {
	//writeDebugStream("%02x ",x);
	sendChar(port,x);
}

unsigned char top[16]={32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
unsigned char bottom[16]={32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
unsigned char backlt=1;

int lcdbtns=0;

int update=0;
task send() {
	while (true) {
		for (int side=0;side<2;side++) {
			while(!bXmitComplete(port)) wait1Msec(1);
			put(0xaa); put(0x55); put(0x1e); put(0x12);
			put((backlt<<1)|side);
			unsigned char checksum=256-((backlt<<1)|side);
			for (int curByte=0;curByte<16;curByte++) {
				unsigned char byte=side?bottom[curByte]:top[curByte];
				put(byte);
				checksum-=byte;
			}
			put(checksum);
			wait1Msec(update?20:100);
		}
		update=0;
	}
}

task receive() {
	while (true) {
		unsigned char checksum=0;
		if (get()!=0xaa)
			continue;
		if (get()!=0x55)
			continue;
		unsigned char packetType=get();
		unsigned char numBytes=get();
		unsigned char data[256];
		for (int curByte=0;curByte<numBytes;curByte++) {
			int bt=get();
			data[curByte]=bt;
			checksum+=bt;
		}
		for (int curByte=0;curByte<numBytes;curByte++) {
			//writeDebugStream(" %02x",data[curByte]);
		}
		if (checksum==0) {
			if (packetType==0x16) {
				lcdbtns=data[1];
			}
		} else {
			writeDebugStreamLine(" inv %i alid type %02x size %02x",checksum,packetType,numBytes);
		}
	}
}

task main() {
	startTask(receive);
	while (true) {
		wait1Msec(100);
		snprintf(top,16,"%i                 ",random(1000));
		snprintf(bottom,16,"%i                 ",random(1000));
		update=1;
	}
}
