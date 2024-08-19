# tabc

This is the code base for temporal core queries in bipartite graphs.

## File structure

```
/tabc
    /src
        main.cpp
        graph.cpp
        graph.h
        query.cpp
        query.h
        utils.cpp
        utils.h
    /data
        graph.e
    README.md
```

## Requirements

- C++11 or later
- A C++ compiler (e.g., g++, clang++)

## Setup

1. Clone the repository or download the source code.
2. Ensure the `graph.e` file is placed in the `data` directory.

## Compilation

Navigate to the `src` directory and run the following command:

```bash
g++ -std=c++11 -o main src/main.cpp src/graph.cpp src/query.cpp src/utils.cpp
```

## Running the Program
After compilation, run the compiled executable:

```bash
./main
```


## Input File

The input file graph.e should contain lines in the format: 

```
u v t
```

where u and v are vertices and t is the timestamp of the edge between them.