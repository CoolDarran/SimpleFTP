/*
 * Function: ftp client
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */ 

#include <client_ftp.h>

int main(int argc, char* argv[])
{
    // initialization begins
    struct sockaddr_in skin_server;
    int sfd_client, x;
    size_t size_sockaddr = sizeof(struct sockaddr), size_packet = sizeof(struct packet);
    short int connection_id;
    struct packet* chp = (struct packet*) malloc(size_packet); // client host packet
    set0(chp);
    struct packet* data; // network data packet

    if((x = sfd_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        er("socket()", x);

    memset((char*) &skin_server, 0, sizeof(struct sockaddr_in));
    skin_server.sin_family = AF_INET;
    skin_server.sin_addr.s_addr = inet_addr(IPSERVER);
    skin_server.sin_port = htons(PORTSERVER);

    if((x = connect(sfd_client, (struct sockaddr*) &skin_server, size_sockaddr)) < 0)
        er("connect()", x);

    printf(ID "FTP Client started up. Attempting communication with server @ %s:%d...\n\n", IPSERVER, PORTSERVER);
    printf(ID "Connect command channel success! Please build the data transfering channel with server by command 'prt [port_num]!'\n");
    connect_flag = ISCONNECT;
    // initialization ends
    
    struct command* cmd;

    char lpwd[LENBUFFER], pwd[LENBUFFER];
    char userinput[LENBUFFER];
    while(1)
    {
        printf("\t> ");
        fgets(userinput, LENBUFFER, stdin);

        cmd = userinputtocommand(userinput);
        if(!cmd)
            continue;

        printcommand(cmd);
        
        // check connect status
        if( connect_flag == ISNOTCONNECT )
        {
            printf(ID "Please connect server first.\n");
            continue;
        }

        // check prt flag
        if(cmd->id != 1 && DATAPORTNOTBUILT == prt_flag)
        {
            printf(ID "Please build the data transfering channel first! command 'prt [port_num]'\n");
            continue;
        }
        else if(cmd->id == 1 && DATAPORTBUILT == prt_flag)
        {
            printf(ID "Data transfering channel has been build!\n");
            continue;
        }
        

        switch(cmd->id)
        {
            case GET:
                if(cmd->arguments)
                    command_get(chp, data, sfd_client, cmd->argname);
                else
                    fprintf(stderr, "No path to file given.\n");
                break;
            case PRT:
                if(cmd->arguments)
                    command_prt(chp, data, sfd_client, cmd->argname);
                else
                    fprintf(stderr, "No portnum given.\n");
                break;
            case DIR_:
                command_dir(chp, data, sfd_client);
                break;
            case BYE:
                command_bye(chp, data, sfd_client);
                return 0;
                break;
            default:
                // display error
                break;
        }
    }

    close(sfd_client);
    printf(ID "Done.\n");
    fflush(stdout);

    return 0;
}
