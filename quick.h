#ifndef _QUICK
#define _QUICK

#include<stdlib.h>

void swap(size_t* x, int* y, int i, int j) {
    size_t temp = x[i];
    x[i] = x[j];
    x[j] = temp;

    int temp2 = y[i];
    y[i] = y[j];
    y[j] = temp2;
}

int partition(size_t* dists, int* indexes, int p, int r) {
    size_t x = dists[r];
    int i = p - 1;
    for (int j = p; j < r; j++) {
        if (dists[j] <= x){
            i++;
            swap(dists, indexes, i, j);
        }
    }
    swap(dists, indexes, i+1, r);
    return i + 1;
}

void quickSort(size_t* dists, int* indexes, int p, int r) {
    if (p < r) {
        int q = partition(dists, indexes, p, r);
        quickSort(dists, indexes, p, q - 1);
        quickSort(dists, indexes, q+1, r);
    };
}

void quickSelect(size_t* dists, int* indexes, int p, int r, int k) {
    if (p < r) {
        int q = partition(dists, indexes, p, r);
        if (q - p >= k - 1) {
            quickSelect(dists, indexes, p, q - 1, k);
        } else if (q - p < k - 1) {
            quickSelect(dists, indexes, q+1, r,  k - (q - p + 1));
        } 
    }
}

#endif