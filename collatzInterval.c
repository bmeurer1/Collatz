/*
MIT License

Copyright (c) 2018, Bruno Meurer.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

	* Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
   	* Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
   	* The name Bruno Meurer may not be used to endorse or promote products derived from this
      software without specific prior written permission.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <gmp.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int num_threads;
int i;
int known = 0;
mpz_t number, numbercpy, numberLimit, *threadNumber, *threadcpy, remains, lowerBound, upperBound;
int allPass = 1, endCalculations = 0;
double maxSteps = 10e10;

pthread_t *threads;

//Calculates Collatz conjecture if previous numbers are known to obey the conjecture
int collatzIntervalKnown(mpz_t number){
	double counter = 0;
	mpz_set(numberLimit, number);

		while(mpz_cmp_ui(number, 1) > 0){
			mpz_mod_ui(remains, number, 2);
			
			//If even, divide by 2
			if(mpz_cmp_ui(remains, 0) == 0){
				mpz_cdiv_q_ui(number, number, 2);

			//If odd, number = (3*number + 1)/2
			}else{
				mpz_mul_ui(number, number, 3);
				mpz_add_ui(number, number, 1);
				mpz_cdiv_q_ui(number, number, 2);
			}

			if(counter++ == maxSteps){
				return 0;
			}

			//If iteration limit reached, end calculations
			if(mpz_cmp(numberLimit, number) > 0){
				return 1;
			}
		}

		return 1;
}

//Calculates Collatz conjecture
int collatzIntervalUnknown(mpz_t number){
	double counter = 0;

		while(mpz_cmp_ui(number, 1) > 0){
			mpz_mod_ui(remains, number, 2);

			//If even, divide by 2
			if(mpz_cmp_ui(remains, 0) == 0){
				mpz_cdiv_q_ui(number, number, 2);

			//If odd, number = (3*number + 1)/2
			}else{
				mpz_mul_ui(number, number, 3);
				mpz_add_ui(number, number, 1);
				mpz_cdiv_q_ui(number, number, 2);
			}

			if(counter++ == maxSteps){
				return 0;
			}
		}

		return 1;
}



void *threadsCollatz(void *threadID){
	int tid;
	tid = (intptr_t) threadID;

	//Assigns first number to each thread
	mpz_set(threadNumber[tid], lowerBound);
	mpz_add_ui(threadNumber[tid], threadNumber[tid], tid);

	//Calulates each number until upperBound
	int passed;
	while(mpz_cmp(threadNumber[tid], upperBound) < 0 && !endCalculations){

		mpz_set(threadcpy[tid], threadNumber[tid]);

		if(known){
			passed = collatzIntervalKnown(threadcpy[tid]);
		}else{
			passed = collatzIntervalUnknown(threadcpy[tid]);
		}

		//If thread exceeds iteration limit, print number and end calculations
		if(!passed){
			gmp_printf("Thread %d: %Zd exceeded iteration limit\n", tid, threadNumber[tid]);
			allPass = 0;
			endCalculations = 1;
		}

		mpz_add_ui(threadNumber[tid], threadNumber[tid], num_threads);

	}

	pthread_exit((void *)(intptr_t)tid);

}

//-----------------------------------------------------------------------------------------------------------

int main(int argc, char const *argv[]){

	if(argc < 3 || argc > 4){
		printf("Please use one of the following formats:\n\n");
		printf("%s <Lower Bound> <Upper Bound>\n", argv[0]);
		printf("%s <Lower Bound> <Upper Bound> <Optional>\n", argv[0]);
		printf("\nOptional Parameter:\n\n");
		printf("\t'-k' or '-known': If all numbers from 1 to lowerBound obey the Collatz conjecture, using this flag cuts down computational time significantly\n");
		return 0;
	}


	//Checks for optional parameter
	if(argc == 4){

		if(strcmp("-k", argv[3]) != 0 && strcmp("-known", argv[3]) != 0){
			printf("Error! Unknown parameter '%s'\n", argv[3]);
			return -1;
		}

		known = 1;
	}

	//Initialize gmp variables
	mpz_init_set_str(lowerBound, argv[1], 10);
	mpz_init_set_str(upperBound, argv[2], 10);

//---------------------------------------------------------------------------
//This section checks to make sure bounds are coherent

	if(mpz_cmp_ui(lowerBound, 1) < 0){
		printf("Lower bound must be greater than zero\n");
		mpz_clear(lowerBound);
		mpz_clear(upperBound);
		return 0;
	}

	if(mpz_cmp(lowerBound, upperBound) > 0 || mpz_cmp(lowerBound, upperBound) == 0){
		printf("Lower bound must be smaller than upper bound\n");
		mpz_clear(lowerBound);
		mpz_clear(upperBound);
		return 0;
	}
//----------------------------------------------------------------------------

	//Initializes the rest of the gmp + pthread variables
	num_threads = get_nprocs();
	threads =  (pthread_t *) malloc(num_threads * sizeof(pthread_t));
	threadcpy =    (mpz_t *) malloc(num_threads * sizeof(mpz_t));
	threadNumber = (mpz_t *) malloc(num_threads * sizeof(mpz_t));

	for(i = 0; i < num_threads; i++){
		mpz_init(threadcpy[i]);
		mpz_init(threadNumber[i]);
	}

	mpz_init(numbercpy);
	mpz_init(remains);
	mpz_init(numberLimit);

	//Launches all threads
	int rc;
	for(i = 0; i < num_threads; i++){

		rc = pthread_create(&threads[i], NULL, threadsCollatz, (void *) (intptr_t)i);

		if(rc){
			printf("Error creating threads. Code %d\n", rc);
			return 0;
		}
	}


	//Waits for threads to finish
	void *status;
	for(i = 0; i < num_threads; i++){
		rc = pthread_join(threads[i], &status);
		if(rc){
			printf("Error joining threads. Code %d\n", rc);
			return 0;
		}
	}


	if(allPass){
		printf("All numbers in the interval obey the collatz conjecture\n");
	}

	//Frees variables
	for(i = 0; i < num_threads; i++){
		mpz_clear(threadcpy[i]);
	}
	mpz_clear(number);
	mpz_clear(numbercpy);
	mpz_clear(numberLimit);
	mpz_clear(remains);
	mpz_clear(lowerBound);
	mpz_clear(upperBound);

	pthread_exit(NULL);

	return 0;
}