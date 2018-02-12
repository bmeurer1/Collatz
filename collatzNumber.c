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

int num_threads;
int i;
mpz_t number, numbercpy, remains;
int allPass = 1, endCalculations = 0;
double maxIterations = 10e10;

//Applies the conjecture to the number
int collatz(mpz_t number){
	double counter = 0;
		while(mpz_cmp_ui(number, 1) > 0){

			//If number is even, divide by 2
			mpz_mod_ui(remains, number, 2);
			if(mpz_cmp_ui(remains, 0) == 0){
				mpz_cdiv_q_ui(number, number, 2);

			//If number is odd, number = (3*number + 1)/2
			}else{
				mpz_mul_ui(number, number, 3);
				mpz_add_ui(number, number, 1);
				mpz_cdiv_q_ui(number, number, 2);
			}

			if(counter++ == maxIterations){
				return 0;
			}
		}

		return 1;
}

//-----------------------------------------------------------------------------------------------------------

int main(int argc, char const *argv[]){

	if(argc == 1 || argc > 2){
		printf("Please use the following format:\n\n");
		printf("%s <Number to be checked>\n", argv[0]);
		return 0;
	}

	//Initialize gmp variables
	mpz_init_set_str(number, argv[1], 10);
	mpz_init_set_str(numbercpy, argv[1], 10);
	mpz_init(remains);

	//Checks if number is greater than zero
	if(mpz_cmp_ui(number,1) < 0){
		printf("Number must be greater than zero\n");
		mpz_clear(number);
		mpz_clear(numbercpy);
		return 0;
	}

	//Calculates and prints result
	int passed = collatz(number);

	if(passed){
		gmp_printf("%Zd obeys the collatz conjecture\n", numbercpy);
	}else{
		gmp_printf("%Zd exceeded iteration limit\n", numbercpy);
	}


	mpz_clear(number);
	mpz_clear(numbercpy);
	mpz_clear(remains);

	return 0;
}