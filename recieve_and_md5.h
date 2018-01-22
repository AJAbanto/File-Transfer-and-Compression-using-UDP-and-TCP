#include <openssl/md5.h>


void recieveFile (int acceptfd , FILE *containerfp) {

unsigned char databuff[1024];
unsigned char hash5[MD5_DIGEST_LENGTH];
long int datarecieved; 
long int datatracker = 0 ;
MD5_CTX ctx;
MD5_Init(&ctx);

memset(databuff,0,sizeof(databuff));

//--------------------recieving loop--------------------------

while((datarecieved = recv(acceptfd, databuff,1024,0)) > 0 ){
		
	  
	  datatracker = datarecieved + datatracker;
	  fwrite(databuff,sizeof(char), datarecieved, containerfp);
	  MD5_Update(&ctx,databuff,datarecieved);
	  if(datarecieved < 0 ) {
	  	printf("error in recieving data\n");
	  }
	  
	  memset(databuff,0,sizeof(databuff));
	  
	 }
	 
//----------------------printing hash --------------------
	 
	 MD5_Final(hash5,&ctx);
	 for(datarecieved=0; datarecieved<MD5_DIGEST_LENGTH; datarecieved++){
        printf("%02x", hash5[datarecieved]);
     }
        printf("\n");
    
     
}
	 