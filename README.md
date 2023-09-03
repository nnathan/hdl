# Hole Descriptor List

Implementation of the "Hole Descriptor List" explained in
[IP DATAGRAM REASSEMBLY ALGORITHMS (RFC815)](https://datatracker.ietf.org/doc/html/rfc815).

This design includes buffering fragments
which doesn't seem apparent in the RFC.
Overlapping fragments are supported.
Duplicate / already buffered fragments are discarded.

A small suite of unit tests are and a basic fuzz tester is included.
