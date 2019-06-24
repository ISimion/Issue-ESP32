# Issue-ESP32
My issue: How I can get to work a foo.local mDNS link between an ESP32 Server and an ESP32 Client?

I am trying to access http:///myServerName.local/ link through a client running on ESP32 after I establish the mDNS on the server running on another ESP32.

I tried implementing an HTTP request with the previous link, but it won't seem to work. I also tried just to access the link from my browser but that won't work either.

The only thing I got so far was -1 for httpCode and nothing else.
