// DMX Receiver
// DMX Channel 1,2,3 are copy onto RGB pin of the Arduino
// PWM are from 0-255 on those pins.

#define myubrr (16000000L/16/250000-1)

// The only available pin for PWM are: 3,5,6,9,10,11
#define pin_RED 11
#define pin_GREEN 5
#define pin_BLUE 6
#define max_DMX 25 // Maximum DMX Channel that this module listen to
volatile unsigned char DMX[max_DMX]; 
volatile unsigned int DMXChannel=0;

ISR(USART_RX_vect)
{
  // Interrupt routine when the Arduino receive a serial caracter
  // Or if there is a BREAK (bad Caracter), This is how DMX reset the sequence
  char myUCSR0A,myUDR0;
  myUCSR0A = UCSR0A;
  myUDR0 = UDR0&0xFF; // USART Data Register
  
  // Break So reset the Sequence
  // FE0 is a Framing Error bit in UCSR0A
  // DOR0 is a Data Overrun bit in UCSR0A
  if ((myUCSR0A & (1<<FE0))||myUCSR0A & (1<<DOR0)) 
  {
    DMXChannel = 0;
    return;
  }
  else if (DMXChannel<max_DMX) // Maximum DMX Channel that this module fill the data into the array
  {
    DMX[DMXChannel++]=myUDR0; // Read the Data value Received from DMX
  }
}

void setup()
{
  pinMode(0,INPUT); //DMX Serial IN for now it is Hardcoded
  pinMode(pin_BLUE,OUTPUT); //BLUE
  pinMode(pin_GREEN,OUTPUT); //GREEN
  pinMode(pin_RED,OUTPUT); //RED
  
  // Baud rate set to 250KBaud https://en.wikipedia.org/wiki/DMX512#Protocol
  // Setting depends on Crystal of the Arduino hence the myubbr #define
  
  // UBRR USART Baud Rate Register
  // 12 bit on 2 register
  UBRR0H = (unsigned char)(myubrr>>8); //High part, only bit 0-3
  UBRR0L = (unsigned char)myubrr; //Low part
  UCSR0B |= ((1<<RXEN0)|(1<<RXCIE0));//Enable Receiver and Interrupt RX
  UCSR0C |= (3<<UCSZ00);//N81 No parity/8 bits/1 Stop bit
  // https://en.wikipedia.org/wiki/DMX512#Protocol
}

void loop()
{
    analogWrite(pin_RED,255-DMX[1]);
    analogWrite(pin_GREEN,255-DMX[2]);
    analogWrite(pin_BLUE,255-DMX[3]);
}
