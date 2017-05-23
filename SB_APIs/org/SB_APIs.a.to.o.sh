#!/bin/bash
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-ar -t SB_APIs.a
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-ar -xv SB_APIs.a SB_Serial.o
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-ar -xv SB_APIs.a SB_Network.o
/cygdrive/c/cygwin/opt/lemonix/cdt/bin/arm-linux-ar -xv SB_APIs.a SB_System.o
