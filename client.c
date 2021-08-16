// socket client multi clients
// File: client.c
// compile: gcc -Wall client.c -o client
// run: ./client localhost
#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include <unistd.h>        	          // for read/write/close
#include <sys/types.h>     	          /* standard system types        */
#include <netinet/in.h>    	          /* Internet address structures */
#include <sys/socket.h>   	          /* socket interface functions  */
#include <netdb.h>         	          /* host to IP resolution            */

#define  BUFLEN        100          /* maximum response size     */
#define  DEFAULT_PORT 80           /* DEFAULT PORT              */
#define  REQUEST " HTTP/1.0\r\nHost: "

int usageError();
void freeValues(char*, char* );
void error(char*); // error fucntion
int checkUrlCell(char *); // check if url exist on our command line
int checkPCell(char *); // check if -p exist on our command line
int checkRCell(char *); // check if -r exist on our command line
int digitAfterR(char *); // check if coming number after -r
int checkProperAfterR(char *); // check if all values after -r are like d=d w=w q=q y=y
int parseUrl(int *, char * , char ** , char **); // Function to parse the url and get PORT / PATH / HOST

int main(int argc, char *argv[])
{
    int rc;            /* system calls return value storage */
    int sockfd;
    int num ;
    int port = DEFAULT_PORT;
    char *hostBuff;
    char *pathBuff;
    char *tmpUrl = NULL;
    char *message = NULL;
    char rbuf[BUFLEN];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int flag ;
    int isUrlExist = -1;
    int isPExist = -1;
    int isRExist = -1;
    int countR = 0;
    
    /*================= check -r cell on argv ==================== */
    for(int i = 0 ; i < argc; i++)
    {
        isRExist = checkRCell(argv[i]);
        if(isRExist == 1)
        {
            isRExist = i;
            break;
        }
    }
    /*=================== END -r CHECK ============================ */

    /*=================== check -p cell on argv =================== */
    for(int i = 0; i < argc ; i++)
    {
        isPExist = checkPCell(argv[i]);
        if(isPExist == 1)
        {
            isPExist = i;
            break;
        }
    }
    /*======================end -p CHECK ========================== */
      /* ======== Check URL Location + If Exist + ParseURL ========== */
    for(int i = 0 ; i < argc ; i++)
    {
        if( isPExist == 0)
            isUrlExist = checkUrlCell(argv[i]);
        else if (isPExist+1 != i)
            isUrlExist = checkUrlCell(argv[i]);
        if(isUrlExist == 1)
        {
            isUrlExist = i;
            tmpUrl = argv[i];
            break;
        }
    }
   // printf("IS URL EXIST = %d isPexist = %d isPexist+1 = %d\n",isUrlExist,isPExist,isPExist+1);
    /* =================== END URL Check ========================= */
   /* ====================== CHECK TERMS TO RIGHT REQUEST =============== */

   if(isRExist+1 == argc || isPExist+1 == argc)
   {
    usageError();
   }

    int digitR = 0;
    if(isRExist != 0)
        digitR = digitAfterR(argv[isRExist+1]);
    
    // if(digitR == 0)
    //     isRExist = 0;

   

    // int amountUrl = 0;
    // int amountP = 0;
    // int amountR = 0;
    // for(int i = 0 ; i<argc ; i++)
    // {
    //     amountUrl = amountUrl + checkUrlCell(argv[i]);
    //     amountP   = amountP   + checkPCell(argv[i]);
    //     amountR   = amountR    + checkRCell(argv[i]);
    // }

    // int isUrlInP = 0;
    // if(isPExist != 0)
    //     isUrlInP = checkUrlCell(argv[isPExist+1]);

    // if((amountUrl > 1 && isUrlInP==0) || amountP > 1 || amountR > 1)
    //     usageError();
    
  
    if(isRExist == argc || isPExist == argc)
        usageError();

  
    if((isPExist+1 == isRExist ) && isPExist != 0) //check if the situation -p -r not happen, it get space like -p _ -r
        usageError();
    
    if(isRExist != 0 && digitR  == 0) //check if -r *number*. exapmle -r 1 . and no -r aw=30
        usageError();
    
    int last=0;
    // checking where does -r paramenters done
    // if(digitR > 0)
    // {
        last=0;
        if(isRExist > isUrlExist && isRExist > isPExist)
            last = argc;
        else if(isRExist < isPExist && isPExist < isUrlExist && isPExist != 0)
            last = isPExist;
        else if(isRExist < isUrlExist && isUrlExist < isPExist && isPExist != 0)
            last = isUrlExist;
        else if(isRExist > isUrlExist && isRExist < isPExist && isPExist != 0)
            last = isPExist;
        else if(isRExist > isPExist && isRExist < isUrlExist && isPExist !=0)
            last = isUrlExist;
        else if(isPExist == 0 && isRExist < isUrlExist)
            last = isUrlExist;
        else
            last = argc;
        
        // check if there are enough parameters
        for(int i = isRExist+2 ; i < last ; i++)
            countR = countR + checkProperAfterR(argv[i]); //counter the proper values and check if it the same as our number after -r
        if(countR != digitR) // not enough values / or bad structure of the following
        {
            usageError();
        }
    //}
   
//    if(isRExist == 0)
//     {
//             for(int i = 0 ; i < argc; i++)
//             {
//                 isRExist = checkRCell(argv[i]);
//                 if(isRExist == 1)
//                 {
//                     isRExist = i;
//                     break;
//                 }
//             }
//     }
    // check if no spare words in our program
    int numOfParams = 0;
    if(isPExist == 0 && isRExist != 0)
        numOfParams = 3+countR;
    else if(isPExist != 0 && isRExist == 0)
        numOfParams = 3;
    else if(isPExist != 0 && isRExist != 0)
        numOfParams = 5 + countR;
    else if(isPExist == 0 && isRExist == 0)
        numOfParams =1;
    // printf("isUrlExist = %d , isRExist =%d , isPexist=%d countR = %d digitR = %d\n",isUrlExist,isRExist,isPExist,countR,digitR);
    // printf("totalParam = %d argc = %d\n",numOfParams,argc);
    if(numOfParams+1 != argc)
        usageError();

    if(digitR == 0)
        isRExist=0;

    
  
    
    
    /* ====================== END OF CHECKING TERMS ====================== */
    
     if(tmpUrl != NULL) // if all terms are ok we parse the url
     {
        flag = parseUrl(&port,tmpUrl,&hostBuff,&pathBuff);
        if(flag < 1)
            printf("flag < 1");
     }
     else
        error("URL NOT EXIST");

    /* ====================== CREATE MESSAGE ============================== */
    // find the size of the message we create
    int size = 0;
    if(isRExist != 0) // check if we need to add R values
    {
        for(int i = isRExist+2 ; i < last ; i++)
            size = size + strlen(argv[i]);
    }

    if(isPExist != 0) // check if we need to add P values
        size = size + strlen(argv[isPExist+1]);

    size = size + strlen(hostBuff) + strlen(pathBuff); // add host + path
    
    message = (char*)calloc(size+100 , sizeof(char)); // we add +100 in case for the strings

    //creating the message
    if(isUrlExist != 0 && isRExist == 0 && isPExist == 0) // simple get message without -r -p
    {
        strcat(message,"GET ");
        if(strlen(pathBuff)<=1)
            strcat(message,pathBuff);
        else
        {
            strcat(message,"/");
            strcat(message,pathBuff);
        }
        strcat(message,REQUEST);
        strcat(message,hostBuff);
        strcat(message,"\r\n\r\n");
        //printf("message = %s LEN = %d\n",message,strlen(message));
    }
    else if(isUrlExist != 0 && isRExist != 0 && isPExist == 0) // message without -p
    {
        strcat(message,"GET ");
        if(strlen(pathBuff)<=1)
            strcat(message,pathBuff);
        else
        {
            strcat(message,"/");
            strcat(message,pathBuff);
        }
        strcat(message,"?");
        for(int i = isRExist+2 ; i<last-1 ; i++)
        {
            strcat(message,argv[i]);
            strcat(message,"&");
        }
        strcat(message,argv[last-1]);
        strcat(message,REQUEST);
        strcat(message,hostBuff);
        strcat(message,"\r\n\r\n");
       
    }
    else if(isUrlExist != 0 && isRExist == 0 && isPExist != 0) // message without -r
    {
        char contLen[2];
        sprintf(contLen,"%ld",strlen(argv[isPExist+1]));
        strcat(message,"POST ");
        if(strlen(pathBuff)<=1)
            strcat(message,pathBuff);
        else
        {
            strcat(message,"/");
            strcat(message,pathBuff);
        }
        strcat(message,REQUEST);
        strcat(message,hostBuff);
        strcat(message,"\n");
        strcat(message,"Content-length:");
        strcat(message,contLen);
        strcat(message,"\r\n\r\n");
        strcat(message,argv[isPExist+1]);
        
    }
    else // message with all 3 contents
    {
        char contLen[2];
        sprintf(contLen,"%ld",strlen(argv[isPExist+1]));
        strcat(message,"POST ");
        if(strlen(pathBuff)<=1)
            strcat(message,pathBuff);
        else
        {
            strcat(message,"/");
            strcat(message,pathBuff);
        }
        strcat(message,"?");
        for(int i = isRExist+2 ; i<last-1 ; i++)
        {
            strcat(message,argv[i]);
            strcat(message,"&");
        }
        strcat(message,argv[last-1]);
        strcat(message,REQUEST);
        strcat(message,hostBuff);
        strcat(message,"\r\n");
        strcat(message,"Content-length:");
        strcat(message,contLen);
        // need to add
        strcat(message,"\r\n\r\n");
        strcat(message,argv[isPExist+1]);
    }
    printf("HTTP request =\n%s\nLEN = %ld\n", message, strlen(message));// print the requied message
    /* ===================== END OF CREATING MESSAGE ======================== */
    
    /* ====================== CREATING CONNECTION ============================ */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        freeValues(hostBuff,pathBuff);
        error("socket failed");
    }


    // connect to server
    server = gethostbyname(hostBuff);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n"); 
        freeValues(hostBuff,pathBuff);
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    rc = connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (rc < 0) 
    {
        freeValues(hostBuff,pathBuff);
        error("connect failed:");
    }

    /* ==================== FINISH CONNECTION ============================ */

    /* ==================== COMMUNICATE WITH SERVER  ===================== */
    num=write(sockfd,message,strlen(message)+1);
    if(num < 0)
        error("Write < 0\n");
    
    int toRead = -1;
    int howManyRead = 0;
    rbuf[BUFLEN-1] = '\0'; // to read without garbage values
    while(toRead != 0)
    {
        toRead = read(sockfd,rbuf,BUFLEN-1);
        howManyRead = howManyRead+toRead;
        if(toRead > 0)
            printf("%s\n",rbuf);
         else
            break;
            // perror("read() failed");
    }
    /* ====================== COMMUNICATION DONE ======================== */
    close(sockfd);
    printf("\n Total received response bytes: %d\n",howManyRead); 
    //free values when we end
    if(message!=NULL)
        free(message);
    if(strlen(pathBuff)==1 && pathBuff[0] == '/')
    {
        //free(pathBuff);
        pathBuff = NULL;
        free(hostBuff);
    }
    else
        freeValues(pathBuff,hostBuff);
    
    return EXIT_SUCCESS;
}

int parseUrl(int *port ,char *url, char **host , char **path )
{ 

    /* ============ To Parse the URL ============ */
    char tmpUrl [strlen(url)+1]; // tp work on temprorate url
    strcpy(tmpUrl,url);

    char *tmpProtocl = strtok(tmpUrl,":"); // get the protocol
    
    char *tmpHost = strtok(NULL,"/"); // get the host with the port
 
    if(tmpProtocl == NULL || strcmp(tmpProtocl,"http") != 0 ) // check if there is no HTTP protocol
         error("parselUrl : PROTOCOL HTTP DID NOT FOUND\n");
    

    char *tmpPath = strtok(NULL,"\n"); // get the path
    
    if (tmpHost == NULL || tmpHost[0] == ':') // if the host is empty
        error("parseUrl : NO HOST DOOMAIN, PLEASE TRY AGAIN");
    
    
    char *tmpPort = strchr(tmpHost,':'); // get the port from the previous host
    int copyPort = 0;

    
    if(tmpPort != NULL && (strlen(tmpPort) != 1) )
    {
        char *cpyport = (char*)calloc(strlen(tmpPort),sizeof(char)); // copy the port without :

        for(int i = 0; i<strlen(tmpPort)-1;i++)
        {
            cpyport[i] = 0;
            cpyport[i]=tmpPort[i+1];
        }
        //printf("CPYPORT = %s \n",cpyport);
        if(cpyport[0]=='-')
        {
            free(cpyport);
            error("parseUrl: NEGATIVE PORT, PLEASE TRY AGAIN");
        }
        int counterWords = 0;
        for(int i = 0; i<strlen(cpyport);i++)
            counterWords=counterWords+digitAfterR(cpyport);
        
        int checkDig = digitAfterR(cpyport);
        if(checkDig == 0 && cpyport != NULL)
            error("parseUrl: WRONG PORT, letters/signs on port");
        copyPort = atoi(cpyport); // save the port value with int
        free(cpyport);
    }
    else
        copyPort = 80;
    

    tmpHost = strtok(tmpHost,":"); // get the host doomain without :
    
    /* ============== END PARSING URL ===============*/

    /* ============== Allocate space to our variables =============== */
    *host = (char*)calloc(strlen(tmpHost)+1,sizeof(char)); // allocate size for host
    if(host == NULL)
    {
        perror("HOST MALLOC FAILED");
        return EXIT_FAILURE;
    }

    if(tmpPath!=NULL) // allocate the path
    {
        *path = (char*)calloc(strlen(tmpPath)+1,sizeof(char));
        strcpy(*path,tmpPath);
    }
    else // in case theres no path we still need to add /
    {
        *path = (char*)calloc(1,sizeof(char));
        *path ="/";
    }

    if(copyPort != 0) // to check what port we us
        *port = copyPort;
    else // if no port we put 80 as default
        *port = 80;
    
    strcpy(*host,tmpHost); // copy in the end that if something fail in the way we dont got used memory
   return 0;
}

int digitAfterR(char *url) // check what digit is after -r 
{
    for(int i = 0; i < strlen(url) ; i++)
        if(url[i] > '9' || url[i] < '0')
            return 0;
    if(url[0] == '-')
        return 0;
    int digit = atoi(url);
    return digit;
}

int checkProperAfterR(char *url) // check if the headers after -r are proper and exist
{
    char *check;
    check = strstr(url,"=");
    if(check == NULL || strlen(url)<3 || url[0] == '=' || url[strlen(url)-1]=='=')
        return 0;
    return 1;
}

/* ================= FUNCTIONS TO FIND THE LOCATION OF THE CURRENT THING ON OUR ARGV ===================== */
int checkUrlCell(char *url) // check if URL exist in the function
{
    char *check;
    check = strstr(url,"http://");
    if(check == NULL)
        return 0;
    return 1;
}

int checkPCell(char *url) // check if -p exist in the function
{
    char *check;
    check = strstr(url,"-p");
    if(check == NULL || strlen(url)>2)
        return 0;
    return 1;
}

int checkRCell(char *url) // check if -r exist in the function
{
    char *check;
    check = strstr(url,"-r");
    if(check == NULL || strlen(url)>2)
        return 0;
    return 1;
}
/* ================================== END OF SEARCHING LOCATION ============================================ */

/* ================== ERRORS ================= */
void error(char* str) // error message
{
    perror(str);
    exit(EXIT_FAILURE) ;
}

int usageError() // usage error message
{
    printf("Usage: client [-p <text>] [-r n <pr1=value1 pr2=value2 …>] <URL> \n");
    exit(EXIT_FAILURE);
}
/* ================== END ERRORS ============= */
void freeValues(char *host,char *path) // free the values we allocate
{
     if(host != NULL)
        free(host);
    if(path != NULL)
        free(path);
}




