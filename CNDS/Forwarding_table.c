#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "List.h"

struct Routring_Entry {
	unsigned long network_ip;
	unsigned long network_mask;
	unsigned long gateway;
	char interface[30];
};

extern struct List * create_List(int (*control_func)(void *lhd, void *rhd)) {
	struct List *l = (struct List *) malloc (sizeof(struct List *));
	l -> control_func = control_func;
	l -> stds = NULL;
	l -> ends = NULL;
	return l;
}

int control_func(void *ldata, void *rdata) {
	struct Routring_Entry *ld = (struct Routring_Entry *)ldata;
	struct Routring_Entry *rd = (struct Routring_Entry *)rdata;
	if(ld -> network_mask > rd -> network_mask) {
		return 0;
	} else {
		return 1;
	}
}

int control_func1(void *ldata, void *rdata) {
	struct Routring_Entry *ld = (struct Routring_Entry *)ldata;
	unsigned long *rd = (unsigned long *)rdata;
	if(ld -> network_ip == (ld -> network_mask & *rd)) {
		return 1;
	} else {
		return 0;
	}
}

struct List * createRoutingTable(char *routingtablepath) {
	if(access(routingtablepath, R_OK) != -1) {
		FILE *fp = fopen(routingtablepath, "r");
		char routringentry[60], nw[30], nm[30], gw[30], interface[30];
		struct Routring_Entry *re;
		struct List *l = create_List(&control_func);
		while((fgets(routringentry, 60, fp)) != NULL) {
			sscanf(routringentry, "%s%s%s%s", nw, nm, gw, interface);
			re = (struct Routring_Entry *) malloc (sizeof(struct Routring_Entry *));
			re -> network_ip = inet_network(nw);
			re -> network_mask = inet_network(nm);
			re -> gateway = inet_network(gw);
			strcpy(re -> interface, interface);
			add(l, NULL, re, BEFORE);
		}
		fclose(fp);
		return l;
	} else {
		perror("Error");
		return NULL;
	}
}

void getRoutes(char *destinationspath, char *outputt, struct List *l) {
	if((access(destinationspath, R_OK)) != -1) {
		FILE *fp1 = fopen(outputt, "w");
		FILE *fp = fopen(destinationspath, "r");
		char buff[101];
		unsigned long ip;
		struct Routring_Entry *re;
		struct in_addr nw, nm, gw;
		char net[30], netm[30], gwm[30];
		while((fgets(buff, 100, fp)) != NULL) {
			ip = inet_network(buff);
			re = (struct Routring_Entry *) get(l, &ip);
			if(re != NULL) {
				nw.s_addr = htonl(re -> network_ip);
				nm.s_addr = htonl(re -> network_mask);
				gw.s_addr = htonl(re -> gateway);
				strcpy(net, inet_ntoa(nw));
				strcpy(netm, inet_ntoa(nm));
				strcpy(gwm, inet_ntoa(gw));
				fprintf(fp1, "%s %s %s %s %s", net, netm, gwm, re -> interface, buff);
			}
		}
		fcloseall();
	} else {
		perror("Error");
	}
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("%s Routring_Table Destination_IP_Table Ouput\n", argv[0]);
	} else {
		struct List *l = createRoutingTable(argv[1]);
		l -> control_func = &control_func1;
		getRoutes(argv[2], argv[3], l);
	}
	return 0;
}
