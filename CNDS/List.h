#ifndef _LIST_H

#define _LIST_H

#define START 23
#define BEFORE 24
#define AFTER 25
#define END 26


struct List {
	int (*control_func) (void *lhd, void *rhd);
	struct data_structure {
		void *data;
		struct data_structure *nextEntry;
	} *stds, *ends;
};

extern struct List * create(int (*control_func)(void *lhd, void *rhd));

extern void add(struct List *l, void *matchingdata, void *newdata, int pos_type);

extern void del(struct List *l, void *data);

extern void * get(struct List *, void *data);

#endif
