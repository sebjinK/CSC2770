#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
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
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;
    FILE * fp;
    FILE * sockfp;
    const char* hostname = argv[1];
    int port = std::atoi(argv[2]);
    const char* filename = argv[2];

    char buffer[1024];

    if (argc != 4)
        std::cerr << "Usage: " << argv[0] << " <hostname> <port> <filename> " << std::endl; return 0; // finish the program if entered wrong

    sockfd = (AF_INET, SOCK_STREAM, 0); //setup the socket
    if (sockfd < 0) //check to see sockfd screwed up
        std::cerr << "ERROR: opening socket: " << strerror(errno) << std::endl; 
    
    server = gethostbyname((const char*)argv[1]); // get the server client address and open a connection to it

    if (server == NULL)
    {
        std::cerr << "ERROR: No such server: " << hostname << std:: endl;
        close(sockfd);
        return 1; 
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
    {
        std::cerr << "ERROR: Connection Failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    char gMessage[255] = "g ";
    strcat(gMessage, filename);
    strcat(gMessage, "\n");
    //strcat(gMessage, "\0"); // just in ccase we need to null terminate

    if (send(sockfd, gMessage, sizeof(gMessage), 0) < 0)
    {
        std::cerr << "ERROR: Writing to socket: " <<  strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    fp = fopen(filename, "wb");
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
    //std::vector<char*> buffer;
    char buffer[1024];
    //bzero(buffer, 1024);
    while (fgets(buffer, sizeof(buffer), fp) != nullptr)
    {
        //check for error
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        bzero(buffer, 1024);        
    }

    if (ferror(sockfp))
    {
        std::cerr << "ERROR: Reading from Socket" << std::endl;
        fclose(fp);
        fclose(sockfp);
        return 1;
    }
    
    if (send(sockfd, "d\n", strlen("d\n"), 0) < 0)
    {
        std::cerr << "ERROR: Failed to send finish" << std::endl;
        fclose(fp);
        fclose(sockfp);
        return 1;
    }

    fclose(fp);
    fclose(sockfp);

    return 0;
}