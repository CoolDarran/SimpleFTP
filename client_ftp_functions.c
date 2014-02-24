/*
 * Function: some functions of client_ftp
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */  

#include <client_ftp.h>

static size_t size_packet = sizeof(struct packet);
static size_t size_sockaddr = sizeof(struct sockaddr);

static const char commandList[NCOMMANDS][10] = 
    {
        "get",

        "prt",

        "dir",

        "bye"
    }; //refers to the COMMANDS enum in commons.h

// check the input command
struct command* userinputtocommand(char input[LENUSERINPUT])
{
    if(DEBUG)
        printf(ID "change userinput to command...\n");

    struct command* cmd = (struct command*) malloc(sizeof(struct command));
    cmd->id = -1;
    cmd->arguments = 0;
    cmd->argname = NULL;
    char* s = input;
    char* savestate;
    char* token;
    char* filename;
    int j;
    
    if(cmd->id == -1)
    {
        token = strtok_r(s, " \t\n", &savestate);
        s = NULL;
        if(token == NULL)
            er("get command", token);
        for(j = 0; j < NCOMMANDS; j++)
        {
            if(!strcmp(token, commandList[j]))
            {
                cmd->id = j;
                if(DEBUG)
                    printf(ID "cmdid done...\n");
                break;
            }
        }
    }
     
    if( (filename = strtok_r(s, " \t\n", &savestate)) != NULL);
    {
        cmd->arguments++;
        cmd->argname = filename;
    }
    

    if(cmd->id != -1)
        return cmd;
    else
    {
        fprintf(stderr, "\t Invalide command ! \n\tThe valide commands are as follows: get [filename], prt [portnum], dir and bye. \n");
        return NULL;
    }
}

void printcommand(struct command* c)
{
    if(!DEBUG)
        return;

    printf("\t\tPrinting contents of the above command...\n");
	printf("\t\tid = %d\n", c->id);
	printf("\t\targuments = %d\n", c->arguments);
	printf("\t\targname = %s\n", c->argname);
}

// get file on FTP Server
void command_get(struct packet* chp, struct packet* data, int sfd_client, char* filename)
{
    FILE* f = fopen(filename, "wb");
    if(!f)
    {
        fprintf(stderr, "File could not be opend for writting. Aborting...\n");
        return;
    }
    int x;
    set0(chp);
    chp->type = REQU;
    chp->conid = -1;
    chp->comid = GET;
    strcpy(chp->buffer, filename);
    data = htonp(chp);

    if(DEBUG)
        printf(ID "before send get command...\n");

    if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);

    if(DEBUG)
        printf(ID "before recevie from server...\n");

    if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
        er("recv()", x);
    chp = ntohp(data);
    
    if(DEBUG)
        printf(ID "begin recieving file...\n");

    printpacket(chp, HP);

    if(chp->type == INFO && chp->comid == GET && strlen(chp->buffer))
    {
        printf("\t%s\n", chp->buffer);
        //receive_file(chp, data, sfd_client, f);
        receive_file(chp, data, sfd_server_data, f);
        fclose(f);
    }
    else
        fprintf(stderr, "Error getting remote file : <%s>\n", filename);

}

// the prt command
void command_prt(struct packet* chp, struct packet* data, int sfd_client, char* portnum)
{
    // data channel initialization starts
    struct sockaddr_in skin_server_data, skin_client_data;
    int sfd_client_data, x;
    short int connection_id = 0;
    DATAPORT = atoi(portnum);
    if(DATAPORT < 1024 || DATAPORT > 65535)
    {
        er("port number must be between 1024 and 65525...", DATAPORT);
        return;
    }

    if(( x = sfd_client_data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        er("socket()", x);

    memset((char*) &skin_client_data, 0, sizeof(struct sockaddr_in));
    skin_client_data.sin_family = AF_INET;
    skin_client_data.sin_port = htons(DATAPORT);
    skin_client_data.sin_addr.s_addr = htonl(INADDR_ANY);

    if(( x = bind(sfd_client_data, (struct sockaddr*) &skin_client_data, size_sockaddr)) < 0)
        er("bind()", x);

    if(( x = listen(sfd_client_data, 1)) < 0)
        er("listen()", x);

    printf(ID "Client data channel started up @ local: %d. Waiting for data transfering...\n\n", DATAPORT);
    // data channel initialization ends
    
    // send prt command to server and wait for connection
    set0(chp);
    chp->type = REQU;
    chp->conid = -1;
    chp->comid = PRT;
    char temp[10];
    strcpy(chp->buffer, portnum);

    data = htonp(chp);
    
    if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);
    chp = ntohp(data);

    printpacket(chp, HP);

    while(chp->type != EOT)
    {
        if(chp->type == REQU && chp->comid == PRT && strlen(chp->buffer))
            ;//printf("\t%s\n", chp->buffer);
        /*
        else
            fprintf(stderr, "\t Error executing command on server.\n");
        */

        if(( x = recv(sfd_client, data, size_packet, 0)) <= 0)
            er("recv()", x);
        chp = ntohp(data);

        printpacket(chp, HP);
       
        if(DEBUG)
            printf(ID "OK, begining connection\n");

        // begin conntction in data channel with server
        if(( x = sfd_server_data = accept(sfd_client_data, (struct sockaddr*) &skin_server_data, &size_sockaddr)) < 0)
            er("accept()", x);
        printf(ID "Communication started in data channel with server @ %s:%d\n", inet_ntoa(skin_server_data.sin_addr), ntohs(skin_server_data.sin_port));
        
        prt_flag = DATAPORTBUILT;
        
        fflush(stdout);

        
        if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
            er("recv()", x);
        chp = ntohp(data);
    }
}

// the dir command
void command_dir(struct packet* chp, struct packet* data, int sfd_client)
{
    int x;
    set0(chp);
    chp->type = REQU;
    chp->conid = -1;
    chp->comid = DIR_;
    data = htonp(chp);
    if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);
    while(chp->type != EOT)
    {
        // printpacket(chp, HP);
        // printpacket(data, HP);
        if(chp->type == DATA && chp->comid == DIR_ && strlen(chp->buffer))
            printf("\t%s\n", chp->buffer);
        /*
        else
            fprintf(stderr, "\t Error executing command on server.\n");
        */
        
        //if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
        if(( x = recv(sfd_server_data, data, size_packet, 0)) <= 0)
            er("recv()", x);
        chp = ntohp(data);
    }

}

// the bye command
void command_bye(struct packet* chp, struct packet* data, int sfd_client)
{
    int x;
    set0(chp);
    chp->type = REQU;
    chp->conid = -1;
    chp->comid = BYE;
    data = htonp(chp);
    if(( x = send(sfd_client, data, size_packet, 0)) != size_packet)
        er("send()", x);

    printf(ID "Data channel closed!\n");
    close(sfd_server_data);
    printf(ID "Command channel closed!\n");
    close(sfd_client);
    connect_flag = ISNOTCONNECT;
    prt_flag = DATAPORTNOTBUILT;
    printf(ID "Connection with server has been closed! Bye!\n");
    fflush(stdout);
    // exit
    return;
}
