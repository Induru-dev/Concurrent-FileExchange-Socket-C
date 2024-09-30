/*
 * Function: send_list
 * Description: This function sends a list of available files to the client.
 *              It retrieves the list from the server's directory and sends
 *              the filenames to the client over the socket.
 * Parameters:
 *      int sockfd - The file descriptor for the client connection.
 * Returns: void
 */

#include "sock.h"

void send_list(int sockfd){

	struct dirent *de;
	DIR *dr = opendir("/home/induru/Documents/files"); // Open the directory containing files
	char file_list[BUFFER_SIZE]; // Buffer to store the list of files
	int file_number = 1; // Counter to number the files
	char file_entry[256]; // Temporary buffer for each file entry

	bzero(file_list,BUFFER_SIZE);

	// Check if the directory was opened successfully
	if (dr == NULL) {
        	printf("Could not open directory");
        	exit(1);
    	}

	//read directory files one by one 
    	while ((de = readdir(dr)) != NULL) {

        	// Ignore '.' and '..'
        	if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {

			bzero(file_entry,256);
			
			// Format the file entry with a number (e.g., "1. file.mp4") and store it in file_entry
            		snprintf(file_entry, sizeof(file_entry), "%d. %s\n", file_number, de->d_name);

			// Concatenate the current file_entry to the file_list
            		strcat(file_list, file_entry);
            		file_number++;

        	}

    	}

	// Close the directory after reading all files
    	closedir(dr);	

    	// Send the list of files to the client
    	if (send(sockfd, file_list, strlen(file_list), 0) == -1) {
        	printf("Failed to send file list");
        	exit(1);
    	}	
	
	
}
