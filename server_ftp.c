/*
 * Function: ftp server
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */  

#include <server_ftp.h>

size_t size_sockaddr = sizeof(struct sockaddr),
       size_packet = sizeof(struct packet);

void* serve_client(void*);

int main(void)
{
    // initialization starts
    struct sockaddr_in skin_server, skin_client;
    int sfd_server, sfd_client, x;
    short int connection_id = 0;

    if(( x = sfd_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <0 )
        er("socket()", x);

    memset((char*) &skin_server, 0, sizeof(struct sockaddr_in));
    skin_server.sin_family = AF_INET;
    skin_server.sin_port = htons(PORTSERVER);
    skin_server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(( x = bind(sfd_server, (struct sockaddr*) &skin_server, size_sockaddr)) < 0)
        er("bind()", x);

    if(( x = listen(sfd_server, 1)) < 0)
        er("listen()", x);

    printf(ID "FTP Server started up @ local: %d. Waiting for clients...\n\n", PORTSERVER);
    // initialization ends

    while(1)
    {
        // begin connetction with client
        if((x = sfd_client = accept(sfd_server, (struct sockaddr*) &skin_client, &size_sockaddr)) < 0)
            er("accept()", x);
        printf(ID "Communication started with %s:%d\n", inet_ntoa(skin_client.sin_addr), ntohs(skin_client.sin_port));
        fflush(stdout);

        struct client_info* ci = client_info_alloc(sfd_client, connection_id++);
        pid_t fpid = fork();
        if( fpid == 0 )
        {
            // handle the request from client
            serve_client(ci);
        }
        else if(fpid < 0)
        {
            printf("server fork error!");
            return -1;
        }
    }

    close(sfd_server);
    print(ID "Done.\n");

    return 0;
}

void* serve_client(void* info)
{
    int sfd_client, connection_id, x;
    struct packet* data = (struct packet*) malloc(size_packet);
    struct packet* shp;
    char lpwd[LENBUFFER];
    struct client_info* ci = (struct client_info*) info;
    sfd_client = ci->sfd;
    connection_id = ci->cid;

    while(1)
    {
        if((x = recv(sfd_client, data, size_packet, 0)) == 0)
        {
            fprintf(stderr, "client closed or terminated. Closing connection...\n");
            break;
        }

        shp = ntohp(data);

        printpacket(shp, HP);

        if(shp->type == TERM)
            break;
        if(shp->conid == -1)
            shp->conid = connection_id;
        if(shp->type == REQU)
        {
            switch(shp->comid)
            {
                // choose the operation of commands according to the comid
                case GET:
                    if(!getcwd(lpwd, sizeof lpwd))
                        er("getcwd()", 0);
                    command_get(shp, data, sfd_client, lpwd);
                    break;
                case DIR_:
                    if(!getcwd(lpwd, sizeof lpwd))
                        er("getcwd()", 0);
                    command_dir(shp, data, sfd_client, lpwd);
                    break;
                case PRT:
                    if(!getcwd(lpwd, sizeof lpwd))
                        er("getcwd()", 0);
                    command_prt(shp, data, sfd_client, lpwd);
                    break;
                case BYE:
                    if(!getcwd(lpwd, sizeof lpwd))
                        er("getcwd()", 0);
                    command_bye(shp, data, sfd_client, lpwd);
                    goto bye_end;
                default:
                    // print error
                    break;
            }
        }
        else
        {
            //show error, send TERM and break
            fprintf(stderr, "packet incomprihensible. closing connection.\n");
            send_TERM(shp, data, sfd_client);
            break;
        }
    }

    close(sfd_client);
    bye_end:
    fflush(stdout);
}
