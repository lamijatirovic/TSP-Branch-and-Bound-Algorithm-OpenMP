This program solves the Traveling Salesman Problem (TSP) using Branch and Bound (B&B) algorithm with OpenMP for parallelism. 
It reads distance matrices from files, dynamically loads them into memory. 
In parallel version subproblems are generated up to a predefined depth (TARGET_DEPTH) using recursive tree search. Subproblems are then stored as nodes and B&B algorithm explores these nodes in parallel, pruning paths that exceed the global minimum cost. OpenMP threads handle different subproblems, and thread-safe updates ensure the global best solution is maintained.
Execution time, optimal path cost, and route are logged to a CSV file for analysis.
 
