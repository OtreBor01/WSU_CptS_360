/*
------------------------------ Client ------------------------------–
(1). Connect to server at server hostname and port number
(2). Prompt user for a command line 1⁄4 cmd pathname
(3). Send command line to server
(4). Receive results from server
(5). Repeat (2) until command line is NULL or quit command
*/


//***** Libraries *****
#include "linux_cmd.c"


//***** Function Prototypes *****
void DisplayMenu(void);
bool client_init(char**);


//***** Global Variables *****
struct hostent *hp;              
struct sockaddr_in  server_addr; 
int server_sock, r;
int SERVER_IP, SERVER_PORT;

//***** Function Declarations *****
void DisplayMenu(void){
	puts("********************** Menu *********************");
	puts("* get  put  ls   cd   pwd   mkdir   rmdir   rm  *");
	puts("* lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *");
	puts("*************************************************");
}

// clinet initialization code
bool client_init(char *argv[])
{
	printf("\n====================== Clinet Init ======================\n\n");
	printf("Step 1: Get Server Information\n");
	//argv[1] = "localhost"
	hp = gethostbyname(argv[1]);
	if (hp==0){
		printf("\tError: Unknown Host %s\n", argv[1]);
		exit(1);
	}
	//IP Address of server, i.e. 127.0.0.1
	SERVER_IP   = *(long *)hp->h_addr;
	//Server Port number, i.e. 58556
	SERVER_PORT = atoi(argv[2]);
	
	//**********************************************************
	printf("Step 2: Create a TCP Socket\n");
	//Conection value to write to server
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	//If failed to successful link to server
	if (server_sock<0){
		printf("\tError: Socket Call Failed\n");
		exit(2);
	}
	//**********************************************************
	printf("Step 3: Fill 'struct sockaddr_in' w/ Server's IP & Port Number\n");
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = SERVER_IP; //IP Address of server, i.e. 127.0.0.1
	server_addr.sin_port = htons(SERVER_PORT); //Server port number, i.e. 58556
	
	//**********************************************************
	// Connect to server
	printf("Step 4: Connecting to Server ....\n");
	r = connect(server_sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
	if (r < 0){
		printf("\tError: Connection Failed\n");
		exit(1);
	}
	
	//**********************************************************
	char ipAddress[20];
	//https://beej.us/guide/bgnet/html/multi/inet_ntopman.html
	//inet_ntop(int af, const void *src, char *dst, socklen_t size);
	inet_ntop(AF_INET, (const void *) &(server_addr.sin_addr), ipAddress, INET_ADDRSTRLEN);
	//(char*) inet_ntoa(SERVER_IP)
	printf("\n----------------- Connection Successful -----------------\n");
	printf(" Hostname = %s || IP = %s || PORT = %d\n", hp->h_name, ipAddress, SERVER_PORT);
	printf("---------------------------------------------------------\n");
	printf("\n===================== Init Finished =====================\n\n");
	return true;
}

int main(int argc, char *argv[ ])
{
	int bytes, n;
	char line[MAX], ans[MAX];
	if (argc < 3){
		printf("Invalid Usage: Client ServerName SeverPort\n");
		exit(1);
	}
	
	bool isInit = client_init(argv);
	// sock <---> server
	puts("================ Processing Loop ================");
	while (1){
		DisplayMenu();
		//Handles User's Input and Validation
		printf("Input: ");
		bzero(line, MAX);                // zero out line[ ]
		fgets(line, MAX, stdin);         // get a line (end with \n) from stdin
		line[strlen(line)-1] = 0;        // kill \n at end
		if (line[0]==0){                  // exit if NULL line
			exit(0);
		}
		//local commands
		if(line[0] == 'l' && line[1] != 's'){
			isServer = false;	
			char command[10], pathname[MAX];
			char *l = &line[1];
			int cmdIndex = getLineInfo(l, command, pathname);
			if(cmdIndex == -1){
				printf("INVALID: '%s'\n", line);
			}
			else{
				printf("Pathname: '%s'\n", pathname);
				int value = fcmds[cmdIndex](pathname);
			}
		}
		else{
			isServer = true;
			int size = 0;
			// Sends user's input, line, to server
			//ssize_t write(sockfd, void *buf, size_t, len)
			n = write(server_sock, line, MAX);
			printf("Client: Wrote %d Bytes || Output = '%s'\n", n, line);
			if(strstr(line, "get")){
				char buff[MAX];
				while((n = read(server_sock, buff, MAX)) != 0)
				{
					size += n;
					printf("N = %d || Total = %d\n", n, size);
					if(!strcmp(buff, EOF_STR)){ break; }
					bzero(buff,MAX);
				}
				printf("Recieved %d bytes\n", size);
			}
			else if(strstr(line, "put")){
				char buff[MAX];
				while((n = read(server_sock, buff, MAX)) != 0)
				{
					size += n;
					printf("N = %d || Total = %d\n", n, size);
					if(!strcmp(buff, EOF_STR)){ break; }
					bzero(buff,MAX);
				}
				printf("Sent %d bytes\n", size);	
			}
			else{
				n = read(server_sock, &size, sizeof(int));
				const int s = size + 1;
				char str[s];// = (char*) malloc(sizeof(char*)*n);
				n = read(server_sock, str, s);
				printf("Client: Read %d Bytes || Input = '%s'\n",n, str);
			}
		}
	}
	return 0;	
}


