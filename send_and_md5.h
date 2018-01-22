#include <openssl/md5.h>



void sendFile(int sockfd , FILE *sourcefp ) {
    
unsigned char hash5[MD5_DIGEST_LENGTH];
MD5_CTX ctx;
MD5_Init(&ctx);
unsigned char databuff[1024];
unsigned int n ;
  
  memset(databuff,0,sizeof(databuff));
    
    while((n = fread(databuff,sizeof(char),1024,sourcefp)) > 0 ){
     	
     	
      	send(sockfd , databuff, n , 0);
      	MD5_Update(&ctx,databuff,n);
      	
      	if(n < 1024){
    	  	if(feof(sourcefp)){
    	  		printf("file transfer complete!\n");
    	  		break;
    	  	}
    	  	
    	  	if(ferror(sourcefp)){
    	  		printf("Error reading\n");
    	  	break;
    	  	}
      	}
      	
      	bzero(databuff , sizeof(databuff));
  
  }
  
  MD5_Final(hash5,&ctx);
  
  for(n=0; n<MD5_DIGEST_LENGTH; n++){
        printf("%02x", hash5[n]);
   }

}
