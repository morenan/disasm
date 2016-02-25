  1: LL   0x000008 (D   8) # label2 # ra = uint(sp[8])
  2: LBL  0x000010 (D  16) # rb = uint(sp[16])
  3: BOUT 0x000000 (D   0)
  4: LEV  0x000000 (D   0)
  5: ENT  0xfffff8 (D  -8) # label4
  6: LI   0x000001 (D   1) # ra = uint(1)
  7: SG   0x000084 (D 132) # uint(gaddr[132]) = ra
  8: LL   0x000020 (D  32) # ra = uint(sp[32])
  9: SL   0x000004 (D   4) # uint(sp[4]) = ra
 10: JMP  0x000024 (D  36) # J to label1
 11: LL   0x000018 (D  24) # label3 # ra = uint(sp[24])
 12: SUBI 0xffffff (D  -1) # ra = ra - rb
 13: SL   0x000018 (D  24) # uint(sp[24]) = ra
 14: LXC  0xffffff (D  -1) # ra = char(v2p(ffffff06d))
 15: PSHA 0x000000 (D   0)
 16: LL   0x000018 (D  24) # ra = uint(sp[24])
 17: PSHA 0x000000 (D   0)
 18: JSR  0xffffb8 (D -72) # J to label2
 19: ENT  0x000010 (D  16)
 20: LL   0x000004 (D   4) # label1 # ra = uint(sp[4])
 21: SUBI 0x000001 (D   1) # ra = ra - rb
 22: SL   0x000004 (D   4) # uint(sp[4]) = ra
 23: ADDI 0x000001 (D   1) # ra = ra + rb
 24: BNZ  0xffffc8 (D -56) # B to label3
 25: LL   0x000004 (D   4) # ra = uint(sp[4])
 26: LEV  0x000008 (D   8)
 27: LEV  0x000008 (D   8)
 28: PSHI 0x00000a (D  10)
 29: LEAG 0x00001c (D  28) # ra = pc+28
 30: PSHA 0x000000 (D   0)
 31: PSHI 0x000001 (D   1)
 32: JSR  0xffff90 (D-112) # J to label4
 33: ENT  0x000018 (D  24)
 34: SG   0x000018 (D  24) # uint(gaddr[24]) = ra
 35: HALT 0x000000 (D   0)
 36: LEV  0x000000 (D   0)
