// copyright Ciocoiu Diana-Iulia 313CA 2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hashtable.h"
#include "utils.h"

// functie care compara 2 int
int compare_function_ints(void *a, void *b)
{
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b)
    {
        return 0;
    }
    else if (int_a < int_b)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

// functie care compara 2 string
int compare_function_strings(void *a, void *b)
{
    char *str_a = (char *)a;
    char *str_b = (char *)b;

    return strcmp(str_a, str_b);
}

// functia de hash luata din lab
unsigned int
hash_function_int(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

// functia de hash luata din lab
unsigned int
hash_function_string(void *a)
{
    unsigned char *puchar_a = (unsigned char *)a;
    int64_t hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

// functie care imi creaza un nou hashtable
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
          int (*compare_function)(void *, void *))
{
    if (!hash_function || !compare_function)
    {
        return NULL;
    }

    hashtable_t *new_hashtable = malloc(sizeof(hashtable_t));
    DIE(!new_hashtable, "Error");

    new_hashtable->buckets = malloc(hmax * sizeof(linked_list_t *));
    DIE(!new_hashtable->buckets, "Error");
    for (unsigned int i = 0; i < hmax; i++)
    {
        new_hashtable->buckets[i] = ll_create(sizeof(info));
    }

    new_hashtable->size = 0;
    new_hashtable->hmax = hmax;
    new_hashtable->hash_function = hash_function;
    new_hashtable->compare_function = compare_function;

    return new_hashtable;
}

// functie care verifica daca exista o cheie in hashtable
// functia returneaza 1 daca exista si 0 daca nu
int ht_has_key(hashtable_t *ht, void *key)
{
    if (!ht || !key)
    {
        return -1;
    }

    int hash_index = ht->hash_function(key) % ht->hmax;
    ll_node_t *curr = ht->buckets[hash_index]->head;

    while (curr != NULL)
    {
        if (!ht->compare_function(key, ((info *)curr->data)->key))
        {
            return 1;
        }
        curr = curr->next;
    }

    return 0;
}

// functie care imi returmeaza o valoare in functie de cheie
void *
ht_get(hashtable_t *ht, void *key)
{
    if (!ht || !key || ht_has_key(ht, key) == 0)
    {
        return NULL;
    }

    int hash_index = ht->hash_function(key) % ht->hmax;
    ll_node_t *curr = ht->buckets[hash_index]->head;

    while (curr != NULL)
    {
        if (!ht->compare_function(key, ((info *)curr->data)->key))
        {
            return ((info *)curr->data)->value;
        }
        curr = curr->next;
    }

    return NULL;
}

// functie care pune in hashtable o pereche cheie - valoare
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
            void *value, unsigned int value_size)
{
    if (!ht || !key || !value)
    {
        return;
    }

    int hash_index = ht->hash_function(key) % ht->hmax;

    if (ht_has_key(ht, key) == 1)
    {
        ll_node_t *curr = ht->buckets[hash_index]->head;
        while (curr != NULL)
        {
            if (!ht->compare_function(key, ((info *)curr->data)->key))
            {
                free(((info *)curr->data)->value);

                ((info *)curr->data)->value = malloc(value_size);
                DIE(!((info *)curr->data)->value, "Error");

                memcpy(((info *)curr->data)->value, value, value_size);
                return;
            }

            curr = curr->next;
        }
    }

    info *data_info = malloc(sizeof(info));
    DIE(!data_info, "Error");

    data_info->key = malloc(key_size);
    DIE(!data_info->key, "Error");
    data_info->value = malloc(value_size);
    DIE(!data_info->value, "Error");

    memcpy(data_info->key, key, key_size);
    memcpy(data_info->value, value, value_size);

    ll_add_nth_node(ht->buckets[hash_index], 0, data_info);
    free(data_info);
    ht->size++;
}

// functie care sterge o pereche cheie valoare din hashtable
void ht_remove_entry(hashtable_t *ht, void *key)
{
    if (!ht || !key || ht_has_key(ht, key) == 0)
    {
        return;
    }

    int hash_index = ht->hash_function(key) % ht->hmax;
    ll_node_t *curr = ht->buckets[hash_index]->head;

    unsigned int node_nr = 0;

    while (curr != NULL)
    {
        if (!ht->compare_function(key, ((info *)curr->data)->key))
        {
            free(((info *)curr->data)->key);
            free(((info *)curr->data)->value);
            free(((info *)curr->data));

            ll_node_t *deleted_node = ll_remove_nth_node
                                      (ht->buckets[hash_index], node_nr);
            free(deleted_node);

            ht->size--;
            return;
        }

        curr = curr->next;
        node_nr++;
    }
}

// functia elibereaza memoria pt n valgrind fericit
void ht_free(hashtable_t *ht)
{
    if (!ht)
    {
        return;
    }

    for (unsigned int i = 0; i < ht->hmax; ++i)
    {
        ll_node_t *node = ht->buckets[i]->head;

        while (node != NULL)
        {
            free(((info *)node->data)->key);
            free(((info *)node->data)->value);
            node = node->next;
        }

        ll_free(&ht->buckets[i]);
    }

    free(ht->buckets);
    free(ht);
}

// arata dimensiunea hastable-lui
unsigned int
ht_get_size(hashtable_t *ht)
{
    if (ht == NULL)
        return 0;

    return ht->size;
}

// arata nr de elemente din vector
unsigned int
ht_get_hmax(hashtable_t *ht)
{
    if (ht == NULL)
        return 0;

    return ht->hmax;
}
