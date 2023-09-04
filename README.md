# Hole Descriptor List

In IP networks, all hosts must support what is known as fragmentation & reassembly. That is large packets exceeding the underlying maximum packet size for a link is packetized into smaller packets sized for the link, known as IP fragments.

Well it turns out theres a really nice data structure called a Hole Descriptor List which sorts a fragment into the corresponding hole in expected time of 1 compare due to fragments being received in almost sorted order for quick coalescing. The data structure is outlined in [IP DATAGRAM REASSEMBLY ALGORITHMS (RFC815)](https://datatracker.ietf.org/doc/html/rfc815) by David D. Clark.

This implementation supports overlapping fragments and seen/duplicate fragment suppression.

An example driver program is included in `driver.c` to see how to use the API. We also have a small suite of unit tests and a basic fuzz tester (which has helped tremendously iron out bugs).
