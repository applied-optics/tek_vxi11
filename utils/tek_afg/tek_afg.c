#include "../../library/tek_user.h"

#ifndef	BOOL
#define	BOOL	int
#endif
#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

BOOL    sc(const char*, const char*);
void printhelp(void);

int	main(int argc, char *argv[]) {

static char *device_ip="128.243.74.108";
CLINK	*clink;
int	i,channel,verbose=0;
float	arg;
char	cmd[256];

	clink = new CLINK; /* allocate some memory */


	if (tek_open(device_ip,clink) != 0) {
		printf("Quitting...\n");
		exit(2);
		}
	for(i=1;i<argc;i++){
		// this is an option so process as such
		if(argv[i][0]=='-'){
			if(argv[i][1]=='v'){
				verbose++;
				}
			if(argv[i][1]=='q'){
				verbose--;
				}
			if(argv[i][1]=='h'){
				printhelp();;
				}
			if(argv[i][1]=='d'){
				// assume next string is a direct command
				vxi11_send(clink, argv[++i]);
				}
			}
		else{
			// this isn't an option so it must be a command,
			if(verbose>0){
				printf("Processing: %s\n",argv[i]);
				}
			channel=atoi(argv[i]+2);
			arg=atof(argv[i]+4);
			if(verbose>0){
				printf("Channel = %d argument = %f\n",channel,arg);
				}
			switch(argv[i][0]){
				case 'E':
					sprintf(cmd, "OUTP%d:STAT %s",channel,argv[i]+4);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'O':
					sprintf(cmd, "SOUR%d:VOLT:LEV:IMM:OFFS %fV",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'A':
				case 'V':
					sprintf(cmd, "SOUR%d:VOLT:LEV:IMM:AMPL %fVPP",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'F':
					sprintf(cmd, "SOUR%d:FREQ:FIX %fHz",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'P':
					sprintf(cmd, "SOUR%d:PHAS:ADJ %fDEG",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				default:
					printf("Uknown command in \"normal\" mode\n");
				}
			}
		}
	tek_close(device_ip,clink);
	}

/* string compare (sc) function for parsing... ignore */
BOOL	sc(const char *con, const char *var){
	if(strcmp(con,var)==0) return TRUE;
	return FALSE;
	}
void printhelp(void){
	printf("Tektronix AFG 3252 control program\n");
	printf("GPL Matt 2008\n");
	printf("Usage: tek_afg [options] <commands>\n");
	printf("Options:\n");
	printf("-v increase verbosity, -q decrease verbosity\n");
	printf("-h help - this help page\n");
	printf("-d \"string\" - send string direct to AFG (see AFG SCPI manual)\n");	
	printf("Commands:\n");
	printf("E - enable channel eg E:1:ON or E:2:OFF\n");
	printf("O - Offset voltage eg O:1:0.5 (offset on channel 1 is 0.5V\n");
	printf("A - synonym for V\n");
	printf("V - set voltage eg V:1:2 (set PP voltage on channel 1 to 2V)\n");
	printf("F - set frequency eg F:2:10000 (set frequency on channel 2 to 10kHz\n");
	printf("P - set phase eg P:1:180 (set phase on channel 1 to 180 degrees)\n");
	}
