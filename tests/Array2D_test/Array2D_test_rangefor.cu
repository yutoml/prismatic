//
// Created by AJ Pryor on 1/30/17.
//
//Test range-based for loops on GPU with PRISM::Array2D

#include <iostream>
#include "ArrayND.h"
#include "thrust/host_vector.h"
#include "thrust/device_vector.h"

using namespace std;
int main(){


    thrust::host_vector<double> test_h(6,0);
    for (auto i = 0; i < test_h.size(); ++i)test_h[i] = i + 1;
    PRISM::ArrayND<2, thrust::host_vector<double> > arr_h(test_h,{2,3});
    cout << "Printing a host-side 2x3 PRISM::Array2D with thrust::host_vector" << endl;
    for (auto& i:arr_h) std::cout << i << std::endl;


    thrust::device_vector<double> test_d(test_h);
    PRISM::ArrayND<2, thrust::device_vector<double> > arr_d(test_d,{2,3});
    thrust::copy(arr_d.begin(), arr_d.end(),arr_h.begin());
    cout << "Printing a device-side 2x3 PRISM::Array2D with thrust::device_vector" << endl;
    for (auto& i:arr_h) std::cout << i << std::endl;

    cout << "Test Successful!" << endl;

}