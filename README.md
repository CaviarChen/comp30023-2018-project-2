# TLS Certificate Checker

COMP30023 (Computer System) Project2

**Name** Zijun Chen  
**Student ID** 813190  
**Login Name** zijunc3  
***  

### Installation

    git clone git@github.com:CaviarChen/comp30023-2018-project-2.git
    cd comp30023-2018-project-2/
    make

### Usage
     ./certcheck [pathToCSVFile]

### Debug Mode
1. Change  

    #define DEBUG 0    
to  

    #define DEBUG 1
from file "debug_setting.h".
2. Recompile the program using

    make
