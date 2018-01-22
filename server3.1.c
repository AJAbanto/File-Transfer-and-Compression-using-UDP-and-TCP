
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "recieve_and_md5.h"
#include "rle.h"

#include <openssl/md5.h>

long GetFileSize( FILE *f )
{
  long pos, size;

  pos = ftell( f );
  fseek( f, 0, SEEK_END );
  size = ftell( f );
  fseek( f, pos, SEEK_SET );

  return size;
}


int ReadWord32( FILE *f )
{
  unsigned char buf[4];
  fread( buf, 4, 1, f );
  return (((unsigned int)buf[0])<<24) +
         (((unsigned int)buf[1])<<16) +
         (((unsigned int)buf[2])<<8)  +
           (unsigned int)buf[3];
}




int main(int argc, char **argv)
{
  int sockfd,acceptfd;
  struct sockaddr_in servaddr,cliaddr;
  socklen_t len = sizeof(cliaddr);
  char cli_ip[32];
  unsigned char *in, *out, command, algo=1;
 
 
  sockfd = socket(PF_INET,SOCK_STREAM,0);
  if(sockfd<0) {
	perror("socket: ");
	return -EIO;
  }

  memset(&servaddr, 0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(12345);
  if ( bind( sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr) ) < 0 ){
	perror("bind: ");
	close(sockfd);
	return -EIO;
  }

  //not present in udp server
  if(listen(sockfd,2)<0){
	perror("listen: ");
	close(sockfd);
	return -EIO;
  }
  
  printf("Server Running...\n");
  
 
   
  	FILE *containerfp = fopen("recieved3.jpg" , "wb");
  	if(containerfp == NULL){
  		printf("File open error (containerfp) \n");
  		return 1;
 	 }	
	FILE *decompressed = fopen("decompressed.jpg","wb");
 	 if(decompressed == NULL){
  		printf("File open error (decompressed) \n");
  		return 1;
 	 }

  //----------------accepting connection------------
  
  while(1){

	
 

	  //not present in udp server
	if( (acceptfd = accept(sockfd,(struct sockaddr *)&cliaddr,&len)) < 0 ){
		perror("accept: ");
		continue;
	}
	inet_ntop(AF_INET,(struct in_addr *) &cliaddr.sin_addr, cli_ip, sizeof(cli_ip) );
	printf("Client %s connected. \n",cli_ip);	
	  
	
	
	printf("recieving data....\n");	
	
	recieveFile(acceptfd , containerfp );
        printf("recieving file complete\n");
   	
 	//fclose(containerfp);

//--------------Decompression---------------------//

	FILE *f;
	
        
	printf("Decompressing...\n");	
       
	f = fopen( "recieved3.jpg", "rb" );
   	 if( !f )
   	 {
        printf( "Unable to open input file\n" );
        return 0;
  	  }

	//unsigned char *in, *out, command, algo=1;
   	unsigned int  insize=0, outsize=0, *work;
    	char *inname, *outname;

	// Get input file size 
    	insize = GetFileSize( f );

	/* Decompress? */
	    if( 1 )
	    {
		/* Read header */
		algo = ReadWord32( f);  /* Dummy */
		algo = ReadWord32( f );
		outsize = ReadWord32( f);
		insize -= 12;
	    }

	printf("%d algo , outsize is %u \n", algo, outsize);
	
	/* Read input file */
    	printf( "Input file: %u bytes\n", insize );
    	in = (unsigned char *) malloc( insize );
    	if( !in )
	    {
		printf( "Not enough memory\n" );
		fclose( f );
		return 0;
	    }
	    fread( in, insize, 1, f );
	    fclose( f );
	
	/* Show output file size for decompression */
        if(1)
           {
           printf( "Output file: %u bytes\n", outsize );
           }

	/* Allocate memory for output buffer */ 
        out = malloc( outsize );
        if( !out )
           {
        printf( "Not enough memory\n" );
        fclose( decompressed );
        free( in );
        return 0;
           }

	
	switch( algo )
        {
            case 1:
                RLE_Uncompress( in, out, insize );
                break;
	}
	
		/* Write output file */
	   fwrite( out, outsize, 1, decompressed );
	   fclose( decompressed );

	

	
//--------------------------------------------------//
        


        printf("Decompression complete\n\n\n\n");
	 
	close(acceptfd);
         
       
  }
 
   /* Free memory*/
	    free( in );
	    free( out );

  close(sockfd);
  
  return 0;
}

