#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

/*extern struct List * create_List(int (*control_func)(void *lhd, void *rhd)) {
	struct List *l = (struct List *) malloc (sizeof(struct List *));
	l -> control_func = control_func;
	l -> stds = NULL;
	l -> ends = NULL;
	return l;
}*/

extern void add(struct List *l, void *matchindata, void *newdata, int pos_type) {
	struct data_structure *newnode = (struct data_structure *) malloc (sizeof(struct data_structure *));
	newnode -> data = newdata;
	struct data_structure *tmpn = NULL, *ptmp = NULL; 
	int ok = 0;
	void *mmdata = (matchindata == NULL) ? newdata : matchindata;
	if(l -> stds == NULL) {
		l -> stds = l -> ends = newnode;
		newnode -> nextEntry = NULL;
	} else {
		switch(pos_type) {
			case START:
				newnode -> nextEntry = l -> stds;
				l -> stds = newnode;
				break;
			case END:
				newnode -> nextEntry = NULL;
				l -> ends -> nextEntry = newnode;
				l -> ends = newnode;
				break;
			default:
				tmpn = l -> stds;
				while(tmpn != NULL) {
					if(l -> control_func(tmpn -> data, mmdata)) {
						ok = 1;
						break;
					}
					ptmp = tmpn;
					tmpn = tmpn -> nextEntry;
				}
				if(!ok) {
					newnode -> nextEntry = NULL;
					l -> ends -> nextEntry = newnode;
					l -> ends = newnode;
					return;
				};
				if(pos_type == BEFORE) {
					if(ptmp == NULL) {
						newnode -> nextEntry = l -> stds;
						l -> stds = newnode;
					} else {
						newnode -> nextEntry = tmpn;
						ptmp -> nextEntry = newnode;
					}
				} else {
					newnode -> nextEntry = tmpn -> nextEntry;
					tmpn -> nextEntry = newnode;
					if(tmpn == l -> ends) {
						l -> ends = newnode;
					}
				}
				break;
		}
	}
}

extern void del(struct List *l, void *matchindata) {
	struct data_structure *ds = l -> stds, *pds = NULL;
	int ok = 0;
	while(ds != NULL) {
		if(l -> control_func(ds -> data, matchindata)) {
			ok = 1;
			break;
		}
		pds = ds;
		ds = ds -> nextEntry;
	}
	if(!ok) return;
	if(ds == l -> stds) {
		l -> stds = ds -> nextEntry;
	} else if(ds == l -> ends) {
		pds -> nextEntry = NULL;
		l -> ends = pds;
	} else {
		pds -> nextEntry = ds -> nextEntry;
	}
	free(ds);
}

extern void * get(struct List *l, void *matchindata) {
	struct data_structure *ds = l -> stds;
	while(ds != NULL) {
		if(l -> control_func(ds -> data, matchindata) == 1) {
			return ds -> data;
		}
		ds = ds -> nextEntry;
	}
	return NULL;
}
