#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <ctype.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

int main(int argc, char* argv[])
{
    int sockfd; // the integer socket we use for this project
    struct sockaddr_in serv_addr; //server address we are going to 
    struct hostent * server; // server object we are working with
    FILE * fp; // file pointer holding the actual binary file we want
    FILE * sockfp;
    const char* hostname = argv[1]; // grab the hostname
    int port = std::atoi(argv[2]); // grab the port number
    const char* filename = argv[2]; //grab the file name

    char buffer[1024]; //   set up input buffer to read 1024 bytes at a time

    if (argc != 4) // make sure the number of args is correct
        std::cerr << "Usage: " << argv[0] << " <hostname> <port> <filename> " << std::endl; return 0; // finish the program if entered wrong

    sockfd = (AF_INET, SOCK_STREAM, 0); //setup the socket
    if (sockfd < 0) //check to see sockfd screwed up
        std::cerr << "ERROR: opening socket: " << strerror(errno) << std::endl; 
    
    server = gethostbyname((const char*)argv[1]); // get the server client address and open a connection to it

    if (server == NULL) //check if the server is there
    {
        std::cerr << "ERROR: No such server: " << hostname << std:: endl;
        close(sockfd); //close the socket if the server ain't there
        return 1; 
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));//fill in the server address with 0s
    serv_addr.sin_family = AF_INET; //set the sin_family to AF_INET
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length); //copy the sin_addrr to the server's addr
    serv_addr.sin_port = htons(port);// use htons to set up the port

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0); //connect using the serv_addr
    {
        std::cerr << "ERROR: Connection Failed: " << strerror(errno) << std::endl;
        close(sockfd); //close the socket if we didn't actually connect
        return 1;
    }

    char gMessage[255] = "g ";
    strcat(gMessage, filename);
    strcat(gMessage, "\n");
    //strcat(gMessage, "\0"); // just in ccase we need to null terminate

    if (send(sockfd, gMessage, sizeof(gMessage), 0) < 0) //send a message with a "g <filename>\n"
    {
        std::cerr << "ERROR: Writing to socket: " <<  strerror(errno) << std::endl;
        close(sockfd);// close the socket if we cannot send the message
        return 1;
    }
    int fsize;
    if (sscanf(buffer, "s %d\n", &fsize) != 1)
    {
        std::cerr << "ERROR: Failed to Recieve Byte Size/Server Response: " << buffer << std::endl;
        close(sockfd);
        return 1;
    }
    fp = fopen(filename, "wb"); //open the filename that is retrieved
    if (fp == NULL)
    {
        std::cerr << "ERROR: Opening File" << std::endl;
        fclose(fp);
        close(sockfd);
        return 1;
    }
    sockfp = fopen(filename, "r");
    if (!sockfp)
    {
        std::cerr << "ERROR: Converting Socketfd to FILE *" << std::endl;
        fclose(fp);
        close(sockfd);
        return 1;
    }
    /*
    while (fgets(buffer, sizeof(buffer), fp) != nullptr)
    {
        //check for error
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        bzero(buffer, 1024);        
    }
    */

    int receievedBytes = 0;
    while (receievedBytes < fsize)
    {
        if (fgets(buffer, sizeof(buffer), sockfp) == NULL)
            break;
        int numToWrite = std::min((fsize - receievedBytes), (int)strlen(buffer));
        fwrite(buffer, sizeof(buffer), numToWrite, fp);
        receievedBytes += numToWrite;
    }
    fclose(fp);
    if (ferror(sockfp))
    {
        std::cerr << "ERROR: Reading from Socket" << std::endl;
        //fclose(fp);
        fclose(sockfp);
        return 1;
    }
    
    if (send(sockfd, "d\n", strlen("d\n"), 0) < 0)
    {
        std::cerr << "ERROR: Failed to send finish" << std::endl;
        //fclose(fp);
        fclose(sockfp);
        return 1;
    }

    //fclose(fp);
    fclose(sockfp);

    return 0;
}