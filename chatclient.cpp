#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <climits>
#include <string.h>
#include <string>
using std::cout;
using std::cin;
using std::endl;
using std::string;

#define MAXDATASIZE 1024    // max number of bytes we can get at once

/* FUNCTION DECLARATIONS */
/* ********************* */
void *get_in_addr(struct sockaddr *sa);  // get sockaddr, IPv4 or IPv6:
int* initialConnection(char*, char*);
int receiveMsg(const int*);
int sendMsg(const string&, const int*);


int main(int argc, char *argv[]) {
	// check for correct number of command line arguments
	if (argc != 3 ) {
		fprintf(stderr,"usage: client hostname port\n");
		exit(1);
	}

	cout << "Server hostname: " << argv[1] << endl;
	cout << "Server port: " << argv[2] << endl << endl;

	// set handle
	string handle;
	cout << "Choose a handle (max length of 10, no spaces): ";
	cin >> handle;
	int badHandle = 1;
	while (badHandle) {
		if (handle.length() > 10) {
			cout << "Too long. Try again: ";
			cin >> handle;
		}
		else {
			badHandle = 0;
		}
	}
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	int* sockfd = initialConnection(argv[1], argv[2]);

	int stay_alive = 1;
	while (stay_alive == 1) {
		stay_alive = receiveMsg(sockfd);
		if (!stay_alive) {
			break;
		}
		stay_alive = sendMsg(handle, sockfd);
	}

	close(*sockfd);

	return 0;
}


int sendMsg(const string& handle, const int* sockfd) {
	int numbytes;
	string msg, fullMsg;
	cout << ">> ";
	getline(cin, msg);

	fullMsg = handle + "> " + msg;

	if ((numbytes = send(*sockfd, fullMsg.data(), fullMsg.size(), 0)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	if (msg == "\\quit") {
		return 0;
	}

	return 1;
}


int receiveMsg(const int* sockfd) {
	int numbytes;
	char buf[MAXDATASIZE];

	if ((numbytes = recv(*sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';

	if (strstr(buf, "\\quit")) {
		cout << "Server closed connection" << endl;
		return 0;
	}

	cout << buf << endl;
	return 1;
}


int* initialConnection(char* arg1, char* arg2) {
	int sockfd, numbytes, temp;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// populate addrinfo struct
	if ((rv = getaddrinfo(arg1, arg2, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		// lose the pesky "Address already in use" error message
		int yes = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	cout << "client: connecting to " << s << endl;

	freeaddrinfo(servinfo); // all done with this structure

	int okay_code = 200;
	temp = htonl(okay_code);
	if ((numbytes = send(sockfd, (const char*)&temp, sizeof(temp), 0)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	return &sockfd;
}


void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
