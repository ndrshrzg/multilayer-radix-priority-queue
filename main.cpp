#include <iostream>
#include <queue>
#include "include/multilayer_radix_pq.h"


typedef struct data
{
    int x = 0;
} DATA;

int main() {

    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq;
    std::priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t>> pq;

    //std::vector<uint64_t> ar{0,1,2,3,5,64,129,257,986,2049,16895,28675,2406987,3698574,7845329,12896586,67895442138, 878954421389634};
    std::vector<uint64_t> ar{16895,28675,2406987,3698574,7845329,12896586,67895442138, 878954421389634};
    size_t size = ar.size();

    std::vector<uint64_t> res_pq;
    std::vector<uint64_t> res_mlrpq;

    for (int i=0; i < size; i++){
        pq.push(ar[i]);
        mlrpq.push(ar[i], 0);
    }

    for (int j=0; j<size; j++){
        uint64_t temp_mlrpq = mlrpq.top().first;
        uint64_t temp_pq = pq.top();

        res_mlrpq.push_back(temp_mlrpq);
        res_pq.push_back(temp_pq);

        mlrpq.pop();
        pq.pop();

    }

    for (int k=0; k<size; k++){
        std::cout << "mlrpqg: " << res_mlrpq[k] << ", pq: " << res_pq[k] << std::endl;
    }

}