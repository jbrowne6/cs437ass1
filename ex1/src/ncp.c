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

#define PORT	    10210
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
{				//this should be defined in a header file so both can use it.
  char header[HEADER_LEN];
  char data[MAX_DATA_SIZE];
} packet;

typedef struct
{
  packet win[MAX_WINDOW_SIZE];
   packet *start_of_window;	//points to the start of the window
   packet *end_of_window;	//points to the end of the window
} window;


int
main (int argc, char **argv)
{
  char *current_state;
  struct sockaddr_in name;
  struct sockaddr_in send_addr;
  struct sockaddr_in from_addr;
  socklen_t from_len;
  struct hostent h_ent;
  struct hostent *p_h_ent;
  char my_name[NAME_LENGTH] = { '\0' };
  int host_num;
  int from_ip;
  int ss, sr;
  fd_set mask;
  fd_set dummy_mask, temp_mask;
  int bytes;
  int num;
  char msg_buff[MAX_DATA_SIZE];	//use to read from file
  char input_buf[80];
  struct timeval timeout;


  if (argc < 5)
    {
      perror
	("ncp <loss_rate_percent> <source_file_name> <dest_file_name> <host_name>\n \
				where <comp_name> is the name of the computer where the server runs \
				(ugrad1, ugrad2, etc.)");
      exit (1);
    }

  // grab CLI
  int loss_rate = (int) strtol(argv[1], NULL, 0);
  char *source_file_name = argv[2];
  char *dest_file_name = argv[3];
  char *host_name = argv[4];

  // create a socket to rcv data on
  sr = socket (AF_INET, SOCK_DGRAM, 0);	/* socket for receiving (udp) */
  if (sr < 0)
    {
      perror ("Ucast: socket");
      exit (1);
    }

  // set essential socket properties
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = INADDR_ANY;
  name.sin_port = htons (PORT);

  // bind our receive socket to PORT
  if (bind (sr, (struct sockaddr *) &name, sizeof (name)) < 0)
    {
      perror ("Ucast: bind");
      exit (1);
    }

  // create a socket to send packets out on
  ss = socket (AF_INET, SOCK_DGRAM, 0);	/* socket for sending (udp) */
  if (ss < 0)
    {
      perror ("Ucast: socket");
      exit (1);
    }

  // do a dns lookup on host_name
  p_h_ent = gethostbyname (host_name);
  if (p_h_ent == NULL)
    {
      printf ("Ucast: gethostbyname error.\n");
      exit (1);
    }

  // copy over essential data from our dns lookup
  memcpy (&h_ent, p_h_ent, sizeof (h_ent));
  memcpy (&host_num, h_ent.h_addr_list[0], sizeof (host_num));

  // set our tx socket properties
  send_addr.sin_family = AF_INET;
  send_addr.sin_addr.s_addr = host_num;
  send_addr.sin_port = htons (PORT);

  // zero out our masks later used for select
  // and set our receive socket in the mask
  FD_ZERO (&mask);
  FD_ZERO (&dummy_mask);
  FD_SET (sr, &mask);

packet working_packet;
packet* working_packet_ptr = &working_packet;
int working_packet_type;
int working_packet_size;
int working_packet_index;
  // main app loop
  while (1){
   
/*STATE_0 sends a hello then waits for (1) A Hello back.  (2) a busy.  (3) time to run out.*/
      if (current_state == STATE_0)	//needs strcmp instead.  int compare is better
	{
working_packet_type = 4;//Hello packet
working_packet_size = MAX_DATA_SIZE;
working_packet_index = 0;
packet_maker(working_packet_ptr,working_packet_type, working_packet_index, working_packet_size,(void*)working_packet_ptr);
	  sendto (ss, (char *)working_packet_ptr ,MAX_MSG_LEN, 0,
		  (struct sockaddr *) &send_addr, sizeof (send_addr));

/*	  sendto (ss, input_buf, strlen (input_buf), 0,
		  (struct sockaddr *) &send_addr, sizeof (send_addr));
*/	}
      else if (current_state == STATE_1)
	{
	  // do state_1 stuff
	}


    }

  for (;;)
    {
      temp_mask = mask;
      timeout.tv_sec = TIMEOUT_SEC;
      timeout.tv_usec = TIMEOUT_USEC;
      num =
	select (FD_SETSIZE, &temp_mask, &dummy_mask, &dummy_mask, &timeout);
      if (num > 0)
	{
	  // rx process
	  if (FD_ISSET (sr, &temp_mask))
	    {
	      from_len = sizeof (from_addr);
	      bytes = recvfrom (sr, msg_buff, sizeof (msg_buff), 0,
				(struct sockaddr *) &from_addr, &from_len);
	      msg_buff[bytes] = 0;
	      from_ip = from_addr.sin_addr.s_addr;

	      printf ("Received from (%d.%d.%d.%d): %s\n",
		      (htonl (from_ip) & 0xff000000) >> 24,
		      (htonl (from_ip) & 0x00ff0000) >> 16,
		      (htonl (from_ip) & 0x0000ff00) >> 8,
		      (htonl (from_ip) & 0x000000ff), msg_buff);


	    }
	  else if (FD_ISSET (0, &temp_mask))
	    {
	      bytes = read (0, input_buf, sizeof (input_buf));
	      input_buf[bytes] = 0;
	      printf ("There is an input: %s\n", input_buf);
	      sendto (ss, input_buf, strlen (input_buf), 0,
		      (struct sockaddr *) &send_addr, sizeof (send_addr));
	    }
	}
      else
	{
	  //we've timed out
	  printf (".");
	  fflush (0);
	}
    }

  return 0;

}

/*send pointer to block in window array.  Then format that block as a packet ready to send.*/
void
packet_maker (packet * x, int packet_type, int index, int size,void *data_buffer)

{

  if (packet_type == 0)
    {				//this is the case when flags are 000 -> data packet
//int *head_ptr = (int *) x; // this might be a better way.  
      ((int *)x)[0] = 0;
      ((int *)x)[0] = (packet_type << 29) | (size << 16) | (index);	// this sets the header.

      memcpy (((int *) x) + 1, data_buffer, size);	//copies the data from the buffer into the window.
    }
  else if (packet_type = 4)
    {				//4 is 100 -> hello packet

      ((int *) x)[0] = 0;
    ((int *) x)[0] = (packet_type << 29);}
  else if (packet_type = 7)
    {				//7 is 111 -> busy packet
    }
  else if (packet_type = 2)
    {				//2 is 010 -> Ack packet
    }
  else if (packet_type = 1)
    {				//1 is 001 -> Nack packet
    }
  else if (packet_type = 3)
    {				//3 is 011 -> Last packet
    }
  else
    {				//handle malformed packet_type
    }
}

void read_packet_header(char *x, int * packet_type, int* size, int* index){//This just provides header info
*packet_type = (((int*)x)[0] & 0xE0000000)>>29;
*size =  (((int*)x)[0] & 0x1FFF0000)>>16;
*index = (((int*)x)[0] & 0x0000FFFF);
}


