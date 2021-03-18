# PCFG_C_Flex
The C-analyze folder contains the codes for analyzing the C code and automatic run IDAPython script in PCFG_IDAPython.

Notice that you should obtain data from source code and binary before analyzing it.
***gcc-E-linux.c_*** file can help you exact data from Linux.

## 
These codes are based on flex2.5.35 and general C language compiler. 
You can directly create a new project based on the C code and then run it.
You can recompile the ***\*.l*** file in your computer before. 

##Paraments
The first parameter indicates the path you want to save the file to.Like ***""***
The second parameter indicates the program name you want to analyze. ***""***
The following parameters indicate the running mode of program. Like ***"-c -x86-O0 -x86-O1 -x86-O2 -x86-O3 -mips-O0 -mips-O1 -mips-O2 -mips-O3"***