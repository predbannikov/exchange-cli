#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Order_ {
    unsigned int num_pack;
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
    if(list->tail == NULL)
        list->tail = tmp;
    list->size++;
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
    free(tmp);
    list->size--;

    return value;
}

void fun(Order *ord) {
    printf("price=%f\tnumber=%d\t\tqty=%d", ord->price, ord->num_pack, ord->qty);
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
