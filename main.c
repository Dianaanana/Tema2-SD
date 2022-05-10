// copyright Ciocoiu Diana-Iulia 313CA 2022

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "linkedlist.h"
#include "users.h"
#include "utils.h"
#include "library.h"

int main()
{
    hashtable_t *library = ht_create(HMAX, hash_function_string,
                                     compare_function_strings);
    hashtable_t *users = ht_create(HMAX, hash_function_string,
                                   compare_function_strings);
    char command[MAX_STRING_SIZE];
    while (1)
    {
        scanf("%s", command);
        if (strncmp(command, "ADD_BOOK", 9) == 0)
        {
            add_book(library);
        }
        else if (strncmp(command, "GET_BOOK", 9) == 0)
        {
            get_book(library);
        }
        else if (strncmp(command, "RMV_BOOK", 9) == 0)
        {
            rmv_book(library);
        }
        else if (strncmp(command, "ADD_DEF", 8) == 0)
        {
            add_def(library);
        }
        else if (strncmp(command, "GET_DEF", 8) == 0)
        {
            get_def(library);
        }
        else if (strncmp(command, "RMV_DEF", 8) == 0)
        {
            rmv_def(library);
        }
        else if (strncmp(command, "ADD_USER", 9) == 0)
        {
            add_user(users);
        }
        else if (strncmp(command, "BORROW", 7) == 0)
        {
            borrow_book(users, library);
        }
        else if (strncmp(command, "RETURN", 7) == 0)
        {
            return_book(users, library);
        }
        else if (strncmp(command, "LOST", 5) == 0)
        {
            lost_book(users, library);
        }
        else if (strncmp(command, "EXIT", 5) == 0)
        {
            sort_books(library);
            // top_users_funct(users);
            sort_users(users);
            // free_users(users);
            ht_free(users);
            free_library(library);
            break;
        }
        else
        {
            printf("Invalid command\n");
        }
    }

    return 0;
}
