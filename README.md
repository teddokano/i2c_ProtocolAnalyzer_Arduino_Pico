# i2c_ProtocolAnalyzer_Arduino_Pico
I²C ProtocolAnalyzer using Arduino-Pico. 

> **Warning**  
> This is just a concept proofing code of I²C bus sniffer.  
> Since this code is using polling method to detect signals, capturing error may occur.  

> **Note**  
> This code is written to use a Raspberry Pi Pico with [Arduino-Pico](https://github.com/earlephilhower/arduino-pico)
> To setup the Arduino IDE, follow instruction of "Installing via Arduino Boards Manager" section in `README.md` of Arduino-Pico repository.  

> **Note**  
> To get best performance, choose optimization seting of "-O2" or "-O3".  
> This optimization setting gives stable performance to capture 400kHz I²C transfers. 
> ![optimization_setting.png](https://github.com/teddokano/bitbang_I2C_controller_Arduino_Pico/blob/main/reference/pic/optimization_setting.png)

Capture data output sample.  
Sniffed communication between Arduino UNO R3 I²C controller and LM75B (temperature sensor) as target. 

```
[1159] captureing 10 transactions
#11590 ( 1) : [S] 0x90-W[A] 0x00[A]
#11591 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11592 ( 1) : [S] 0x90-W[A] 0x00[A]
#11593 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11594 ( 1) : [S] 0x90-W[A] 0x00[A]
#11595 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11596 ( 1) : [S] 0x90-W[A] 0x00[A]
#11597 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11598 ( 1) : [S] 0x90-W[A] 0x00[A]
#11599 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
[1160] captureing 10 transactions
#11600 ( 1) : [S] 0x90-W[A] 0x00[A]
#11601 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11602 ( 1) : [S] 0x90-W[A] 0x00[A]
#11603 ( 2) : [R] 0x90-R[A] 0x1C[A] 0xE0[N] [P]
#11604 ( 1) : [S] 0x90-W[A] 0x00[A]
#11605 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x00[N] [P]
#11606 ( 1) : [S] 0x90-W[A] 0x00[A]
#11607 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x00[N] [P]
#11608 ( 1) : [S] 0x90-W[A] 0x00[A]
#11609 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x00[N] [P]
```
