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
int singlestep=int(STEPS_PER_OUTPUT_REVOLUTION/9);  //step to take to move one mask 40 degrees
int initmoves=256;
int step4init=int(STEPS_PER_OUTPUT_REVOLUTION/initmoves); //in initalization, the tolarance of each steps to take is 2048/256=8
int incoming;

// setup
void setup(){
  BTserial.begin(38400);
  BTserial.println("Connected to BT.");
  mlx.begin();  
  pinMode(opticalpin, INPUT);
  //cli();
}

//loop
void loop(){

  if (BTserial.available()) {
    // read the incoming:
    incoming = BTserial.read();
    
    if(incoming==49){ //ascii(49)=1
      BTserial.println(incoming);
      Initialization();
    }
    
    if(incoming==50){ //ascii(50)=2
      BTserial.println(incoming);
      my_stepper.setSpeed(1000);
      for (int j=0;j<1;j++){ //record 10 rounds
        for (int i=0;i<9;i++){
          my_stepper.step(singlestep);
          delay(1000);
          for (int j=0;j<samples_per_move;j++){   //samples per move is 30, each will have a delay of 25ms, total 750ms+250ms=1s
            BTserial.print(i); BTserial.print(",");BTserial.print(mlx.readObjectTempC()); BTserial.print(",");BTserial.println(mlx.readAmbientTempC());
            delay(25);
          }
        }
      }
      Serial.println("***********");
    }
    
  }

}

void Initialization(){
  // poll pin2 until the value is high (means that the optical endstop is blocked), then immediately return.
  // meanwhile rotating
    my_stepper.setSpeed(1000);                               
    BTserial.println("enter init");
    delay(1000);
    attachInterrupt (0, StopStepper, RISING);  
    delay(1000);  
    my_stepper.step(STEPS_PER_OUTPUT_REVOLUTION);   //stepper motor rotates one round
    detachInterrupt(0);
    return;
}

void StopStepper(){
  BTserial.println("enter ISR");
  my_stepper.setSpeed(1000);
}



