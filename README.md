# ipwnder_lite  
lightweight version of ipwnder  


## Support  
| chip | name |
|---------|----------|
| S5L8920 |  |
| S5L8922 |  |
| S5L8930 | Apple A4 |
| S5L8950 | Apple A6 |
| S5L8960 | Apple A7 |
| T7000 | Apple A8 |
| S8000 | Apple A9 |
| S8003 | Apple A9 |
| T8010 | Apple A10 |


## How to use?
```
git clone https://github.com/dora2-iOS/ipwnder_lite && cd ipwnder_lite
make [all/noA6]
```
- for iOS 14 environment, binary must be placed under `/usr/local/bin`.  

### cmd
```
ipwnder_lite [-p/-d/-e]  
```
- `-p`: enter pwndfu to allow unsigned images  
- `-d`: demote and enable jtag/swd (only checkm8 devices)  
- `-e`: enter pwndfu to allow unsigned images with eclipsa/checkra1n style (only a8/a9 devices)  


## Credit  
axi0mX: checkm8 exploit  
checkra1n team: checkra1n  
LinusHenze: rmsigcheck  
MatthewPierson: ipwndfu fork  
Cryptiiiic: t8010 support  
synackuk: s5l8950x support  
geohot: limera1n exploit  
0x7ff: eclipsa  

license: gplv3  
