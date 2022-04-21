// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

     if (argv[0][1] == 's')
    {
        int socket = argv[0][0];
        valread = read(socket, buffer, 1024);
        printf("%s\n", buffer);
        send(socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }

    // Setting up socket

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
        sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
  
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Privilege Seperation Logic

    pid_t currentProcessId = fork();
    if (currentProcessId == 0)
    {
        printf("Child process execution\n");

        struct passwd *p;
        char user[]="nobody";
        if ((p = getpwnam(user)) == NULL){
            perror("getpwnam() error");
            exit(EXIT_FAILURE);
        }
        printf("The userid of nobody : %d\n", (int) p->pw_uid);

        int userIdSet = setuid( p->pw_uid);
        if (userIdSet == -1)
        {
            printf("setuid command failed\n");
            return 0;
        }

       /* valread = read(new_socket, buffer, 1024);
        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");*/



        //code for child process exec

        char argumentsToPass[2];

        argumentsToPass[0] = new_socket;
        argumentsToPass[1] = 's';

        execl(argv[0], argumentsToPass, NULL);

        printf("This is a dummy line to test exec. Ideally this shouldn't be printed");

    }
    else if (currentProcessId > 0)
    {
        wait();
        printf("This is parent process execution\n");
    }
    else
    {
        perror("Fork operation failed");
        exit(2);
    }

    return 0;
}
                     
