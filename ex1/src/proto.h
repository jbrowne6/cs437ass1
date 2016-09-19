#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define PORT	    10200
#define MAX_MSG_LEN 1400	//the total allowed size of our packet
#define HEADER_LEN 4		//the length of the header
#define NAME_LENGTH 80		//the max length of the file??
#define WINDOW_SIZE 1000	//intial window size, this is fixed for now
#define MAX_WINDOW_SIZE 64000	// this should be 2^16
#define TIMEOUT_SEC 1
#define TIMEOUT_USEC 0
#define MAX_DATA_SIZE 1396	//lets calculate this
#define STATE_0 "state0"
#define STATE_1 "state1"
#define MASK_MSG_TYPE 0x3
#define MASK_MSG_SIZE 0x0	//not sure what the purpose of this one is

typedef struct
{ 				//this should be defined in a header file so both can use it.
  char header[HEADER_LEN];
  char data[MAX_DATA_SIZE];
} packet;

typedef struct
{
  packet win[MAX_WINDOW_SIZE];
   packet *start_of_window;	//points to the start of the window
   packet *end_of_window;	//points to the end of the window
} window;

void packet_maker (packet * x, int packet_type, int index, int size,void *data_buffer);

void read_packet_header(packet *x, int * packet_type, int* size, int* index);//This just provides header info



