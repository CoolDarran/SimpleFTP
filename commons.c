/*
 * Function: implementation of commons.h
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */

#include <commons.h>

static size_t size_packet = sizeof(struct packet);

void set0(struct packet* p)
{
    memset(p, 0, sizeof(struct packet));
}

// network packet transforms to host packet
struct packet* ntohp (struct packet* np)
{
    struct packet* hp = (struct packet*) malloc(size_packet);
    memset(hp, 0, size_packet);

    hp->conid = ntohs(np->conid);
    hp->type = ntohs(np->type);
    hp->comid = ntohs(np->comid);
    hp->datalen = ntohs(np->datalen);
    memcpy(hp->buffer, np->buffer, LENBUFFER);

    return hp;
}

// host packet transforms to network packet
struct packet* htonp (struct packet* hp)
{
    struct packet* np = (struct packet*) malloc(size_packet);
    memset(np, 0, size_packet);

    np->conid = ntohs(hp->conid);
    np->type = ntohs(hp->type);
    np->comid = ntohs(hp->comid);
    np->datalen = ntohs(hp->datalen);
    memcpy(np->buffer, hp->buffer, LENBUFFER);

    return np;
}

// print the information of a packet
void printpacket(struct packet* p, int ptype)
{
    if(!DEBUG)
        return;
    if(ptype)
        printf("\tHOST PACKET\n");
    else
        printf("\tNETWORK PACKET\n");

    printf("\t\tconid = %d\n", p->conid);
    printf("\t\ttype = %d\n", p->type);
    printf("\t\tcomid = %d\n", p->comid);
    printf("\t\tdatalen = %d\n", p->datalen);
    printf("\t\tbuffer = %s\n", p->buffer);

    fflush(stdout);
}

