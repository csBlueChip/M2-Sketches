# M2-Sketches
Sketches for the Macchina M2

# HowTo patch due_can.cpp to add RTR support

https://forum.macchina.cc/t/can-library-does-not-return-rtr-solved/610

* Open ```C:\Users\YourUserName\AppData\Local\Arduino15\packages\macchina\hardware\sam\0.2.1\libraries\due_can\due_can.cpp```<br/>
...If you're not using Windows - you're just going to have to go hunting for that file - sorry.

* Search for the string "fid" [Family ID] ...there's only one occurence of this string in this file:<br/>
```rxframe->fid    = m_pCan->CAN_MB[uc_index].CAN_MFID;```

* Just after/before/near ^that^, line add this line:<br/>
```rxframe->rtr    = (m_pCan->CAN_MB[uc_index].CAN_MSR & CAN_MSR_MRTR) ? 1 : 0 ;```

* That's it, you can now identify Remote Transmit Request packets with something like:<br/>
```if (frame.rtr)  rtrHandler(frame) ;```<br/>
...See ```m2bc_RtrReply``` sketch for an example

* If you like diff files, enjoy this:
```
--- C:\original\due_can.cpp	2018-03-03 15:35:22.000000000 -0000
+++ C:\rtr_fix\due_can.cpp	2018-03-03 15:38:02.000000000 -0000
@@ -1026,12 +1026,13 @@
 		rxframe->id = (ul_id  >> CAN_MID_MIDvA_Pos) & 0x7ffu;
 		rxframe->extended = false;
 	}
 	rxframe->fid = m_pCan->CAN_MB[uc_index].CAN_MFID;
 	rxframe->length = (ul_status & CAN_MSR_MDLC_Msk) >> CAN_MSR_MDLC_Pos;
     rxframe->time   = (ul_status & CAN_MSR_MTIMESTAMP_Msk);
+    rxframe->rtr    = (m_pCan->CAN_MB[uc_index].CAN_MSR & CAN_MSR_MRTR) ? 1 : 0;
     ul_datal = m_pCan->CAN_MB[uc_index].CAN_MDL;
 	ul_datah = m_pCan->CAN_MB[uc_index].CAN_MDH;
 
 	rxframe->data.high = ul_datah;
 	rxframe->data.low = ul_datal;```
