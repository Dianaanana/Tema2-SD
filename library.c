// copyright Ciocoiu Diana-Iulia 313CA 2022

#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "library.h"
#include "utils.h"

// functie care imi parseaza elementele de la tastatura
// adaug in hashtable ul carte definitiile.
hashtable_t *read_book(int nr_def)
{
    hashtable_t *new_book = ht_create(HMAX, hash_function_string,
                                      compare_function_strings);

    char word[MAX_STRING_SIZE], def[MAX_STRING_SIZE];

    for (int i = 0; i < nr_def; i++)
    {
        scanf("%s %s", word, def);

        ht_put(new_book, word, strlen(word) + 1, def, strlen(def) + 1);
    }

    return new_book;
}

// functie care imi adauga o carte noua. Daca cartea exista deja,
// o elimin si o inlocuiesc.
void add_book(hashtable_t *library)
{
    char *token = malloc(MAX_STRING_SIZE * sizeof(char));
    DIE(!token, "maloc failed token");
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    // daca se baga din nou o carte se suprascrie
    if (ht_has_key(library, book_name) == 1)
    {
        imdb_book_t *delete = ht_get(library, book_name);
        ht_free(delete->book);
    }

    char *nr_string = strtok(NULL, "\0");
    int nr_def = atoi(nr_string + 1);

    imdb_book_t *imbd_book = malloc(sizeof(imdb_book_t));
    imbd_book->book = read_book(nr_def);
    imbd_book->book_rating_sum = 0;
    imbd_book->purchases = 0;
    imbd_book->borrowed = 0;

    ht_put(library, book_name, strlen(book_name) + 1, imbd_book,
           sizeof(imdb_book_t));

    free(imbd_book);
    free(token);
}

// functie care imi printeaza informatii despre o anumita carte.
void get_book(hashtable_t *library)
{
    char *token = malloc(MAX_STRING_SIZE * sizeof(char));
    DIE(!token, "malloc failed token");
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    if (ht_has_key(library, book_name) != 1)
    {
        printf("The book is not in the library.\n");
        free(token);
        return;
    }

    imdb_book_t *imdb_book = ht_get(library, book_name);

    float rating = 0;
    if (imdb_book->purchases != 0)
    {
        rating = imdb_book->book_rating_sum / imdb_book->purchases;
    }
    printf("Name:%s Rating:%.3f Purchases:%d \n", book_name, rating,
           imdb_book->purchases);
    free(token);
}

// functie care imi printeaza o definitie dintr-o carte
void get_def(hashtable_t *library)
{
    char *token = malloc(MAX_STRING_SIZE * sizeof(char));
    DIE(!token, "malloc failed");
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    if (ht_has_key(library, book_name) != 1)
    {
        printf("The book is not in the library.\n");
        free(token);
        return;
    }

    imdb_book_t *imdb_book = ht_get(library, book_name);

    char *word = strtok(NULL, "\n");
    while (*word == ' ')
        word++;

    if (ht_has_key(imdb_book->book, word) != 1)
    {
        printf("The definition is not in the book.\n");
        free(token);
        return;
    }

    char *definiton = ht_get(imdb_book->book, word);

    printf("%s\n", definiton);
    free(token);
}

// functie care imi sterge o definitie dintr-o carte
void rmv_def(hashtable_t *library)
{
    char *token = malloc(MAX_STRING_SIZE * sizeof(char));
    DIE(!token, "malloc failed token");
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    imdb_book_t *imdb_book = ht_get(library, book_name);

    if (ht_has_key(library, book_name) == 0)
    {
        printf("The book is not in the library.\n");
        free(token);
        return;
    }

    char *word = strtok(NULL, "\n");
    if (*word == ' ')
        word++;

    if (ht_has_key(imdb_book->book, word) == 0)
    {
        printf("The definition is not in the book.\n");
        free(token);
        return;
    }

    ht_remove_entry(imdb_book->book, word);
    free(token);
}

// functie care sterge o carte din biblioteca
void rmv_book(hashtable_t *library)
{
    char token[MAX_STRING_SIZE];
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    if (ht_has_key(library, book_name) != 1)
    {
        printf("The book is not in the library.\n");
        return;
    }

    imdb_book_t *aux = ht_get(library, book_name);
    ht_free(aux->book);
    ht_remove_entry(library, book_name);
}

// functie care adauga o definitie intr-o carte
void add_def(hashtable_t *library)
{
    char token[MAX_STRING_SIZE];
    fgets(token, MAX_STRING_SIZE, stdin);
    strtok(token, "\"");
    char *book_name = strtok(NULL, "\"");

    if (ht_has_key(library, book_name) != 1)
    {
        printf("The book is not in the library.\n");
        return;
    }

    char *new_key = strtok(NULL, " ");
    while ((*new_key) == ' ')
        new_key++;

    char *new_value = strtok(NULL, "\n");

    imdb_book_t *imdb_book = ht_get(library, book_name);
    ht_put(imdb_book->book, new_key, strlen(new_key) + 1,
           new_value, strlen(new_value) + 1);
}

void free_library(hashtable_t *library)
{
    if (!library)
    {
        return;
    }

    for (unsigned int i = 0; i < library->hmax; i++)
    {
        linked_list_t *list = library->buckets[i];

        ll_node_t *curr = list->head;
        for (unsigned int j = 0; j < list->size; j++)
        {
            info *list_info = curr->data;
            imdb_book_t *book = list_info->value;
            ht_free(book->book);
            curr = curr->next;
        }
    }
    ht_free(library);
}

// functia de sortare. Pentru printarea cartilor in functie de rating-ul
// lor, sortez cartile intr-un vector, apoi le printez
void sort_books(hashtable_t *library)
{
    int size = ht_get_size(library);
    info *sorted = malloc(size * sizeof(info));
    int n = 0;

    for (unsigned int i = 0; i < library->hmax; i++)
    {
        linked_list_t *list = library->buckets[i];

        ll_node_t *curr = list->head;
        for (unsigned int j = 0; j < list->size; j++)
        {
            info *list_info = curr->data;
            // imdb_book_t *book_name = list_info->key;
            // imdb_book_t *book = list_info->value;
            memcpy((sorted + n), list_info, sizeof(info));
            n++;

            curr = curr->next;
        }
    }

    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            float rating1 = 0, rating2 = 0;

            if ((*(imdb_book_t *)sorted[i].value).purchases != 0)
            {
                rating1 = (*(imdb_book_t *)sorted[i].value).book_rating_sum /
                          (*(imdb_book_t *)sorted[i].value).purchases;
            }

            if ((*(imdb_book_t *)sorted[j].value).purchases != 0)
            {
                rating2 = (*(imdb_book_t *)sorted[j].value).book_rating_sum /
                          (*(imdb_book_t *)sorted[j].value).purchases;
            }

            if (rating1 < rating2)
            {
                info *aux;
                memcpy(&aux, &sorted[i], sizeof(info));
                memcpy(&sorted[i], &sorted[j], sizeof(info));
                memcpy(&sorted[j], &aux, sizeof(info));
            }
            if (rating1 == rating2)
            {
                if ((*(imdb_book_t *)sorted[i].value).purchases <
                    (*(imdb_book_t *)sorted[j].value).purchases)
                {
                    info aux;
                    memcpy(&aux, &sorted[i], sizeof(info));
                    memcpy(&sorted[i], &sorted[j], sizeof(info));
                    memcpy(&sorted[j], &aux, sizeof(info));
                }
                if ((*(imdb_book_t *)sorted[i].value).purchases ==
                    (*(imdb_book_t *)sorted[j].value).purchases)
                {
                    if (strcmp(sorted[i].key, sorted[j].key) > 0)
                    {
                        info aux;
                        memcpy(&aux, &sorted[i], sizeof(info));
                        memcpy(&sorted[i], &sorted[j], sizeof(info));
                        memcpy(&sorted[j], &aux, sizeof(info));
                    }
                }
            }
        }
    }

    // printf
    printf("Books ranking:\n");
    for (int i = 0; i < size; i++)
    {
        float rating = 0;
        if ((*(imdb_book_t *)sorted[i].value).purchases != 0)
        {
            rating = (*(imdb_book_t *)sorted[i].value).book_rating_sum /
                     (*(imdb_book_t *)sorted[i].value).purchases;
        }
        printf("%d. Name:%s Rating:%.3f Purchases:%d\n", i + 1,
              ((char *)sorted[i].key), rating,
              (*(imdb_book_t *)sorted[i].value).purchases);
    }

    free(sorted);
}
