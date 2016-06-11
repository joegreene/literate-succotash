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
#include <stdio.h>
#include <pthread.h>
#include <string.h>

//Constant for number of threads to use (usually this value works best with the number of cores you have)
const int NUM_THREADS    = 2;
// to calculate the number of nanoseconds the program ran for (27 is from 2.7 GHz, the clock rate on my cpu)
const int CPU_CLOCK_RATE = 27;

// Assembly functions
extern unsigned long gettime();
extern double computehsum(unsigned long start, unsigned long end, double* last_term);

// for pthread_create work
struct Args
{
  unsigned long start;
  unsigned long end;
  double sum;
  double last_term;
};

void threaded_compute(unsigned long num_terms, double* total_sum, double* last_term);
void remove_overadd(unsigned long num_terms, double* total_sum, double* last_term); 
void* filter_function(void* args_to_pass);

int main()
{
  //Program variables
  unsigned long  start_time,                 // Clock time before harmonic sum calculation
                 end_time,                   // Clock time after harmonic sum calculation
                 num_terms,                  // User-inputted number of terms to compute
                 elapsed_tics,               // elapsed time of start/end
                 nanoseconds,                // used in time calculations
                 sum_in_hex;                 // for representing the sum (double) as a hexadecimal value
  double         total_sum = 0.0;            // The harmonic sum itself
  double         last_term = 0.0;            // The last term computed in the harmonic sum (get it from the assembly function)
  
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
  }
  else //run threads (NOTE: Below assumes divisible by 4)
  {
    threaded_compute(num_terms, &total_sum, &last_term);
  }

  // account for inputs not divisible by 4
  if(num_terms % 4 != 0) {
    remove_overadd(num_terms, &total_sum, &last_term);
  }

  end_time = gettime();
  
  elapsed_tics = end_time - start_time;
  nanoseconds = (unsigned long)(elapsed_tics*10/CPU_CLOCK_RATE);
  
  // weird conversion required to output double as IEEE 754 64-bit hexadecimal format
  // steps: cast the total_sum address as an unsigned long*, dereference that (to prevent any implicit casts from double to long)
  sum_in_hex = *((unsigned long*)(&total_sum));
  
  printf("The sum has been computed and returned from the assembly file.\n");
  printf("The clock time after computing the harmonic sum was %ld \n",     end_time);
  printf("The clock time before computing the sum was         %ld \n",     start_time);
  printf("The harmonic computation required %ld clock cycles (tics) ",     elapsed_tics); 
  printf("which is %ld nanoseconds on this machine.\n",                    nanoseconds);
  printf("The harmonic sum of %ld terms is %1.18lf, which is 0x%016lX.\n", num_terms, total_sum, sum_in_hex);
  
  printf("\nH-SUM: %1.18lf\n%s %1.18lf\n", total_sum,    "Last term:", last_term);
  printf("Time taken: %ld %s",             elapsed_tics, "tics\n");

  double num_seconds = nanoseconds/1000000000.0;
  
  printf("Tics to seconds: %1.18lf seconds\n", num_seconds);

  return 0;
}

void threaded_compute(unsigned long num_terms, double* total_sum, double* last_term) {
  pthread_t     thread_list[NUM_THREADS];             
  unsigned int  i;                                    // your everyday run-of-the-mill loop control variable
  unsigned long term_length = num_terms/NUM_THREADS;  // how big of a chunk each thread should perform
  struct Args args_to_pass[NUM_THREADS];

  // run each pthread
  for(i = 0; i < NUM_THREADS; ++i)
  {
    // Create specialized arguments for function to call; 0 2 
    args_to_pass[i].start = (i*term_length)+1;
    args_to_pass[i].end   = (i+1)*term_length;

    pthread_create( &thread_list[i],            // which thread to use
                    NULL,                       // attribute for thread (?)
                    filter_function,            // function (needed because pthreads are stupid and only take void*)
                    (void*)(&args_to_pass[i])); // pointer to struct of arguments
                                     
  }

  // Accumulate sum from each thread
  for(i = 0; i < NUM_THREADS; ++i)
  {
    pthread_join(thread_list[i], NULL);
    (*total_sum) += args_to_pass[i].sum;
  }

  // only need the last one (as that's the actual last term)
  (*last_term) = args_to_pass[NUM_THREADS-1].last_term;
}

void remove_overadd(unsigned long num_terms, double* total_sum, double* last_term) 
{
  // The below variable helps us find how many terms the assembly code actually computed (which is 
  // always the next highest multiple of 4, as compared to the input)
  //
  // Example: num_terms = 6, assembly code computes 8
  //          num_terms_computed = ((6 + 4 - 1)/4) * 4 --> (9/4)*4 --> 2 * 4 --> 8
  unsigned long num_terms_computed = ((num_terms + 3)/4)*4;

  for(; num_terms_computed > num_terms; --num_terms_computed)
  {
    (*total_sum) = (*total_sum) - (1.0/num_terms_computed);
  }

  (*last_term) = 1/num_terms;
}

void* filter_function(void* args_to_pass)
{
  struct Args* atp = (struct Args*)args_to_pass;

  // used so threading can work
  atp->sum = computehsum(atp->start, atp->end, &(atp->last_term));

  return NULL;
}

//=======1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1=========2=========3=========4=========5=========6=========7**

