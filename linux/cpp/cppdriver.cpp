//=======1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1=========2=========3=========4=========5=========6=========7**
//Author information
//  Author name: Joseph Greene
//Project information
//  Project title: Harmonic Series Computer
//  Purpose: To compute the harmonic sum of an integer divisible by four. Mainly, this program is a means 
//           to test processor performance and optimize speed of the program.
//  Status: Complete
//  Project files: cdriver.cpp, computehsum.asm, time.asm, macros.inc
//Module information
//  This module's call name: harmonicsum.out (This module is invoked by the user.)
//  Language: C++
//  Purpose: Runs assembly program and splits into multiple threads if necessary.
//  File name: assign6-cdriver.cpp
//  Status: Finished.  No known errors.
//  Future enhancements: Add functionality to allow number of terms not divisible by 4
//Translator information
//  Gnu compiler: g++ -c -m64 -Wall -l c_driver.lis -o c_driver.o assign6-cdriver.cpp -std=c++11 -pthread
//  Gnu linker:   g++ -m64 -o harmonicsum.out c_driver.o computehsum.o asm_time.o -pthread -std=c++11
//===== Begin code area ===================================================================================================================================================
#include <iostream>
#include <iomanip>
#include <future>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::async;
using std::future;

//Constant for number of threads to use (usually this value works best with the number of cores you have)
const int NUM_THREADS    = 2;
// to calculate the number of nanoseconds the program ran for (27 is from 2.7 GHz, the clock rate on my cpu)
// NOTE: MODIFY THIS IF YOU REQUIRE ACCURATE TICS RESULTS
const int CPU_CLOCK_RATE = 27;

// multithreading threshold (NOTE: Code breaks if THRESHOLD <= 4)
const unsigned long MULTITHREAD_THRESHOLD = 1000000000;

// Assembly functions
extern "C" unsigned long gettime();
extern "C" double computehsum(unsigned long start, unsigned long end, double* last_term);

void threaded_compute(unsigned long num_terms, double& total_sum, double& last_term);
void remove_overadd(unsigned long num_terms, double& total_sum, double& last_term);

int main()
{
  //Program variables
  unsigned long  start_time,                 // Clock time before harmonic sum calculation
                 end_time,                   // Clock time after harmonic sum calculation
                 num_terms,                  // User-inputted number of terms to compute
                 nanoseconds,
                 num_seconds,
                 elapsed_tics,
                 sum_in_hex;                 // for representing the sum (double) as a hexadecimal value
  double         total_sum = 0.0;            // The harmonic sum itself
  double         last_term = 0.0;            // The last term computed in the harmonic sum (get it from the assembly function)
  
  // grab, from user input, the number of terms to compute
  cout << "Please enter a positive integer for the number of terms to include in the harmonic sum: ";
  cin >> num_terms;

  cout << "The harmonic sum H(" << num_terms << ") is being computed." << endl;
  cout << "Please wait . . . ." << endl;
  
  start_time = gettime();

  // don't bother with multi-threading (pretty quick unless the value exceeds a certain threshold)
  if(num_terms < MULTITHREAD_THRESHOLD)
  { 
    total_sum = computehsum(1, num_terms, &last_term);
  }
  else
  {
    threaded_compute(num_terms, total_sum, last_term);
  }

  // account for inputs not divisible by 4 (since the code works with ymm registers, i.e. floating point operations done in fours)
  if(num_terms % 4 != 0) {
    remove_overadd(num_terms, total_sum, last_term);
  }

  end_time = gettime();
  
  elapsed_tics = end_time - start_time;
  nanoseconds = (unsigned long)(elapsed_tics*10/CPU_CLOCK_RATE);
  
  // reinterpret cast required to output double as a hexadecimal value (which is in the IEEE 754 format)
  // steps: cast the total_sum address as an unsigned long*, then dereference that (to prevent any implicit casts from double to long)
  sum_in_hex = *(reinterpret_cast<unsigned long*>(&total_sum));
  
  // set flags for hex output and decimal space printing
  cout << std::hex << std::uppercase << std::fixed << std::setprecision(18);

  cout << "The sum has been computed and returned from the assembly file."  << endl
       << "The clock time after computing the harmonic sum was " << end_time << endl;
  cout << "The clock time before computing the sum was " << start_time << endl;
  cout << "The harmonic computation required " << elapsed_tics << " clock cycles (tics) "
       << "which is " << nanoseconds << " nanoseconds on this machine." << endl;
  cout << "The harmonic sum of " << num_terms << " terms is " << total_sum << ", which is 0x" << sum_in_hex << endl;
  
  cout << endl << "H-SUM: " << total_sum << endl 
       << "Last term: " << last_term << endl;
  cout << "Time taken: " << elapsed_tics << " tics" << endl;

  num_seconds = nanoseconds/1000000000.0;
  
  cout << "Tics to seconds: " << num_seconds << " seconds" << endl;

  return 0;
}

void threaded_compute(unsigned long num_terms, double& total_sum, double& last_term) {
  future<double> thread_list[NUM_THREADS];             // each thread (which each returns a double)
  double         last_terms[NUM_THREADS];              // last terms parallel to each thread
  unsigned int   i;                                    // your everyday run-of-the-mill loop control variable
  unsigned long  term_length = num_terms/NUM_THREADS;  // how big of a chunk each thread should perform

  // call each thread
  for(i = 0; i < NUM_THREADS; ++i)
  {
    thread_list[i] = async(computehsum, (i*term_length)+1, (i+1)*term_length, &last_terms[i]);
  }

  // accumulate sum from each thread
  for(i = 0; i < NUM_THREADS; ++i)
  {
    total_sum += thread_list[i].get();
  }

  // only need the last one (as that's the actual last term)
  last_term = last_terms[NUM_THREADS-1];
}

void remove_overadd(unsigned long num_terms, double& total_sum, double& last_term) 
{
  // the below variable helps us find how many terms the assembly code actually computed (which is 
  // always the next highest multiple of 4, as compared to the input)
  //
  // example: num_terms = 6, assembly code computes 8
  //          num_terms_computed = ((6 + 4 - 1)/4) * 4 --> (9/4)*4 --> 2 * 4 --> 8
  unsigned long num_terms_computed = ((num_terms + 3)/4)*4;

  // for each over-added term, subtract such term
  for(; num_terms_computed > num_terms; --num_terms_computed)
  {
    total_sum = total_sum - (1.0/num_terms_computed);
  }

  // store the real last term
  last_term = 1/num_terms;
}

//=======1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1=========2=========3=========4=========5=========6=========7**

