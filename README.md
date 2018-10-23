# Cache-Simulator
Cache Simulator in C. This has direct mapping and 4way associativity
*****To generate executable file use the following command*****

gcc FinalCacheSimulator.c -lm -o final

*****To generate output use the following command*****

To see the usage please use the following command

./final and hit enter.
You will see the usage to run the program

Yo run the executable:
./final <cachesize> <blocksize> <shared/split cache> <associativity>
<cachesize>:32768
<blocksize>:8/32/128
<sared cache>:0
<split cache>:1
<associativity>:1(direct mapping)
<associativity>:4(4-way)

Example command:(cache size 32768, blocksize 8, combined cache, direct mapping)

./final 32768 8 0 1

Example command:(cache size 32768, blocksize 128, split cache, 4-Way)


./final 32768 128 1 4
