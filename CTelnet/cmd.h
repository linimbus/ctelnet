
#ifndef __CMD_h_
#define __CMD_h_

extern int CMD_Init(char * ini);

extern void CMD_RUN();

extern void CMD_Check(char * nbuf);

extern int CMD_Argx(int argc,char ** argv);

extern int CMD_Fini();



#endif

