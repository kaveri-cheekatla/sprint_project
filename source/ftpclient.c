/* FTP Client Program which send the commands to the server program to perform various functions */
/************************************************************************************
                         Header file
************************************************************************************/			 
#include <stdio.h>      //includes input output related functions 
#include <string.h>     //include all functions of string handling
#include<stdlib.h>      //includes functions involving memory allocation, utility functions for type conversion
#include<unistd.h>      //provides access to the posix operation system api

#include <sys/types.h>  //includes typedef symbols and structures 
#include <sys/socket.h> //includes sockaddr structure
#include <netinet/in.h> //includes the internet address family
#include <netdb.h>      //defines the netent structure
#include <fcntl.h>      //defines the flock structure describing a file lock

#include<sys/ioctl.h>   //includes input output definitions and structures
#include<arpa/inet.h>   //defines internet operation
#include<net/if_arp.h>  //implementation of tcp/ip protocol for linuxOSes
#include<sys/stat.h>    //defines structure of data
/****************************************************************************************
                         Macros
****************************************************************************************/			 

#define FNAME file1
#define PORT 8021
#define BUFSIZE 128

int main(int argc, char **argv)
{
	/* Declaring the variables*/

	int sockfd, fd, n, size,count=0;
	long int size1,size2;
	char buf[BUFSIZE], fname[50], command[50];
	struct sockaddr_in servaddr;
	struct stat stat_buf;


	//Prompt the user to include the localhost address along with the executable file
	if (argc != 2) {
	printf("Usage: %s localhost_address", argv[0]);
	exit(1);
	}

	printf("11 - socket creation\n");

	//if the socket function value return less than 0, it exit with 1.
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(1);

	bzero(&servaddr, sizeof(servaddr));  	//bzero() function erases the data and sets it to '\0'(NULL)
	servaddr.sin_family = AF_INET;			// IPv4 Address   //AF_INET6 - IPv6 Address
	servaddr.sin_port   = htons(PORT);  	// PORT has been defined as 8021. refer p.17
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		exit(1);

	printf("22 - connection establishment\n");

	/* Connection establishment is started */

	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		exit(1);
	printf("Connection has been established\n");


	/* The client has successfully connected to the server successfully
	   and the client is being prompted to enter commands */

	printf("Enter FTP command: ");
	scanf("%s",command);			//input the command
	send(sockfd,command,sizeof(command),0);		//Sends the command to the server


	//DOWNLOAD Function
	// strcmp compares the given input to command(GET)
	if(strcmp(command, "GET") == 0){
		printf("Enter the name of the file u want to receive : ");
		scanf("%s",fname);					//input file name
		send(sockfd,fname,sizeof(fname),0); //sends the file name along with size of the same to the server
		
		//file open with write-only access with read, write, execute/search permission for owner
		fd=open(fname,O_WRONLY|O_CREAT,S_IRWXU);

		//if the data in buf and size of buf is greater than 0, the while function gets executed.
		while ( (n = read(sockfd, buf, BUFSIZE-1)) > 0)
		{
			buf[n] = '\0';		//set buff[] to '\0'
			write(fd,buf,n);
			if( n < BUFSIZE-2)
				break;
		}
		//fstat() is identical to stat(), except that the file to be stat-ed is specified by the file descriptor fd.
		fstat(fd, &stat_buf);

		size = stat_buf.st_size;
		printf("File Size is %d\n", size);   //prints file size
		printf("File Download Successful \n");
		close(sockfd);
		close(fd);
	}

	//UPLOAD Function
	// strcmp compares the given input to command(PUT)

	else if(strcmp(command, "PUT") == 0){
		printf("Enter the name of the file u want to receive : ");
		scanf("%s",fname);		//input the file name
		send(sockfd,fname,sizeof(fname),0);		//sends the file name along with the size of the same
		fd=open(fname,O_RDONLY,S_IRUSR);
		
		//fstat() is identical to stat(), except that the file to be stat-ed is specified by the file descriptor fd.
		fstat(fd, &stat_buf);

		size = stat_buf.st_size;
		
		printf("Fize is %d\n", size);  //prints file size
		printf("\nopened file\n");
		
		//if the data in buf and size of buf is greater than 0, the while function gets executed.
		while ( (n = read(fd, buf, BUFSIZE-1)) > 0)
		{
			buf[n] = '\0';
			write(sockfd,buf,n);
		}
		printf("File has been sent successfully \n");
		close(sockfd);
		close(fd);
	}

	//File List Function
	
	else if(strcmp(command, "LIST") == 0){
		printf("Files in the current directory : \n");
		for(;;)
		{
			memset(&fname, '\0', 50);  //unless variables are declared as static storage duration, they will have an arbitrary value upon creation. So, we set them to a known value before using them.
			// '\0' is the value to be set. 50 is the number of bytes to be set to the value(\0).


			int temp = recv(sockfd, fname, 50, 0);    //The recv() function receives data on a socket with descriptor connfd and stores it in fname. The recv() call applies only to connected sockets.
			if(strcmp(fname, "STOP") == 0){
				printf("No more files in the directory.\n");
				break;
			}
			printf("%s\n", fname);
		}
		close(sockfd);
	}

	// If the user send a command other than the ones mentioned above, It send an error.
    else{
        printf("Error: Not a valid FTP command.\n");
        close(sockfd);
    }
	close(sockfd);
	exit(0);
}


/* 
   close() call shuts down the socket associated with the socket descriptor socket, and frees resources allocated to the socket.
   If socket refers to an open TCP connection, the connection is closed.
 
   close(sockfd) function to stop listening.
*/
