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
#include <cstdio>  // might change later to iostream (to stay consistent with the future library usage)
#include <pthread>
#include <cstring>

//Constant for number of threads to use (the last is extra and used for leftover terms)
const int NUM_THREADS    = 8;
// to calculate the number of nanoseconds the program ran for
const int CPU_CLOCK_RATE = 27;

extern "C" long   gettime();
extern "C" double computehsum(long start, long end, double* lastterm);

void threaded_compute(double& total_sum, double& last_term, unsigned long num_terms);

int main()
{
  //Program variables
  unsigned long  start_time,                 // Clock time before harmonic sum calculation
                 end_time,                   // Clock time after harmonic sum calculation
                 num_terms,                  // User-inputted number of terms to compute
                 elapsed_tics,               // elapsed time of start/end
                 nanoseconds,                // 
                 sum_in_hex;                 // for representing the sum (double) as a hexadecimal value
  double         total_sum = 0.0;            // The harmonic sum itself
  double         last_term;                  // The last term computed in the harmonic sum (get it from the assembly function)
  
  // grab, from user input, the number of terms to compute
  printf("Please enter a positive integer for the number of terms to include in the harmonic sum: ");
  scanf("%ld", &num_terms);

  printf("The harmonic sum H(%ld) is being computed.\n", num_terms);
  printf("Please wait . . . .\n");
  
  start_time = gettime();

  // don't bother with multi-threading (pretty quick unless the value exceeds a certain threshold)
  if(num_terms < 1000000000)
  { 
    // run normally as if the num_terms is divisible by 4
    total_sum = computehsum(1, num_terms, &last_term);

    //TODO: Finish below (which is to remove unnecessarily added terms if num_terms % 4 != 0)
    /*
    while(num_terms%4 != 0) {
      total_sum -= 1/num_terms;
      last_term = 1/num_terms;
      num_terms--;
    }
    */
  }
  else //run threads (NOTE: Below assumes divisible by 4)
  {
    threaded_compute(total_sum, last_term, num_terms);
  }

  end_time = gettime();
  
  elapsed_tics = end_time - start_time;
  nanoseconds = (unsigned long)(elapsed_tics*10/CPU_CLOCK_RATE);
  
  // weird conversion required to output double as IEEE 754 64-bit hexadecimal format
  // steps: reinterpret the total_sum as an unsigned long*, dereference that (to prevent any implicit casts from double to long)
  sum_in_hex = *(reinterpret_cast<unsigned long*>(&total_sum));
  
  printf("The sum has been computed and returned from the assembly file.\n");
  printf("The clock time after computing the harmonic sum was %ld \n",     end_time);
  printf("The clock time before computing the sum was         %ld \n",     start_time);
  printf("The harmonic computation required %ld clock cycles (tics) ",     elapsed_tics); 
  printf("which is %ld nanoseconds on this machine.\n",                    nanoseconds);
  printf("The harmonic sum of %ld terms is %1.18lf, which is 0x%016lX.\n", num_terms, total_sum, sum_in_hex);
  
  printf("\nH-SUM: %1.18lf\n%s %1.18lf\n", total_sum,    "Last term:", last_term);
  printf("Time taken: %ld %s",             elapsed_tics, "tics\n");

  double num_seconds = nanoseconds/1000000000.0;
  
  printf("Tics to seconds: %1.18lf\n", num_seconds);

  return 0;
}

void threaded_compute(double& total_sum, double& last_term, unsigned long num_terms) {
  p_thread      thread_list[NUM_THREADS];                   // List of threads (where each thread returns a double value)
  int           i, j;                                       // your everyday run-of-the-mill loop control variables
  unsigned long term_length    = num_terms/NUM_THREADS;     // 
  double        last_term_list[NUM_THREADS];                // 

  for(i = 0; i < NUM_THREADS; ++i)
  {
    //The async code starts a thread with the function and arguments supplied after the function call
    thread_list[i] = async( computehsum, ( i*term_length + 1 ), (i+1)*term_length, &last_term_list[i] );
  }

  //Accumulate sum from each thread
  for(i = 0; i < NUM_THREADS; ++i)
  {
    total_sum+=thread_list[i].get();
  }
  
  // TODO: Fix below leftover work

  // this portion does a leftover count (leftover count range will always be between 1 to 3) if the 
  // number inputted wasn't divisible by 4.
  if(num_terms%4 != 0)
  {
    for(j = (i+1)*term_length; j <= num_terms; ++j)
    {
      total_sum+=(1.0/(double)j);
    }

    last_term = 1.0/(double)num_terms;
  }
  else //normal last_term grab
  {
    last_term = last_term_list[NUM_THREADS-1];
  }
}

//=======1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1=========2=========3=========4=========5=========6=========7**

