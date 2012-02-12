#!/usr/bin/python

import os

try:
    print "HTTP/1.0 200 OK"
    print "CONTENT-TYPE: text/plain\n\n"

    print os.getenv("QUERY_STRING")
    for i in range(10):
        print i, "I love money so much"
except:
    exit(0)
