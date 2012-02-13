Introduction
-----------------------------
In this project, both baseline server and improved server serve the content in the "event-driven" way. The concurrency was achieved by epoll/kqueue.

Dependencies:

* libevent: libevent is a event library that could work in different platform (for example, in Mac OS libevent it will use kqueue and in Ubuntu it will use epoll)
* boost::filesystem: I use boost::filesystem library to deal with file system related operations.

Baseline Server
-----------------------------
The baseline server(event-based) serve the dynamic contents via traditional CGI, that is when a new request arrives, the server will fork-exec the CGI script.

Improved Server
-----------------------------
In the improved server, the CGI scripts are represented as shared libraries(so). Thus when a new request arrives, the server will 

1. check if the CGI script(shared library) has been loaded. If not, server will load.
2. calls the "write_to"(all CGI scripts need to export the "write_to" function.

How to run the server
-----------------------------
Run baseline server: `make cgi`

Run improved server: `make so`

Performance
-----------------------------
I used the httperf to evaluate the performance. The request rate per second `rate` varies from 500/s~5500/s
The content of requested dynamic page are the same, each page is 102 KB.

Here is the performance evaluation("I" is improved server and "B" is baseline server):

                        Error rate(B)   Error rate(I)      Net I/O(B)         Net I/O(I)     Response time(B)    Response time(I)
           500 req/s        0.0             0.0           1216.5 KB/s        2207.2   KB/s      4.9 ms              0.0 ms
          1000 req/s        0.0             0.0           2433.2 KB/s        4413.8   KB/s      10.2 ms             0.0 ms
          1500 req/s        0.0             0.0           3649.3 KB/s        5619.8   KB/s      55.4 ms             0.0 ms
          2000 req/s        7.2             0.0           3240.5 KB/s        8224.8   KB/s      127.7 ms            3.0 ms
          2500 req/s        22.0            0.0           2616.3 KB/s        10025.2  KB/s      242.2 ms            2.1 ms
          3000 req/s        34.5            0.0           2051.7 KB/s        14217.3  KB/s      442.2 ms            8.5 ms
          4500 req/s        43.5            2.5           1712.5 KB/s        18457.2  KB/s      551.4 ms            14.3 ms
          5000 req/s        57.0            6.5           1214.2 KB/s        19242.2  KB/s      741.4 ms            24.3 ms
          5500 req/s        78.0           10.5           321.2  KB/s        18791.5  KB/s      431.1 ms            41.2 ms

The "error" is all the non 2xx response or timeout.

As we can see in this figure, with smaller request amount, the baseline server has even slightly better performance. However as request rate increases its performance decays rapidly.

For the improved server, its performance is more stable, although its initial performance is slightly worse than baseline.


