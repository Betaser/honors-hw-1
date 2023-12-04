# An exploration into dynamic associative trees.

Learning Goals 
==============

To implement reading an input file and creating a recursive associative tree from it. Part 1 of a dive into possibilities of persistent data structures (though, unfortunately I ran out of time, and the two hoomeworks ended up fairly similar)

Main Takeaways
==============

- The input files have to be formatted in a specific way, such that referenced data comes earlier. This way, a single pass over the file allows the construction of trees to be made and trees to store other trees inside of them.

- Due to time constraints, and losing my laptop charger, there are a few drawbacks to my implementation.
1. It could use some more comments, especially for all the structs used and the many Linked-List with dummy heads.
2. There are memory leaks, that I unfortunately left till last. These would be the first thing I woulf fix, although this is the last open-ended honors assignment and I have finals to get to.
3. I wanted to implement persistent trees with memory offsets to store values, but void * memory offsets are annoying to implement. It should still be possible, and I had a few pointers and offset ideas, but I did not get to trying them out.



History
=======
I got this idea from tinkering in Clojure every so often, and hearing about the persistent manner in which they store their immutable data structures in an efficient manner. Check out a summary on their website: https://clojure.org/reference/data_structures 

# Who Gets the Cake

Learning Goals 
==============
# An exploration into dynamic associative trees.

Learning Goals 
==============

To implement reading an input file and creating a recursive associative tree from it. Part 1 of a dive into possibilities of persistent data structures (though, unfortunately I ran out of time, and the two hoomeworks ended up fairly similar)

Main Takeaways
==============

- The input files have to be formatted in a specific way, such that referenced data comes earlier. This way, a single pass over the file allows the construction of trees to be made and trees to store other trees inside of them.

- Due to time constraints, and losing my laptop charger, there are a few drawbacks to my implementation.
1. It could use some more comments, especially for all the structs used and the many Linked-List with dummy heads.
2. There are memory leaks, that I unfortunately left till last. These would be the first thing I woulf fix, although this is the last open-ended honors assignment and I have finals to get to.
3. I wanted to implement persistent trees with memory offsets to store values, but void * memory offsets are annoying to implement. It should still be possible, and I had a few pointers and offset ideas, but I did not get to trying them out.



History
=======
I got this idea from tinkering in Clojure every so often, and hearing about the persistent manner in which they store their immutable data structures in an efficient manner. Check out a summary on their website: https://clojure.org/reference/data_structures 

This assignment asks you to keep track of information using an array.
