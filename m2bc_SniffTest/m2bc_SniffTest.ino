//---------------------------------------------------------------------------------------------------------------
// Test logging speed
// MIT license ("free" as in "free")
//

//---------------------------------------------------------------------------------------------------------------
// System libraries

// Macchina libraries
#include "variant.h"
#include <due_can.h>

//---------------------------------------------------------------------------------------------------------------
#define  Serial  SerialUSB

//+==============================================================================================================
void  setup (void)
{
  uint32_t  i;
  
  Serial.begin(115200);
  for (i = millis();  (i + (5 * 1000) > millis()) && !Serial;  delay(10)) ;
  
  Can0.begin(CAN_BPS_500K);
  // Declare 4 standard, and 4 extended mailboxes for each controller
  for (i = 0 ;  i < 4;             i++)  Can0.setRXFilter(i, 0, 0, false) ;  // Mailbox#, MID, MAM, extended
  for (      ;  i < 7/*CANMB_NUMBER*/;  i++)  Can0.setRXFilter(i, 0, 0, true) ;
  
  Serial.println("# m2bc_SniffTest Running...");
}

//+==============================================================================================================
void  loop (void)
{
  CAN_FRAME  incoming;
  
  uint32_t   oldms = 0;
  int        cnt   = 0;
  
  // https://arduino.stackexchange.com/questions/337/would-an-infinite-loop-inside-loop-perform-faster/339#339
  for (;;) {  
    if (Can0.available()) {
      uint32_t  ms = millis();

      //  inline  uint32_t  read (CAN_FRAME &msg)  { return get_rx_buff(msg); }
      //      lock()  ;  removeFromRingBuffer(rxRing, msg) ? 1 : 0  ;  unlock()
      //          memcpy(&msg, (const void*)&ring.buffer[ring.tail], sizeof(CAN_FRAME));
      Can0.read(incoming);

      if (ms > oldms) {
        oldms = ms;
        cnt   = 1;
        Serial.println(); 
        Serial.print(cnt,HEX); 
      } else {
        cnt++;
        Serial.print(cnt,HEX); 
      }
    }
    
    if (serialEventRun)  serialEventRun() ;
  }
}

