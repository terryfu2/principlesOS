
#include <thread>
#include <iostream>
#include <cmath>
#include <vector>
#include "calcpi.h"

std::thread threads[256];
int r;
double rsq;

struct Task {
    int start_x;
    int end_x;
    uint64_t partial_count;
};
Task tasks[256];

void thread_calc(Task& task) {

    uint64_t curr = 0;

    for (double x = task.start_x; x < task.end_x; x++) {
        for (double y = 1; y <= r; y++) {
            if (x * x + y * y <= rsq) {
                curr++;
            }
        }
    }

    task.partial_count = curr;
}

uint64_t count_pixels(int r, int n_threads) {


    if (r < 0 || n_threads < 1) {
        return 0;
    }

    int size_row = std::ceil(double(r) / n_threads);
    uint64_t count = 0;
    rsq = double(r) * r; 

    if (size_row < 1) {
        size_row = 1;
    }

    for (int i = 0; i < n_threads; i++) {
        ::r = r;
        ::rsq = rsq;

        Task& task = tasks[i];
        task.start_x = size_row * i;
        task.end_x = size_row * i + size_row;

        threads[i] = std::thread(thread_calc, std::ref(task));
    }

    for (int i = 0; i < n_threads; i++) {
        threads[i].join();
        count += tasks[i].partial_count;
    }

    return count * 4 + 1;
}


