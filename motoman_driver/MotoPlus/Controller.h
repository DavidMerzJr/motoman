﻿//Controller.h
//
/*
* Software License Agreement (BSD License) 
*
* Copyright (c) 2013, Yaskawa America, Inc.
* Copyright (c) 2021, Institute for Factory Automation and Production Systems (FAPS)
* All rights reserved.
*
* Redistribution and use in binary form, with or without modification,
* is permitted provided that the following conditions are met:
*
*       * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*       * Neither the name of the Yaskawa America, Inc., nor the names 
*       of its contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/ 

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define TCP_PORT_MOTION						50240
#define TCP_PORT_STATE						50241
#define TCP_PORT_IO							50242

#define IO_FEEDBACK_WAITING_MP_INCMOVE		11120  //output# 889 
#define IO_FEEDBACK_MP_INCMOVE_DONE			11121  //output# 890 
#define IO_FEEDBACK_INITIALIZATION_DONE		11122  //output# 891 
#define IO_FEEDBACK_CONNECTSERVERRUNNING	11123  //output# 892 
#define IO_FEEDBACK_MOTIONSERVERCONNECTED	11124  //output# 893 
#define IO_FEEDBACK_STATESERVERCONNECTED	11125  //output# 894 
#define IO_FEEDBACK_IOSERVERCONNECTED		11126  //output# 895 
#define IO_FEEDBACK_FAILURE					11127  //output# 896

#define IO_FEEDBACK_RESERVED_1				11130  //output# 897 
#define IO_FEEDBACK_RESERVED_2				11131  //output# 898 
#define IO_FEEDBACK_RESERVED_3				11132  //output# 899 
#define IO_FEEDBACK_RESERVED_4				11133  //output# 900 
#define IO_FEEDBACK_RESERVED_5				11134  //output# 901 
#define IO_FEEDBACK_RESERVED_6				11135  //output# 902 
#define IO_FEEDBACK_RESERVED_7				11136  //output# 903
#define IO_FEEDBACK_RESERVED_8				11137  //output# 904 

#define MAX_IO_CONNECTIONS	1
#define MAX_MOTION_CONNECTIONS	1
#define MAX_STATE_CONNECTIONS	4

#if (DX100)
	#define MAX_CONTROLLABLE_GROUPS	3
#else
	#define MAX_CONTROLLABLE_GROUPS	4
#endif

#define INVALID_SOCKET -1
#define INVALID_TASK -1

#ifndef IPPROTO_TCP
#define IPPROTO_TCP  6
#endif

#define ERROR_MSG_MAX_SIZE 64

#define START_MAX_PULSE_DEVIATION 30

#define CONTROLLER_STATUS_UPDATE_PERIOD 10

#define MASK_ISALARM_ACTIVEALARM 0x02
#define MASK_ISALARM_ACTIVEERROR 0x01

typedef enum 
{
	IO_ROBOTSTATUS_ALARM_MAJOR = 0,
	IO_ROBOTSTATUS_ALARM_MINOR,
	IO_ROBOTSTATUS_ALARM_SYSTEM,
	IO_ROBOTSTATUS_ALARM_USER,
	IO_ROBOTSTATUS_ERROR,
	IO_ROBOTSTATUS_PLAY,
	IO_ROBOTSTATUS_TEACH,
	IO_ROBOTSTATUS_REMOTE,
	IO_ROBOTSTATUS_OPERATING,
	IO_ROBOTSTATUS_HOLD,
	IO_ROBOTSTATUS_SERVO,
	IO_ROBOTSTATUS_ESTOP_EX,
	IO_ROBOTSTATUS_ESTOP_PP,
	IO_ROBOTSTATUS_ESTOP_CTRL,
	IO_ROBOTSTATUS_WAITING_ROS,
	IO_ROBOTSTATUS_INECOMODE,
#if (YRC1000||YRC1000u)
	IO_ROBOTSTATUS_PFL_STOP,
	IO_ROBOTSTATUS_PFL_ESCAPE,
	IO_ROBOTSTATUS_PFL_AVOIDING,
	IO_ROBOTSTATUS_PFL_AVOID_JOINT,
	IO_ROBOTSTATUS_PFL_AVOID_TRANS,
#endif
	IO_ROBOTSTATUS_MAX
} IoStatusIndex;

// forward declaration of StateMachine
typedef struct _StateMachine StateMachine;

typedef struct
{
	UINT16 interpolPeriod;									// Interpolation period of the controller
	int numGroup;											// Actual number of defined group
	int numRobot;											// Actual number of defined robot
	CtrlGroup* ctrlGroups[MP_GRP_NUM];						// Array of the controller control group

	// Controller Status
	MP_IO_INFO ioStatusAddr[IO_ROBOTSTATUS_MAX];			// Array of Specific Input Address representing the I/O status
	USHORT ioStatus[IO_ROBOTSTATUS_MAX];					// Array storing the current status of the controller
	int alarmCode;											// Alarm number currently active
	BOOL bRobotJobReady;									// Indicates the robot job is on the WAIT command (ready for motion)
	BOOL bStopMotion;										// Flag to stop motion
	BOOL bPFLduringRosMove;									// Flag to keep track PFL activation during RosMotion

	// Connection Server
	int tidConnectionSrv;

	// Io Server Connection
	int	sdIoConnections[MAX_IO_CONNECTIONS];				// Socket Descriptor array for Io Server
	int	tidIoConnections[MAX_IO_CONNECTIONS];				// ThreadId array for Io Server

	// State Server Connection
	int tidStateSendState;  								// ThreadId of thread sending the controller state
	int	sdStateConnections[MAX_STATE_CONNECTIONS];			// Socket Descriptor array for State Server

	// Motion Server Connection
	int	sdMotionConnections[MAX_MOTION_CONNECTIONS];		// Socket Descriptor array for Motion Server
	int	tidMotionConnections[MAX_MOTION_CONNECTIONS];  		// ThreadId array for Motion Server
	int tidIncMoveThread;  									// ThreadId for sending the incremental move to the controller

	// Velocity Control
	VelocityControl velocityControl;

	// State machine
	StateMachine* stateMachine;

#ifdef DX100
	BOOL bSkillMotionReady[2];								// Boolean indicating that the SKILL command required for DX100 is active
	int RosListenForSkillID[2];								// ThreadId for listening to SkillSend command
	BOOL bIsDx100Sda;										// Special case to control the waist axis (axis 15)
#endif

} Controller;

extern BOOL Ros_Controller_Init(Controller* controller);
extern BOOL Ros_Controller_IsValidGroupNo(Controller* controller, int groupNo);
extern void Ros_Controller_ConnectionServer_Start(Controller* controller);

extern void Ros_Controller_StatusInit(Controller* controller);
extern BOOL Ros_Controller_StatusRead(Controller* controller, USHORT ioStatus[IO_ROBOTSTATUS_MAX]);
extern BOOL Ros_Controller_StatusUpdate(Controller* controller);
extern BOOL Ros_Controller_IsAlarm(Controller* controller);
extern BOOL Ros_Controller_IsError(Controller* controller);
extern BOOL Ros_Controller_IsPlay(Controller* controller);
extern BOOL Ros_Controller_IsTeach(Controller* controller);
extern BOOL Ros_Controller_IsRemote(Controller* controller);
extern BOOL Ros_Controller_IsOperating(Controller* controller);
extern BOOL Ros_Controller_IsHold(Controller* controller);
extern BOOL Ros_Controller_IsServoOn(Controller* controller);
extern BOOL Ros_Controller_IsEcoMode(Controller* controller);
extern BOOL Ros_Controller_IsEStop(Controller* controller);
extern BOOL Ros_Controller_IsWaitingRos(Controller* controller);
extern BOOL Ros_Controller_IsMotionReady(Controller* controller);
extern BOOL Ros_Controller_IsPflActive(Controller* controller);
extern int Ros_Controller_GetNotReadySubcode(Controller* controller);
extern int Ros_Controller_StatusToMsg(Controller* controller, SimpleMsg* sendMsg);

extern BOOL Ros_Controller_GetIOState(ULONG signal);
extern void Ros_Controller_SetIOState(ULONG signal, BOOL status);
extern void Ros_Controller_ErrNo_ToString(int errNo, char errMsg[ERROR_MSG_MAX_SIZE], int errMsgSize);

#ifdef DX100
extern void Ros_Controller_ListenForSkill(Controller* controller, int sl);
#endif

typedef enum
{
	SUBCODE_INVALID_AXIS_TYPE
} ROS_ASSERTION_CODE;
extern void motoRosAssert(BOOL mustBeTrue, ROS_ASSERTION_CODE subCodeIfFalse, char* msgFmtIfFalse, ...);

extern void Db_Print(char* msgFormat, ...);

extern void Ros_Sleep(float milliseconds);

//#define DUMMY_SERVO_MODE 1	// Dummy servo mode is used for testing with Yaskawa debug controllers
#ifdef DUMMY_SERVO_MODE
#warning Dont forget to disable DUMMY_SERO_MODE when done testing
#endif

#endif
