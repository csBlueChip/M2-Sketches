//---------------------------------------------------------------------------------------------------------------
// Reply to all CAN Bus RTR packets
// MIT license ("free" as in "free")

//---------------------------------------------------------------------------------------------------------------
// System libraries
#include <stdio.h>    // snprintf
#include <string.h>   // memcpy

// Macchina libraries
#include "variant.h"
#include <due_can.h>

//---------------------------------------------------------------------------------------------------------------
// Leave defined if you use native port ... Comment-out if using programming port
// This sketch could provide a lot of traffic so it might be best to use the native port
#define Serial SerialUSB

//---------------------------------------------------------------------------------------------------------------
typedef
  enum canDir {
    CDIR_IN,
    CDIR_OUT,
  }
canDir_t;

typedef
  enum rtrMode {
    // Control Flags
    RTR_LOOP = 0x0001,  
    RTR_INC  = 0x0002,
    // Modes
    RTR_AA   = 0x0010 | RTR_LOOP,
    RTR_RND  = 0x0020 | RTR_LOOP,
    RTR_BINC = 0x0030 | RTR_LOOP | RTR_INC,
    RTR_QINC = 0x0040            | RTR_INC,
  }
rtrMode_t;

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

  Serial.println("CAN Reply Engine Ready...");
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
void  replyData (CAN_FRAME &frame,  rtrMode mode)
{
  static  uint64_t  val = 0;
  
  if (mode & RTR_LOOP)
    for (int i = 0;  i < frame.length;  i++)
      switch (mode) {
        case RTR_AA   : frame.data.byte[i] = 0xAA;           break;  // 1: 1010'1010b ... Oscilloscope friendly
        case RTR_RND  : frame.data.byte[i] = rand() & 0xFF;  break;  // 2: ooohhh, fuzzy
        case RTR_BINC : frame.data.byte[i] = val;            break;  // 3: Incrementing byte value
        default:  break;
      }
  else if (mode == RTR_QINC)  memcpy(frame.data.byte, &val, 8) ;     // 4: One big, ever incrementing, little-endian number
  if      (mode & RTR_INC)  val++ ;
}

//---------------------------------------------------------------------------------------------------------------
void  doReply (int canid,  CAN_FRAME &frame)
{
  CAN_FRAME  reply;

  if (!frame.rtr)  return ;  // We only reply to RTR packets

  // Build an RTR reply packet
  memcpy(&reply, &frame, sizeof(reply));
  reply.priority = 0;        // {0=higher .. 15=lower} (CAN bus, remember?)
  reply.rtr      = 0;
  replyData(reply, RTR_AA);  // Choose: {RTR_AA, RTR_RND, RTR_BINC, RTR_QINC}

  printFrame(CDIR_OUT, canid, reply);  // Dump the packet to the console

  (canid == 0) ? Can0.sendFrame(reply) : Can1.sendFrame(reply) ;  // Trasmit the packet
}

//---------------------------------------------------------------------------------------------------------------
void  loop (void)
{
  CAN_FRAME incoming;

  while (Can0.available()) {             // While packets are available on CAN0
    if (Can0.read(incoming)) {           // Check packet has not magically disappeared
      printFrame(CDIR_IN, 0, incoming);  // Dump the packet to the console
      doReply(0, incoming);              // Consider replying to the packet
    } else {
      Serial.println("Error: Missing Message on Can0") ;
    }
  }

  while (Can1.available()) {             // While packets are available on CAN1
    if (Can1.read(incoming)) {           // Check packet has not magically disappeared
      printFrame(CDIR_IN, 1, incoming);  // Dump the packet to the console
      doReply(1, incoming);              // Consider replying to the packet
    } else {
      Serial.println("Error: Missing Message on Can1") ;
    }
  }

}

