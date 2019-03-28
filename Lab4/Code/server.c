/*
------------------------------ Server ------------------------------–
(1). Set virtual root to Current Working Directory (CWD)
(2). Advertise server hostname and port number
(3). Accept a connection from client
(4). Get a command line 1⁄4 cmd pathname from client
(5). Perform cmd on pathname
(6). Send results to client
(7). Repeat (4) until client disconnects
(8). Repeat (3) to accept new client connection
*/


//***** Libraries *****
#include "linux_cmd.c"


//***** Function Prototypes *****
bool server_init(char*);
int runCommand(char*);
int _get(char*), _put(char*);


//***** Global Variables *****
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;
int  mysock, client_sock;              // socket descriptors


//***** Function Declarations *****
// Server initialization code
bool server_init(char *name)
{
	int r;
	printf("\n==================== Server Init ======================\n\n");   
	// get DOT name and IP address of this host
	printf("Step 1: Get and Display Server Host Information\n");
	hp = gethostbyname(name);
	if (hp == 0){
		printf("\tError: Unknown Host\n");
		exit(1);
	}

	
	char ipAddress[20];
	//https://beej.us/guide/bgnet/html/multi/inet_ntopman.html
	//inet_ntop(int af, const void *src, char *dst, socklen_t size);
	inet_ntop(AF_INET, (const void *) &(*(long *)hp->h_addr), ipAddress, INET_ADDRSTRLEN);
	//inet_ntoa(*(long *)hp->h_addr);
	printf("  *** Hostname = %s || IP = %s ***\n", hp->h_name, ipAddress);

	//**********************************************************
	//Create a TCP socket by socket() syscall
	printf("Step 2: Create a Socket\n");
	mysock = socket(AF_INET, SOCK_STREAM, 0);
	if (mysock < 0){
	  printf("\tError: Socket Call Failed\n");
	  exit(2);
	}
	
	//**********************************************************
	printf("Step 3: Fill 'struct sockaddr_in' w/ Host's IP & Port Number\n");
	// initialize the server_addr structure
	server_addr.sin_family = AF_INET;                  // for TCP/IP
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address  
	server_addr.sin_port = 0;   // let kernel assign port

	//**********************************************************
	printf("Step 4: Bind Socket to Host Information\n");
	// bind syscall: bind the socket to server_addr info
	r = bind(mysock,(struct sockaddr *)&server_addr, sizeof(server_addr));
	if (r < 0){
	   printf("\tError: Bind Failed\n");
	   exit(3);
	}

	//**********************************************************
	printf("Step 5: Get and Display Kernel Assigned Port Number\n");
	// find out socket port number (assigned by kernel)
	int length = sizeof(name_addr);
	r = getsockname(mysock, (struct sockaddr *)&name_addr, &length);
	if (r < 0){
	  printf("\tError: Get Socketname Error\n");
	  exit(4);
	}

	// show port number
	int serverPort = ntohs(name_addr.sin_port);   // convert to host ushort, server port number
	printf("  *** Port = %d ***\n", serverPort);

	// listen at port with a max. queue of 5 (waiting clients) 
	printf("Step 6: Server is Listening\n");
	listen(mysock, 5);
	printf("\n===================== Init Done =======================\n\n");
	return true;
}

//get: download the pathname file from server
int _get(char* pathname){
	/*	
		For the get filename command, it is essentially the same as a file copy program split into two parts. 
		The server opens the file for READ, read the file contents and send them to the client. 
		The client opens a file for WRITE, receives data from the server and writes the received data to the file.
	*/
	//stat filename to get type AND SIZE
	struct stat mystat, *sp = &mystat;
	if (lstat(pathname, sp) < 0){
		printf("\tError: No Such File '%s'\n", pathname);
		return -1;
	}
	if (!S_ISREG(sp->st_mode)){
		printf("\tError: '%s' is Not a Regular File\n", pathname);
		return -1;
	}
	char buff[MAX];
	//open filename for READ
	int fd = open(pathname, O_RDONLY), n;
	if(fd < 0)
	{
		printf("\tError: Unable to Open '%s' for Reading\n", pathname);
		return -1;
	}
	else
	{
		printf("Filename = '%s'\nExpecting %ld bytes\n", pathname, sp->st_size);
		bzero(buff,MAX);
		while((n = read(fd, buff, MAX)) != 0) //	while(n=read(fd, buf, MAX))
		{
		    n = write(client_sock, buff, n); //send n bytes from buf
		    bzero(buff,MAX);
		}
		printf("File Transfer Complete\n");
        write(client_sock, EOF_STR , strlen(EOF_STR)+1);
	}
	close(fd); //close file;
}

//put: upload the pathname file to server
int _put(char* pathname){
	//For the put filename command, simply reverse the roles of the server and client.
	struct stat mystat, *sp = &mystat;
	if (lstat(pathname, sp) < 0){
		printf("\tError: No Such File '%s'\n", pathname);
		return -1;
	}
	if (!S_ISREG(sp->st_mode)){
		printf("\tError: '%s' is Not a Regular File\n", pathname);
		return -1;
	}
	char buff[MAX];
    //char *lastparenth = strrchr(myargs[1],'/')+1;
	int fd = open(pathname, O_RDONLY), n;
    if(fd < 0)
    {
		printf("\tError: Unable to Open '%s' for Reading\n", pathname);
        return -1;
    }
    else{
    	printf("Pathname = '%s' || Size %ld bytes\n", pathname, sp->st_size);
		bzero(buff,MAX);
		while((n = read(fd, buff, MAX)) != 0) //	while(n=read(fd, buf, MAX))
		{
		    n = write(client_sock, buff, n); //send n bytes from buf
		    bzero(buff,MAX);
		}
		printf("File Transfer Complete\n");
        write(client_sock, EOF_STR , strlen(EOF_STR)+1);
    }
	close(fd); //close file;
	return 0;
}


int runCommand(char *line){
	char command[10], pathname[MAX];
	int cmdIndex = getLineInfo(line, command, pathname), r = -1;
	//Invalid Command
	if(cmdIndex == -1){
		if(!strcmp(command, "get")){ r = _get(pathname); }
		else if(!strcmp(command, "put")){ r = _put(pathname); }
		else{ 
			printf("INVALID: '%s'\n", line);
			strcat(line, " -> Invalid Input"); 
			r = -1;
		}
	}
	//Valid Command
	else{
		//Run command
		int value = fcmds[cmdIndex](pathname);
		//Command ran successfully
		if(value == 0){
			r = 0;
		}
		//Command Failed	
		else if(value == -1){
			r = -1;
		}
		//Command returned and address to a string
		else{ 
			char *str = (char*)value;
			//Valid Address
			if(str != NULL){
				strcpy(line, str);
				r = 1;
			}
			//Invalid Address
			else{
				printf("\tError: Returned Invalid String Address\n");
				r = -1;

			}
		}
	}
	if(r == 0){ strcat(line, " -> SUCCESS"); }
	else if(r == -1){ strcat(line, " -> FAILURE"); }
	return r;
}

int main(int argc, char *argv[], char *env[])
{
	//sets cwd to root
	chdir("/");
	printf("cwd: '%s'\n", (char*)_pwd(""));
	char *hostname;
	if (argc < 2){
		hostname = "localhost";
	}
	else{
		hostname = argv[1];
	}
	bool isInit = server_init(hostname); 
	// Try to accept a client request
	while(1){
		printf("Server: Accepting New Connection...\n"); 
		// Try to accept a client connection as descriptor newsock
		int length = sizeof(client_addr);
		client_sock = accept(mysock, (struct sockaddr *)&client_addr, &length);
		if (client_sock < 0){
			printf("\tServer Error: Accept Error\n");
			exit(1);
		}
		char ipAddress[20];
		//https://beej.us/guide/bgnet/html/multi/inet_ntopman.html
		//inet_ntop(int af, const void *src, char *dst, socklen_t size);
		inet_ntop(AF_INET, (const void *) &(client_addr.sin_addr.s_addr), ipAddress, INET_ADDRSTRLEN);
		//inet_ntoa(client_addr.sin_addr.s_addr);
		printf("Server: Accepted a Client Connection\n");
		printf("-----------------------------------------------\n");
		printf("\tIP = %s || Port = %d\n", ipAddress, ntohs(client_addr.sin_port));
		printf("-----------------------------------------------\n");
		// Processing loop: newsock <----> client
		while(1){
			char line[MAX];
			memset(line, 0,MAX); 
			int bytes = 0;
			bytes = read(client_sock, line, MAX);
			if (bytes == 0){
				printf("Server: Client Died, Server Loops\n");
				close(client_sock);
				break;
			}
			// show the line string
			printf("Server: Read %d Bytes || Input = '%s'\n", bytes, line);
			isServer = true;	
			int result = runCommand(line);
			int n = strlen(line)+1;
			bytes = write(client_sock, &n, sizeof(int));
			bytes = write(client_sock, line, n);
			printf("Server: Wrote %d Bytes || Output = '%s'\n", bytes, result > -1? "SUCCESS" : "FAILURE");
			printf("Server: Ready For Next Request\n");
			memset(line, 0, n); 
		}
	}
	return 0;
}

