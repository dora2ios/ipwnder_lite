# s5l8960x DFU's bug
There is obviously a bug in A7(s5l8960x)'s DFU, so apnonce is not randomized.  
You can use the checkm8 exploit to find a generator for the "special nonce" that this DFU mode generates.  


## list
| apnonce | generator |
|---------|----------|
| 6b83f831a6305ae90d57a78ba8eb9d81e7a9058f | 0x4f9d565d1d6e7324 |
| ee4b7f9b2d7d41bfde4c8390734a83d63c2fe997 | 0xd4bb858015d4e361 |
| b803b8b5bbb727ebcd70f705f1f03eb45ceeca08 | 0xd497d3e13c7c99c0 |
| f5cce05e81a9be2ef66ec287f692ffdf20b13860 | 0xa1182dab36264eac |
| e20792a25245bf0af451d91864429497d6d38343 | 0x30c722debc9d6890 |
| 2890f2304faeb3ef156006b2ba5225d873ab1e9d | 0xb5bdb84effc8b032 |
| 7ce1657233867e988e1b48988ef98fc28ddf20f5 | 0x7849d37d6137b146 |
| 41234383e4311bca58b55807009f68c202d1a4f6 | 0x9f66d49541eefa34 |
| 1dedf288afea588e803be0737af7ae5ca87d107f | 0xb99e7dd315b59f74 |
| 8f760412c8653de657e8ea2352f706de2e9ca85c | 0x204f4c3387883ee5 |
| 7d7bdc28e5eca36dc5bc20c791850f110dc28269 | 0x0f38e5f9aba5d219 |
| b05a70468054cfe94251b34b58f28450054f1aa9 | 0x701061388c1f8697 |
| 778282f0cf6e5234446d88ebc5dcfde81f415b57 | 0xb711a0c3de8d97e7 |
| efd3060f847c13dbd79b955cc9761a76bb6b9653 | 0xb337acabd685069b |
| 03011429dca6e0e1e71cc99716f27d94131d8128 | 0x2de7406ade2dd5bf |
| 0a6b1027d0758d9f3699d8e40ce1189848190cc0 | 0x7e5ca15004b5b0eb |
| c067414bc17bc7d65dd036f56b81b522265ba79f | 0x25b82ced5e24a7ad |
| 728a82a4bf7246939ea5db839ca782604cd97511 | 0xd5b99d403ee04922 |
| 8e33f9b17ca3ce754f5e7b4674ff796bc481302f | 0xc862efb5468e23a7 |


## tinyumbrella's shsh nonce??
The shsh (ios<10) saved using tinyumbrella appears to have the following nonce (at least on A7?  
The generator for this nonce is as follows:
| apnonce | generator |
|---------|----------|
| da218206498ff76001c8ae2e224713c293c5056c | 0x3611ceec45414272 |

