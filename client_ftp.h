/*
 * Function: head file of client_ftp
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */  
//#ifndef _CLIENT_FTP_H
//#define _CLIENT_FTP_H

#include <commons.h>
#include <file_transfer_function.h>

#define IPSERVER "127.0.0.1"
#define ID "CLIENT=> "

// the length of user's input
#define LENUSERINPUT 1024

// some definitions of command prt
#define DATAPORTBUILT 1
#define DATAPORTNOTBUILT 0
#define ISCONNECT 1
#define ISNOTCONNECT 0
int prt_flag;
int connect_flag;

//#endif

// command struct
struct command
{
    short int id;
    int arguments;  // get more files
    char** argname;
};

struct command* userinputtocommand(char [LENUSERINPUT]);

void printcommand(struct command*);

void command_get(struct packet*, struct packet*, int, char*);
void command_dir(struct packet*, struct packet*, int);
void command_prt(struct packet*, struct packet*, int, char*);
void command_bye(struct packet*, struct packet*, int);
