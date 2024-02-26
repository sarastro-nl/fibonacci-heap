# fibonacci-heap
C implementation of a [fibonacci heap](https://en.wikipedia.org/wiki/Fibonacci_heap).

This implementation is loosly based on the python files shown in [these videos](https://www.youtube.com/watch?v=0vsX3ZQFREM&list=PL9xmBV_5YoZNkwWDXcSiZjMgacw2P0U2j&index=1) by Michael Sambol but with some improvements in efficiency.

The following functions for a fibonacci heap are implemented:

*  `insert`
*  `minimum`
*  `remove_minimum`
*  `change_value`
*  `remove_node`

Also a `print_heap()` function is provided that outputs the current heap to console.

An example of what can be done is:

```c
    insert(7);
    insert(10);
    insert(9);
    insert(4);
    insert(5);
    insert(3);
    insert(8);
    print_heap();
    while (node_t *node = remove_minimum()) {
        free(node);
    }
```
which will give the following output

```text
7-10-9-4-5-3-8
======================
removing minimum: 3
7-10-9-4-5-8
======================
consolidating
7--9-4-5-8
│
10
======================
7--4-5-8
│  │
10 9
======================
4----5-8
│
9-7
  │
  10
======================
4----5
│    │
9-7  8
  │
  10
======================
removing minimum: 4
5-9-7
│   │
8   10
======================
consolidating
5----9
│
8-7
  │
  10
======================
removing minimum: 5
9-8-7
    │
    10
======================
consolidating
8-7
│ │
9 10
======================
7
│
10-8
   │
   9
======================
removing minimum: 7
10-8
   │
   9
======================
consolidating
removing minimum: 8
10-9
======================
consolidating
9
│
10
======================
removing minimum: 9
10
======================
consolidating
removing minimum: 10
```
