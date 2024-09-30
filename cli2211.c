/*
 * File: cli2211.c
 * Description: This file contains the implementation of a client that connects 
 *              to a server, requests a file, and receives the file data from the server.
 * Author: Induru sasmitha
 * Date: 09/27/2024
 */


#include "sock.h"

int main(int argc, char **argv){

        int     srvfd, check_con, selected_file_number; // Server socket file descriptor, connection status, and file selection number
        char    buffer[BUFFER_SIZE]; // Buffer to hold data from the server
	char    send_buffer[BUFFER_SIZE]; // Buffer to hold the user's file selection

	// Check if the correct number of arguments is provided
	if (argc != 2) { 
        	printf(" Enter ip address correctly ");
        	exit(1);
    	}	

        struct sockaddr_in      servsock; // Structure to store server socket address

	// Create a socket
        if ((srvfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
                perror("Socket creation failed\n");
                exit(1);
        }

        bzero(&servsock, sizeof(servsock));

        //here socket addres structure shold contain servers address and its port number
        servsock.sin_family = AF_INET;
        servsock.sin_port   = htons(2211);
        servsock.sin_addr.s_addr = inet_addr(argv[1]);
        // Alternatively: inet_pton(AF_INET, argv[1], &servsock.sin_addr);

        // Connect to the server
        check_con = connect(srvfd, (struct sockaddr*)&servsock,  sizeof(servsock));
        if(check_con == 0){
                printf("connection successfull\n");
        }
        else{
                printf("connect error");
                exit(1);
        }

	
	bzero(buffer,BUFFER_SIZE);

	// Read the list of available files from the server
        read(srvfd, buffer, BUFFER_SIZE);
        printf("Available files:\n%s\n", buffer);

	// Ask the user to enter the number corresponding to the file they want to download
	printf("Enter the number of the file you want to download: ");
	scanf("%d", &selected_file_number);

	// Send the selected file number to the server
	snprintf(send_buffer, sizeof(send_buffer), "%d", selected_file_number);
        write(srvfd, send_buffer, strlen(send_buffer));

	// Receive the file name from the server
	bzero(buffer,BUFFER_SIZE);
	read(srvfd, buffer, sizeof(buffer));

	// Call the function to receive the file from the server and save it
	recv_file(srvfd,buffer);

	return 0;

}

