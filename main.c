#include "the3.h"
#include <stdio.h>
#include <stdlib.h>
#define  NEXT(p) ((p)->next)
#define  PREV(p) ((p)->prev)
#define ID(p) ((p)->id)
#define EMPTY(p) ((p)->is_empty)
#define INITIALBW(p) ((p)->initial_bandwidth)
#define MAXBW(p) ((p)->max_bandwidth)
#define NAME(p)  ((p)->name)

Flat* mkflat(int id, int initial_bandwidth);
Apartment* mkapt(char * apartment_name, int max_bandwidth);
void free_list(Flat* head);
void shift_flat_helper(Flat* merge_point, Flat** head, Flat* shifted_flat);



int main()
{
    Apartment *a, *b, *c, *head;
    Flat  *x, *y, *f1, *f2, *f3, *curr, *b1, *b2;
    int i, *list;
    list = malloc(sizeof(int) * 4);
    list[0] = 2; 
    list[1] = 12; 
    list[2] = 9; 


    a = mkapt("okan",100);
    b = mkapt("esin", 200);
    c = mkapt ("vedat",300);
    NEXT(a) = b;
    NEXT(b) = c;
    NEXT(c) = a;

    x = mkflat(1, 10);
    y = mkflat(2, 20);
    a->flat_list = x;
    b1 = mkflat(12, 45);
    b2 = mkflat(9, 55);
    b->flat_list = b1;
    f1 = mkflat(4, 5);
    f2 = mkflat(5, 15);
    f3 = mkflat(6, 25);
    c->flat_list = f1;
    NEXT(x) = y;
    NEXT(y) = NULL;
    PREV(y) = x;
    NEXT(b1) = b2;
    PREV(b2) = b1;
    NEXT(b2) = NULL;
    NEXT(f1) = f2;
    NEXT(f2) = f3;
    NEXT(f3) = NULL;
    PREV(f2) = f1;
    PREV(f3) = f2;    

    head = a;

    relocate_flats_to_same_apartment(a, "vedat", 5, list);
    for (i=0; i < 3; i++)
    {
        printf("%d\n", MAXBW(head));
        printf("%s\n",NAME(head));
        curr = head->flat_list;
        if (curr)
        {
            for (; NEXT(curr); curr = NEXT(curr))
            {
                printf("%d\n", ID(curr));
            }
            printf("%d\n", ID(curr));
            printf("**************\n");
            for (; curr; curr = PREV(curr)) 
                printf("%d\n", ID(curr));
            printf("**************\n");
        }
        head = NEXT(head);
        
    }

    return 0;
}