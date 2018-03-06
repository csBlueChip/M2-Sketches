//---------------------------------------------------------------------------------------------------------------
// This is here for reference only
//

#if 0 

typedef
  struct canFrame {
    // Defined in a strange order to help the compiler optimise data storage
    uint32_t  mS;       // mS - the CAN timer is too small, save the due timestamp as well!
    uint32_t  aid;      // Arbitration ID
    uint32_t  fid;      // Family ID
    uint8_t   data[8];  // Payload
    uint16_t  ts;       // Timestamp
    uint8_t   dlc;      // Length {0..8}
    uint8_t   pri;      // Priority {0..15} - Only used when transmitting frames
    bool      ext;      // Extended
    bool      rtr;      // Remote Tranmit Request
    uint8_t   bus;      // Can0 or Can1
    canDir_t  dir;      // CDIR_IN, CDIR_OUT
  }
canFrame_t;

//
// 0         1         2
// 0123456789*123456789*123 = 24
// MMMMTTGLIIIIFFFFDDDDDDDD
// |   | |||   |   |
// |   | |||   |   Data (unused bytes are "undefined" - to keep packets fixed-length) 
// |   | |||   Family ID
// |   | ||Arbitration ID (standard=11bits, extended=28bits)
// |   | |Length (bytes of data requested / bytes of payload)
// |   | Flags 0x80=ext, 0x10=rtr, 0x01=bus
// |   Timestamp (some kind of 16bit clock/counter in the controller)
// millis() from the Due (the controller timestamp really doesn't cut it for packet logs)
//
int  decodeFrameBin (canFrame_t* frame,  uint8_t* fp)
{
  frame->mS   = (*(fp+0) << 24) | (*(fp+1) << 16) | (*(fp+ 2) << 8) | *(fp+ 3);
  
  frame->bus  = (*(fp+6) & FLG_BUS) ? 1 : 0;
  frame->dir  = (*(fp+6) & FLG_OUT) ? CDIR_OUT : CDIR_IN;
  frame->ts   = (*(fp+4) << 8) | *(fp+5);
  
  frame->ext  = (*(fp+6) & FLG_EXT) ? 1 : 0;
  frame->aid  = (*(fp+ 8) << 24) | (*(fp+ 9) << 16) | (*(fp+10) << 8) | *(fp+11);
  frame->fid  = (*(fp+12) << 24) | (*(fp+13) << 16) | (*(fp+14) << 8) | *(fp+15);
  
  frame->rtr  = (*(fp+6) & FLG_RTR) ? 1 : 0;
  frame->dlc  = *(fp+7);
  for (int i = 0;  i < 8;  i++)  frame->data[i] = *(fp+16+i) ;

  return 24; // number of bytes consumed
}

/*
uint8_t     buffer[1000];
canFrame_t  decoded;
uint8_t*    packetPointer;

loadLogFileInToBuffer(buffer, "mylogfile");
packetPointer = buffer + (i * 20);
decodeFrameBin(&decoded, packetPointer);
printf("%08lX", decoded.aid);

 */

#endif

