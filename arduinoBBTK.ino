typedef struct{
  volatile unsigned long time;
  volatile byte portb;
  volatile byte portd;
}measurement_type;

#define BUFFERLEN 128
volatile measurement_type measurement[BUFFERLEN];

// measurement count - number of unsent measurements
volatile byte mcount = 0; 

// number of measurements so far
long run = 0;


void setup(){                      
  for(int pin = 0; pin <= 13; pin++){
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH); // turn on pullup resistors
  }
  
  pinMode(13,OUTPUT); 
 
  Serial.begin(115200);
  Serial.print("Arduino latency test\n");
  
  // Pin to interrupt map:
  // D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
  // D8-D13 = PCINT 0-5 = PCIR0 = PB = PCIE0 = pcmsk0
  // A0-A5 (D14-D19) = PCINT 8-13 = PCIR1 = PC = PCIE1 = pcmsk1
  // arduino pin mapping: http://brittonkerin.com/cduino/pin_map.html
  
  // Enable Pin Change Interrupt 0 (PORTB) and 2 (PORTD)
  PCICR = (1 << PCIE0) | (1 << PCIE2);
  
  // Interrupt on change of pin 3
  PCMSK2 = (1<<PCINT18) ;
  // Interrupt on change of pin 9, 10, 11
  PCMSK0 = (1<<PCINT1) | (1<<PCINT2) | (1<<PCINT3); 
}


void loop(){
  if(mcount > 0){
    sendResults();    
    mcount--;
    run++;
 }
}


void sendResults(){
  static unsigned long lasttime;
  byte m=mcount-1; // current measurement number  
  Serial.print((String)run + "\t" + (String)measurement[m].portb \
                           + "\t" + (String)measurement[m].portd + "\t");    
  Serial.println(measurement[m].time - lasttime );
  lasttime = measurement[m].time;
  //Serial.println(measurement[m].time);
}


inline void takeMeasurement(){  
  measurement[mcount].portb = PINB;
  measurement[mcount].portd = PIND;
  measurement[mcount].time  = micros();  
  mcount++;
}


ISR( PCINT2_vect){
  takeMeasurement();
}

ISR( PCINT0_vect){ 
  takeMeasurement();
}
