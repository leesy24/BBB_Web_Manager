#!/bin/bash
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-objdump.exe -d -s -S SB_Network.o > SB_Network.s
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-objdump.exe -d -s -S SB_System.o > SB_System.o.s
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-objdump.exe -d -s -S SB_Serial.o > SB_Serial.o.s

