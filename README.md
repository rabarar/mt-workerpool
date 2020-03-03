# mt-workerpool
Multi-Threaded Worker Pool

### A simple (albeit complex) example of how to develop and use a Multi-Threaded (MT)

This code demonstrates a Worker Pool with a dispatcher and a worker task along with an interrupt handler to gracefully shutdown the dispatch thread, and worker task threads


### Refactor(1)

The next thing to do is to refactor the WorkerPool class making the dispatcher and the task functions private - instead of exposing these functions - which MUST BE EXECUTED PROPERLY with synch to coordinate for graceful shutdown, we instead will go to a source/sink model for events and
dispatch. Then the interface to the pool will be the Qeuues - Source and Sink to add and process events.


