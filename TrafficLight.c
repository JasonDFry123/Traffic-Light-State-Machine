// TrafficLight.c
// Runs TM4C123
// Index implementation of a Moore finite state machine to operate
// a traffic light.
// Your Name: Jason Fry
// created: 03/06/2020
// last modified: 03/24/2020
// 

// Included headers 

#include "tm4c123gh6pm.h"
#include "PLL.h"

// Linked data structure

void PortA_Init(void);
void PortC_Init(void);
void PortE_Init(void);
void SysTick_Init(void);
void SysTick_Wait(void);
void SysTick_Wait1s(unsigned long time);

// State Deinitions

struct State{
 unsigned long lights;
 unsigned long  time;
 const struct State *Next[16];

};

typedef const struct State SType;

#define goIvy &FSM[0]
#define waitIvy &FSM[1]
#define go1st &FSM[2]
#define wait1st &FSM[3]
#define walkIvy &FSM[4]
#define dontWalkIvy &FSM[5]
#define walk1st &FSM[6]
#define dontWalk1st &FSM[7]


//Declare your states here 

SType FSM[8]= {

  {0x30, 7, {goIvy,waitIvy,goIvy,goIvy,walk1st, walk1st,walk1st,walk1st, waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy}},

  {0x50, 2, {go1st,go1st,goIvy,go1st,walk1st,walk1st,walk1st,walk1st,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy}},

	{0x84, 7, {wait1st,go1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st,wait1st}},

  {0x88, 2, {goIvy,goIvy,goIvy,goIvy,walk1st,walk1st,walk1st,walk1st,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy,walkIvy}},

	{0x20>>4, 4, {dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy,dontWalkIvy}},

	{0x90, 3, {goIvy,go1st,goIvy,goIvy,walk1st,go1st,goIvy,goIvy,goIvy,go1st,goIvy,goIvy,goIvy,go1st,goIvy,goIvy}},

	{0x10>>4, 4, {dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st,dontWalk1st}},

	{0x30, 3, {goIvy,waitIvy,goIvy,goIvy,goIvy,waitIvy,goIvy,goIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy,waitIvy}}
};


int main(void){ volatile unsigned long delay;
	
	// Decleration of State variables.
	unsigned long time;
	unsigned long sensor;
	unsigned long output;
	SType *Ptr; 		//Declare Pointer

	// Definition of State variables.
	Ptr = goIvy; 		//Pointer initalized as goIvy		
  
	sensor=GPIO_PORTC_DATA_R&0x0F;
	output= Ptr -> lights; //Inital output and lights as goIvy
	
	// Initialization of Ports, SysTick and PLL.
	PLL_Init();
	SysTick_Init();
	
  PortA_Init();
	PortC_Init();
	PortE_Init();
	
	//Finite State Function
	
	while(1){
	
 SysTick_Wait1s(Ptr -> time);

	sensor=GPIO_PORTC_DATA_R&0x0F;

  Ptr = Ptr -> Next[sensor];

	output = Ptr -> lights;

  }
}


// Inilize Port Functions
	
// Initialize Port A for Traffic Lights (PA2-PA7 as outputs)

void PortA_Init(void){
	SYSCTL_RCGC2_R |= 0x00000001;						// 1. Activate Port A
	while((SYSCTL_PRGPIO_R & 0x0000000) == 0){}; 
																					// 2. Wait for clock to stabilize. No need to unlock
	GPIO_PORTA_AMSEL_R &= ~0xFC;						// 3. Disable analog functions on PA2-PA7
	GPIO_PORTA_PCTL_R = 0x00000000;					// 4. Enable the general purpose I/O on Port A
	GPIO_PORTA_DIR_R |= 0xFC;								// 5. DIR (set PA2-PA7 as outputs)
	GPIO_PORTA_AFSEL_R &= ~0xFC;						// 6. Disable alternative functionality on PA2-PA7
	GPIO_PORTA_DEN_R |= 0xFC;					    	// 7. DEN (enable digital for PA2-PA7)
	}

// Initialize Port C for Sensors and Walk Buttons (PC4-PC7 as inputs)

	void PortC_Init(void){
	SYSCTL_RCGC2_R |= 0x04;									// 1. Activate Port C
	while((SYSCTL_PRGPIO_R & 0x0000000) == 0){}; 
																					// 2. Wait for clock to stabilize. No need to unlock
	GPIO_PORTC_AMSEL_R &= ~0xF0;						// 3. Disable analog functions on PC4-PC7
	GPIO_PORTC_PCTL_R = 0x00000000;					// 4. Enable the general purpose I/O on Port C
	GPIO_PORTC_DIR_R &= ~0xF0;							// 5. DIR (set PC4-PC7 as inputs)
	GPIO_PORTC_AFSEL_R &= ~0xF0;						// 6. disable alternative functionality on PC4-PC7
	GPIO_PORTC_DEN_R |= 0xF0;					    	// 7. DEN (enalbe digital on PC4-PC7)
	}
	
//Initialize Port E for Walk Signals (PE4-PE5 as ouptuts)

	void PortE_Init(void){	
	SYSCTL_RCGC2_R |= 0x10;									// 1. Activate Port E
	while((SYSCTL_PRGPIO_R & 0x0000000) == 0){}; 
																					// 2. Wait for clock to stabilize. No need to unlock GPIO A
	GPIO_PORTE_AMSEL_R &= ~0x30;						// 3. Disable analog functions on PE4-PE5
	GPIO_PORTA_PCTL_R = 0x00000000;					// 4. Enable the general purpose I/O on Port E
	GPIO_PORTA_DIR_R |= 0x30;								// 5. DIR (set PE4-PE5 to outputs)
	GPIO_PORTA_AFSEL_R &= ~0x30;						// 6. disable alternative functionality on Port a
	GPIO_PORTA_DEN_R |= 0x30;					    	// 7. DEN (enable digital on PE4-PE5)
		
	}