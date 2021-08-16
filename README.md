# HTTP-Client
 HTTP client with HTTP request and user command line input

=== Description ===

Description: 
The followed program will create a HTTP client with HTTP request and user command line input, 
that include -r -p functions, get / post ,
supporting HTTP/1.0 and IPv4 connections.

Program files:client.c - main client http server with all functions inside



Functions:
    
    
int usageError(); - An error message that in case of wrong usage of the program will explain how to use it right.
void freeValues(char*, char* ); - A function that release all allocated data in the end of the program to not leave any data that are not released.
void error(char*); - An global error message in few cases that we can enter free string an exit from the program.
int checkUrlCell(char *); - Find in argv array in which cell the URL is.
int checkPCell(char *) - find in argv array in which cell the -p function is.
int checkRCell(char *) - find in argv array in which cell the -r function is.
int digitAfterR(char *) - Check if we got a digit after -r for a proper use.
int checkProperAfterR(char *) - Check that the headers we inserted into -r are in the proper form W=W.
int parseUrl(int *, char * , char ** , char **); - Parsing the url into it PORT / HOST / PATH.
main(argc,argv[]) - the main function that making the request to HTTP server, we use all functions in there and check all the cases to let the program work proper.

    
    
    
