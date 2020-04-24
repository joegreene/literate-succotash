// =================================================================================================
// Author information
//   Author name: Joseph Greene
// Project information
//   Project title: Harmonic Series Computer
//   Purpose: To compute the harmonic sum of an integer divisible by four. Mainly, this program is a means
//            to test processor performance and optimize speed of the program.
//   Status: Complete
//   Project files: cdriver.cpp, computehsum.asm, time.asm, macros.inc
// Module information
//   This module's call name: harmonicsum.out (This module is invoked by the user.)
//   Language: C++
//   Purpose: Runs assembly program and splits into multiple threads if necessary.
//   File name: assign6-cdriver.cpp
//   Status: Finished.  No known errors.
//   Future enhancements: Add functionality to allow number of terms not divisible by 4
// Translator information
//   Gnu compiler: g++ -c -m64 -Wall -l c_driver.lis -o c_driver.o assign6-cdriver.cpp -std=c++11 -pthread
//   Gnu linker:   g++ -m64 -o harmonicsum.out c_driver.o computehsum.o asm_time.o -pthread -std=c++11
// =================================================================================================
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <future>
#include <string>

// TODO(joegreene) Get CPU info programmatically, or via sys.argv
const uint64_t THREAD_COUNT = 6;    // # physical CPU cores
const uint64_t CPU_CLOCK_RATE = 36;  // GHz per core
const uint64_t MULTITHREAD_THRESHOLD = 1000000000;  // BREAKS IF THRESHOLD <= 4

// Assembly functions
extern "C" uint64_t gettime();
// TODO(joegreene) change so return is param double*
extern "C" double computehsum(uint64_t start, uint64_t end, double* final_term);

void threaded_compute(uint64_t term_count, double* harmonic_sum, double* final_term);
void remove_overadd(uint64_t term_count, double* harmonic_sum, double* final_term);

int main() {
    using std::cout;
    using std::cin;

    uint64_t start_time, end_time, term_count, nanoseconds,
             seconds, elapsed_tics, harmonic_sum_hex;
    double harmonic_sum = 0.0;
    double final_term = 0.0;

    cout << "\n# of terms to compute in the harmonic sum: ";
    cin >> term_count;

    // TODO(joegreene): Do input validation here

    cout << "Computing sum "
         << "using " << THREAD_COUNT << " threads "
         << "at " << CPU_CLOCK_RATE/10.0 << " GHz." << std::endl;

    start_time = gettime();

    // Only multithread when the threshold is met
    if (term_count < MULTITHREAD_THRESHOLD) {
      harmonic_sum = computehsum(1, term_count, &final_term);
    } else {
      threaded_compute(term_count, &harmonic_sum, &final_term);
    }

    // Account for inputs not divisible by 4 (since the code works with ymm
    // registers, i.e. floating point operations done in fours)
    if (term_count % 4 != 0) {
      remove_overadd(term_count, &harmonic_sum, &final_term);
    }

    end_time = gettime();

    elapsed_tics = end_time - start_time;
    nanoseconds = (uint64_t)(elapsed_tics*10/CPU_CLOCK_RATE);

    // reinterpret_cast is required to output double as a hexadecimal value
    // (which is in the IEEE 754 format)
    // Steps:
    // 1. Cast the harmonic_sum address as an uint64_t*
    // 2. Dereference the pointer (to prevent any implicit casts from double to
    //    uint64_t)
    harmonic_sum_hex = *(reinterpret_cast<uint64_t*>(&harmonic_sum));
    seconds = nanoseconds/1000000000.0;

    cout << "Stats:"
         << "\n- Start time: " << start_time
         << "\n- End time: " << end_time
         << "\n- Elapsed clock cycles (tics): " << elapsed_tics
         << "\n- Elapsed nanoseconds: " << nanoseconds
         << "\n- Elapsed seconds: " << seconds
         << "\n- H(" << term_count << ") = " << harmonic_sum;

    // set flags for hex output and decimal space printing
    cout << std::hex << std::uppercase << std::fixed << std::setprecision(18);

    cout << "\n\nResults:"
         << "\n- Sum: " << harmonic_sum
         << "\n- Sum in hex: 0x" << harmonic_sum_hex
         << "\n- Final term: " << final_term
         << std::endl;

    return 0;
}

void threaded_compute(uint64_t term_count, double* harmonic_sum, double* final_term) {
    using std::async;
    using std::future;

    future<double> thread_list[THREAD_COUNT];
    double final_terms[THREAD_COUNT];  // Last terms computed (per thread)
    uint64_t term_length = term_count/THREAD_COUNT;
    uint64_t thread_start;
    uint64_t thread_end;

    // Spawn each thread
    uint64_t i;
    for (i = 0; i < THREAD_COUNT; ++i) {
        thread_start = (i*term_length)+1;
        thread_end = (i+1)*term_length;

        thread_list[i] = async(computehsum, thread_start, thread_end, &final_terms[i]);
    }

    // Accumulate sum from each thread
    for (i = 0; i < THREAD_COUNT; ++i) {
        *harmonic_sum += thread_list[i].get();
    }

    // Only yield the final term (as that's the actual last term)
    *final_term = final_terms[THREAD_COUNT-1];
}

void remove_overadd(uint64_t term_count, double* harmonic_sum, double* final_term) {
    // term_count_computed helps find how many terms the assembly code
    // actually computed (which is always the next highest multiple of 4)
    //
    // Example: term_count = 6, assembly code computes 8
    //          term_count_computed = 6 + (4 - 6 % 4)
    //                              = 6 + 2
    //                              = 8
    uint64_t term_count_computed = term_count + (4 - term_count % 4);

    // Remove each over-added term
    for (; term_count_computed > term_count; --term_count_computed) {
        *harmonic_sum = *harmonic_sum - (1.0/term_count_computed);
    }

    // store the real last term
    *final_term = 1.0/term_count;
}