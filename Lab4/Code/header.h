//***** Libraries *****
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>



//***** Defined Constants *****
#define  MAX 256
#define EOF_STR "***** EOF *****"


//***** Global Variables *****
bool isServer = false;


//***** Source Code *****
/*
struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
};

struct in_addr {
	uint32_t s_addr;
};
*/
