#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(9, 10);

const uint64_t pipes[2] = {"NodeAA", "NodeBB"};
byte frame[22];
int node = 2;
int availableSlaves[256];
int programSetup = 0;
int masterSetup = 109;
int slaveSetup = 115;
byte functionTestSlave = 0x01;
byte functionSlaveId = 0x02;

void setup()
{
  frame[0] = 99;
  frame[1] = 180;
  frame[2] = node;
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipes[1]);  
  radio.openReadingPipe(1, pipes[0]);
  radio.setRetries(15, 15);
  radio.startListening();

  while(true) {
    if (Serial.available() > 0) {
      // read the incoming byte:
      programSetup = Serial.read();
  
      // say what you got:
      Serial.print("I received: ");
      Serial.println(programSetup, DEC);
      
      if (programSetup == masterSetup) {
         runMasterMode();
      } else {
        while(true){
          runSlaveMode();
        };
      }
    }
    
  }
}

void runMasterMode() {
  Serial.println("wyslano");
  radio.openWritingPipe(pipes[0]);  
  radio.openReadingPipe(1, pipes[1]);
  radio.stopListening();      // First, stop listening so we can talk  
  radio.write( "Czesc tu borys", 14 );              // Send the final one back.     
  radio.startListening();  
  radio.openWritingPipe(pipes[1]);  
  radio.openReadingPipe(1, pipes[0]);// Now, resume listening so we catch the next packets.     
}

void runSlaveMode() {
  char got_time[23];
  if( radio.available()){
    Serial.println("wiadomosc odebrana:");// Variable for the received timestamp
    while (radio.available()) {                                   // While there is data ready
      radio.read( &got_time, sizeof(got_time) );             // Get the payload
    }    
    Serial.println(got_time); 
  }
}

void loop() {
  
};


