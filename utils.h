// copyright Ciocoiu Diana-Iulia 313CA 2022

#ifndef UTILS_H__
#define UTILS_H__

#include <errno.h>
#include <stdio.h>
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define DIE(assertion, call_description)            \
	do                                              \
	{                                               \
		if (assertion)                              \
		{                                           \
			fprintf(stderr, "(%s, %d): ", __FILE__, \
					__LINE__);                      \
			perror(call_description);               \
			exit(errno);                            \
		}                                           \
	} while (0)

#define MAX_STRING_SIZE 64

#endif  // UTILS_H__
