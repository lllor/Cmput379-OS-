#include <stdio.h>
#include <openssl/md5.h>
#include <string.h>
int main()
{
    unsigned char c[MD5_DIGEST_LENGTH];
    char *filename="1.txt";
    int i;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) 
	{
        printf ("%s can't be opened.\n", filename);
        return 0;
    }
    printf("%d\n",MD5_DIGEST_LENGTH);
    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    char result[MD5_DIGEST_LENGTH*2] ={0}; 
    char temp[2]={0};
    printf("%d\n",MD5_DIGEST_LENGTH);
    for(i = 0; i < MD5_DIGEST_LENGTH; i++){
        printf("%02x", c[i]);
        //printf("%lu,%x\n",sizeof(c[i]),c[i]);
        sprintf(temp,"%02x",c[i]);
        strcat(result,temp);
        //result[i] == (hex)c[i];
    } 
    printf ("%s\n", filename);
    
    printf("%s\n",result);

    //printf ("%s\n",c);
    fclose (inFile);
    return 0;
}
