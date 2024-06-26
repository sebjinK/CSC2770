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
    int sockfd, fsize; // the integer socket we use for this project
    struct sockaddr_in serv_addr; //server address we are going to 
    struct hostent * server; // server object we are working with
    FILE * FILE_fp; // file pointer holding the actual binary file we want
    FILE * socket_fp;
    
    const char* hostname = argv[1]; // grab the hostname
    int port = std::atoi(argv[2]); // grab the port number
    const char* filename = argv[3]; //grab the file name

    char buffer[1024]; //   set up input buffer to read 1024 bytes at a time
    ////////////////////////////CHECK FOR ALL ARGUMENTS ARE THERE/////////////////
    if (argc != 4) // make sure the number of args is correct
    {
        std::cerr << "Usage: " << argv[0] << " <hostname> <port> <filename> " << std::endl; 
        return 1; // finish the program if entered wrong
    }
    std::cout << "Correct Arguments/# of arguments" << std::endl;

    ////////////////////////////CHECK IF SOCKFD CONNECCTS//////////////////////////
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //setup the socket
    if (sockfd < 0) //check to see sockfd screwed up
    {
        std::cerr << "ERROR: opening socket: " << strerror(errno) << std::endl; 
        close(sockfd);
        return 1;
    };
    std::cout << "Socket connection successful" << std::endl;

    /////////////////////////CHECK FOR THE SERVER/////////////////////////////////
    server = gethostbyname((const char*)argv[1]); // get the server client address and open a connection to it
    if (server == NULL) //check if the server is there
    {
        std::cerr << "ERROR: No such server: " << hostname << std:: endl;
        close(sockfd); //close the socket if the server ain't there
        return 1; 
    }
    std::cout << "Server " << hostname << " exists\n";

    ////////////////////////////////server connecction setup/////////////////////
    bzero((char *)&serv_addr, sizeof(serv_addr));//fill in the server address with 0s
    serv_addr.sin_family = AF_INET; //set the sin_family to AF_INET
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length); //copy the sin_addrr to the server's addr
    serv_addr.sin_port = htons(port);// use htons to set up the port

    /////////////////////////////////connecct to server////////////////////////////////////
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //connect using the serv_addr
    {
        std::cerr << "ERROR: Connection Failed: " << strerror(errno) << std::endl;
        close(sockfd); //close the socket if we didn't actually connect
        return 1;
    }
    else
    {
        std::cout << "Correctly establish Successful Connection to " << hostname << std::endl; 
    }

    char gMessage[255] = "g ";
    strcat(gMessage, filename);
    strcat(gMessage, "\n");
    //strcat(gMessage, "\0"); // just in ccase we need to null terminate
    //////////////////////////////SEND REQUEST TO SERVER////////////////////////////
    //if (send(sockfd, gMessage, sizeof(gMessage), 0) < 0) //send a message with a "g <filename>\n"
         ///////////////////////////////OPEN SOCKFP/////////////////////////////
    socket_fp = fdopen(sockfd, "r");
    if (!socket_fp)
    {
        std::cerr << "ERROR: Converting Socketfd to FILE *" << std::endl;
        fclose(socket_fp);
        close(sockfd);
        return 1;
    }
    else
        std::cout << "Correctly Converted SocketFD to FILE *" << std::endl;
    /////////////////////////////OPEN THE FILE///////////////////////////////////////
    FILE_fp = fopen(filename, "wb"); //open the filename that is retrieved
    if (!FILE_fp)
    {
        std::cerr << "ERROR: Opening File" << std::endl;
        fclose(FILE_fp);
        close(sockfd);
        return 1;
    }
    else
        std::cout << "Correctly Opened file " << filename << std::endl;
    //////////////////////////////////print out the buffer///////////////////////
    if (send(sockfd, gMessage, strlen(gMessage), 0) < 0)
    {//fprintf()
        std::cerr << "ERROR: Writing to socket: " <<  strerror(errno) << std::endl;
        fclose(socket_fp);// close the socket if we cannot send the message
        close(sockfd);
        return 1;
    }
    std::cout << "Correctly sent request to download " << filename << std::endl;
    
    //////////////////////////////SCAN FOR RETURN FROM SERVER/////////////////////////
    if ((fgets(buffer, 1024, socket_fp) == NULL))
    //if (fread(buffer, 1, 1024, socket_fp))
    {
        std::cerr << "ERROR: Failed to Recieve Byte Size/Server Response: " << buffer << std::endl;
        close(sockfd);
        return 1;
    }
    else
        std::cout << "Correctly Read in Filesize: " << buffer << std::endl;
    
    int readBytes = sscanf(buffer, "s %d\n", &fsize);
    //printf("Your response is: %s", buffer);
    if (buffer == "e No such file\n")
    {
        std::cerr << "ERROR: NO SUCH FILE EXISTS\n";
        close(sockfd);
        fclose(socket_fp);
        return 1;
    }
    /*
    std::string response(buffer);
    if (!response.empty() && response.back() != '\n')
        response.pop_back();
        */
    if (readBytes == 0)
    {
        std::cerr << "ERROR: No Such File exists\n";
        fclose(socket_fp);
        close(sockfd);
        return 1;
    }
    else
        std::cout << "Correctly Parsed Server Response\n";

    //fsize = stoi(response.substr(2, response.size() - 1));

    
    /*
    while (fgets(buffer, sizeof(buffer), FILE_fp) != nullptr)
    {
        //check for error
        fwrite(buffer, sizeof(char), strlen(buffer), FILE_fp);
        bzero(buffer, 1024);        
    }*/
    //std::cout << "Saved " << filename << " to buffer through download\n"; 
 
    int receievedBytes = 0;
    /*
    while (receievedBytes < fsize)
    {    //if (fgets(buffer, 1024, socket_fp) == NULL)
        int bytesRead = fread(buffer, 1, 1024, socket_fp);
        int bytesWritten = fwrite(buffer, 1, bytesRead, FILE_fp);
        if (bytesRead <= 0) 
            break;//fread
        if (bytesWritten <= 0)
            break;
        if (bytesWritten < bytesRead)
        {
            std::cerr << "ERROR: Writing to file Failed" << std::endl;
            fclose(FILE_fp);
            fclose(socket_fp);
            close(sockfd);
            return 1;
        }
        receievedBytes += bytesRead;
        */
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    while (receievedBytes < fsize && !feof(socket_fp))
    {
        int bytesRead = fread(buffer, 1, 1024, socket_fp);
        if (bytesRead > 0)
        {
            int bytesWritten = fwrite(buffer, 1, bytesRead, FILE_fp);
            if (bytesRead <= 0) 
                break;//fread
            if (bytesWritten <= 0)
                break;
            receievedBytes += bytesRead;
        }
        else
        {
            break;
        }
    }
        /*
        int numToWrite = std::min((fsize - receievedBytes), (int)strlen(buffer));
        fwrite(buffer, sizeof(buffer), numToWrite, FILE_fp);
        receievedBytes += numToWrite;*/
    
    if (receievedBytes != fsize)
    {
        std::cerr << "ERROR: File size mismatch: Expected: " << fsize << ", got " << receievedBytes << std::endl; 
        fclose(FILE_fp);
        fclose(socket_fp);
        close(sockfd);
        return 1;
    }
    fclose(FILE_fp);
    std::cout << "Correctly Transferred File from Server\n";

    /////////////////////////////////Check if the cllient is reading from the socket////////////////////////////
    /*if (ferror(socket_fp))
    {
        std::cerr << "ERROR: Reading from Socket" << std::endl;
        //fclose(fp);
        fclose(socket_fp);
        close(sockfd);
        return 1;
    }
    std::cout << "Correctly read from socket File Pointer" << std::endl;
    //////////////////////////////////////Send a final message bacck to the server////////////////////////////////
    //int sendFinalMessageToServer = send(sockfd, "d\n", strlen("d\n"), 0);*/
    if (send(sockfd, "d\n", 2, 0) < 0)
    {
        std::cerr << "ERROR: Failed to send finish" << std::endl;
        //fclose(fp);
        fclose(socket_fp);
        close(sockfd);
        return 1;
    }
    std::cout << "Correctly Sent d plus a newline to the server\n";
    // cleanup
    //fclose(fp);
    close(sockfd);
    fclose(socket_fp);

    return 0;
}