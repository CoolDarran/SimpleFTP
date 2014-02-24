/*
 * Function: head file of server_ftp
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */

#include <commons.h>
#include <time.h>
#include <sys/stat.h>

#define IPCLIENT "127.0.0.1"
#define ID "FTPSERVER=> "

struct client_info
{
    int sfd;  // scoket fd
    int cid;  // client id
};

struct client_info* client_info_alloc(int, int);

void command_get(struct packet*, struct packet*, int, char*);
void command_dir(struct packet*, struct packet*, int, char*);
void command_prt(struct packet*, struct packet*, int, char*);
void command_bye(struct packet*, struct packet*, int, char*);


