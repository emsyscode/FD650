//                  0x01
//                 ------
//        0x20    |      |  0x02
//                | 0x40 |
//                 ------
//        0x10   |      |  0x04
//                | 0x08 |
//                 ------  o 0x80
//
//This is the assignment of the bit weight to the segments.
//Your digits should be created based on these values.
//When sent 0x00 this means all segments will be ON because the
//display is anode commum.
//the weight of bit at binary format: 0b84218421
//and are aligned with standard seg :   hgfedcba



#define sda 8
#define scl 9

#define LED1 6
#define LED2 7

// add this to the top of your sketch
#define NOP __asm__ __volatile__ ("nop\n\t")

// and then use it in code as follows
// NOP; // delay 62.5ns on a 16MHz AtMega


#define I2C_wr 0x7E  // This is address of I2C to module of RTC arduino.
#define I2C_rd 0x7D  // This is address of I2C to module of RTC arduino.

/******************************** Define FD650 Commands *******************************/
#define FD650_KEY_RDCMD		0x4F	/* Read keys command			                            */
#define FD650_MODE_WRCMD		0x48	/* Write mode command		                           	*/
#define FD650_DISP_ON			0x01	/* FD650 Display On			                              */
#define FD650_DISP_OFF			0x00	/* FD650 Display Off			                          */
#define FD650_7SEG_CMD			0x40	/* Set FD650 to work in 7-segment mode	            */
#define FD650_8SEG_CMD			0x00	/* Set FD650 to work in 8-segment mode	            */
#define FD650_BASE_ADDR		0x68	/* Base data address			                            */
#define FD655_BASE_ADDR		0x66	/* Base data address			                            */
#define FD650_DISP_STATE_WRCMD		0x00	/* Set display modw command		                */
/**************************************************************************************/

//I only do the creation of first 16 char's (numbers: 0-9 and letters: A-F)
uint8_t letters[128] {
//0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0xFF, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, // 0x00
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
  0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50 
  0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00  // 0x70
};
uint8_t msgHello[15] = {0x00, 0x00, 0x00, 0x00, 0x76, 0x79, 0x38, 0x38, 0x3F, 0x00, 0x00, 0x00, 0x00};

int flag = 0;
int ack;
unsigned char arr[14];

void nop(void);
 unsigned char readVal, write2, readed;

 unsigned char counter=0;



void nop(unsigned int multiple){
 // and then use it in code as follows
 for (int i =0; i< multiple; i++){
  NOP; // delay 62.5ns on a 16MHz AtMega
 }
}
//*********************myDelay****************************/
void myDelay(unsigned int count){
  int i,j;
  for(i=0;i<count;i++)
    for(j=0;j<1275;j++);
}
//*********************START****************************/
void start(){    //start condition
  // Time need waite until start new communication. ( 1.3 uSec) but the delay cant be done here,I'm do it on stp
   // The start condition is: Downing SDA while SCL is HIGH
   // The status of SCL is HIGH because the Stop let it in this value!
  digitalWrite(scl, LOW);
  nop(10);
  digitalWrite(sda, HIGH);
  nop(10);
  digitalWrite(scl, HIGH);
  nop(10);
  digitalWrite(sda,LOW);
  nop(10);
}
//*********************STOP****************************/
void stp(){     //stop condition
  digitalWrite(sda, LOW);
  nop(10);
  digitalWrite(scl, HIGH);  // The SCL stay every time on upper value, this mean must finish allways upper!
  nop(10);
  digitalWrite(sda, HIGH); // The stop condition is: Rise SDA while SCL is HIGH
  nop(10);
  delayMicroseconds(2);  // time of free bus before a new start communication(1.3 uSec)
}
//*********************Slave AKNOWLEDGE****************************/
void readAcknowledge(){    //acknowledge condition
  digitalWrite(scl, LOW);
  nop(10);
  pinMode(sda, INPUT);
  nop(10);
  digitalWrite(scl, HIGH);
  nop(20);
  digitalWrite(scl, LOW);
  nop(10);
  digitalWrite(sda, HIGH);
  nop(10);
  pinMode(sda, OUTPUT);
  nop(10);
  
}
//*********************Master AKNOWLEDGE****************************/
void sendAcknowledge(){    //acknowledge condition
  // This is used when the Master is reading and need send a ACK by each byte received drom Slave!
  // SDA must be as a output pin to force bus sda go down!
  digitalWrite(scl, LOW);
  nop(10);
  digitalWrite(sda, LOW); // Here is the master wich send a ack to the slave!!!
  nop(10);
  digitalWrite(scl, HIGH);
  nop(10);
  digitalWrite(scl, LOW);
  nop(10);
  digitalWrite(sda, HIGH);
  nop(10);
}
void noAcknowledge(){    //acknowledge condition
  digitalWrite(sda, HIGH);
  nop(10);
  digitalWrite(scl, HIGH);
  nop(10);
  ack=digitalRead(PD7);         //reading acknowledge
  digitalWrite(scl, LOW);
  nop(10);
}
void send_byte(unsigned char Bits){  //send byte serially
  // On the send Byte, the SDA must be High to allow the 
  //receiver send a low pulse in sda(like a pull-up status)
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  data=Bits;
  digitalWrite(scl, LOW);   // Must stay low value after 8 bits
  nop(10);
  pinMode(sda, OUTPUT);
  // The start let the SCL in low value
          for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
                  if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(sda, HIGH);
                    nop(10);
                    //Serial.print("1");
                  }
                  else{ //if bitwise and resolves to false
                    digitalWrite(sda, LOW);
                    nop(10);
                    //Serial.print("0");
                  }
                  
            //Note: The change of data must occurr while the SCL is LOW, only after the pulse of SCL take place!
            digitalWrite(scl, HIGH);  // Generate a pulse to validation of data on bus.
            nop(10);
            digitalWrite(scl, LOW);   // Must stay low value after 8 bits
            nop(10);
          }
    
    /*
        for (int r=0; r<4; r++){
          digitalWrite(sda, LOW);
          nop(10);
          digitalWrite(sda, HIGH);
          nop(10);  
        }
     */
        pinMode(sda, INPUT);
}
unsigned char read_byte(){     //reading from EEPROM serially
 unsigned int i;
 int val = 0;      // variable to store the read value
 readVal=0;
 digitalWrite(scl, LOW);
 pinMode(sda, INPUT_PULLUP);
 nop(10);
        for(i=0;i<8;i++){
          readVal=readVal<<1;
          digitalWrite(scl, HIGH);
          nop(10);
          val = digitalRead(sda);
            if(val == 1){
            readVal++;}
       digitalWrite(scl, LOW);
       nop(10);   
        }
 pinMode(sda, OUTPUT);
  //Serial.print(" reead: ");  // This print lines do a delay to big... use it only to debug!
  //Serial.println(reead, BIN);// Only to debug
  return readVal;       //Returns 8 bit data here
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //pinMode(sda, INPUT_PULLUP);  // Need a study! This is necessary be a pin of input & output!!!
  //pinMode(scl, INPUT_PULLUP);  // Pull up active, when digitalWrite Low, he deactivate the pull up resistor
  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  //
  digitalWrite(sda, HIGH); // to activate the port
  digitalWrite(scl, HIGH); // to activate the port

  pinMode(13, OUTPUT);

 //cli();           // disable all interrupts
  // initialize timer1 
  //noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
  TCNT1  = 0;
  // Use 62499 to generate a cycle of 1 sex 2 X 0.5Secs (16MHz / (2*256*(1+62449) = 0.5
  OCR1A = 62499;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  //interrupts();             // enable all interrupts

  //sei();             // enable all interrupts
}
// 
void switchOn(){
        start();
        send_byte(0x48);// Only after this cmd it starting show something.
        readAcknowledge();
        send_byte(0x01); // 
        readAcknowledge();
        stp();
}
void switchOff(){
        start();
        send_byte(0x48);// Only after this cmd it starting show something.
        readAcknowledge();
        send_byte(0x00); // 
        readAcknowledge();
        stp();
}
void testBright(){
  //This allow send the value from 0x10 to 0x70 and call function of bright with a vlue!
  for(uint8_t i = 0x00; i < 8; i++){
  uint8_t br = 0x00;
  br = i;
  br = br << 4;
  bright(br);
  Serial.println(br, HEX);
  delay(500);
  }
}
void bright(uint8_t intensity){
  // Definition of bright intensity is defined by 0xB1 where B take value of 1 to 7. The word 1 is LED 1.
        start();
        send_byte(0x48);// Only after this cmd it starting show something.
        readAcknowledge();
        send_byte((0x01 | intensity)); // 
        readAcknowledge();
        stp();

}
void write4digits(){
  for (uint8_t i = 0x00; i<16; i++){
  char ch0,ch1,ch2,ch3 = 0x00;
  ch0 = letters[i];  //This get the position of the table with letters! The first 10 is numbers.
        start();
        send_byte(0x68);// 
        readAcknowledge();
        send_byte(ch0); //
        start();
        send_byte(ch0); //
        start();
        send_byte(ch0); //
        start();
        send_byte(ch0); //
        start();
        send_byte(ch0); //
        readAcknowledge();
        stp();
        delay(800);
  }
}
void msg(){
  for (uint8_t i = 0x00; i<16; i++){
  char ch0,ch1,ch2,ch3 = 0x00;
  ch0 = msgHello[i];  //This get the position of the table with letters! The first 10 is numbers.
  ch1 = msgHello[i+1];
  ch2 = msgHello[i+2];
  ch3 = msgHello[i+3];
        start();
        send_byte(0x68);// 
        readAcknowledge();
        send_byte(ch0); //
        start();
        send_byte(ch1); //
        start();
        send_byte(ch2); //
        start();
        send_byte(ch3); //
        readAcknowledge();
        stp();
        delay(800);
  }
}
void readKeys(){
   start();
      send_byte(0x4F);// 
      readAcknowledge();
          for(int s=0; s<2; s++){
             arr[s]=read_byte();
              if (s==1){
              noAcknowledge();
              }
              else{
              sendAcknowledge();
              }
          }
    stp();
  delay(30);
        for (int n=0; n< 4; n++){
          Serial.print(arr[n],HEX);
        }
  Serial.println("Read keys:");
  delay(10);
}
/*************************************************************************/
void loop() {
  // put your main code here, to run repeatedly:
  unsigned char second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  int val = 0;      // variable to store the read value
  unsigned char info=0;
  unsigned int s=0;
  unsigned int t=0;
  unsigned char chrVar=0;
  //temp=0;

    // Serial.println(PIN_WIRE_SDA, DEC); // This outputs '22'
    // Serial.println(PIN_WIRE_SCL, DEC); // This outputs '23'

    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    switchOff();
    switchOn();

    for(uint8_t i = 0x00; i < 3; i++){
      switchOn();
      delay(400);
      switchOff();
      delay(400);
    }
    switchOn();
    testBright();
    msg();
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    write4digits();
    readKeys();   
}

ISR(TIMER1_COMPA_vect)   {    //This is the interrupt request
                            // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
     // this timer is to avoid read many times the RTC... read one time by second... avoid load at uC
     digitalWrite(13, !digitalRead(13));
     counter++;
} 
