/*
 * Function: some functions of server_ftp
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */ 

#include <server_ftp.h>

static size_t size_packet = sizeof(struct packet);

struct client_info* client_info_alloc(int s, int c)
{
    struct client_info* ci = (struct client_info*) malloc(sizeof(struct client_info));
    ci->sfd = s;
    ci->cid = c;
    return ci;
}

// the operation of GET file command
void command_get(struct packet* shp, struct packet* data, int sfd_client, char* lpwd)
{
    int x;
    FILE* f = fopen(shp->buffer, "rb");
    shp->type = INFO;
    shp->comid = GET;
    strcpy(shp->buffer, f ? "OK: File found; processing..." : "ER: Error opening file.");
    printpacket(shp, HP);
    data = htonp(shp);
    if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);
    if(f)
    {
        shp->type = DATA;
        //send_file(shp, data, sfd_client, f);
        send_file(shp, data, sfd_server_data, f);
        fclose(f);
    }
    send_EOT(shp, data, sfd_server_data);
    //send_EOT(shp, data, sfd_client);
}

// the opration of DIR command
void command_dir(struct packet* shp, struct packet* data, int sfd_client, char* lpwd)
{
    int x;
    shp->type = DATA;
    DIR* d = opendir(lpwd);
    if(!d)
    {
        er("opendir()", (int) d);
        sprintf(shp->buffer, "ER: could not get the dir info!\n");
        data = htonp(shp);
        if( (x = send(sfd_server_data, data, size_packet,0)) != size_packet)
            er("send()",x);
    }
    else
    {
        sprintf(shp->buffer, "OK: get the dir info.\n");
        data = htonp(shp);
        if(( x = send(sfd_server_data, data, size_packet, 0)) != size_packet)
            er("send()",x);
    }
    struct dirent* e;

    while(e = readdir(d))
    {
        sprintf(shp->buffer, "%s\t%s", e->d_type == 4 ? "DIR:" : e->d_type == 8 ? "FILE:" : "UNDEF:" , e->d_name);
        data = htonp(shp);
        //if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
        if(( x = send(sfd_server_data, data, size_packet, 0)) != size_packet)    
            er("send()", x);
    }
    send_EOT(shp, data, sfd_server_data);
    //send_EOT(shp, data, sfd_client);
}

// the operation of PRT command
void command_prt(struct packet* shp, struct packet* data, int sfd_client, char* lpwd)
{
    if(DEBUG)
        printf("Starting prt...\n");
    int x;
    int dataportnum = atoi(shp->buffer);
    shp->type = INFO;
    shp->comid = PRT;
    strcpy(shp->buffer, dataportnum ? "OK: Port Gotten; processing..." : "ER: Error getting port num.");
 
    printpacket(shp, HP);

    data = htonp(shp);
    if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);

    if(DEBUG)
        printf("OK, waiting for connection\n");

    if(dataportnum)
    {
        struct sockaddr_in skin_server_data;
        int x;
        size_t size_sockaddr = sizeof(struct sockaddr), size_packet = sizeof(struct packet);

        if(( x = sfd_server_data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            er("socket()", x);

        memset((char*) & skin_server_data, 0, sizeof(struct sockaddr_in));
        skin_server_data.sin_family = AF_INET;
        skin_server_data.sin_addr.s_addr = inet_addr(IPCLIENT);
        skin_server_data.sin_port = htons(dataportnum);

        if(( x = connect(sfd_server_data, (struct sockaddr*) &skin_server_data, size_sockaddr)) < 0)
            er("connect()", x);
        printf(ID "Data channel build. Communicating with client @ %s:%d...\n\n", IPCLIENT, dataportnum);
    }
    send_EOT(shp, data, sfd_client);    
}


// the operation of BYE command
void command_bye(struct packet* shp, struct packet* data, int sfd_client, char* lpwd)
{
   if(DEBUG)
       printf("Connection closing...\n");
   
   /*
   int x;
   shp->type = INFO;
   shp->comid = BYE;
   strcpy(shp->buffer, "Connection closing...\n");

   data = htonp(shp);
   
   if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
       er("send()", x);
   
   send_EOT(shp, data, sfd_client);
   */

   printf(ID "Data channel with client closed!\n");
   close(sfd_server_data);
   printf(ID "Command channel with client closed!\n");
   close(sfd_client);
}
