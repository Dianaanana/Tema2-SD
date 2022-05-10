// copyright Ciocoiu Diana-Iulia 313CA 2022

#ifndef LIBRARY_H__
#define LIBRARY_H__

typedef struct imdb_book_t
{
    hashtable_t *book;
    float book_rating_sum;
    int purchases;
    int borrowed;
} imdb_book_t;

void add_book(hashtable_t *library);

void get_book(hashtable_t *library);

void get_def(hashtable_t *library);

void rmv_def(hashtable_t *library);

void rmv_book(hashtable_t *library);

void add_def(hashtable_t *library);

void free_library(hashtable_t *library);

void sort_books(hashtable_t *library);

#endif  // LIBRARY_H__
