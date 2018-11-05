/***********************
 * Name: new thermopile sensor compressive sensing 
 * Author: Libo Wu, libo.wu@tamu.edu, libo.wu1992@gmail.com
 * 
 */

#include <SoftwareSerial.h>
#include <Stepper.h>
#include <Adafruit_MLX90614.h>

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
//The pin connections need to be pins 8,9,10,11 connected
// to Motor Driver In1, In2, In3, In4 
// Then the pins are entered here in the sequence 1-3-2-4 for proper sequencing
Stepper my_stepper(STEPS, 8, 10, 9, 11);

// attach serial of BT
SoftwareSerial BTserial(0,1); //set Bluetooth Rx=0,Tx=1
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int samples_per_move = 30;
int opticalpin = 2;  //use pin 2 (D2) as external interrupt
int singlestep=40;

void setup(){
  BTserial.begin(38400);
  mlx.begin();  
}

void loop(){
  Initialization();
  for (int i=0;i<9;i++){
    my_stepper.step(singlestep);
    delay(50);
    for (int j=0;j<samples_per_move;j++){   //samples per move is 30, each will have a delay of 25ms, total 750ms+50ms=800ms
      BTserial.print(singlestep*i); BTserial.print(",");BTserial.print(mlx.readObjectTempC()); BTserial.print(",");BTserial.print(mlx.readAmbientTempC());
      delay(25);
    }
  }
}

void Initialization(){
  // poll pin2 until the value is high (means that the optical endstop is blocked), then immediately stop?
  pinMode(2, INPUT);
  int val=digitalRead(opticalpin);
  while(val==LOW){
    val=digitalRead(opticalpin);
  }
  return;
}

