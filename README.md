This program solves the Traveling Salesman Problem (TSP) using Branch and Bound (B&B) algorithm with OpenMP for parallelism. 
It reads distance matrices from files, dynamically loads them into memory, and generates subproblems up to a predefined depth (TARGET_DEPTH) using recursive tree search. Subproblems are stored as nodes
and the B&B algorithm explores these nodes in parallel, pruning paths that exceed the global minimum cost. OpenMP threads handle different subproblems, and thread-safe updates ensure the global best solution is maintained. Execution time, optimal path cost, and route are logged to a CSV file for analysis.
 
