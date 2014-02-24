/*
 * Function: some common definitions
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */ 

# ifndef COMMONS_H
# define COMMONS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define DEBUG 0 // define for debuging the program

// the default port num
#define PORTSERVER  8487
#define CONTROLPORT (PORTSERVER + 1)
int DATAPORT; // port num of data channel
int sfd_server_data; // socket id of data channel

#define get_filename(filepath) ((strrchr(filepath, '/') != NULL) ? (strrchr(filepath, '/') + ) : filepath)

enum TYPE
    {
        REQU,  // request
        DONE,  // process done
        INFO,  // infor sent back
        TERM,
        DATA,  // send data, like file and dir result
        EOT    // end
    };

#define NP 0
#define HP 1

#define er(e, x)\
    do\
    {\
        perror("ERROR IN: " #e "\n");\
        fprintf(stderr, "%d\n", x);\
        exit(-1);\
    }\
    while(0)

#define LENBUFFER 504 // make the whole packet well-rounded ( = 512 bytes)
struct packet
{
    short int conid;
    short int type;
    short int comid;
    short int datalen;
    char buffer[LENBUFFER];
};

void set0(struct packet*);

struct packet* ntohp(struct packet*);
struct packet* htonp(struct packet*);

void printpacket(struct packet*, int);

#define NCOMMANDS 4
enum COMMAND
    {
        GET,
        PRT,
        DIR_, //_ to avoid conflict with directory pointer DIR
        BYE
    };

#endif
