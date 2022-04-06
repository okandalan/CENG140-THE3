#include <stdio.h>
#include <stdlib.h>
#include "the3.h"
#define  NEXT(p) ((p)->next)
#define  PREV(p) ((p)->prev)
#define ID(p) ((p)->id)
#define EMPTY(p) ((p)->is_empty)
#define INITIALBW(p) ((p)->initial_bandwidth)
#define MAXBW(p) ((p)->max_bandwidth)
#define NAME(p)  ((p)->name)


int strcomp(char* str1, char* str2)
{
    while (*str1 == *str2)
    {
        if (*str1 == '\0' && *str2 == '\0') return 1;
        str1++;
        str2++;
    }
    return 0;
}

Flat* mkflat(int id, int initial_bandwidth)
{
    Flat * fl;

    fl = (Flat*) malloc(sizeof(Flat));
    if (fl)
    {
        ID(fl) = id;
        INITIALBW(fl) = initial_bandwidth;
        EMPTY(fl) = 0;
        NEXT(fl) = NULL;
        PREV(fl) = NULL;
    }
    return fl;
}

Apartment* mkapt(char * apartment_name, int max_bandwidth)
{
    Apartment* ap;

    ap = (Apartment*) malloc(sizeof(Apartment));
    if (ap)
    {
        ap->name = apartment_name;
        ap->max_bandwidth = max_bandwidth;
        NEXT(ap) = NULL;
        ap->flat_list = NULL;
    }
    else
        printf("There is not enough space.\n");
    return ap;
}

void free_list(Flat* head)
{
    Flat *temp;

    if (!head) free(head);

    while(head)
    {
        temp = head;
        head = NEXT(head);
        free(temp);
    }
}

void shift_flat_helper(Flat* merge_point, Flat** head, Flat* shifted_flat)
{
    if (merge_point == *head)
    {
        PREV(shifted_flat) = NULL;
        *head = shifted_flat;
        NEXT(*head) = merge_point;
        PREV(merge_point) = *head; 
    }
    else
    {
        NEXT(PREV(merge_point)) = shifted_flat;
        PREV(shifted_flat) = PREV(merge_point);
        NEXT(shifted_flat) = merge_point;
        PREV(merge_point) = shifted_flat;
    }
}

Apartment* add_apartment(Apartment* head, int index, char* apartment_name, int max_bandwidth)
{
    Apartment *temp, *new_apt;
    int i;

    new_apt = mkapt(apartment_name, max_bandwidth);
    temp = head;
    

    if (!head)
    {
        head = new_apt;
        NEXT(head) = new_apt;
        return head;
    }

    if (index == 0)
    {
        NEXT(new_apt) = head;
        for (; NEXT(temp) != head; temp = NEXT(temp));
        NEXT(temp) = new_apt;
        return new_apt;
    }
    for (i=0; i < index - 1; temp = NEXT(temp), i++);

    NEXT(new_apt) = NEXT(temp);
    NEXT(temp) = new_apt;
    return head;
        
}

void add_flat(Apartment* head, char* apartment_name, int index, int flat_id, int initial_bandwidth)
{
    Flat *temp, *new_flat;
    int i, sum = 0, diff;

    for (; !(strcomp(NAME(head), apartment_name)); head = NEXT(head));
    temp = head->flat_list;

    for (; temp; temp = NEXT(temp)) sum += INITIALBW(temp);

    diff = MAXBW(head) - sum;
    if (initial_bandwidth > diff) initial_bandwidth = diff;

    new_flat = mkflat(flat_id, initial_bandwidth);
    temp = head->flat_list;

    if (!temp)
    {
        head->flat_list = new_flat;
        return;
    }
    
    if (index == 0)
    {
        head->flat_list = new_flat;
        NEXT(new_flat) = temp;
        PREV(temp) = new_flat;
        return;
    }

    for (i=0; i < index - 1; temp = NEXT(temp), i++);

    PREV(new_flat) = temp;
    NEXT(new_flat) = NEXT(temp);
    if ((NEXT(temp)))
        PREV(NEXT(temp)) = new_flat;
    NEXT(temp) = new_flat;
    return; 
}

Apartment* remove_apartment(Apartment* head, char* apartment_name)
{
    Apartment *prev, *curr;
    
    prev = NULL;
    curr = head;

    if (NEXT(head) == head)
    {
        free_list(head->flat_list);
        free(head);
        return NULL;
    }

    for(; !(strcomp(NAME(curr), apartment_name)); prev = curr, curr = NEXT(curr));

    if (curr == head)
    {
        for (; NEXT(curr) != head; curr = NEXT(curr));
        NEXT(curr) = NEXT(head);
        free_list(head->flat_list);
        free(head);
        return NEXT(curr);
    }

    free_list(curr->flat_list);
    NEXT(prev) = NEXT(curr);
    free(curr);
    return head;
}

void make_flat_empty(Apartment* head, char* apartment_name, int flat_id)
{
    Flat * flat;

    for (; !(strcomp(NAME(head), apartment_name)); head = NEXT(head));
    flat = head->flat_list;
    for (; ID(flat) != flat_id; flat = NEXT(flat));
    EMPTY(flat) = 1;
    INITIALBW(flat) = 0;
}

int find_sum_of_max_bandwidths(Apartment* head)
{
    Apartment *curr = head;
    int sum = 0;
    
    if (!head) return 0;

    for (; NEXT(curr) != head; curr = NEXT(curr)) sum += MAXBW(curr);
    sum += MAXBW(curr);
    return sum;
}

Apartment* merge_two_apartments(Apartment* head, char* apartment_name_1, char* apartment_name_2)
{
    Apartment *apt1, *apt2;
    Flat *merge_point, *apt2_flats;
    apt1 = apt2 = head;

    for(; !(strcomp(NAME(apt1), apartment_name_1)); apt1 = NEXT(apt1));
    for(; !(strcomp(NAME(apt2), apartment_name_2)); apt2 = NEXT(apt2));

    MAXBW(apt1) += MAXBW(apt2);

    apt2_flats = apt2->flat_list;
    apt2->flat_list = NULL;
    head = remove_apartment(head, apartment_name_2);

    if (!apt2_flats) return head;

    merge_point = apt1->flat_list;
    if (!merge_point)
    {
        apt1->flat_list = apt2_flats;
        return head;
    }

    for(; NEXT(merge_point); merge_point = NEXT(merge_point));
    NEXT(merge_point) = apt2_flats;
    PREV(apt2_flats) = merge_point;
    return head;
}

void relocate_flats_to_same_apartment(Apartment* head, char* new_apartment_name, int flat_id_to_shift, int* flat_id_list)
{
    
    Flat *merge_point, *shifted_flat, *apt_flat_list, *curr2;
    Apartment *curr, *new_apt;
    int i, length, shift;
    new_apt = head;

    
    for (; !(strcomp(NAME(new_apt), new_apartment_name)); new_apt = NEXT(new_apt));

    
    merge_point = new_apt->flat_list;
    for (; ID(merge_point) != flat_id_to_shift; merge_point = NEXT(merge_point));

    curr = head;
    for (; ; curr = NEXT(curr))
    {
        if (strcomp(NAME(curr), new_apartment_name)) 
        {
            if (NEXT(curr) == head) break;
            continue;
        }
        shifted_flat = curr->flat_list;
        for (; shifted_flat; )
        {
            shift = 0;
            for (i=0; flat_id_list[i] != 0; i++)
            {
                if (flat_id_list[i] == ID(shifted_flat))
                {
 

                    if (shifted_flat == curr->flat_list)
                    {
                        curr->flat_list = NEXT(curr->flat_list);
                        if (curr->flat_list)
                        {

                            PREV(curr->flat_list) = NULL;
                        }
                    }
                    else if (!NEXT(shifted_flat))
                    {
                        NEXT(PREV(shifted_flat)) = NULL;
                    }
                    else
                    {
                        PREV(NEXT(shifted_flat)) = PREV(shifted_flat);
                        NEXT(PREV(shifted_flat)) = NEXT(shifted_flat);
                    }
                    MAXBW(curr) -= INITIALBW(shifted_flat);
                    MAXBW(new_apt) += INITIALBW(shifted_flat);
                    curr2 = shifted_flat;
                    shifted_flat = NEXT(shifted_flat);

                    shift = 1;
                    shift_flat_helper(merge_point, &(new_apt->flat_list), curr2);
                    break;

                }
            }
            if (!shift) shifted_flat = NEXT(shifted_flat);
        }

        if (NEXT(curr) == head) break;
    }
    
    
    apt_flat_list = new_apt->flat_list;

    for (length=0; flat_id_list[length] != 0; length++);
    for (i=length - 1; i >= 0; i--)
    {
        for(; ID(apt_flat_list) != flat_id_list[i]; apt_flat_list = NEXT(apt_flat_list)); 

        if (NEXT(apt_flat_list) == merge_point)
        {
            apt_flat_list = new_apt->flat_list;
            merge_point = PREV(merge_point);
            continue;
        } 
        else if (apt_flat_list == new_apt->flat_list)
        {
            new_apt->flat_list = NEXT(apt_flat_list);
            PREV(new_apt->flat_list) = NULL;

        }
        else
        {
            NEXT(PREV(apt_flat_list)) = NEXT(apt_flat_list);
            PREV(NEXT(apt_flat_list)) = PREV(apt_flat_list);
        }
        NEXT(apt_flat_list) = merge_point;
        PREV(apt_flat_list) = PREV(merge_point);
        NEXT(PREV(merge_point)) = apt_flat_list;
        PREV(merge_point) = apt_flat_list;
        apt_flat_list = new_apt->flat_list;
        merge_point = PREV(merge_point);
    }

}

