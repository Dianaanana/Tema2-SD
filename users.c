// copyright Ciocoiu Diana-Iulia 313CA 2022

#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "library.h"
#include "utils.h"
#include "users.h"

// Functia adauga un nou user in hashtable-ul users.
void add_user(hashtable_t *users)
{
    char user_name[MAX_STRING_SIZE];
    scanf("%s", user_name);

    // daca cartea exista deja
    if (ht_get(users, user_name) != NULL)
    {
        printf("User is already registered.\n");
        return;
    }
    user_t *new_user = malloc(sizeof(user_t));

    strcpy(new_user->user_name, user_name);
    new_user->days_borrow = 0;
    new_user->borrow = 0;
    new_user->banned = 0;
    new_user->points = 100;

    ht_put(users, user_name, strlen(user_name) + 1, new_user, sizeof(user_t));

    free(new_user);
}

// Un user poate sa imprumute o carte.
void borrow_book(hashtable_t *users, hashtable_t *library)
{
    char token[MAX_STRING_SIZE];
    fgets(token, MAX_STRING_SIZE, stdin);
    char *user_name = strtok(token, " \"");
    char *book_name = strtok(NULL, "\"");
    char *daysc = strtok(NULL, "\0");
    int days = atoi(daysc);

    // daca nu exista userul
    if (ht_get(users, user_name) == NULL)
    {
        printf("You are not registered yet.\n");
        return;
    }

    // daca userul e banat
    user_t *curr_user = ht_get(users, user_name);
    if (curr_user->banned == 1)
    {
        printf("You are banned from this library.\n");
        return;
    }

    // daca userul are deja o carte
    if (curr_user->borrow == 1)
    {
        printf("You have already borrowed a book.\n");
        return;
    }

    // daca cartea nu e in biblioteca
    if (ht_get(library, book_name) == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }

    imdb_book_t *curr_book = ht_get(library, book_name);

    // daca cartea e deja imprumutata
    if (curr_book->borrowed == 1)
    {
        printf("The book is borrowed.\n");
        return;
    }

    memcpy(curr_user->book_name, book_name, strlen(book_name) + 1);
    curr_book->borrowed = 1;
    curr_user->borrow = 1;
    curr_user->days_borrow = days;
}

// userul returneaza o carte. Se calculeaza punctele dupa formula data.
void return_book(hashtable_t *users, hashtable_t *library)
{
    char token[MAX_STRING_SIZE];
    fgets(token, MAX_STRING_SIZE, stdin);
    char *user_name = strtok(token, " \"");
    char *book_name = strtok(NULL, "\"");
    char *days_sincec = strtok(NULL, " ");
    int days_since = atoi(days_sincec);
    char *ratingc = strtok(NULL, "\0");
    int rating = atoi(ratingc);

    // daca nu exista userul
    if (ht_get(users, user_name) == NULL)
    {
        printf("You are not registered yet.\n");
        return;
    }

    // daca userul e banat
    user_t *curr_user = ht_get(users, user_name);
    if (curr_user->banned == 1)
    {
        printf("You are banned from this library.\n");
        return;
    }

    // daca nu a imprumutat nicio carte
    if (curr_user->borrow == 0)
    {
        printf("You didn't borrow this book.\n");
        return;
    }

    // daca a imprumutat alta carte
    if (curr_user->borrow == 1 && strcmp(curr_user->book_name, book_name) != 0)
    {
        printf("You didn't borrow this book.\n");
        return;
    }

    int old_points = curr_user->points;
    int new_points = user_points_funct(curr_user->days_borrow, days_since,
                     old_points);

    // pct < 0 => userul este banat
    if (new_points < 0)
    {
        curr_user->banned = 1;
        printf("The user %s has been banned from this library.\n", user_name);
        return;
    }

    curr_user->points = new_points;
    imdb_book_t *curr_book = ht_get(library, book_name);
    curr_book->book_rating_sum += rating;
    curr_book->purchases += 1;
    curr_book->borrowed = 0;
    curr_user->borrow = 0;
}

// functie de calcul a punctelor unui user
int user_points_funct(int days_borrowed, int days_since, int user_points)
{
    if (days_since > days_borrowed)
    {
        user_points -= 2 * (days_since - days_borrowed);
    }

    if (days_since < days_borrowed)
    {
        user_points += days_borrowed - days_since;
    }

    return user_points;
}

// daca un user pierde o carte i se scad 50 de puncte si cartea este stearsa
void lost_book(hashtable_t *users, hashtable_t *library)
{
    char token[MAX_STRING_SIZE];
    fgets(token, MAX_STRING_SIZE, stdin);
    char *user_name = strtok(token, " \"");
    char *book_name = strtok(NULL, "\"");

    // daca nu exista userul
    if (ht_get(users, user_name) == NULL)
    {
        printf("You are not registered yet.\n");
        return;
    }

    // daca userul e banat
    user_t *curr_user = ht_get(users, user_name);
    if (curr_user->banned == 1)
    {
        printf("You are banned from this library.\n");
        return;
    }

    // se sterge cartea din sistem
    imdb_book_t *curr_book = ht_get(library, book_name);
    ht_free(curr_book->book);
    ht_remove_entry(library, book_name);

    curr_user->borrow = 0;

    curr_user->points -= 50;

    // pct < 0 => userul este banat
    if (curr_user->points < 0)
    {
        curr_user->banned = 1;
        printf("The user %s has been banned from this library.\n", user_name);
        return;
    }
}

// functie de sortare a userilor in functie de nr de puncte.
// bag userii intr-un vector pe care il sortez.
void sort_users(hashtable_t *users)
{
    int size = ht_get_size(users);
    info *sorted = malloc(size * sizeof(info));
    int n = 0;

    for (unsigned int i = 0; i < users->hmax; i++)
    {
        linked_list_t *list = users->buckets[i];

        ll_node_t *curr = list->head;
        for (unsigned int j = 0; j < list->size; j++)
        {
            info *list_info = curr->data;
            memcpy((sorted + n), list_info, sizeof(info));
            n++;

            curr = curr->next;
        }
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if ((*(user_t *)sorted[i].value).points <
                (*(user_t *)sorted[j].value).points)
            {
                info aux;
                memcpy(&aux, &sorted[i], sizeof(info));
                memcpy(&sorted[i], &sorted[j], sizeof(info));
                memcpy(&sorted[j], &aux, sizeof(info));
            }
            if ((*(user_t *)sorted[i].value).points ==
                (*(user_t *)sorted[j].value).points)
            {
                if (strcmp((*(user_t *)sorted[i].value).user_name,
                   (*(user_t *)sorted[j].value).user_name) > 0)
                {
                    info aux;
                    memcpy(&aux, &sorted[i], sizeof(info));
                    memcpy(&sorted[i], &sorted[j], sizeof(info));
                    memcpy(&sorted[j], &aux, sizeof(info));
                }
            }
        }
    }

    // printf
    printf("Users ranking:\n");
    int cnt = 1;
    for (int i = 0; i < size; i++)
    {
        if ((*(user_t *)sorted[i].value).banned == 0)
        {
            printf("%d. Name:%s Points:%d\n", cnt, ((char *)sorted[i].key),
                  (*(user_t *)sorted[i].value).points);
            cnt++;
        }
    }

    free(sorted);
}

// fara sa sortez userii: il aut mereu pe cel cu cele mai multe puncte,
// il printez apoi ii elimin din hashtable.
// void top_users_funct(hashtable_t *users)
// {
// 	int cnt = 1;
// 	printf("Users rating:\n");
//     while(ht_get_size(users) != 0)
//     {
//         user_t *top = malloc(sizeof(user_t));
//         top->points = 0;
//         for( unsigned int i = 0; i < users->hmax; i++)
//         {
//             linked_list_t *list = users->buckets[i];

//             ll_node_t *curr = list->head;
//             for(unsigned int j = 0 ; j < list->size; j++)
//             {
//                 info *list_info = curr->data;
//                 user_t *user = list_info->value;
//                 if(top->points < user->points && user->banned == 0)
//                 {
//                     memcpy(top, user, sizeof(user_t));
//                 }
//                 curr = curr->next;
//             }
//         }

// 		printf("%d. Name:%s Points:%d\n", cnt, top->user_name, top->points);
// 		ht_remove_entry(users, top->user_name);
// 		free(top);

// 		cnt++;
//     }
// 	ht_free(users);

// }
