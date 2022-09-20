#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "object.h"
#include "packets.h"
#include "queue.h"
#include "../utils/utils.h"

ssize_t (*real_send)(int socket, const void *buffer, size_t length, int flags);

__attribute__((constructor))
void before_run()
{
	printf("YEAAAAAAHHHHHH\n");
}

/*
int main()
{
	printf ("this is main");
}
*/

/*
 * ssize_t send(int socket, const void *buffer, size_t length, int flags)
 * {
 * 		real_send = (ssize_t *(int, const void *, size_t, int))dlysm(RTLD_NEXT, send);
 * }
 */
