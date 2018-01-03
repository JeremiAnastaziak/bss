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
int slaveSetup = 5;
byte functionTestSlave = 0x01;
byte functionSlaveId = 0x02;
int buffer[1];

void setup()
{
  frame[0] = 99;
  frame[1] = 180;
  frame[2] = 5; // id wezla
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
  radio.write( "Czesc tu borys", 14 ); 
  // Send the final one back. 
      
    for (int i=1; i<=255; i++) {
    radio.openWritingPipe(pipes[0]);  
    radio.openReadingPipe(1, pipes[1]);
    radio.stopListening();      // First, stop listening so we can talk  
      
      buffer[0] = i;
      radio.write(&buffer[0], sizeof(buffer));
      Serial.print("Message send with adress: " );
      Serial.println(buffer[0]);
      delay(500);

       radio.startListening();  
       radio.openWritingPipe(pipes[1]);  
       radio.openReadingPipe(1, pipes[0]);
       
       if(radio.available()){
        radio.read( &buffer[0], sizeof(buffer));
        Serial.println(buffer[0]);
       }
      
    }
    
 // Now, resume listening so we catch the next packets.
 
  //nodeTest();   
}

void runSlaveMode() {
  char slaveResponse[22];
  radio.startListening();
  if( radio.available()){
    Serial.println("wiadomosc odebrana:");// Variable for the received timestamp
    while (radio.available()) {      
      
      radio.startListening();
      radio.read( &buffer[0], sizeof(buffer));
      
      Serial.println("Message retrieved: ");
      Serial.println(buffer[0]);

      if( buffer[0] == frame[2]){
        Serial.println("to jest moje id");
        radio.openWritingPipe(pipes[0]);  
        radio.openReadingPipe(1, pipes[1]);
        radio.stopListening();      // First, stop listening so we can talk  
        radio.write( &frame[2], sizeof(frame[2]) );
        radio.startListening();  
        radio.openWritingPipe(pipes[1]);  
        radio.openReadingPipe(1, pipes[0]);// Now, resume listening so we catch the next packets. 
      }
      
    }    
    //Serial.println(slaveResponse); 
  }
}

void loop() {
  
};

void sendMessage(byte functionNumber, byte slaveId) {
  frame[1] = byte(slaveId);
  frame[3] = byte(functionNumber);
  radio.stopListening();
  radio.write(&frame, sizeof(frame));
  Serial.print("Message send with adress: " );
  Serial.print(frame[1]);
  Serial.println("");
}

void handleSlaveResponse() {
  char slaveResponse[22];
  radio.startListening();
  radio.read( &slaveResponse, sizeof(slaveResponse));
  Serial.println("Message retrieved: ");
  Serial.print(slaveResponse[3]);
  if(slaveResponse[1] == slaveSetup) {
    availableSlaves[0] = slaveResponse[1];
    radio.stopListening();
    radio.write(&frame, sizeof(frame));
    Serial.print("moj id to: " );
        Serial.print(slaveSetup);

  }
  
}


void testSlaveAddress() {
  for (int i=1; i++; i<=255) {
    sendMessage(functionTestSlave, i);
  }
}

void nodeTest(){
  for (int i=1; i<=255; i++) {
  //radio.stopListening();
    int buffer[1];
    buffer[0] = i;
    radio.write(&buffer[0], sizeof(buffer));
    Serial.print("Message send with adress: " );
    Serial.println(buffer[0]);
    delay(500);
    
  }
}

