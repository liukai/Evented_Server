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
I used the httperf to evaluate the performance. In the evaluation I use 200 connections. The request rate per second `rate` varies from 10~100.
The content of requested dynamic page are the same, each page is 102 KB.

Here is the performance evaluation("I" is improved server and "B" is baseline server):

            Error rate(B)   Error rate(I)     Net I/O(B)        Net I/O(I)
            10 req/s        0.0             0.0           216.5 KB/s        207.2   KB/s

            20 req/s        0.0             0.0           433.2 KB/s        413.8   KB/s

30 req/s        0.0             0.0           649.3 KB/s        619.8   KB/s

40 req/s        32.5            0.0           240.5 KB/s        824.8   KB/s

50 req/s        50.0            0.0           116.3 KB/s        1025.2  KB/s

60 req/s        75.5            0.0           51.5  KB/s        1227.3  KB/s

70 req/s        92.5            2.5           12.5  KB/s        1347.2  KB/s

80 req/s        100.0           0.5           4.5   KB/s        1577.2  KB/s

90 req/s        100.0           30.5          3.2   KB/s        791.5   KB/s

100 req/s       100.0           42.0          4.1   KB/s        413.8   KB/s


As we can see in this figure, with smaller request amount, the baseline server has even slightly better performance. However as request rate increases its performance decays rapidly.

For the improved server, its performance is more stable, although its initial performance is slightly worse than baseline.


