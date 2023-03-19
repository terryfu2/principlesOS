
#include "detectPrimes.h"
#include <pthread.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <atomic>


using namespace std;
pthread_barrier_t barrier;
std::vector<int64_t> nums;
std::vector<int64_t> output;
int64_t n_threads;
int64_t lower;
int64_t upper;
int64_t partition;
int n_index;
int64_t n;
bool finished;
atomic<bool> thread_atomic;
atomic<bool> prime_atomic;


//function given in start code
static bool is_prime_function(int64_t n)
{
    // handle trivial cases
    if (n < 2) return false;
    if (n <= 3) return true; // 2 and 3 are primes
    if (n % 2 == 0) return false; // handle multiples of 2
    if (n % 3 == 0) return false; // handle multiples of 3
    // try to divide n by every number 5 .. sqrt(n)
    int64_t i = 5;
    int64_t max = sqrt(n);
    while (i <= max) {
        if (n % i == 0) return false;
        if (n % (i + 2) == 0) return false;
        i += 6;
    }
    // didn't find any divisors, so it must be a prime
    return true;
}

void* is_prime_thread(void* thread_id) {

    while (1) {

        if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
            if (n_index >= (int)nums.size()) {
                finished = true;
            } else {
                thread_atomic = true;
                n = nums[n_index];
                prime_atomic.store(true, memory_order_release);

                if (thread_atomic) {
                    lower = 5;
                    upper = sqrt(n);
                    partition = ceil(double(upper - lower) / n_threads);

                    if (partition > 6) {
                        
                        partition = partition - (partition % 6) + 6;
                    } 
                    
                    if (partition <= 6) {
                        if (thread_atomic) {
                            thread_atomic = false;
                            prime_atomic.store(is_prime_function(n), memory_order_release);
                        }
                    }
                }
                n_index++;
            }
        }

        pthread_barrier_wait(&barrier);
        if (finished) {
            break;
        } 
        
        if (thread_atomic) {

            int64_t start = lower + partition * long(thread_id);
            int64_t end = start + partition;

            for (int64_t t = start; t <= end; t += 6) {
                if (!prime_atomic.load()) {
                    break;
                }
                if (n % t == 0 || n % (t + 2) == 0) {
                    prime_atomic.store(false, memory_order_release);
                    break;
                }
            }
        }

        if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
            bool prime_atomic = ::prime_atomic.load();
            if (prime_atomic) {
                output.push_back(n);
            }

        }
    }
    pthread_exit(0);
}


std::vector<int64_t>
detect_primes(const std::vector<int64_t>& nums, int n_threads)
{
    
    ::nums = nums;
    ::n_threads = n_threads;
    finished = false;
    n_index = 0;

    pthread_barrier_init(&barrier, NULL, n_threads);
    pthread_t threads[n_threads];

    for (int i = 0; i < n_threads; i++) {
        pthread_create(&threads[i], NULL, is_prime_thread, (void*)(long)i);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return output;

}

