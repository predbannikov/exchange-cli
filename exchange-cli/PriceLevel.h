#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Order_ {
    unsigned int oid;
    unsigned int qty;
    float price;
    char side;
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

void push_back_list(PriceLevel *list, Order *value) {
    OrderLevel *tmp = (OrderLevel*) malloc(sizeof(OrderLevel));
    if (tmp == NULL) {
        fprintf(stderr, "error: insufficient memory (push_back_list)\n");
        exit(EXIT_FAILURE);
    }

    tmp->next = NULL;
    tmp->value = value;
    if(list->tail)
        list->tail->next = tmp;
    if (list->head == NULL) {
        list->head = tmp;
    }
        list->tail = tmp;
    list->size++;
}

Order* pop_front_list(PriceLevel *list) {
    OrderLevel *tmp = NULL;
    Order *value = NULL;
    if (list->head == NULL) {
        fprintf(stderr, "error: (pop_front_list)");
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

void delete_Nth_of_oid(PriceLevel *list, unsigned int oid) {
    OrderLevel *ol = list->head;
    if (ol->value->oid == oid) {
        pop_front_list(list);
    } else {
        while(ol->next != NULL) {
            if(ol->next->value->oid == oid)
                break;
            ol = ol->next;
        }
        if(ol->next != NULL) {
            OrderLevel *tmp = ol->next;
            ol->next = ol->next->next;
            if(list->tail == tmp) {
                list->tail = ol;
            }
            free(tmp->value);
            free(tmp);
            list->size--;
        }
    }
}

#endif // PRICELEVEL_H
