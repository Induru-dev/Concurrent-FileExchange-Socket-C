/*
 * File: srv2378.c
 * Description: This file contains the implementation of a server that accepts
 *              client connections and transfers files based on client requests.
 * Author: Induru Sasmitha
 * Date: 09/27/2024
 */

#include        "sock.h"

int main(int argc, char **argv){

    	int listenfd;
    	int connfd[MAX_CLIENTS];  // Array to hold client socket file descriptors
    	int i = 0, trans_status;  // Loop index and transaction status
    	pid_t childpid;  // Process ID for the child process
    	socklen_t clilen;  // Length of the client socket address
    	const char *ptr;  // Pointer to store the client's IP address
    	void sig_chld(int);  // Function to handle zombie processes
    	char buffer[BUFFER_SIZE];  // Buffer to store temporary data
    	char file_name[BUFFER_SIZE];  // Buffer to store the file name
    	FILE *fp;  // File pointer to handle file operations
    	FILE *log_file;  // File pointer for the log file
    	char *direc = "/home/induru/Documents/files";  // Directory containing files
	
	// Check if the correct number of arguments is provided
	if (argc != 2) { 
        	printf("Enter ip address correctly\n");
        	exit(1);
    	}	

	//Structure to store server and client socket addresses
        struct sockaddr_in servsock, clisock;

	//open log file
	log_file = fopen("log_srv2378.txt", "a");
    	if (log_file == NULL) {
        	perror("Failed to open log file");
        	exit(1);
    	}
	
	//creating socket
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
                perror("Socket creation failed\n");
                exit(1);
        }

	//make all 0
        bzero(&servsock,sizeof(servsock));
	

	// Assign IP address and port for the server
        servsock.sin_family = AF_INET;
        servsock.sin_port = htons(2211);
        servsock.sin_addr.s_addr = inet_addr(argv[1]);

	//bind function
        if ((bind(listenfd, (struct sockaddr*)&servsock, sizeof(servsock))) < 0){
                perror("bind failed\n");
                exit(1);
        }
	
	//listen incoming connections, with a backlog of 100
        if(listen(listenfd,100) < 0){
                perror("listen failed\n");
                exit(1);
        }

	//hadle zombie process created by child processes
	signal(SIGCHLD,sig_chld);
	
	printf("waiting for client connections.....\n");
	fflush(stdout);
       
        for(i=0; i < MAX_CLIENTS; i++){

				
		bzero(&clisock,sizeof(clisock));
		clilen = sizeof(clisock); // Get the size of the client address

		//accept a client connection
	        if( (connfd[i] = accept(listenfd, (struct sockaddr*)&clisock , &clilen)) < 0 ){
			if(errno == EINTR){
				continue;
			}
			else{
				perror("accept error\n");
				exit(1);
			}
		}


		//getting clients details
		if( (ptr = inet_ntop(AF_INET, &clisock.sin_addr, buffer, sizeof(buffer))) == 0 ) {
			perror("inet_ntop error \n");
			exit (1);
		}
		printf("connection from %s, port %d\n", ptr, ntohs(clisock.sin_port));
		ptr = NULL;


		//creating child server
		if((childpid =  fork())==0)
		{
			close(listenfd);// Close the listening socket in the child process
			
			send_list(connfd[i]); // Send the list of files to the client
			
			int file_number, file_count = 1;
			struct dirent *de; //Structure for directory entries
			DIR *dr = opendir(direc); // Open the directory with files

			if (dr == NULL) {
    				perror("Could not open directory");
    				exit(1);
			}

			bzero(buffer,BUFFER_SIZE);
			
			// Get the selected file number from the client
			read(connfd[i], buffer, sizeof(buffer));
			file_number = atoi(buffer);

			//read each enty of directory to find the selected file
			while ((de = readdir(dr)) != NULL) {
				//ignore . and  ..
				if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) { 
					if (file_count == file_number){ 
						
						char full_path[BUFFER_SIZE]; // Buffer for full file path

						//get file name from de and append it with folder path
            					snprintf(full_path, sizeof(full_path), "%s/%s", direc, de->d_name); 

						//open file in binary read mode
            					fp = fopen(full_path, "rb");
						if (fp == NULL) {
							perror("Error opening file");
							exit(1);
	    					}

						printf("File opened successfully.\n");
						
						//send file name to client
						strcpy(file_name,de->d_name);
						send(connfd[i], file_name, sizeof(file_name), 0);
						
					}

					file_count++;

				}
			}

			//sending file
			trans_status = send_file(fp,connfd[i]);

			// Log the transaction details
			log_info(log_file, &clisock, file_name, trans_status);

			// Close the file and the connection
			fclose(fp);
			close(connfd[i]);
			exit(0); //Terminate the child process						
		
		}
		
		close(connfd[i]); // Close the client socket in the parent process

        }

        return 0;

}












