//---------------------------------------------------------------------------------------------------------------
// Test logging speed
// MIT license ("free" as in "free")
//

//
// https://en.wikipedia.org/wiki/CAN_bus#Remote_frame
// 
// |                 |  Length*  |
// |   Frame Type    | Min | Max |
// +-----------------+-----+-----+
// | Standard Data   |  47 | 111 |
// | Extended Data   |  67 | 131 |
// | Standard Remote |  47 |  47 |
// | Extended Remote |  67 |  67 |
// | Error           |     |     |
// | Overload        |     |     |
// 
// * INCLUDING 3 bits of inter-frame space ; EXCLUDING Stuff Bits 
// 
// Naming convention:
//   bps .. Baudrate (in bits-per-second)
//   len .. Length of frame (in bits)
//   
//   fps .. frames per second
//   tpf .. Time per frame (in Seconds)
//   
//   fpm .. frames per millisecond
//   tms .. Time per frame (in mS)  [1 second = 1000mS]
// 
// Time for 1 frame:
//   fps = bps / len
//   tpf = 1 / fps
// 
//   fpm = fps / 1000
//   tms = tpf * 1000
// 
// Eg. How many Standard Data frames can be sent in 1mS at 250KbpS ?
//   fpm = (bps / len) / 1000
//       = (500000 / 47) / 1000
//       = 5.32 f/mS
// 
// Eg. How many Extended Remote frames can be sent in 1mS at 500KbpS ?
//   fpm = (bps / len) / 1000
//      = (500000 / 67) / 1000
//      = 7.46 f/mS
// ...So, on my test device (which is spewing out Extended Remote frames at full speed)
//    I  /should/  see (literally) 7-or-8 frames per mS
//    I /actually/ see (literally) 1-or-2

//---------------------------------------------------------------------------------------------------------------
// Macchina libraries
#include "variant.h"
#include <due_can.h>

//+==============================================================================================================
void  setup (void)
{
  uint32_t  i;

  // Start the Serial port at the highest speed we can
  SerialUSB.begin(115200);
  // Allow [5 seconds] for the user to attach a serial monitor
//for (i = millis();  (i + (5 * 1000) > millis()) && !SerialUSB;  delay(10)) ;
  while(!SerialUSB) ; // wait forever for the user

  // Initialise the CAN bus at our specified speed
  Can0.begin(CAN_BPS_500K);

  // The driver code (currently) steals mailbox #7 whenever it feels like it - Do NOT rely on it in your code!
  // NB. Mailboxes are numbered {0..7} so mailbox[0] is the first mailbox, and mailbox[7] is the eigth mailbox
  //     ...welcome to "big school".
# define MBOX_MAX  (CANMB_NUMBER - 1)  // this will (for the M2) resolve to the value: 7
  
  // Declare some standard and extentended Rx mailboxes
  for (i = 0 ;  i < 4;         i++)  Can0.setRXFilter(i, 0, 0, false) ;  // Mailbox#, MID, MAM, extended
  for (      ;  i < MBOX_MAX;  i++)  Can0.setRXFilter(i, 0, 0, true) ;

  // Spew something out to the serial port to let the user know the system is alive
  // This test program is speed-optimised, so it will NOT display "0 frames received"
  // Therefore, if the CAN bus is NOT running at the speed which we start the M2 [see above]
  //            then we will not see any output after this!
  SerialUSB.println("# m2bc_SniffTest Running...");
}

//+==============================================================================================================
void  loop (void)
{
  CAN_FRAME  incoming;   // Somewhere to store a frame
  
  uint32_t   oldms = 0;  // previous read of millis()
  int        cnt   = 0;  // Number of frames this mS

  // We will eschew the overhead of jumping in-and-out of main()
  // https://arduino.stackexchange.com/questions/337/would-an-infinite-loop-inside-loop-perform-faster/339#339
  for (;;) {  
    
    // irqlock()  ;  return #entries in rxRingBuffer
    if (Can0.available()) {

      // We NEED to read the mS timer
      // wiring.c : getTickCount()
      //     timetick.c : return _dwTickCount
      uint32_t  ms = millis();

      // This is the least amount of work we need to do per-frame
      // can_common.h : inline  uint32_t  read (CAN_FRAME &msg)  { return get_rx_buff(msg); }
      //     due_can.cpp : irqlock()  ;  removeFromRingBuffer(rxRing, msg) ? 1 : 0 
      //         due_can.cpp : memcpy(&msg, (const void*)&ring.buffer[ring.tail], sizeof(CAN_FRAME));
      Can0.read(incoming);

      // Have we ticked to a new mS?
      //   Yes - display, and the restart the counter 
      //   No  - just count this frame
      if (ms > oldms) {  
        SerialUSB.println(cnt);  // This is likely to be time-hungry, so only do it when required
        oldms = ms;    //
        cnt   = 1;     // We will have already read the 1st frame for this mS
      } else  cnt++ ;  // We found another frame this mS
      
    }//if (Can0.available())

    // This line is the only reason we need to keep jumping in-and-out of main()
    // Let's do it locally as a speed optimisation
    if (serialEventRun)  serialEventRun() ;
    
  }//for(;;)
  
}//loop()

