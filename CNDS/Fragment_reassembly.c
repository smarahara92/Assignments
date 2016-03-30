#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "List.h"


struct Fragment {
	unsigned long identification;
	unsigned long offset;
	int moreFragment;
	unsigned long length;
};

struct Fragments_info {
	unsigned long identification;
	unsigned long received_data_length;
	unsigned long total_data_length;
};

struct IP_table {
	unsigned long ip_addr;
	struct List *fragments_list;
};

int ip_matching_func(void *ldata, void *rdata) {
	struct IP_table *ld = (struct IP_table *) ldata;
	unsigned long *rd = (unsigned long *) rdata;
	if(ld -> ip_addr == *rd) {
		return 1;
	} else {
		return 0;
	}
}

int id_matching_func(void *ldata, void *rdata) {
	struct Fragments_info *ld = (struct Fragments_info *) ldata;
	unsigned long *rd = (unsigned long *) rdata;
	if(ld -> identification == *rd) {
		return 1;
	} else {
		return 0;
	}
}

struct List * create_List(int (*control_func)(void *ldata, void *rdata)) {
	struct List *l = (struct List *) malloc (sizeof(struct List *));
	l -> control_func = control_func;
	l -> stds = NULL;
	l -> ends = NULL;
}

void insert_new_fragment_info(struct IP_table *ip_table, struct Fragment *fragment) {
	struct Fragments_info *fi = (struct Fragments_info *) malloc (sizeof(struct Fragments_info *));
	fi -> identification = fragment -> identification;
	fi -> received_data_length = fragment -> length;
	if(fragment -> moreFragment == 0) {
		fi -> total_data_length = fragment -> offset + fragment -> length;
	} else {
		fi -> total_data_length = 0;
	}
	add(ip_table -> fragments_list, NULL, fi, START);
}

void add_new_ip_table(struct List *ip_list, unsigned long ip, struct Fragment *fragment) {
	struct IP_table *ipt = (struct IP_table *) malloc (sizeof(struct IP_table *));
	ipt -> ip_addr = ip;
	ipt -> fragments_list = create_List(&id_matching_func);
	add(ip_list, NULL, ipt, START);
	insert_new_fragment_info(ipt, fragment);
}

struct Fragments_info * get_Fragments_table(struct IP_table *fragl, unsigned long id) {
	return (struct Fragments_info *) get(fragl -> fragments_list, &id);
}

void delete_id_table(struct List *l, struct IP_table *ipl, unsigned long id) {
	del(ipl -> fragments_list, &id);
	if(l -> stds == NULL) {
		del(l, &ipl -> ip_addr);
	}
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("%s filename1 filename2\n", argv[0]);
		return 0;
	}
	struct List *ip_list = create_List(&ip_matching_func);
	FILE *fp = fopen(argv[1], "r");
	FILE *fp1 = fopen(argv[2], "w");
	char buff[100], ipp[30];
	struct Fragment fragmnt;
	unsigned long ip_addr;
	struct IP_table *iptable = NULL;
	struct Fragments_info *fragmnt_info = NULL;
	while((fgets(buff, 100, fp)) != NULL) {
		sscanf(buff, "%s%lu%lu%d%lu", ipp, &fragmnt.identification, &fragmnt.offset, &fragmnt.moreFragment, &fragmnt.length);
		ip_addr = inet_network(ipp);
		if(fragmnt.offset == 0 && fragmnt.moreFragment == 0) {
			fprintf(fp1, "%s\t%lu\t%lu\n", ipp, fragmnt.identification, fragmnt.length);
		} else {
			iptable = (struct IP_table *) get(ip_list, &ip_addr);
			if(iptable == NULL) {
				add_new_ip_table(ip_list, ip_addr, &fragmnt);
			} else {
				fragmnt_info = get_Fragments_table(iptable, fragmnt.identification);
				if(fragmnt_info == NULL) {
					insert_new_fragment_info(iptable, &fragmnt);
				} else {
					fragmnt_info -> received_data_length += fragmnt.length;
					if(fragmnt.moreFragment == 0) {
						fragmnt_info -> total_data_length = fragmnt.offset + fragmnt.length;
					}
					if(fragmnt_info -> total_data_length == fragmnt_info -> received_data_length) {
						fprintf(fp1, "%s\t%lu\t%lu\n", ipp, fragmnt_info -> identification, fragmnt_info-> total_data_length);
						delete_id_table(ip_list, iptable, fragmnt.identification);
					}
				}
			}
		}
	}
	struct data_structure *iplist = ip_list -> stds;
	struct data_structure *idlist;
	struct in_addr hip;
	while(iplist != NULL) {
		iptable = (struct IP_table *) iplist -> data;
		idlist = iptable -> fragments_list -> stds;
		hip.s_addr = htonl(iptable -> ip_addr);
		strcpy(buff, inet_ntoa(hip));
		while(idlist != NULL) {
			fragmnt_info = (struct Fragments_info *) idlist -> data;
			if(fragmnt_info -> total_data_length == 0) {
				fprintf(fp1, "%s\t%lu\t\"MISSING FRAGMENTS\"\n", buff, fragmnt_info -> identification);
			} else if(fragmnt_info -> total_data_length < fragmnt_info -> received_data_length) {
				fprintf(fp1, "%s\t%lu\t\"DUPLICATE FRAGMENTS\"\n", buff, fragmnt_info -> identification);
			} else {
				fprintf(fp1, "%s\t%lu\t\"MISMATCH IN LENGTH\"\n", buff, fragmnt_info -> identification);
			}
			idlist = idlist -> nextEntry;
		}
		iplist = iplist -> nextEntry;
	}
	fclose(fp);
	fclose(fp1);
	return 0;
}
