# Http-Server-From-Stratch

Basic multi-threaded HTTP server that is implemented with native C/C++ libraries.

## Features
- Does not depend on any third-party library/framework
- Supports CRUD operations
- Supports query strings
- Session support
- Session management (garbage collector for sessions)
- Has multi-threaded approach
- Using thread pooling
- Control the speed ratio between new comers and continued processes
- Handles big amount of concurrent connections
- Load static files
- Create dynamic files with the help of utility methods
- Filter incoming requests (all requests are first faced with a service method)
- Custom routes and filters can be added easily (just modify Routes.cpp file)

## Installation - Usage

Use C++/11 compiler, this will be enough to run the existig small web-app in the server:

```bash
make
./exec
```

To run the server with your application, you can implement your routes under the related methods:</br>
- httpGet
- httpPost
- httpPut
- httpDelete</br>
<p>
under the file routes/Routes.cpp</br>
And, the rest will be handled by the server...
</p>

## How it works?

There are mainly 5 type of threads that run the server:
- main thread (default amount: 1)
  - All incoming request are first faced here.
  - Then the socketIDs are sent to worker threads to be parsed.

- worker thread (default amount: 45)
  - Worker threads read all the incoming request from their sockets and parses the request.
  - If the request does contain a sessionID, the parsed request is added to the request queue of the related session, which will be handled by client session threads.
  - If the request does not contain a sessionID, the parsed request is forwarded to client initializer thread.

- client initializer thread (default amount: 20)
  - Not sessioned request are forwarded here.
  - Then a new session is created for the client and forwards the request to the Routes::service() method.
  - Take the html response from Routes::service() method and send the response to the related socket and close the socket.
  - Also assign the session to a client session thread to carry out the client's(session) further requests.

- client session thread (default amount: 35)
  - Loops on the clients(sessions) and meets their requests.
  - Clients(sessions) are shared equally amoung client session threads, and each thread loops on its clients(sessions).

- session cleaner thread (default amount: 1)
  - Loops on all sessions and check when they were last used.
  - If they are not used for the last X time (default duration: 10 min) they will be expired by this thread.

## License
[MIT](https://choosealicense.com/licenses/mit/)
