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
#include <iostream>

#define PORT 8007

void *handle_connect(void *arg);
void handle_protocol(FILE *fp);
void get(FILE *fp, const std::string &fname);
char *rtrim(char *s);
char *ltrim(char *s);

int main(int argc, char* argv[]) {
        int sockfd, newsockfd; 
	socklen_t clilen;
        struct sockaddr_in cli_addr, serv_addr;

        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " port" << std::endl;
            return 1;
        }

        int port = atoi(argv[1]);
        port = port == 0 ? PORT : port;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
               std::cerr << "Cannot create socket: " << strerror(errno) <<
			std::endl; 
		return 1;
        }
        bzero((void *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) { 
		std::cerr << "Cannot bind socket: " << strerror(errno) <<
			std::endl; 
		return 1;
	}
        listen(sockfd, 5);
        std::cout << "Net fileserver running and accepting connections on port " << port << "..."
            << std::endl;
        for (;;) {
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, 
                        (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) { /* error */ }
		            pthread_t tid;
		            pthread_create(&tid, NULL, handle_connect, (void *) (long) newsockfd);
                }
}

void *handle_connect(void *arg) {
	pthread_detach(pthread_self());
	int newsockfd = (long) arg; 
	FILE *fp = fdopen(newsockfd, "r+");
    std::cout << "Got connection..." << std::endl;
    handle_protocol(fp);
	//char msg[1024];
	//fgets(msg, 1024, fp);  /* should check for error here */
	//fprintf(fp, "%s", msg);
	fclose(fp);
	return NULL;
}

void handle_protocol(FILE *fp) {
    // read command from fp
    //     form is g filename<nl>
    //          or p filename<nl>
    char sbuf[1024];
    char *buf = sbuf;
    if (fgets(buf, 1024, fp) == NULL) {
        fprintf(fp, "e premature end of line\n");
        return;
    }
    std::cout << "Got command: " << sbuf << std::endl;
    int len = strlen(buf);
    if (len == 0) {
        fprintf(fp, "e protocol error - command of length 0\n");
        return;
    }
    if (buf[len-1] != '\n' ) {
        fprintf(fp, "e command too long\n");
        return;
    }
    ltrim(buf);
    rtrim(buf);

    // format must now be [gp]\s(.*)
    char command = tolower(buf[0]);
    buf++;

    if (*buf != ' ') {
        fprintf(fp, "e expected space after command\n");
        return;
    }
    while (*buf == ' ') buf++;
    std::string fname(buf);

    if (command == 'g') 
        get(fp, fname);
    //else if (command == 'p') 
        //put(fp, fname);
    else {
        fprintf(fp, "e unknown command '%c'\n", command);
        return;
    }

    return;
}

void get(FILE *fp, const std::string &fname) {
    int fsize = 0;
    try {
        fsize = std::filesystem::file_size(fname);
    } catch (std::filesystem::filesystem_error& e) {
        fprintf(fp, "e No such file\n");
        return;
    }
    fprintf(fp, "s %d\n", fsize);
    std::ifstream fs(fname, std::ifstream::binary);
    if (!fs) {
        fprintf(fp, "e no such file\n");
        return;
    }
    char buf[1024];
    fs.read(buf, 1024);
    int num_read = fs.gcount();

    while (!(fs.eof() && num_read == 0)) {
        int items = fwrite(buf, 1, num_read, fp);
        if (items != num_read) {
            std::cerr << "write failed, return (close socket)" << std::endl;
            return;
        }
        fs.read(buf, 1024);
        num_read = fs.gcount();
    }
    fflush(fp);
    buf[0] = 0;
    if (fgets(buf, 1024, fp) == NULL) {
        fprintf(fp, "e premature termination\n");
        return;
    }
    if (*buf != 'd') {
        fprintf(fp, "e illegal termination character %c\n", *buf);
    }

    return; // file sent back, we're done
    
}

char *ltrim(char *s) {
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s) {
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}
