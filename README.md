Instructions for use
********************
1. compile client.cpp on client with the following command
		g++ -std=c++11 chatclient.cpp -o chatclient

2. start server.py on the server by executing the script with a single argument: the listen port
		e.g. 'python chatserve.py 12234'

3. run client executable on client machine with 2 arguments: the server address and port number
		e.g. './chatclient servername.example 12234'	

4. In client app, enter a handle

5. Upon successful connection to the server, client will wait for a message from the server

6. Send message from server

7. Send message from client

8. repeat

9. Kill the connection from either end by including '\quit' in your message

10. Upon '\quit', client app will terminate and server will remain open for connections on the listen port

