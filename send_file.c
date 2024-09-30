/*
 * Function: send_file
 * Description: Sends the contents of a file to the client over a socket. It first 
 *              sends the file size, then reads the file in chunks and transmits
 *              it to the client.
 * Parameters:
 *      FILE *fp - A pointer to the file to be sent.
 *      int sockfd - The file descriptor for the client socket connection.
 * Returns: int - Returns 0 upon successful file transfer.
 */

#include "sock.h"

int send_file(FILE *fp, int sockfd){

	char data[BUFFER_SIZE]; // Buffer to hold chunks of file data
	long file_size; // Variable to hold the file size

	bzero(data,BUFFER_SIZE);

	// Get the file size
    	fseek(fp, 0L, SEEK_END);
    	file_size = ftell(fp);  
    	rewind(fp);

    	// Send the file size to the client
    	if (send(sockfd, &file_size, sizeof(file_size), 0) == -1) {
        	perror("Failed to send file size");
        	exit(1);
    	}

	// Read the file in chunks and send each chunk to the client
	while (fread(data, sizeof(char), BUFFER_SIZE, fp) > 0) {
        
		if (send(sockfd, data, BUFFER_SIZE, 0) == -1) {
            		perror("Failed to send file");
            		exit(1);
        	}

        	bzero(data, BUFFER_SIZE);
    	}

	printf("File sent successfully\n");

	// Close the write-side of the socket to signal the end of the file transfer
	shutdown(sockfd, SHUT_WR);  

	return 0;

}
