// copyright Ciocoiu Diana-Iulia 313CA 2022

#ifndef USERS_H__
#define USERS_H__

#define USERNAME 30
#define BOOKNAME 50

typedef struct user_t
{
    char book_name[BOOKNAME];
    char user_name[USERNAME];
    int days_borrow;
    int borrow;
    int banned;
    int points;
} user_t;

void add_user(hashtable_t *users);

void borrow_book(hashtable_t *users, hashtable_t *library);

void return_book(hashtable_t *users, hashtable_t *library);

int user_points_funct(int days_borrowed, int days_since, int user_points);

void lost_book(hashtable_t *users, hashtable_t *library);

void top_users_funct(hashtable_t *users);

void sort_users(hashtable_t *users);

void free_users(hashtable_t *users);

#endif  // USERS_H__
