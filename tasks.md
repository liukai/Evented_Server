Phase 2
==============
Architecture

1. Parse the Request
    - O: the url
2.1 Service the static content.
    - A module to service static content
2.2 Service the dynamic content.
3.1 How to organize the server thing?
    * conf
        - call which conf?
        - Directory mapping.

Improvement:
- Per process is harmful.
- Even simple cgi.
- Cache can be useful.


Phase 1
==============
1. Implement the socket: without concurrency: 0/4
    1.1 Implement the client: 0.0/1
    1.1 Implement the server: 0.5/2
    1.2 inter-communicate: 0.0/1
2. Implement the concurrent server: 0/4
    1.0 hot points:
        - Concurrency: Client distribution will be OK
        - Resource Accessor -- Generalized one
            * WHAT I WANT: Parse the request: what are you looking for.
                - Dynamic
                - Static
                 - Non-exist
            * Get it from cache
            * Get it from the disk. `Loader` introduced, a generalized concept of it.
                - Static
                - Dynamic
                - Non-exist
    1.1 study the evented-server: 0/1.5
    1.2 implement the evented server: 0/2
    1.3 test the evented server: 0/1
3. Implement the HTTP 1.0 protocol: 0/4
4. Serving the dynamic content and static content: 0/8
3. Implement the Cache: 0/4
5. Implement the new Protocol: 6/6
    5.1 Desisions
        * Text-based, but was able to compress the data.
        * Dual port -- it would be unnecessary.
6. Experiment
    6.1: What metrics will be used for the evaluation.
7. Side-bar: 
    7.1 refactor server to a class: 0.5/1.0
    7.2 refactor client to a class

