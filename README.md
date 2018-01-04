# HT1621-LCD-on-STM8S
Sample code to use HT1621 LCD driver with STM8S processors

The STM8S processor is a small, simple, low power and cheap processor. Especially the small footprint package variants such as STM8S103F3P6 are great because they do not require external components to run (well, one capacitor), they are easily programmed, and much free software exists for development such as SDCC. They are well suited for simple measurement applications where one sensor must be interfaced to serial communication or a display.

Although character or even graphic LCD displays that are simple to connect to microprocessors, for low power applications they are overkill. They may require high bias voltage (5V for the ubiquitous HD44780 2x16 modules for example), or may require much current to operate. In many cases, if one variable will be displayed, a simple 7 segment display is sufficient. One such display controller is the HT1621, and many LCD displays can be found for it from China. In this project, I used this one, from DX.com, written for Arduino:

http://www.dx.com/p/2-4-inch-6-digit-7-segment-lcd-display-module-blue-backlit-for-arduino-435834

I modified the library heavily for ARM STM32F103. I added other features such as blanking (removing left hand zeros), and preparing the LCD variables in a structure to make them more convenient. Later, I ported the library again, this time for STM8S, and here I share it with you. This version is for Linux, but straightforward to port to Windows etc.

The sample code is in the file 'HT1621Demo.c'. The file sets up the processor and peripherals, and initializes the LCD module. Then it makes an ADC measurement once every 128ms, displays the result, and sleeps until the next measurement. Sample commented out code shows how you can display different values, negative numbers, and also manipulate the included battery level meter. You can modify it easily for your requirements. You could also hook up several displays and manipulate each differently (with a few modifications of the pin manipulation functions)

This system uses only about 900uA (microamperes) continuously while doing actual ADC conversions 8 times a second.

The code is designed for SDCC compiler, and STM supplied Standard Peripheral Libraries. I have a writeup to describe how to set this up on your computer:

https://aviatorahmet.blogspot.com.tr/2018/01/programming-stm8s-using-sdcc-and-gnu.html

To run it, make the connections (shown in HT1621.h file), connect the ST-Link V2 SWIM interfce pins, modify the paths defined in the two Makefiles (one is in the directory 'libs', and execute the command "make flash". This will compile the library files, compile the main file, and download the program to the microcontroller.

Please let me know if there are any difficulties or problems.
