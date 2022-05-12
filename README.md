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
| S8000 | Apple A9 |
| S8003 | Apple A9 |
| T8010 | Apple A10 |
| T8011 | Apple A10X |
| T8015 | Apple A11 |


## How to use?
```
git clone https://github.com/dora2-iOS/ipwnder_lite --recursive && cd ipwnder_lite
make [all/noA6]
```
- for iOS 14 environment, binary must be placed under `/usr/local/bin`.  

### cmd
```
ipwnder_macosx -h
  Usage: ./ipwnder_macosx [option]
    -h, --help            show usage
    -l, --list            show list of supported devices
    -c, --cleandfu        use cleandfu [BETA]
    -d, --debug           enable debug log
    -j, --demote          enable jtag/swd
    -e, --eclipsa         use eclipsa/checkra1n style
```

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
