//using some code and tuning settings from wonderful Federico Dossena Work 
//https://fdossena.com/?p=ArduinoFanControl/i.md
//actually using only PWM timer 1 ports (9-10) on a XX Board

#define PIN_SENSE 2 //where we connected the fan sense pin. Must be an interrupt capable pin (2 or 3 on Arduino Uno)
#define DEBOUNCE 0 //0 is fine for most fans, crappy fans may require 10 or 20 to filter out noise
#define FANSTUCK_THRESHOLD 500 //if no interrupts were received for 500ms, consider the fan as stuck and report 0 RPM
#define DELAYTIME 4000 //milliseconds to wait
//Interrupt handler. Stores the timestamps of the last 2 interrupts and handles debouncing
unsigned long volatile ts1=0,ts2=0;

String inputFanSpeed;
float dutyVal;
String delayS;
int delayT;
int i;

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
        return (60000/(ts2-ts1))/2;
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
    return percentage / 100;

}

void setup(){
    //enable outputs for Timer 1
    pinMode(9,OUTPUT); //1A
    pinMode(5, OUTPUT); //on-off auto-switch
    //pinMode(10,OUTPUT); //1B
    setupTimer1();
    //enable outputs for Timer 2
    //pinMode(3,OUTPUT); //2
    //setupTimer2();
    //note that pin 11 will be unavailable for output in this mode!

    pinMode(PIN_SENSE,INPUT_PULLUP); //set the sense pin as input with pullup resistor
    attachInterrupt(digitalPinToInterrupt(PIN_SENSE),tachISR,FALLING); //set tachISR to be triggered when the signal on the sense pin goes low
    Serial.begin(9600); //enable serial so we can see the RPM in the serial monitor
    digitalWrite(5,HIGH); //spengo

}
void loop(){
    Serial.println("Set Fan Speed (10-100): ");
    while (Serial.available() == 0)
   /* just wait */ ;

/* read the incoming byte*/
 
    //delay(2000);
    inputFanSpeed = Serial.readString();    
    Serial.println("Readed: ");
    Serial.print(inputFanSpeed);

    Serial.println("Set Duration (number of 4sec. slots to try) ");
    while (Serial.available() == 0)
   /* just wait */ ;

/* read the incoming byte*/
 
    //delay(2000);
    delayS = Serial.readString();    
    Serial.println("Readed: ");
    Serial.print(delayS.toInt());
    delayT = delayS.toInt();

    dutyVal = convertToDutyCycle(inputFanSpeed.toInt());
    Serial.println("Readed DutyVal: ");
    Serial.print(dutyVal);
    setPWM1A((float)dutyVal);
    i=0;
    while(i != delayT)
    {
        delay(DELAYTIME);
        Serial.print(" - RPM:");
        Serial.println(calcRPM());
        i++;
    }
    

// switch(inputFanSpeed) {
//     case "0":
//     Serial.print("Set Duty to 0% ");
//     setPWM1A(0.0f); //set duty to 50% on pin 9
//     //digitalWrite(5,LOW); //spengo
//     break;
//     case '10':
//     Serial.print("Set Duty to 10% ");
//     setPWM1A(0.1f); //set duty to 50% on pin 9
//     break;
//     case '20':
//     Serial.print("Set Duty to 20% then measure \n");
//     setPWM1A(0.2f); //set duty to 50% on pin 9
//     break;
//     case '50':
//     Serial.print("Set Duty to 50% then measure \n");
//     setPWM1A(0.5f); //set duty to 50% on pin 9

//     break;
//     case '70':
//     Serial.print("Set Duty to 70% then measure \n");
//     setPWM1A(0.7f); //set duty to 50% on pin 9
//     break;
//     case '100':
//     Serial.print("Set Duty to 100% then measure \n");
//     setPWM1A(1.0f); //set duty to 50% on pin 9

//     break;
//     case 'M':
//     delay(DELAYTIME);
//     Serial.print(" - RPM:");
//     Serial.println(calcRPM());
//     delay(DELAYTIME);
//     Serial.print(" - RPM:");
//     Serial.println(calcRPM());
//     break;
// }


    //example...
    //setPWM1B(0.2f); //set duty to 20% on pin 10
    //setPWM2(0.8f); //set duty to 80% on pin 3


//reading RPM
    

    //example...
    //setPWM1B(0.2f); //set duty to 20% on pin 10
    //setPWM2(0.8f); //set duty to 80% on pin 3




    //example...
    //setPWM1B(0.2f); //set duty to 20% on pin 10
    //setPWM2(0.8f); //set duty to 80% on pin 3




    //example...
    //setPWM1B(0.2f); //set duty to 20% on pin 10
    //setPWM2(0.8f); //set duty to 80% on pin 3


    //example...
    //setPWM1B(0.2f); //set duty to 20% on pin 10
    //setPWM2(0.8f); //set duty to 80% on pin 3



}


