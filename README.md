# bitmap-info-and-create
C and ASM - Get bitmap info colors and create a new one

- I will create a new bitmap that will be 2bpp (will contain 4 colors - the most used from color table)
- I will have a new color table with the colors from the old one; the table values will be sorted descending (e.g. to have the most used colors in the top table, colors that I will be using in the new bitmap)
- I'll loop the pixel array and for each index I will put from the new color table the color on 2 bytes.
  
-------------------------------------------
SPec:
352(10) = 0001 0110 0000(16) --> bh.bpp * bh.width (8*44)

0000 0010 0011 +    --> 0001 0110 0000 >> 3
0000 0000 0011      --> 3(10)
----------------
0000 0010 0110      --> (((bh.bpp * bh.width) >> 3) + 3)

0000 0000 0000 0000 0000 0000 0010 0110 &   --> (((bh.bpp * bh.width) >> 3) + 3)
1111 1111 1111 1111 1111 1111 1111 1100     --> mask with 0xFFFFFFFC
-----------------------------------------
1111 1111 1111 1111 1111 1111 1111 1110     --> FFFFFFFE


4294967292 & 4294967294

get last x bytes

1 DWORD (4bytes) = 2*WORD (2 bytes)

-------------------------------------------
