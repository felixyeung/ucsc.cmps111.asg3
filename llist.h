#ifndef LLIST_H
#define LLIST_H

struct llist {
  int freespace; //amount of free space
  int blocksize; //size of block
  struct llist *next;
  struct llist *prev;
};

#endif