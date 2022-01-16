#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Order_ {
    unsigned int oid;
    unsigned int qty;
    char side;
    float price;
} Order;

typedef struct OrderLevel_ {
    Order *value;
    struct OrderLevel_ *next;
} OrderLevel;

typedef struct PriceLevel_ {
    OrderLevel *head;
    OrderLevel *tail;
    unsigned int size;
} PriceLevel;

PriceLevel* createLinkedList() {
    PriceLevel* tmp = (PriceLevel*) malloc(sizeof(PriceLevel));
    tmp->head = tmp->tail = NULL;
    tmp->size = 0;
    return tmp;
}

void push_back(PriceLevel *list, Order *value) {
    OrderLevel *tmp = (OrderLevel*) malloc(sizeof(OrderLevel));
    if (tmp == NULL) {
        printf("error malloc:");
        exit(EXIT_FAILURE);
    }
//    if(list->size > 0)
//        printf("stop");

    tmp->next = NULL;
    tmp->value = value;
    if(list->tail)
        list->tail->next = tmp;
    if (list->head == NULL) {
        list->head = tmp;
    }
    //if(list->tail == NULL)
        list->tail = tmp;
    list->size++;
//    if(list->size == 2 && list->head->next == NULL)
//        printf("stop");
}

Order* pop_front(PriceLevel *list) {
    OrderLevel *tmp = NULL;
    Order *value = NULL;
    if (list->head == NULL) {
        printf("error:");
    }

    tmp = list->head;
    value = tmp->value;
    list->head = list->head->next;
    if (list->tail == tmp) {
        list->tail = NULL;
    }
    free(tmp->value);
    free(tmp);
    list->size--;

    return value;
}

void delete_oid(PriceLevel *list, unsigned oid) {
    static int counter = 0;
    counter++;
    if(counter == 105)
        printf("stop");
    OrderLevel *ol = list->head;
    if(ol == NULL) {
        printf("stop");
        return;
    }
    if (ol->value->oid == oid) {
        pop_front(list);
        //return;
    } else {
        while(ol->next != NULL) {
            if(ol->next->value->oid == oid)
                break;
            ol = ol->next;
        }
        if(ol->next == NULL) {
            printf("Error: OrderLevel not contain oid=%d for delete\n", oid );
        } else {
            OrderLevel *tmp = ol->next;
            ol->next = ol->next->next;
            if(list->tail == tmp) {
                list->tail = ol;
                //list->tail = tmp->next;	// TODO ol or ol->next
            }
            free(tmp->value);
            free(tmp);
            list->size--;
        }
    }
    if(list->size == 1 && list->head != list->tail)
        printf("stop");
}

void fun(Order *ord) {
    printf("price=%f\tnumber=%d\t\tqty=%d", ord->price, ord->oid, ord->qty);
}

void printList(PriceLevel *list) {
    if(list->size > 1)
        printf("");
    OrderLevel *iter = list->head;
    while (iter) {
        fun(iter->value);
        iter = iter->next;
        printf("\n");
        fflush(stdout);
    }
}

#endif // PRICELEVEL_H
