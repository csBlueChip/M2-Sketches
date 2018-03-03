//---------------------------------------------------------------------------------------------------------------
// Display all traffic on CAN busses
// MIT license ("free" as in "free")

//---------------------------------------------------------------------------------------------------------------
// System libraries
#include <stdio.h> // snprintf

// Macchina libraries
#include "variant.h"
#include <due_can.h>

//---------------------------------------------------------------------------------------------------------------
// Leave defined if you use native port, comment if using programming port
// This sketch could provide a lot of traffic so it might be best to use the native port
#define Serial SerialUSB

//---------------------------------------------------------------------------------------------------------------
typedef
  enum canDir {
    CDIR_IN,
    CDIR_OUT,
  }
canDir_t;

//---------------------------------------------------------------------------------------------------------------
void  setup (void)
{
  int  i = 0;

  // Start the serial port
  Serial.begin(115200);
  while (!Serial);  // If you print before the port is ready, it will never work
  
  // Initialize CAN0 and CAN1 & set the baud rates
  Can0.begin(CAN_BPS_500K);
  Can1.begin(CAN_BPS_500K);

  // Declare 4 standard and 4 extended mailboxes for each controller
  for (i = 0 ;  i < 4;  i++) {         // standard (0..3)
    Can0.setRXFilter(i, 0, 0, false);  // Mailbox#, MID, MAM, extended
    Can1.setRXFilter(i, 0, 0, false);
  }
  for ( ;  i < CANMB_NUMBER;  i++) {   // extended (4..7)
    Can0.setRXFilter(i, 0, 0, true);
    Can1.setRXFilter(i, 0, 0, true);
  }

  Serial.println("CAN Sniffer Ready...");
}

//---------------------------------------------------------------------------------------------------------------
void  printFrame (canDir_t dir,  int canid,  CAN_FRAME &frame) 
{
  char  s[128];
  int   idlen = frame.extended ? 8 : 3 ;

  // Yeah, I realise this won't make a lot of sense to many people; google "printf format strings" 
  snprintf(s, sizeof(s), "%c %d : ts = %5d,  (A)ID_%s = 0x%*s%0*lX,  FID = 0x%*s%0*lX",
           (dir == CDIR_IN) ? '>' : '<',  canid,
           /*MTIMESTAMP*/frame.time, 
           /*MIDE*/frame.extended ? "ext" : "std",  /*padding*/8-idlen,"",  /*AID*/idlen,frame.id,
           /*padding*/8-idlen,"", /*FID*/idlen,frame.fid ) ;
  Serial.print(s);
            
  Serial.print(",  data.");
  if (frame.rtr) {  // Data requested
    Serial.print("req = ");
    Serial.print(/*DLC*/frame.length);
    
  } else {          // Data received/transmitted
    Serial.print((dir == CDIR_IN) ? "rcv[" : "xmt[");
    Serial.print(/*DLC*/frame.length);
    Serial.print("] = 0x{");
    for (int i = 0;  i < frame.length;  i++) {
      Serial.print(frame.data.byte[i], HEX);
      if (i + 1 != frame.length)  Serial.print(", ") ;  // Next byte is not the last one? Print a comma
    }
    Serial.print("}");
  }

  Serial.println("");
}

//---------------------------------------------------------------------------------------------------------------
void  loop (void)
{
  CAN_FRAME incoming;

  while (Can0.available())               // While packets available
    if (Can0.read(incoming))             // Check packet has not magically disappeared
      printFrame(CDIR_IN, 0, incoming);  // Dump the packet
    else                      
      Serial.println("Error: Missing Message on Can0") ;
    
  while (Can1.available())
    if (Can1.read(incoming))  printFrame(CDIR_IN, 1, incoming) ;
    else                      Serial.println("Error: Missing Message on Can1") ;
}

