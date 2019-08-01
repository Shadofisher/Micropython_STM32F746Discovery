In stmhal issue command  make BOARD=STM32F7DISC to build for STM32F746Discovery 

*//--OLD To debug I use SystemWorkbench for now. This will hopefilly move to STM32_IDE
Also, st-util must be run from linux command line to start the gdb-server 
--//*

Procedure to Debug in STMCubeIDE

1. Create a new ST MCU project
2. When selector tool comes up, slect board.
3. When selecting type of project select "empty". This will copy a minimal set of directories to the project.
4. Once created, delete the new directories
5. Go to project properties and select Buil/C++. Uncheck the "Build makeefile automatically" checkbox
6. Browse to directory where makefile resides and Appy
7. In environment setting creat BOARD and set it to STM32F7DISC.
8. At this stage the project can be built (a test.) If successful move on
9. Import files from Filesystem. The files to import are in the stmhal directory in micropython source tree. Import as virtual files and      folders. We don't want to copy them as the Makefile will build the wrong files
10.Build the project
11.Right click on the project tree and select "Debug As".
12 Goto debug configuration and select the option OpennOCD.
13.Should now be able to perform source level debugging


![testimage](https://github.com/Shadofisher/Micropython_STM32F746Discovery/tree/master/test.jpeg)
