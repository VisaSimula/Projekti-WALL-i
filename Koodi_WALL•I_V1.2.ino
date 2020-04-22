#include <SD.h>                             // Include SD module library. -VS
#include <TMRpcm.h>                         // Include library for speaker control. -VS

#define SD_ChipSelectPin 4                  // Defining CS pin. -VS

#define enable 5                            // Defining enable pin (L298N driver). -VS
#define in1 0                               // Defining in1-in4 pin numbers (L298N driver). -VS
#define in2 1                               
#define in3 2                               
#define in4 3                               

#define DIST 15                             // Control detection distance. -VS
#define SPEED 125                           // Control the overall speed of both motors. -VS
#define ROTATION_TIME 650                   // Control the time used during turning functions. During short travel it is ROTATION_TIME*1.5. -VS

#define LED_PIN 8                           // Defining LED-pin number. -JM
//------------------------------------------------------------------------------------
// Following parts are for microSD module & speaker. -VS
TMRpcm tmrpcm;                              // Creating object for speaker library. -VS 
                                            // Note that Pin 9 must be used, the library is using Pin 9. -VS
                                            // Pinning for Arduino is following: CS=4, SCK=13, MOSI=11, MISO=12. -JM
//------------------------------------------------------------------------------------
// Following parts are for HC-SR04 sensor. -VS
const int trigPin = 6;                       // Defining Pin for HC-SR04 sensor trig. -VS
const int echoPin = 7;                       // Defining Pin for HC-SR04 sensor echo. -VS
long duration;                               // Variable used to measure distance. -VS
int distance;

//------------------------------------------------------------------------------------
// Condition variables & speed.
bool soundCondition = true;                  // With "soundCondition" it is possible to make sure that certain functions go through only once. -VS
bool distCondition = true;                   // With "distCondition" it is possible to make sure that certain functions go through only once. -VS
int motorSpeed = SPEED;

//------------------------------------------------------------------------------------
// Introducing the functions- -VS
int checkObstacle (void); // -VS
void sound1 (void);// -VS
void sound2 (void);// -VS

void turnLeft (void);// -VS
void turnRight (void);// -VS
void shortTravel (void);// -VS
void travelForward (void); // -JM
void stay (void); //-JM 

//------------------------------------------------------------------------------------
void setup() {
tmrpcm.speakerPin = 9;                     // Defining speaker Pin. (Moved from global to setup). -VS
                                      
if(!SD.begin (SD_ChipSelectPin))           // Check if SD card is present and can be initialized. -VS
{
  return;                                  // If no SD card is present, end program. -VS
}

tmrpcm.setVolume (4);                      // Volume can be set from 0 to 7. Set volume to 4. -VS

pinMode(trigPin, OUTPUT);                  // Set Pin mode for trigPin and echoPin. -VS
pinMode(echoPin, INPUT);                   

pinMode(LED_PIN, OUTPUT);                  // Set pin mode for LED_PIN. -JM
digitalWrite (LED_PIN, HIGH);              // Set LED_PIN to HIGH-state = green light. -JM

pinMode (enable, OUTPUT);                  // Will control speed of both motors.
pinMode (in1, OUTPUT);                     // Set pin mode for in1-in4 pins (L298N driver). -JM
pinMode (in2, OUTPUT);                     
pinMode (in3, OUTPUT);                     
pinMode (in4, OUTPUT);                     
// Set initial rotation direction (-> robot moves forward). - JM
digitalWrite (in1, LOW);                   // Set states (HIGH/LOW) for in1-in4 pins. -JM
digitalWrite (in2, LOW);                   
digitalWrite (in3, LOW);                   
digitalWrite (in4, LOW);                   

analogWrite (enable, motorSpeed);

sound1();                                   // Plays .wav file when switched ON or reset. (if not working, check filename)-VS
delay(9000);                               // Delay allows sound1 to end before starting loop. -VS
}
//------------------------------------------------------------------------------------
void loop() {
  if(checkObstacle() > DIST)                // Checks if there is any obstacles closer than 20 cm. -JM
  {
    distCondition=false;                    // By setting distCondition to false, stops reading values from SR04. -VS
    digitalWrite (LED_PIN, HIGH);           // If no obstacle, turn LED green (HIGH-state) and travel forward. -JM
    soundCondition = true;                  // Set soundCondition to true to make sure that sound plays only once. -VS
    travelForward();                        // Normal state when no obstacles. -VS
  }

  if(checkObstacle() <= DIST)               // Checks if there is any obstacles closer than 20 cm. -JM           
  {
    distCondition=false;                    // By setting distCondition to false, stops reading values from SR04. -VS
    digitalWrite (LED_PIN, LOW);            // If obstacle, turn LED yellow (LOW-state) -JM
    stay();
    if(soundCondition==true)                // Added if and soundCondition so that sound plays only once, will be able to play it again only after completing another function. -VS
    {
      soundCondition = false;               // Set soundCondition to false so sound wont play more than once. -VS
      sound2();                             // Plays .wav file for sound2. -JM
    }
    turnRight();                            // Turn right when facing first obstacle. -VS
    
    if(checkObstacle() <= DIST)             // Checks if there is any obstacles closer than 20 cm. -VS           
    {
      distCondition=false;
     if(soundCondition==true)               // Added if and soundCondition so that sound plays only once, will be able to play it again only after completing another function. -VS
      {
        soundCondition = false;             // Set soundCondition to false so sound wont play more than once. -VS
        sound2();                           // Plays .wav file for sound2. -VS
       }
      turnRight();                          // Call trunRight function. -VS
    }
    if(checkObstacle() > DIST)
    {
      distCondition=false;
      shortTravel();                        // Call shortTravel function. -VS
      turnLeft();                           // Call leftRight function. -VS
      if(checkObstacle() <= DIST)
      {
        distCondition=false;
        sound2();
        turnRight();                        // Call turnRight function. -VS
      }
    }
  }
}
//------------------------------------------------------------------------------------
int checkObstacle (void)                    // Function that will read and convert values from SR-04. -VS
{
  if(distCondition==true)
  { 
    int avg=0;                              // Add avg variable to calculate average distance out of 3 tries.
    
    for(int i=0; i<3;i++)
    {
    digitalWrite(trigPin, LOW);             // Will reset SR-04 trig to LOW state to avoid false information. -VS
    delayMicroseconds (2);                  // Wait 2 microseconds. -VS
  
    digitalWrite(trigPin, HIGH);            // Send ultrasonic sound for 10 microseconds. -VS
    delayMicroseconds (10);
    digitalWrite(trigPin, LOW);             // Stop sending ^

    duration = pulseIn(echoPin, HIGH);      // Save time value of the pulse lenght. -VS
    distance = duration*0.034/2;            // Calculate distance in CM (s=t*v/2). -VS
    
    avg= avg+distance;
    }
    distance = avg/3;
    return distance;
  }
}
//------------------------------------------------------------------------------------
void sound1 (void)
{
  tmrpcm.play ("1.wav");                     // Function that will play 1.wav file. -VS
}
//------------------------------------------------------------------------------------
void sound2 (void)
{
  digitalWrite (in1, LOW);                  // Set states (LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, LOW);                 
  digitalWrite (in3, LOW);                  
  digitalWrite (in4, LOW);                  
                                            // Stops with no extra delay. -VS
                                              
  tmrpcm.play ("2.wav");                    // Function that will play 2.wav file. -VS
  delay(4000);
}
//------------------------------------------------------------------------------------
void turnLeft (void)
{
  // Here is function fur turning robot to left. -VS
  digitalWrite (in1, HIGH);                  // Set states (HIGH/LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, LOW);                   
  digitalWrite (in3, LOW);                   
  digitalWrite (in4, HIGH);                  
  delay (ROTATION_TIME);                     // Turning left for 650 millseconds.
  stay();
  soundCondition = true;                     // Will allow sound to play again when facing new obstacle. -VS
  distCondition = true;
}
//------------------------------------------------------------------------------------
void turnRight (void)
{
  // Here is function for turning robot to right-VS
  digitalWrite (in1, LOW);                    // Set states (HIGH/LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, HIGH);                   
  digitalWrite (in3, HIGH);                   
  digitalWrite (in4, LOW);                    
  delay (ROTATION_TIME);                      // Turning right for 650 millseconds. -JM
  stay();                                
  soundCondition = true;                      // Will allow sound to play again when facing new obstacle. -VS
  distCondition = true;
}
//------------------------------------------------------------------------------------
void shortTravel (void)
{
  // This function will allow robot to travel forward for around 20cm. -VS
  digitalWrite (in1, HIGH);                   // Set states (HIGH/LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, LOW);                    
  digitalWrite (in3, HIGH);                   
  digitalWrite (in4, LOW);                    
  delay (ROTATION_TIME*1.5);                  // Travelling forward for 975 millseconds.
  stay(); 
  soundCondition = true;                      // Will allow sound to play again when facing new obstacle. -VS
  distCondition = true;
}
//------------------------------------------------------------------------------------
void travelForward (void)
{
  // This function will allow robot to travel forward. -JM
  digitalWrite (in1, HIGH);                   // Set states (HIGH/LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, LOW);                    
  digitalWrite (in3, HIGH);                   
  digitalWrite (in4, LOW);                    
  delay(200);
  distCondition = true;
}
//-----------------------------------------------------------------------------------
void stay (void)
{
  // This function will stop robot for a moment. -JM
  digitalWrite (in1, LOW);                    // Set states (LOW) for in1-in4 pins (L298N driver). -JM
  digitalWrite (in2, LOW);                    
  digitalWrite (in3, LOW);                    
  digitalWrite (in4, LOW);                    
  delay(700);                                 // Wait for 700 ms before executing next line. -JM
}
