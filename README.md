# README #

This is the codebase of Efficient Core Query over Temporal Bipartite Graphs

## File Organization ##

* **src/bigraph.***: the basic data structure of temporal bipartite graphs
* **src/indConst.***: the construction and query algorithms of the index
* **src/utility.***: the supportive functions
* **src/main.cpp**: the main function

## How to Use the Code? ##

* First, running the code under the path `src` to construct the index for the graph.
* Then, using this index as input, you can perform query via the constructed index, the detailed commands are as follows.

### Command Lines

- Dominant coreness computation: `./domind -domIndDelta path_of_datasets`.

- Query via the Index: `./domind -domQuery path_of_datasets alpha beta`.

### Data formats

- `graph.meta` contains the number of edges and the number of nodes in each layer.

    ```
    500 //n1 
    1000 //n2
    1301942 //m
    ``` 

- `graph.e` contains all the edges. e.g.,

    ```
    1 1 1222992000 // u v timestamp
    2 2 1222992000
    3 3 1222992000
    4 4 1222992000
    5 5 1222992000
    6 6 1222992000
    7 7 1222992000
    8 8 1222992000
    9 9 1222992000
    10 7 1222992000
    ```

### Acknowledgment ###

*Part of code is from [this work](https://github.com/QuanQuan0/Bicore-Maintenance).*



