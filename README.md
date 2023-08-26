# i2c_ProtocolAnalyzer_Arduino_Pico
I²C ProtocolAnalyzer using Arduino-Pico. 

Capture data output sample.  
There are capturing error on #4 and #7. Bit slip may be occured on those transactions :(

```
I2C protocol analyzer started
  transaction captureing depth    =     64
  transaction maximum byte length =    128
  memory size for data capturing  =  17152
[0] captureing 20 transactions
# 0 ( 1) : [S] 0x90-W[A] 0x00[A]
# 1 ( 0) : [R] 0x90-R[A]
# 2 ( 0) : [R] 0x90-W[A] [P]
# 3 ( 1) : [S] 0x90-W[A] 0x00[A]
# 4 ( 2) : [R] 0x92-W[A] 0x3A[A] 0x41[A] [P]
# 5 ( 1) : [S] 0x90-W[A] 0x00[N]
# 6 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x20[N] [P]
# 7 ( 1) : [S] 0x20-W[A] 0x00[N]
# 8 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x40[N] [P]
# 9 ( 1) : [S] 0x90-W[A] 0x00[A]
#10 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x40[N] [P]
#11 ( 1) : [S] 0x90-W[A] 0x00[A]
#12 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x40[N] [P]
#13 ( 1) : [S] 0x90-W[A] 0x00[A]
#14 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x20[N] [P]
#15 ( 1) : [S] 0x90-W[A] 0x00[A]
#16 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x60[N] [P]
#17 ( 1) : [S] 0x90-W[A] 0x00[A]
#18 ( 2) : [R] 0x90-R[A] 0x1D[A] 0x41[A] [P]
#19 ( 1) : [S] 0x90-W[A] 0x00[A]
```
