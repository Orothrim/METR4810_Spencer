// METR4810 first revision


//Includes for the main function

//Base C++ includes, formatting includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Motor Control Includes
#include "PWM_Functions.h"

//Communication Protocal Includes
#include "Serial_Functions.h"
#include "TWI_Functions.h"

//Sensor Includes
#include "Camera_Functions"
#include "IR_Functions"
#include "Timer_Functions"

//First LED flash to say includes have concluded


//Defines for main function

//Debugging defines
#define DEBUG 	0 //Set to 1 during testing, will simplify debugging
#define LED1 	0
#define LED2 	0
#define LED3 	0
//DEBUG Legend:


//Defines for Rotors
#define PWM_FREQUENCY			50
#define PWN_DUTY_CYCLE			2000
#define MAX_DEGREES				240  // Servo limits
#define MIN_DEGREES				65   // 
#define ROTOR1					0
#define ROTOR2					0


//Define for Communication Protocals
#define TX_FREQUENCY			0
#define TWI_FREQUENCY			0


//Defines for Sensors
#define CAM_TX
#define CAM_RX
#define CAM_DEBUG

#define IR_PIN
#define BUTTON1
#define BUTTON2

//Second LED flash to say defines have concluded



//Setup Functions

//Base system setup functions, and formatting setup functions


//Rotor setup functions
pwm_setup(PWM_FREQUENCY);
rotor_comm_setup();


//Communication setup functions
twi_setup();
serial_setup();

//Sensor setup functions
ir_setup(IR_PIN);
cam_setup();
button_setup(BUTTON1, BUTTON2);


//Third LED flashing to indicate setup functions are completed


int main( void )
{


	
}

