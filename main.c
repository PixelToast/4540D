#pragma config(Sensor, in1,    pot_arm,        sensorPotentiometer)
#pragma config(Sensor, in2,    pot_claw,       sensorPotentiometer)
#pragma config(Sensor, dgtl1,  ,               sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  ,               sensorQuadEncoder)
#pragma config(Sensor, dgtl11, uls_1,          sensorSONAR_cm)
#pragma config(Motor,  port1,           arm_right,     tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           top_left,      tmotorVex269_MC29, openLoop)
#pragma config(Motor,  port3,           claw_grab,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           bottom_left,   tmotorVex393_MC29, openLoop, driveLeft, encoderPort, dgtl3)
#pragma config(Motor,  port5,           side_wheel,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           top_right,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           claw_tilt,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           bottom_right,  tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, dgtl1)
#pragma config(Motor,  port9,           claw_slide,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          arm_left,      tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"

// gets the maximum value of two
float max(float a,float b) {
	if (a>b) {
		return a;
		} else {
		return b;
	}
}

// gets the minimum value of two
float min(float a,float b) {
	if (a<b) {
		return a;
		} else {
		return b;
	}
}

// makes sure a value is within a range
float range(float n,float mn,float mx) {
	return max(min(n,mx),mn);
}

int mod(int n,int d) {
	const int result=n%d;
	return result>=0?result:result+d;
}

#include "autonSelect.c"
#include "movement.c"
task armControl;

void pre_auton()
{
	stopTask(autonomous);
	stopTask(usercontrol);
	stopTask(armControl);
	menuUseRT=true;
	startTask(autonSelect);
	startTask(usercontrol);
}

void armDirect(int pow) {
	motor[arm_left]=pow;
	motor[arm_right]=pow;
};

void setClaw(int l) {
	while (SensorValue[pot_claw]/50!=l/50) {
		motor[claw_tilt]=range(SensorValue[pot_claw]-l,-127,127);
	}
}

#define clawUp() setClaw(2241)
#define clawMiddle() setClaw(2855)
#define clawDown() setClaw(3552)

void slideUp(float time) {
	motor[claw_slide]=127;
	wait1Msec(time*1000);
	motor[claw_slide]=0;
}

void slideDown(float time) {
	motor[claw_slide]=-127;
	wait1Msec(time*1000);
	motor[claw_slide]=0;
}

void clawGrab() {
	motor[claw_grab]=127;
	wait1Msec(800);
	motor[claw_grab]=32;
	wait1Msec(200);
	motor[claw_grab]=0;
}

void clawRelease() {
	motor[claw_grab]=-127;
	wait1Msec(800);
	motor[claw_grab]=-32;
	wait1Msec(200);
	motor[claw_grab]=0;
}

int getBtns(int btn1, int btn2) {
	return (vexRT[btn1]*128)+(vexRT[btn2]*-128);
}

int last[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int getBtn(int btn) {
	if (last[btn]!=vexRT[btn]) {
		last[btn]=vexRT[btn];
		return vexRT[btn];
	}
	return 0;
}

int armhold=0;
int arm=0;
int doarmhold=0;
task armControl() {
	armhold=0;
	arm=0;
	doarmhold=0;
	while (true) {
		if (doarmhold && armhold!=0) {
			arm=SensorValue[pot_arm]>4000?0:(SensorValue[pot_arm]-armhold)/20;
		}
		motor[arm_left]=arm;
		motor[arm_right]=arm;
		wait1Msec(50);
	}
}

void setArm(int l) {
	doarmhold=0;
	arm=range(SensorValue[pot_arm]-l,-127,127);
	while (SensorValue[pot_arm]/100!=l/100) {
		wait1Msec(50);
	}
	armhold=l;
	doarmhold=1;
}

task autonomous() {
	doarmhold=1;
	startTask(armControl);
	stopTask(usercontrol);
	switch (autonMode) {
		case 0:
			backward(200);
			setArm(2900);
			clawMiddle();
			clawRelease();
			forward(150);
			motor[claw_grab]=127;
			slideUp(1.5);
			motor[claw_grab]=0;
			slideUp(2);
			turnLeft(20);
			clawUp();
			setArm(3830);
			leftDirect(32);
			while (SensorValue[uls_1]>35) wait1Msec(50);
			stopDirect();
			forwardDirect(127);
			wait1Msec(900);
			stopDirect();
			backward(180);
			clawMiddle();
			slideDown(1);
			clawRelease();
			backward(100);
		break;
		case 1:
			clawDown();
			clawGrab();
			forward(200);
			clawMiddle();
			turnRight(180);
			clawDown();
			backward(200);
		break;
		case 2:
			backward(300);
			clawDown();
			clawGrab();
			forward(400);
			clawUp();
			setArm(2000);
			turnRight(70);
			backward(100);
			clawMiddle();
			backward(500);
	}
}

int lastl=0;
#define getBtns(a,b) ((vexRT[a]*-127)+(vexRT[b]*127))
int mode=0;
float clawslow=1;
task usercontrol() {
	startTask(armControl);
	menuUseRT=0;
	startTask(autonSelect);
	while (true) {
		if ((!lastl)&&vexRT[Btn7R]!=lastl) {
			mode=!mode;
		}
		lastl=vexRT[Btn7R];
		arm=getBtns(Btn5D,Btn5U);
		if (arm!=0) {
			armhold=SensorValue[pot_arm];
		}
		doarmhold=arm==0;
		if (getBtns(Btn6D,Btn6U)==0) {
			clawslow=1;
		} else {
			clawslow=clawslow*0.93;
		}
		char top[16];
		snprintf(top,16,"%i",SensorValue[pot_arm]);
		displayLCDCenteredString(0,top);
		motor[claw_grab]=getBtns(Btn6D,Btn6U)*clawslow;
		motor[claw_tilt]=getBtns(Btn8D,Btn8U);
		motor[claw_slide]=getBtns(Btn7D,Btn7U);
		switch (mode) {
			case 0: // normal tank controls
				motor[bottom_left]=vexRT[Ch3];
				motor[top_left]=vexRT[Ch3];
				motor[bottom_right]=vexRT[Ch2];
				motor[top_right]=vexRT[Ch2];
				motor[side_wheel]=getBtns(Btn8L,Btn8R);
			break;
			case 1: // left stick moves robot in four directions and right stick turns
				motor[side_wheel]=vexRT[Ch4];
				int turn=vexRT[Ch1];
				if (abs(turn)<15) {
					turn=0;
				}
				int fwd=vexRT[Ch3];
				motor[bottom_left]=fwd+turn;
				motor[top_left]=fwd+turn;
				motor[bottom_right]=fwd-turn;
				motor[top_right]=fwd-turn;
			break;
		}
		for (int i=0;i<9;i++) {
			if (abs(motor[i])<15) {
				motor[i]=0;
			}
		}
		wait1Msec(50);
	}
}
