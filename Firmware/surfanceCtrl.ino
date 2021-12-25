//using some code and tuning settings from Federico Dossena Work (GPL license)
//https://fdossena.com/?p=ArduinoFanControl/i.md
//reading part from Charles Gantt (since federico's one was faulty)
//http://themakersworkbench.com
//actually using only PWM timer 1 ports (9-10) on a CH341 Nano Arduino Clone Board
//setting up a pull-up resistor 10kOhm on pin D2
    //To disable interrupts: cli(); disable global interrupts and
    //to enable them: sei(); enable interrupts
//NOTE: I'm Assuming you are using the same fan model.

#define PIN_SENSE 2 //where we connected the fan sense pin. Must be an interrupt capable pin (2 or 3 on Arduino Uno)
#define PIN_BUTTON 3 //where we connected the button controller. Must be an interrupt capable pin.
#define DEBOUNCE 0 //0 is fine for most fans, crappy fans may require 10 or 20 to filter out noise
#define FANSTUCK_THRESHOLD 500 //if no interrupts were received for 500ms, consider the fan as stuck and report 0 RPM
#define DELAYTIME 4000 //milliseconds to wait
#define REDPIN 4
#define GREENPIN 5
#define BLUEPIN 6

//Interrupt handler. Stores the timestamps of the last 2 interrupts and handles debouncing
unsigned long volatile ts1=0,ts2=0;
volatile int pressed = 0;

//VARIABLES
bool INPUTALLOWED = true;
String inputFanSpeed, LightsCommand,delayS;
float dutyVal;
int delayT, i, input, NbTopsFan, Calc;
int redVal = 0, greenVal = 0, blueVal = 0;
//This is the varible used to select the fan and it's divider,
    //set 1 for unipole hall effect sensor
    //and 2 for bipole hall effect sensor
unsigned int fanDiv = 2;

void tachISR() {
    unsigned long m=millis();
    if((m-ts2)>DEBOUNCE){
        ts1=ts2;
        ts2=m;
    }
}
//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
unsigned long calcRPM(){
    if(millis()-ts2<FANSTUCK_THRESHOLD&&ts2!=0){
        unsigned long res;
        res = (60000/(ts2-ts1))/2;
        return res;
    }else return 0;
}

//configure Timer 1 (pins 9,10) to output 25kHz PWM
void setupTimer1(){
    //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << CS10) | (1 << WGM13);
    ICR1 = 320;
    OCR1A = 0;
    OCR1B = 0;
}

//equivalent of analogWrite on pin 9
void setPWM1A(float f){
    f=f<0?0:f>1?1:f;
    OCR1A = (uint16_t)(320*f);
}

float convertToDutyCycle(int percentage){
    float dutyConversion;
    
    dutyConversion = percentage;
    Serial.print(dutyConversion);
    dutyConversion = dutyConversion / 100;
    return dutyConversion;

}

void CalcRpm(){
    NbTopsFan = 0; //Set NbTops to 0 ready for calculations
    sei(); //Enables interrupts
    delay(1000); //wait 1 sec.
    cli(); //Disable interrupts
    //Times NbTopsFan (which is apprioxiamately the fequency the fan 
    //is spinning at) by 60 seconds before dividing by the fan's divider
    Calc = ((NbTopsFan * 60)/fanDiv);
    sei(); //re-enabling interrupts to allow other functions
    Serial.print (Calc, DEC);//Prints the number calculated above   
    Serial.print (" rpm\r\n"); //Prints " rpm" and a new line
}

void SetLight(){
    Serial.print("Select Light Color: (RED/GREEN/BLUE/WHITE/OFF) \n");
    while (Serial.available() == 0);

    LightsCommand = Serial.readString();
    if (LightsCommand == "RED")
        redVal = 255;
    else if (LightsCommand == "GREEN")
        greenVal  = 255;
    else if (LightsCommand == "BLUE")
        blueVal = 255;
    else if (LightsCommand == "WHITE"){
        redVal = 255;
        blueVal = 255;
        greenVal = 255;
    }
    else if (LightsCommand == "OFF"){
        redVal = 0;
        blueVal = 0;
        greenVal = 0;
    }else
        Serial.println("Wrong Input");

    analogWrite(REDPIN, redVal);
    analogWrite(GREENPIN, greenVal);
    analogWrite(BLUEPIN, blueVal);
}

void setFanSpeed(int speed){
    if(speed == -1)
    { //manual mode
        Serial.println("Manual Mode: ");
        Serial.println("Set Fan Speed (10-100): ");
        while (Serial.available() == 0)
    /* just wait */ ;

        /* read the incoming byte*/
    
        //delay(2000);
        inputFanSpeed = Serial.readString();    
        Serial.print("Readed: ");
        Serial.println(inputFanSpeed);

        Serial.print("Set Duration (number of 4sec. slots to try) ");
        while (Serial.available() == 0)
    /* just wait */ ;

        /* read the incoming byte*/
    
        //delay(2000);
        delayS = Serial.readString();    
        Serial.print("Readed: ");
        Serial.println(delayS.toInt());
        delayT = delayS.toInt();

        dutyVal = convertToDutyCycle(inputFanSpeed.toInt());
        Serial.print("Readed DutyVal: ");
        Serial.println(dutyVal);
        setPWM1A(dutyVal);
        i=0;
        while(i != delayT)
        {
            
            CalcRpm(); //reads RPM
            

            i++;
        }
    }else //auto mode
    {
        dutyVal = convertToDutyCycle(speed);
        Serial.print("Readed DutyVal: ");
        Serial.println(dutyVal);
        setPWM1A(dutyVal);
        
    }
}

void rpmCount ()
//This is the function that the interupt calls
{ 
    NbTopsFan++; 
}

void buttonPressed(){
    //se token occupato non faccio nulla
    if(!INPUTALLOWED){
        Serial.println("BUTTON DISABLED!");
    }else {
    //detachInterrupt(buttonPressed);
    INPUTALLOWED = false;
    //noInterrupts();
    pressed++;
    Serial.println("BUTTON PRESSED!");

    if(pressed > 4)
        pressed = 0;
    switch (pressed)
    {
    case 0:
        setFanSpeed(0);
        //Serial.println("Speed set at: " + 25*pressed);
        break;
    case 1:
        setFanSpeed(25);
        //Serial.println("Speed set at: " + 25*pressed);
        break;
    case 2:
        setFanSpeed(50);
        //Serial.println("Speed set at: " + 25*pressed);
        break;
    case 3:
        setFanSpeed(75);
        //Serial.println("Speed set at: " + 25*pressed);
        break;
    case 4:
        setFanSpeed(100);
        //Serial.println("Speed set at: " + 25*pressed);
        break;
    default:
        setFanSpeed(0);
        Serial.println("Wrong number insertion: " + pressed);
        break;
    }
    }
}

void setup(){
    //enable outputs for Timer 1
    pinMode(9,OUTPUT); //1A
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    //pinMode(5, OUTPUT); //on-off auto-switch
    //pinMode(10,OUTPUT); //1B
    setupTimer1();
    //enable outputs for Timer 2
    //pinMode(3,OUTPUT); //2
    //setupTimer2();
    //note that pin 11 will be unavailable for output in this mode!

    pinMode(PIN_SENSE,INPUT_PULLUP); //set the sense pin as input with pullup resistor
    pinMode(PIN_BUTTON,INPUT_PULLUP); //set the button pin as input with pull
   // attachInterrupt(digitalPinToInterrupt(PIN_SENSE),tachISR,FALLING); //set tachISR to be triggered when the signal on the sense pin goes low
    attachInterrupt(digitalPinToInterrupt(PIN_SENSE),rpmCount,RISING); //set tachISR to be triggered when the signal on the sense pin goes low
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonPressed, LOW); // trigger when button pressed, but not when released.
    Serial.begin(9600); //enable serial so we can see the RPM in the serial monitor
    //digitalWrite(5,HIGH); //spengo
    
    



    
        
}
void loop(){  
    //Serial.println("Fan Standing By, awaiting command...");
    delay(2000);
    INPUTALLOWED = true;
    //attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonPressed, LOW); // trigger when button pressed, but not when released.
    //interrupts();

// Serial.println("1 : Fan Speed - 2 : Light Color \n");
//     while (Serial.available() == 0)
//    /* just wait */ ;
//    input = Serial.readString().toInt();
//    if(input == 1)
//         setFanSpeed();
//    else if(input == 2)
//         SetLight();
//     else;
//     //delay(DELAYTIME);
    



}


