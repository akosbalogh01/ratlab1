//C[i]=A[i]+B[i]
__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
 
    // Lekérjük az aktuális work-item indexet
    int i = get_global_id(0);
 
    // Elvégezzük az összeadást az adott elemre
    C[i] = A[i] + B[i];
}
