//---------------------------------------------------------------------------------------------------------------
// Logs all traffic on CAN busses to SDCard
// MIT license ("free" as in "free")

//---------------------------------------------------------------------------------------------------------------
// System libraries
#include <stdio.h> // snprintf

// Macchina libraries
#include "variant.h"
#include <due_can.h>
#include <Arduino_Due_SD_HSMCI.h> // This creates the object SD

//---------------------------------------------------------------------------------------------------------------
// Choose logging method    
//
#if 1 // 0=Binary, 1=Human-readable     <-------*************----****---******* [LOOK]
# define LOG logFrame
#else
# define LOG logFrameBin
#endif

//---------------------------------------------------------------------------------------------------------------
// Leave defined if you use native port, comment if using programming port
// This sketch could provide a lot of traffic so it might be best to use the native port
#define  Serial  SerialUSB

#define  DIE  while (1)  delay(1000)

#define  NL  "\r\n"  // DOS/Windoze="\r\n",  *nix="\n"

//---------------------------------------------------------------------------------------------------------------
FileStore fs;

#define  FN_PRE    "can2sd_"
#define  FN_MAX    (99999)
#define  FN_MAX_N  5 // 99999 is 5 digits long
#define  FN_EXT    ".log"
char  fname[sizeof(FN_PRE) + FN_MAX_N + sizeof(FN_EXT) + 1];

//---------------------------------------------------------------------------------------------------------------
typedef
  enum canDir {
    CDIR_IN  = 0,
    CDIR_OUT = 1,
  }
canDir_t;

#define  FLG_EXT  (0x80)
#define  FLG_RTR  (0x10)
#define  FLG_BUS  (0x01)
#define  FLG_OUT  (0x08)

//---------------------------------------------------------------------------------------------------------------
// Write a string to the log file
//
inline 
void  logs (const char* s)
{
  if (LOG == logFrame) {
    fs.Write(s);
    fs.Flush();
  }
  
#if 1
  Serial.print(s);
#endif
}

//---------------------------------------------------------------------------------------------------------------
// 0         1         2         3         4         5         6         7         8         9        10        11        12
// 0123456789*123456789*123456789*123456789*123456789*123456789*123456789*123456789*123456789*123456789*123456789*123456789*
//                                                                              |           |
// > 0 : ts=0x87654321_65535,  (A)ID_xxx = 0x1FFFFFFF,  FID = 0x1FFFFFFF,  data.req = 8     |
// > 0 : ts=0x87654321_65535,  (A)ID_std = 0x     7FF,  FID = 0x     7FF,  data.yyy[8] = 0x{00, 00, 00, 00, 00, 00, 00, 00}
// > 0 : ts=0x87654321_65535,  (A)ID_ext = 0x1FFFFFFF,  FID = 0x1FFFFFFF,  data.yyy[8] = 0x{00, 00, 00, 00, 00, 00, 00, 00}
//                                   |                                          |
//                                   std|ext                                    xmt|rcv
//
void  logFrame (uint32_t mS,  canDir_t dir,  int canid,  CAN_FRAME* frame) 
{
  char  s[124];
  int   idlen = frame->extended ? 8 : 3 ;

  if (!frame) {
    sprintf(s, "%c %d : ts=0x%08lX_%05d - packet lost",
            (dir == CDIR_IN) ? '>' : '<',  canid,
            mS, /*MTIMESTAMP*/frame->time
           );
    logs(s);
    return;
  }

  // Yeah, I realise this won't make a lot of sense to many people; google "printf format strings" 
  sprintf(s, "%c %d : ts=0x%08lX_%05d,  (A)ID_%s = 0x%*s%0*lX,  FID = 0x%*s%0*lX,  data.",
          (dir == CDIR_IN) ? '>' : '<',  canid,
          mS, /*MTIMESTAMP*/frame->time, 
          /*MIDE*/frame->extended ? "ext" : "std",  /*padding*/8-idlen,"",  /*AID*/idlen,frame->id,
          /*padding*/8-idlen,"", /*FID*/idlen,frame->fid 
         );
         
  if (frame->rtr) {  // Data requested
    sprintf(s+77, "req = %d", /*DLC*/frame->length);
    
  } else {          // Data received/transmitted
    sprintf(s+77, "rcv[%d] = 0x{", /*DLC*/frame->length);
    for (int i = 0;  i < frame->length;  i++)
      sprintf(s+89+(i*4), "%02X%s", frame->data.byte[i], (i+1 == frame->length) ? "}" : ", ") ;
  }
  strcat(s, NL);
  logs(s);
}

//---------------------------------------------------------------------------------------------------------------
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
void  logFrameBin (uint32_t mS,  canDir_t dir,  int canid,  CAN_FRAME* frame) 
{
  uint8_t   buf[24];
  uint8_t*  bp    = buf;
  uint8_t   flags = (frame->extended ? FLG_EXT : 0x00) 
                  | (frame->rtr      ? FLG_RTR : 0x00)
                  | (canid           ? FLG_BUS : 0x00)
                  | (dir == CDIR_OUT ? FLG_OUT : 0x00) ;

  if (!frame) {
    return;  // just ignore lost frames for now
  }

  digitalWrite(LED_BUILTIN, HIGH);

  *bp++ = (mS       >> 24) & 0xFF   ; // 0
  *bp++ = (mS       >> 16) & 0xFF   ;     
  *bp++ = (mS       >>  8) & 0xFF   ;
  *bp++ = (mS            ) & 0xFF   ;

  *bp++ = (frame->time >> 8)        ;  // 4
  *bp++ = (frame->time     ) & 0xFF ;

  *bp++ = flags                     ;  // 6
  *bp++ = frame->length             ;  // 7

  *bp++ = (frame->id >> 24)         ;  // 8
  *bp++ = (frame->id >> 16) & 0xFF  ;
  *bp++ = (frame->id >>  8) & 0xFF  ;
  *bp++ = (frame->id      ) & 0xFF  ;

  *bp++ = (frame->fid >> 24)        ;  // 12
  *bp++ = (frame->fid >> 16) & 0xFF ;
  *bp++ = (frame->fid >>  8) & 0xFF ;
  *bp++ = (frame->fid      ) & 0xFF ;

#if 0
  if (flags & FLG_RTR) {              // 16..23
    (uint64_t*)*bp = 0x0000000000000000ull;
  } else {
    for (int i = 0;  i < frame.length;  i++)  *bp++ = frame->data.byte[i] ;
    for (         ;  i < 8           ;  i++)  *bp++ = 0x00                ;
  }
#else  
  //for (int i = 0;  i < 8;  i++)  *bp++ = frame->data.byte[i] ;
  *((uint64_t*)bp) = frame->data.value ;
#endif  
  //bp += 8;

  fs.Write((char*)buf, sizeof(buf));
  fs.Flush();
  
#if 0
  Serial.print(".");
#endif

  digitalWrite(LED_BUILTIN, LOW);
}

//---------------------------------------------------------------------------------------------------------------
void  loop (void)
{
  // https://arduino.stackexchange.com/questions/337/would-an-infinite-loop-inside-loop-perform-faster/339#339
  for (;;) {  
    CAN_FRAME  incoming;
    if (Can0.available())  LOG(millis(), CDIR_IN, 0, Can0.read(incoming) ? &incoming : NULL) ;
    if (Can1.available())  LOG(millis(), CDIR_IN, 1, Can1.read(incoming) ? &incoming : NULL) ;
    if (serialEventRun)  serialEventRun() ;
  }
}

