/***********************
 * Name: new thermopile sensor compressive sensing 
 * Author: Libo Wu, libo.wu@tamu.edu, libo.wu1992@gmail.com
 * 
 */

#include <SoftwareSerial.h>
#include <Stepper.h>
#include <Adafruit_MLX90614.h>


//---( Number of steps per revolution of INTERNAL motor in 4-step mode )---
#define STEPS_PER_MOTOR_REVOLUTION 32
// Number of steps per revolution of OUTPUT SHAFT (= gear reduction; 2048 steps)
#define STEPS_PER_OUTPUT_REVOLUTION 32*64

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
//The pin connections need to be pins 8,9,10,11 connected
// to Motor Driver In1, In2, In3, In4 
// Then the pins are entered here in the sequence 1-3-2-4 for proper sequencing
Stepper my_stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10,9, 11);

// attach softwareserial of BT
SoftwareSerial BTserial(5,4); //set Bluetooth Rx=D5,Tx=D4

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int samples_per_move = 30;
int opticalpin = 2;  //use pin 2 (D2) as external interrupt
int step2take[9]={228,228,228,228,228,228,228,228,228};  //step to take to move one mask 40 degrees
int initmoves=1024;
int step4init=int(STEPS_PER_OUTPUT_REVOLUTION/initmoves); //in initalization, the tolarance of each steps to take is 2048/256=8
int incoming;
volatile bool motorflag;  //motorflag=true: stepper motor is running, false: is stopped
volatile int initpos=0;

// setup
void setup(){
  BTserial.begin(38400);
  BTserial.println("Connected to BT.");
  mlx.begin();  
  pinMode(opticalpin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //cli();
}

//loop
void loop(){
  
  if (BTserial.available()) {
    // read the incoming:
    incoming = BTserial.read();
    //********************************//
    // Method 1: divide 2048 steps into small steps(8steps), 
    // for each step, poll optical pin untill it is blocked
    //********************************//
//    if(incoming==49){ //ascii(49)=1
//      BTserial.println(incoming);
//      BTserial.println("enter init");
//      my_stepper.setSpeed(1000); 
//      for(int i=0;i<initmoves;i++){
//        int readval=digitalRead(opticalpin);
//        if(readval==HIGH){
//          break;
//        }
//        else{
//          my_stepper.step(step4init);
//        }
//      }
//    return; 
    
    //********************************//
    // Method 2: use external interrupt, 
    //********************************//
    if(incoming==49){ //ascii(49)=1
      BTserial.println(incoming);
      BTserial.println("enter init");
      
      my_stepper.setSpeed(1000);                               
      motorflag=true;
      BTserial.print("motorflag is:"); BTserial.println(motorflag);
      
      noInterrupts ();
      attachInterrupt (0, StopStepper, RISING);  
      interrupts ();
      
      delay(1000);  
      digitalWrite(LED_BUILTIN, LOW); 
      for (int i=0;i<2*initmoves;i++){
        if(motorflag){
          my_stepper.step(step4init);
        }
        else{
          BTserial.println("command 1: arrive init");
          break;
        }
      }
      detachInterrupt(0);
      return;
    }
    
    if(incoming==50){ //ascii(50)=2
      digitalWrite(LED_BUILTIN, LOW); 
      BTserial.println(incoming);
      my_stepper.setSpeed(1000);
      for (int j=0;j<3;j++){ //record 10 rounds
        findInitPos();
        for (int i=0;i<9;i++){
          for (int j=0;j<samples_per_move;j++){   //samples per move is 30, each will have a delay of 25ms, total 750ms+250ms=1s
            BTserial.print(i); BTserial.print(",");BTserial.print(mlx.readObjectTempC()); BTserial.print(",");BTserial.println(mlx.readAmbientTempC());
            delay(25);
          }
          my_stepper.step(step2take[i]+1);
          delay(1000);
        }
      }
      Serial.println("***********");
    }   
  }
}

void StopStepper(){
  motorflag=false;
}

void findInitPos(){ //run this function for every turn
  my_stepper.setSpeed(1000);                               
  for (int i=0;i<initmoves;i++){
    if(digitalRead(opticalpin)==LOW){
      my_stepper.step(step4init);
    }
    else{
      BTserial.println("command 2: arrive init");
      break;
    }
  }
  delay(2000);
  return;
}


