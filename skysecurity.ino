#define BAUDRATE 57600
#define DEBUGOUTPUT 0

#define buzzer  3
#define panico  4
#define sono  5
#define atencao 6
#define conexao 7

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0; 
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup() {

  pinMode(buzzer, OUTPUT);
  pinMode(panico, OUTPUT);
  pinMode(sono, OUTPUT);
  pinMode(atencao, OUTPUT);
  pinMode(conexao, OUTPUT);
 
  Serial.begin(BAUDRATE);           // USB

  delay(3000) ;
  Serial.write(byte(194)) ;



}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte() 
{
  int ByteRead;

  while(!Serial.available());
  ByteRead = Serial.read();

#if DEBUGOUTPUT  
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop() {


  // Look for sync bytes
  if(ReadOneByte() == 170) {
    if(ReadOneByte() == 170) {

      payloadLength = ReadOneByte();
      if(payloadLength > 169)                      //Payload length can not be greater than 169
          return;

      generatedChecksum = 0;        
      for(int i = 0; i < payloadLength; i++) {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }   

      checksum = ReadOneByte();                      //Read checksum byte from stream      
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

        if(checksum == generatedChecksum) {    

        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for(int i = 0; i < payloadLength; i++) {    // Parse the payload
          switch (payloadData[i]) {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;          
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        
        if(bigPacket) {
          if(poorQuality > 0)
          {
            digitalWrite(conexao, HIGH);
            delay(500);
            digitalWrite(conexao,LOW);
            delay(500);
          }
          else{
            digitalWrite(conexao, LOW);
          }
          Serial.print("Qualidade Ruim de conexão: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Atencao: ");
          Serial.print(attention, DEC);
          Serial.print(" Meditacao: ");
          Serial.print(meditation, DEC);
          

          Serial.print(" Tempo de resposta: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");

          verificacao();
        
        }
        
#endif        
        bigPacket = false;        
        
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}

void verificacao(){
          if(attention >= 0 && attention < 20 && poorQuality == 0){
            digitalWrite(atencao, HIGH);
            digitalWrite(buzzer, HIGH);          
          }
           else{
            digitalWrite(buzzer, LOW);
            digitalWrite(atencao, LOW);
            } 
            
         if(meditation >= 95 && poorQuality == 0){
           digitalWrite(sono, HIGH);
           digitalWrite(buzzer, HIGH);
          }                     
         else if (meditation < 95 && attention > 20){
           digitalWrite(sono,LOW);
           digitalWrite(buzzer,LOW);
          }
  
  }

