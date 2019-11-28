#ifndef BST_H_
#include "bst.h"
#endif

#include <iostream>
#include <thread>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <fstream>
#include <time.h>
#include <unistd.h>

#define NR_OPERATIONS   10000
#define BST_OBJECT_NAME "bst3"
#define INPUT_FILE      "values.txt"   // using input_gen.cpp to generate data file

#define NR_THREADS      1           // must be equal or less nr_cpus
#define NR_RECOVERY_THREADS     1   //the default value

bool stop = 0;
double *times;

double mysecond() {
    struct timeval tp;
    int i;
    i = gettimeofday(&tp, NULL);
    return ( (double)tp.tv_sec + (double)tp.tv_usec*1.e-6);
}

void insert(BSTree *bst, Value_t *values, int cur_nr_threads) {
    for(int i=0; !stop && i<NR_OPERATIONS/cur_nr_threads; i++) {
        bst->Insert(values[i]);
    }
}

void search(BSTree *bst, Value_t *values, int cur_nr_threads) {
    bool tmp_value;
    for(int i=0; !stop && i<NR_OPERATIONS/cur_nr_threads; i++) {
        tmp_value = bst->Search(values[i]);
        if(__glibc_unlikely(tmp_value != true)){
            std::cout << "search failed, value: " << values[i] << std::endl;
        }
    }
}

void _delete(BSTree *bst, Value_t *values, int cur_nr_threads) {
    bool is_delete_success;
    for(int i=0; !stop && i<NR_OPERATIONS/cur_nr_threads; i++) {
        is_delete_success = bst->Delete(values[i]);
        if(__glibc_unlikely(is_delete_success == false)){
            std::cout << "delele failed, key: " << (unsigned long)values[i] << std::endl;
        }
    }
}

void print_result(int thread_num, BSTree *bst) {
    double total_time = times[1] - times[0];
    std::cout << "thread num: " << thread_num;
    std::cout <<", total time: " << total_time << \
    ", OPS: " << NR_OPERATIONS*1.0/(total_time) << \
    ", size: " << bst->Size() << std::endl;
}

int main() {
    // values from file
    Value_t *values = new Value_t[NR_OPERATIONS];
    std::ifstream ifs;
    ifs.open(INPUT_FILE);
    unsigned long tmp;
    for(long long int i=0; i<NR_OPERATIONS; ++i) {
        ifs >> values[i];
    }

    int nr_cpus = get_nprocs_conf();
    std::thread *threads[nr_cpus];
    times = new double [2];
    BSTree *bst;
    //test insert
    std::cout << "test insert: " << std::endl;
    for(int thread_idx=0; thread_idx<NR_THREADS; thread_idx++) {
        bst = new BSTree(BST_OBJECT_NAME);
        bst->Open();
        times[0] = mysecond();
        for(long long int i=0; i<=thread_idx; i++)
            threads[i] = new std::thread(insert, bst, values+i*NR_OPERATIONS/(thread_idx+1),
                 thread_idx+1);
        for(int i=0; i<=thread_idx; i++)
            threads[i]->join();
        times[1] = mysecond();
        print_result(thread_idx+1, bst);
        delete bst;
    }
    // test get
    std::cout << "test get: " << std::endl;
    for(int thread_idx=0; thread_idx<NR_THREADS; thread_idx++) {
        bst = new BSTree(BST_OBJECT_NAME);
        bst->Open();
        times[0] = mysecond();
        for(long long int i=0; i<=thread_idx; i++)
            threads[i] = new std::thread(search, bst, values+i*NR_OPERATIONS/(thread_idx+1),
                thread_idx+1);
        for(int i=0; i<=thread_idx; i++)
            threads[i]->join();
        times[1] = mysecond();
        print_result(thread_idx+1, bst);
        delete bst;
    }
    // test _delete
    std::cout << "test _delete: " << std::endl;
    for(int thread_idx=0; thread_idx<NR_THREADS; thread_idx++) {
        bst = new BSTree(BST_OBJECT_NAME);
        bst->Open();
        times[0] = mysecond();
        for(long long int i=0; i<=thread_idx; i++)
            threads[i] = new std::thread(_delete, bst, values+i*NR_OPERATIONS/(thread_idx+1),
               thread_idx+1);
        for(int i=0; i<=thread_idx; i++)
            threads[i]->join();
        times[1] = mysecond();
        print_result(thread_idx+1, bst);
        delete bst;
    }
    return 0;
}
