/*FTP Server Program which handles the requests from the client*/
/**********************************************************************************************
                        Header files
*********************************************************************************************/			

#include <stdio.h>      //includes input output related functionsi 
#include <string.h>     //include all functions of string handling
#include<stdlib.h>      //includes functions involving memory allocation, utility functions for type conversion
#include<unistd.h>      //provides access to the posix operation system api

#include <sys/types.h>  //includes typedef symbols and structures 
#include <sys/socket.h> //includes sockaddr structure
#include <netinet/in.h> //includes the internet address family
#include <netdb.h>      //defines the netent structure
#include <fcntl.h>      //defines the flock structure describing a file lock
#include <dirent.h>     //defines the following datatype
#include<sys/stat.h>    //includes file attributes
/********************************************************************************************************
                         Macros
*******************************************************************************************************/			 

#define FNAME file1
#define PORT 8021
#define BUFSIZE 128
#define LISTENQ 5

int main(int argc, char **argv)
{
	/* Declaring the variables*/

	int	listenfd, connfd, fd, pid, n, size, ret,len;
	struct sockaddr_in servaddr, cliaddr;
	char buf[BUFSIZE],fname[50],command[50];
	
	ssize_t rw_ret;

	struct stat stat_buf;


	/* 
	   Listenfd will be an endpoint for all requests to port 
	   on any IP address for this host
	*/ 
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
 
	bzero(&servaddr, sizeof(servaddr));				//bzero() function erases the data and sets it to '\0'(NULL)
	
	bzero(&cliaddr, sizeof(cliaddr));

	servaddr.sin_family      = AF_INET;				//IPv4 Address   //AF_INET6 - IPV6 Address
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//It is used so that it doesn't need to get binded to a particular IP
	servaddr.sin_port        = htons(PORT);			//PORT has been defined to 8021. refer p.15

	// bind() function binds a unique local name to the socket with descriptor socket.
	// After calling socket(), a descriptor does not have a name associated with it
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("Bind Successful\n");
	printf("Connection Established\n");

	// listen indicated the it is ready to accept client connection requests.
	// It transforms an active socket into a passive socket.
	listen(listenfd, LISTENQ);
		printf("Listening !!!\n");

	for ( ; ; )
	{
		/* Accept() function extract the first connection on the queue of pending connections,
		   create a new socket with the same socket type protocol and address family as the specified socket,
		   and allocate a new file descriptor for that socket
		*/
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);

		printf("Handling connection request\n");
		rw_ret = recv(connfd, command, 50, 0);			//Receives the command from the client
		printf("command received: %s\n", command);


		//Download Function

		if(strcmp(command, "GET") == 0)
		{
			rw_ret = recv(connfd,fname,50,0);
			printf("File name is %s \n:  ", fname);

			fd=open(fname,O_RDONLY,S_IRUSR);
			fstat(fd, &stat_buf);
			size = stat_buf.st_size;
			printf(" size is %d\n", size);   //prints file size
			printf("\nFile is Opened\n");
			while ( (n = read(fd, buf, BUFSIZE-1)) > 0)
			{
				buf[n] = '\0';   //set the buff[] to '\0'
				rw_ret = write(connfd,buf,n);
			}
			printf("File Sent successfully \n");
			ret = close(connfd);
			ret = close(fd);
		}


		//Upload Function

		else if(strcmp(command, "PUT") == 0)
		{
			/* recv() function is used to read incoming data on connection-oriented sockets, or connectionless sockets.
			   When using a connection-oriented protocol, the sockets must be connected before calling recv.
			   When using a connectionless protocol, the sockets must be bound before calling recv.
			*/
			rw_ret = recv(connfd,fname,50,0);				 //The recv() function receives data on a socket with descriptor connfd and stores it in fname. The recv() call applies only to connected sockets.
			printf("File name is %s :  ", fname);    //prints the sent file name
			fd=open(fname,O_WRONLY|O_CREAT,S_IRWXU); //file open with write-only access with read, write, execute/search permission for owner
			while ( (n = read(connfd, buf, BUFSIZE-1)) > 0)
			{
				buf[n] = '\0';
				rw_ret = write(fd,buf,n);
				if( n < BUFSIZE-2)
					break;
			}

			printf("file receiving completed \n");
			ret = close(connfd);
			ret = close(fd);
		}


		//File List Function

		else if(strcmp(command, "LIST") == 0)
		{
			struct dirent *de;

		    DIR *dr = opendir(".");		//opens the current directory
		    
			//if the directory couldn't be opened or returs an error, the below code executes.
			if (dr == NULL)
		    {
		    	char result[50] = "Could not open current directory\n";
		        printf("%s\n", result);
		        rw_ret = send(connfd, result, sizeof(result), 0);
		    }

			//if the directory has been opened successfully, the following code gets executed.
			while ((de = readdir(dr)) != NULL)
			{
                	    char string[50];
		            strcpy(string, de->d_name);
        		    printf("%s\n", string);
			    send(connfd, string, sizeof(string), 0);
            		}

		    char stop[] = "STOP";
		    rw_ret = send(connfd, stop, sizeof(stop), 0);
		    ret = closedir(dr);
		    printf("file listing completed \n");
		    ret = close(connfd);
		}
		// exit(1);
	}
}

/* 
   close() call shuts down the socket associated with the socket descriptor socket, and frees resources allocated to the socket.
   If socket refers to an open TCP connection, the connection is closed.
*/
