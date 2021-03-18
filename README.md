# PCFG_C_Flex
The PCFG_C_Flex can analyze the C code and automatic run IDAPython script in PCFG_IDAPython.

Notice that you should obtain data from source code and binary before analyzing it.

***gcc-E-linux.c*** file can help you exact data from Linux.

## How to run it
These codes are based on ***Windows*** system.

The ***\*.l file*** should be compiled by ***flex2.5.35*** 

The ***c style code*** can be compiled by general C language compiler like ***Visual Studio***.

You can directly create a new project based on the C code and then run it.

You can also recompile the ***\*.l file***  or ignore it. 

## Paraments
The first parameter indicates the path you want to save the file to.Like ***"C:\Users\nocan\Desktop\Data\"***

The second parameter indicates the program name you want to analyze. Like ***"openssl"***

The following parameters indicate the running mode of program. Like ***"-c -x86-O0 -x86-O1 -x86-O2 -x86-O3 -mips-O0 -mips-O1 -mips-O2 -mips-O3"***
