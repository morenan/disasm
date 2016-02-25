#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

char ops[] =
  "HALT,ENT ,LEV ,JMP ,JMPI,JSR ,JSRA,LEA ,LEAG,CYC ,MCPY,MCMP,MCHR,MSET," // system
  "LL  ,LLS ,LLH ,LLC ,LLB ,LLD ,LLF ,LG  ,LGS ,LGH ,LGC ,LGB ,LGD ,LGF ," // load a
  "LX  ,LXS ,LXH ,LXC ,LXB ,LXD ,LXF ,LI  ,LHI ,LIF ,"
  "LBL ,LBLS,LBLH,LBLC,LBLB,LBLD,LBLF,LBG ,LBGS,LBGH,LBGC,LBGB,LBGD,LBGF," // load b
  "LBX ,LBXS,LBXH,LBXC,LBXB,LBXD,LBXF,LBI ,LBHI,LBIF,LBA ,LBAD,"
  "SL  ,SLH ,SLB ,SLD ,SLF ,SG  ,SGH ,SGB ,SGD ,SGF ,"                     // store
  "SX  ,SXH ,SXB ,SXD ,SXF ,"
  "ADDF,SUBF,MULF,DIVF,"                                                   // arithmetic
  "ADD ,ADDI,ADDL,SUB ,SUBI,SUBL,MUL ,MULI,MULL,DIV ,DIVI,DIVL,"
  "DVU ,DVUI,DVUL,MOD ,MODI,MODL,MDU ,MDUI,MDUL,AND ,ANDI,ANDL,"
  "OR  ,ORI ,ORL ,XOR ,XORI,XORL,SHL ,SHLI,SHLL,SHR ,SHRI,SHRL,"
  "SRU ,SRUI,SRUL,EQ  ,EQF ,NE  ,NEF ,LT  ,LTU ,LTF ,GE  ,GEU ,GEF ,"      // logical
  "BZ  ,BZF ,BNZ ,BNZF,BE  ,BEF ,BNE ,BNEF,BLT ,BLTU,BLTF,BGE ,BGEU,BGEF," // conditional
  "CID ,CUD ,CDI ,CDU ,"                                                   // conversion
  "CLI ,STI ,RTI ,BIN ,BOUT,NOP ,SSP ,PSHA,PSHI,PSHF,PSHB,POPB,POPF,POPA," // misc
  "IVEC,PDIR,SPAG,TIME,LVAD,TRAP,LUSP,SUSP,LCL ,LCA ,PSHC,POPC,MSIZ,"
  "PSHG,POPG,NET1,NET2,NET3,NET4,NET5,NET6,NET7,NET8,NET9,"
  "POW ,ATN2,FABS,ATAN,LOG ,LOGT,EXP ,FLOR,CEIL,HYPO,SIN ,COS ,TAN ,ASIN," // math
  "ACOS,SINH,COSH,TANH,SQRT,FMOD,"
  "IDLE,";
 
struct Header {
	int magic, bss, entry, flags;
};

Header hdr;
int dat[65536];
int label[65536];
bool cmt = false;

int main(int argc, char** argv) {
	char *file_i, *file_o;
	for (int i = 1 ; i < argc ; i++) {
		if (!strcmp(argv[i], "-help")) {
			printf("disasm file_input -o file_output [-cmt]\n");
			printf("	-cmt : print comment\n");
		} else if (!strcmp(argv[i], "-cmt"))
			cmt = true;
		else if (!strcmp(argv[i], "-o"))
			file_o = argv[++i]; 
		else
			file_i = argv[i];
	}
	FILE* fi = fopen(file_i, "rb");
	FILE* fo = fopen(file_o, "wb");
	fread(&hdr, 1, sizeof(hdr), fi);
	printf("magic = 0x%08x\n", hdr.magic);
	printf("bss = %d\n", hdr.bss);
	int c_sz = (fread(&dat, 1, sizeof(dat), fi)>>2)-(hdr.bss>>1);
	int label_sz = 0, label_ct = 0;
	for (int i = 0 ; i < c_sz ; i++) {
		int inst = dat[i]&255;
		int imme = dat[i]>>8;
		if (ops[inst*5] == 'B' && ops[inst*5+1] != 'I' && ops[inst*5+1] != 'O') {
			printf("b to %d\n", i+(imme>>2)+1);
			label[i+(imme>>2)+1] = ++label_sz;
		}
		if (ops[inst*5] == 'J') {
			printf("j to %d\n", i+(imme>>2)+1);
			label[i+(imme>>2)+1] = ++label_sz;
		}
	}
	for (int i = 0 ; i < c_sz ; i++) {
		int inst = dat[i]&255;
		int imme = (int)(((unsigned int)(dat[i]))>>8);
		fprintf(fo, "%3d: %c%c%c%c 0x%06x (D%4d)",
			i+1, 
			ops[inst*5], ops[inst*5+1], ops[inst*5+2], ops[inst*5+3],
			imme, dat[i]>>8
		);
		if (i == hdr.entry>>2)
			fprintf(fo, " # <== entry ");
		if (label[i])
			fprintf(fo, " # label%d", label[i]);
		else
			fprintf(fo, "          ");
		if (ops[inst*5] == 'B' && ops[inst*5+1] != 'I' && ops[inst*5+1] != 'O')
			fprintf(fo, " # B to label%d", ++label_ct);
		else if (ops[inst*5] == 'J')
			fprintf(fo, " # J to label%d", ++label_ct);
		if (!cmt) {
			fprintf(fo, "\n"); continue;
		}
		if (inst >= 0x4f && inst <= 0x83) {
			if (inst-0x4f < 4) {
				switch (inst-0x4f) {
					case 0 : fprintf(fo, " # rf = rf + rg"); break;
					case 1 : fprintf(fo, " # rf = rf - rg"); break;
					case 2 : fprintf(fo, " # rf = rf * rg"); break;
					case 3 : fprintf(fo, " # rf = rf / rg"); break;
				}
			} else if ((inst-0x54)/3 < 13) {
				switch ((inst-0x54)/3) {
					case 0 : fprintf(fo, " # ra = ra +"); break;
					case 1 : fprintf(fo, " # ra = ra -"); break;
					case 2 : fprintf(fo, " # ra = int(ra) * "); break;
					case 3 : fprintf(fo, " # ra = int(ra) /"); break;
					case 4 : fprintf(fo, " # ra = uint(ra) /"); break;
					case 5 : fprintf(fo, " # ra = int(ra) %"); break;
					case 6 : fprintf(fo, " # ra = uint(ra) %"); break;
					case 7 : fprintf(fo, " # ra = ra &"); break;
					case 8 : fprintf(fo, " # ra = ra |"); break;
					case 9 : fprintf(fo, " # ra = ra ^"); break;
					case 10: fprintf(fo, " # ra = ra <<"); break;
					case 11: fprintf(fo, " # ra = int(ra) >>"); break;
					case 12: fprintf(fo, " # ra = uint(ra) >>"); break;
				}
				switch ((inst-0x54)%3) {
					case 0 : fprintf(fo, " rb"); break;
					case 1 : fprintf(fo, " %d", imme); break;
					case 2 : fprintf(fo, " sp[%d]", imme); break;
				}
			} else {
				switch (inst-0x7a) {
					case 0 : fprintf(fo, " # ra = (ra == rb)"); break;
					case 1 : fprintf(fo, " # ra = (rf == rg)"); break;
					case 2 : fprintf(fo, " # ra = (ra != rb)"); break;
					case 3 : fprintf(fo, " # ra = (rf != rg)"); break;
					case 4 : fprintf(fo, " # ra = (ra == rb)"); break;
					case 5 : fprintf(fo, " # ra = ((int)ra < (int)rb)"); break;
					case 6 : fprintf(fo, " # ra = ((uint)ra < (uint)rb)"); break;
					case 7 : fprintf(fo, " # ra = (f < g)"); break;
					case 8 : fprintf(fo, " # ra = ((int)ra < (int)rb)"); break;
					case 9 : fprintf(fo, " # ra = ((uint)ra < (uint)rb)"); break;
					case 10: fprintf(fo, " # ra = (f > g)"); break;
				}
			}
		}
		if (ops[inst*5] == 'L') {
			if (ops[inst*5+2] == 'V') {
				fprintf(fo, "\n"); continue;
			}
			if (ops[inst*5+1] == 'E') {
				if (ops[inst*5+3] == 'G') 
					fprintf(fo, " # ra = pc+%d\n", imme);
				else
					fprintf(fo, " # ra = sp+%d\n", imme);
				continue;
			}
			fprintf(fo, " # ");
			int opi = inst*5+1;
			if (ops[opi] == 'B') {
				fprintf(fo, "rb = ");
				opi++;
			} else
				fprintf(fo, "ra = ");
			switch (ops[opi+1]) {
				case ' ' : fprintf(fo, "uint(");  break;
				case 'S' : fprintf(fo, "short("); break;
				case 'H' : fprintf(fo, "ushort(");break;
				case 'C' : fprintf(fo, "char(");  break;
				case 'B' : fprintf(fo, "uchar("); break;
				case 'D' : fprintf(fo, "double(");break;
				case 'F' : fprintf(fo, "float("); break;
			}
			switch (ops[opi]) {
				case 'L' : fprintf(fo, "sp[%d])",    imme); break;
				case 'G' : fprintf(fo, "gaddr[%d])", imme); break;
				case 'X' : fprintf(fo, "v2p(%x06d))",imme); break;
				case 'I' : fprintf(fo, "%d)", 		 imme); break;
			}
		}
		if (ops[inst*5] == 'S') {
			if (ops[inst*5+1] == 'U') {
				fprintf(fo, "\n"); continue;
			}
			fprintf(fo, " # ");
			int opi = inst*5+1;
			if (ops[opi] == 'B') opi++;
			switch (ops[opi+1]) {
				case ' ' : fprintf(fo, "uint(");  break;
				case 'S' : fprintf(fo, "short("); break;
				case 'H' : fprintf(fo, "ushort(");break;
				case 'C' : fprintf(fo, "char(");  break;
				case 'B' : fprintf(fo, "uchar("); break;
				case 'D' : fprintf(fo, "double(");break;
				case 'F' : fprintf(fo, "float("); break;
			}
			switch (ops[opi]) {
				case 'L' : fprintf(fo, "sp[%d])",    imme); break;
				case 'G' : fprintf(fo, "gaddr[%d])", imme); break;
				case 'X' : fprintf(fo, "v2p(%x06d))",imme); break;
			}
			if (opi == inst*5+2)
				fprintf(fo, " = rb");
			else
				fprintf(fo, " = ra");
		}
		fprintf(fo, "\n");
	}
	return 0;
}


