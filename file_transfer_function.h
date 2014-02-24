/*
 * Function: file transfer
 *
 * Author    :    Daniel Lai
 * Created On:    2014-02-21
 */

#include <commons.h>

void send_EOT(struct packet*, struct packet*, int);
void send_TERM(struct packet*, struct packet*, int);

void send_file(struct packet*, struct packet*, int, FILE*);
void receive_file(struct packet*, struct packet*, int, FILE*);
