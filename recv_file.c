/*
 * Function: recv_file
 * Description: Receives a file sent by the server and saves it to a specified directory.
 *              The function first receives the file size, then reads the file data 
 *              from the socket in chunks and writes it to a local file. It also tracks
 *              and displays the download progress.
 * Parameters:
 *      int sockfd - The file descriptor for the server socket connection.
 *      char *filename - The name of the file to be saved locally.
 * Returns: void
 */

#include "sock.h"

void recv_file(int sockfd, char *filename){

    	int bytes_received;  // Stores the number of bytes received in each chunk
    	int last_printed_progress = 0;  // Tracks the last printed progress percentage
    	FILE *fp;  // File pointer to write the received file
    	char *direc = "/home/induru/Documents/recv_files";  // Directory to save the file
    	char full_path[BUFFER_SIZE];  // Buffer to hold the full file path
    	char buffer[BUFFER_SIZE];  // Buffer to hold chunks of data received
    	long file_size;  // The size of the file being received
    	long total_bytes_received = 0;  // Tracks the total number of bytes received
    	float progress = 0;  // Tracks the download progress as a percentage

	bzero(buffer, BUFFER_SIZE);
	bzero(full_path, BUFFER_SIZE);

	// Create the full path for the file (directory + filename)
	snprintf(full_path, sizeof(full_path), "%s/%s", direc, filename);

	// Open the file in write-binary mode
	fp = fopen(full_path, "wb");
	if (fp == NULL) {
    		perror("Failed to open file\n");
    		exit(1);
	}

	// Receive the file size from the server
    	if (recv(sockfd, &file_size, sizeof(file_size), 0) <= 0) {
        	perror("Failed to receive file size");
        	exit(1);
    	}
	
	printf("Downloading file: %s \n", filename);
	printf("File size: %ld bytes\n", file_size);

	//Receive the file data in chunks and write it to the file
	while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
    		fwrite(buffer, sizeof(char), bytes_received, fp);
		total_bytes_received += bytes_received;

		// Calculate and display the download progress
        	progress = ((float)total_bytes_received / file_size) * 100;

		// Print progress every 10% increment
		if ((int)progress >= last_printed_progress + 10 ) {
        		last_printed_progress = (int)progress;
        		printf("Download progress: %d%%\n", last_printed_progress); 	         	
			fflush(stdout); 
		}

    		bzero(buffer, BUFFER_SIZE);
	}

	// Check if there was an error receiving the file	
	if (bytes_received == -1) {
    		perror("Error receiving file\n");
    		exit(1);
	}else{
		printf("Download complete. File saved as %s \n",filename);
	}

	// Close the file after the transfer is complete
	fclose(fp);

}
