#include "proto.h"
/*send pointer to block in window array.  Then format that block as a packet ready to send.*/

void zero_packet(packet *x, const int size);


void packet_maker (packet *x, int packet_type, int index, int size,void *data_buffer)
{
zero_packet(x, size);
  if (packet_type == 0)
    {				//this is the case when flags are 000 -> data packet
//int *head_ptr = (int *) x; // this might be a better way.  
      ((int *)x)[0] = 0;
      ((int *)x)[0] = (packet_type << 29) | (size << 16) | (index);	// this sets the header.
      memcpy (((int *) x) + 1, data_buffer, size);	//copies the data from the buffer into the window.
    }
  else if (packet_type == 4)
    {				//4 is 100 -> hello packet

      ((int *) x)[0] = 0;
    ((int *) x)[0] = (packet_type << 29);}
  else if (packet_type == 7)
    {				//7 is 111 -> busy packet
    }
  else if (packet_type == 2)
    {				//2 is 010 -> Ack packet
    }
  else if (packet_type == 1)
    {				//1 is 001 -> Nack packet
    }
  else if (packet_type == 3)
    {				//3 is 011 -> Last packet
    }
  else
    {				//handle malformed packet_type
    }
}

void read_packet_header(packet *x, int *packet_type, int *size, int *index){//This just provides header info
*packet_type = (((int*)x)[0] & 0xE0000000)>>29;
*size =  (((int*)x)[0] & 0x1FFF0000)>>16;
*index = (((int*)x)[0] & 0x0000FFFF);
}

void zero_packet(packet *x, const int size){
int i;

for(i=0; i<size/sizeof(char); i++)
((char *)x)[i] = 0;

}
