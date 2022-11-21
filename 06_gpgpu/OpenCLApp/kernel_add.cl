//C[i]=A[i]+B[i]
__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
 
    // Lek�rj�k az aktu�lis work-item indexet
    int i = get_global_id(0);
 
    // Elv�gezz�k az �sszead�st az adott elemre
    C[i] = A[i] + B[i];
}
