//TODO 2 
//set local memory size
#define LOCALMEMSIZE 64

//A input ints
//results A length

__kernel void sumreduce(__global const unsigned int* A,  __global unsigned int* results, const int totalItemsNum) {

    //TODO 2
    //sum of items in workgroup[i] in A -> results[i]
    unsigned int blocksize = get_local_size(0)/2;
    __local unsigned int localmem[LOCALMEMSIZE];

    unsigned int i = get_local_id(0);
    localmem[i] = A[get_global_id(0)];
    // EZ NAGYON FONTOS:
    barrier(CLK_LOCAL_MEM_FENCE);

    while (blocksize >= 1)
    {
        if (i < blocksize) 
        {
            localmem[i] += localmem[blocksize + i];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
        blocksize /= 2;
    }
    
    if (get_local_id(0) == 0)
    {
        results[get_group_id(0)] = localmem[0];
    }
}

