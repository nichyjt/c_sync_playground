The goal of this mini project is to implement a Multi-Producer, Single Consumer atomic message queue.

This message queue is essentially a circular buffer.

Constraints
1. No need for FIFO behaviour on producer side (can be unfair)
2. FIFO enforcement only needed once a producer's data has been put in
3. No locks at all

Implementation Limitations:
- Operations are serialized (no true read/write concurrency)
  - a possibly OK trade-off since Golang channels are also implemented similarly and are still performant (enough)...