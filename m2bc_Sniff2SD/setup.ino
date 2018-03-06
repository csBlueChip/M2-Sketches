//---------------------------------------------------------------------------------------------------------------
// If the library debug output is annoying you (as it did me)...
// Open this file:
//   C:\Users\Username\AppData\Local\Arduino15\packages\macchina\hardware\sam\0.2.1\libraries\M2_SD_HSMCI\src\Arduino_Due_SD_HSMCI.cpp
// At the top of this file ... comment out ALL FOUR of the debug functions ... and add this line
//   #define Debug(...)
// ...The "#" character MUST be in the left-most column of the source code (or some compilers will get upset)
//

//---------------------------------------------------------------------------------------------------------------
void canSetSpeed(int id,  CAN_COMMON* bus,  uint32_t defaultSpeed)
{
  uint32_t  speed;
  char      s[40];

#if 0
  bus->begin(defaultSpeed);
  sprintf(s, "Can%d.speed = %lu : ", id, speed);
  logs(s);
  
#else  // Autobaud
  bus->enable();

  sprintf(s, "Can%d.speed : ", id);
  logs(s);
  
  if ((speed = bus->beginAutoSpeed()) > 0) {
    sprintf(s, "Autobaud=%lu" NL, speed);
    logs(s);
    
  } else {
    bus->begin(defaultSpeed);
    sprintf(s, "Autobaud failed. Using speed=%lu" NL, defaultSpeed);
    logs(s);
  }  
#endif  
}

//--------------------------------------------------------------------
void  init_CAN (void) 
{
  int  i;
  
  // Initialize CAN0 and CAN1 & set the baud rates
  canSetSpeed(0, &Can0, CAN_BPS_500K);
  canSetSpeed(1, &Can1, CAN_BPS_500K);

  // Declare 4 standard, and 4 extended mailboxes for each controller
  for (i = 0 ;  i < 4;  i++) {         // standard (0..3)
    Can0.setRXFilter(i, 0, 0, false);  // Mailbox#, MID, MAM, extended
    Can1.setRXFilter(i, 0, 0, false);
  }
  for ( ;  i < CANMB_NUMBER;  i++) {   // extended (4..7)
    Can0.setRXFilter(i, 0, 0, true);
    Can1.setRXFilter(i, 0, 0, true);
  }
}

//--------------------------------------------------------------------
void  init_SD (void) 
{
  sd_mmc_err_t  sderr;
  int           i;
  
  // Mount the SD card
  SD.Init();
  // SD.Init() fails to return an error code, so I'm going to call sd_mmc_check() *again*
  if ((sderr = sd_mmc_check(0)) != SD_MMC_OK) {
    switch (sderr) {
      case SD_MMC_ERR_NO_CARD :  Serial.println("SD: No card");               break;
      case SD_MMC_ERR_UNUSABLE:  Serial.println("SD: Faulty card");           break;
      case SD_MMC_ERR_SLOT    :  Serial.println("SD: Uknown slot");           break;
      case SD_MMC_ERR_COMM    :  Serial.println("SD: Comms error");           break;
      case SD_MMC_ERR_PARAM   :  Serial.println("SD: Illegal Input Param");   break;
      case SD_MMC_ERR_WP      :  Serial.println("SD: Card is R/O");           break;
      default:  Serial.print("SD: Unknown Error =");  Serial.println(sderr);  break;
    }
    DIE;
  }
    
  // Given the failings of SD.Init(), we should probably also (re)check that the drive mounted OK
  // ...but I'm borded of fighting with badly written libraries for today!

  // Initialise the filing system
  fs.Init();  

  // Find an unused filename
  Serial.print("Creating log file : ");
  for (i = 1;  i <= FN_MAX;  i++) {
    snprintf(fname, sizeof(fname), "%s%0*d%s", FN_PRE, FN_MAX_N, i, FN_EXT);
    if (fs.Open(NULL, fname, false))  fs.Close() ;  // dir, name, write
    else                              break;
  }
  if (i > FN_MAX) {
    Serial.println("FS: File System full");
    DIE;
  }

  // Create the log file
  if (!fs.Open(NULL, fname, true)) {  // Open for write
    Serial.println("FS: Cannot create new log file");
    DIE;
  }
  
  Serial.println(fname);
}

//--------------------------------------------------------------------
void  init_Serial (void) 
{
  // Start the serial port
  Serial.begin(115200);

  // Give the user 5 second to connect to the USB/Serial port
  // If they connect after this time, they (apparently) will not see any data
  for (uint32_t i = millis();  (i + (5 * 1000) > millis()) && !Serial;  delay(10)) ;
}

//--------------------------------------------------------------------
void  setup (void)
{
  init_Serial();
  init_SD();  
  init_CAN();
  pinMode(LED_BUILTIN, OUTPUT);
  logs("# m2bc_Sniff2SD Running...\n");
}

