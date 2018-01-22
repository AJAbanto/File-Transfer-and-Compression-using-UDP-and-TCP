#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "send_and_md5.h"
#include "rle.h"
#include <stdlib.h>

#include <openssl/md5.h>

long GetFileSize( FILE *f )		//check filesize
{
  long pos, size;

  pos = ftell( f );
  fseek( f, 0, SEEK_END );
  size = ftell( f );
  fseek( f, pos, SEEK_SET );

  return size;
}


void WriteWord32( int x, FILE *f )
{
  fputc( (x>>24)&255, f );
  fputc( (x>>16)&255, f );
  fputc( (x>>8)&255, f );
  fputc( x&255, f );
}



int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr;
  socklen_t len = sizeof(servaddr);
  long int tracker = 0;


  if(argc!=7){ 
	printf("Usage: %s -i <ip_addr> -p <port> -f <file_name>\n",argv[0]);
	return -EINVAL; 
  }


//----------------------argument handling ------------------------//

   char *filename = calloc(100,sizeof(char));
   unsigned int port_num;
   char *ip_add = calloc(9, sizeof(char));;
   int i , i3;

   for(i = 1 , i3 = 0; i3 < 3; i++){
	if(!(strcmp(argv[i], "-f"))){
		strcpy(filename, argv[i+1]);
		i3++;
	}else if (!(strcmp(argv[i], "-p"))){
		port_num = atoi (argv[i+1]);
		i3++;
	}else if(!(strcmp(argv[i], "-i"))){
		strcpy(ip_add, argv[i+1]);
		i3++;
  	}
	
	if(i > argc) i=0;
 
    }
printf("test\n");
	printf("port: %u \n", port_num);
	printf("File: %s\n", filename);
	printf("IP address: %s\n" , ip_add);

//----------------------socket creation and setup------------------------//
  sockfd = socket(PF_INET,SOCK_STREAM,0);
  if(sockfd<0) {
	perror("socket: ");
	return -EIO;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port_num);
  inet_pton(AF_INET,ip_add,&servaddr.sin_addr);

  //not present in udp clients
  if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
	perror("connect: ");
	close(sockfd);
	return -EIO;
  }

//-------------------------Opening File----------------------------------

  FILE *sourcefp = fopen(filename , "r");
  if(sourcefp == NULL){
  	printf("File open error\n");
  	return 1;
  }

  FILE *compressed = fopen("compressing.jpg" ,"w+");
  if(compressed == NULL){
  	printf("File open error (compressed) \n");
  	return 1;
  }

//-----------------------File compression--------------------------------//

    unsigned char *in, *out, command, algo=1;
    unsigned int  insize, outsize=0, *work;
    char *inname, *outname;
    
    
    /* Get input file size */
    insize = GetFileSize( sourcefp );
    
    /* Read input file */
    printf( "Input file: %u bytes\n", insize );
    in = (unsigned char *) malloc( insize );
    if( !in )
    {
        printf( "Not enough memory\n" );
        fclose( sourcefp);
        return 0;
    }
    fread( in, insize, 1, sourcefp );
    fclose( sourcefp );
  
    /* Write header */
        fwrite( "BCL1", 4, 1, compressed );
        WriteWord32( algo, compressed );
        WriteWord32( insize, compressed );

        /* Worst case buffer size */
        outsize = (insize*104+50)/100 + 384;

     /* Allocate memory for output buffer */
    out = malloc( outsize );
    if( !out )
    {
        printf( "Not enough memory\n" );
        fclose( compressed );
        free( in );
        return 0;
    }


    switch( algo )
        {
            case 1:
                outsize = RLE_Compress( in, out, insize );
                break;
        }

     /* Write output file */
    fwrite( out, outsize, 1, compressed );
	printf("output size %u \n" , outsize);
    /* Free memory */
    free( in );
    free( out );

//-----------------------File Transmission----------------------------------//
  

    

  printf("initiating transfer now\n");
  fseek(compressed, 0, SEEK_SET);
  sendFile(sockfd, compressed); 
  
  printf("\ntransfer complete\n");
  printf("\n");
  close(sockfd);
  
  
  return 0;
}

