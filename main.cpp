/*
 * @Author: Nicholas hays 
 * @Date: 2018-01-26 12:27:04 
 * @Last Modified by: Nicholas hays 
 * @Last Modified time: 2018-01-26 12:27:04 
 */

#include <iostream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

/*

To determine the xor sum over a given range (potentially 10^15) in a sequence of numbers
requires fast indexing via bit pattern recognition, leverage of certain properties of the xor operation, 
and 'caching'. 

Fast indexing via bit pattern recognition

 The problem statement requires us to generate a sequence using the xor sum over natural numbers. 
 So, for instance, if we look at numbers 0,1,2,3,4 and 5..the xor sum over these numbers is as follows.

 0 ^ 1 = 1 ^ 2 = 3 ^ 3 = 0 ^ 4 = 7 ^ 5 = 2 ...
 
 The xor sum over [0-24] is shown in B below.

 A= indices, B=input sequence (xOr sum over natural numbers)

 A: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 ...
 B: 0, 1, 3, 0, 4, 1, 7, 0, 8, 1,11, 0,12, 1,15, 0,16, 1,19, 0,20, 1,23, 0,24 ...
 
 We can see some patterns beginning to emerge. 
 For instance, a repeating '0' at index 3,7,11,15,19... and
 a repeating '1' at 1,5,9,13,17...

 Not all numbers in the sequence repeat, but all numbers have a relation to their index.
 For instance, notice at indices 2,6,10,14,18... values in the sequence are 3,7,11,15,19.
 All +1 over the index.
 
 In total there are 4 patterns([0-3]) of numbers that can be segmented from the xor sum over natural numbers.
 
 #		pattern		          output

 0   [ 3, 7,11,15] | 0
 1   [ 0, 4, 8,12] | same value as sample input
 2   [ 2, 6,10,14] | add 1 to sample input
 3   [ 1, 5, 9,13] | 1

 Notice that this pattern holds true for any number in the sequence, but only the number's 4 least significant bits
 are necessary to determine its pattern number. Once we have the pattern number we can easily retrieve the output.

 Great, now we can determine a value in the sequence of a given index in constant time by masking its 4 least significant
 bits, and accessing the output based on its pattern number.

 Therefore, if given a starting range index, we can easily determine the starting value in the sequence; however, we
 still need to calculate the  xor sum over a range, and accessing the value in the sequence at the end range index 
 gives us the xor sum starting from 0, but the start range index may be a number different from 0. 

 So how do we get the value of the xor sum over a range?
 Well, we can start by looking at the properties of the xor operation itself.

 For instance, notice this property, a ^ b ^ c ^ d = a ^ e where e is the xor sum over b,c, and d.

 starting value: 5

 set: {2 6 4 8 3 1 9}

 5 ^ 2 ^ 6 ^ 4 ^ 3 ^ 1 ^ 9  = 12
 (2 ^ 6 ^ 4 ^ 3 ^ 1 ^ 9) = 11
 5 ^ (11) = 12

 Interesting, so we only need to xor the starting range index with a single number, x,
 to get our answer. But how do we get x?

 We can think about how the xor sum works.

 When we xor a sequence of numbers together, the output of one calculation feeds as input to the next calculation in the 
 sequence. 

 For instance, 

 a ^ b ^ c ^ d = e
 a ^ b = z <------- z is a temporary value 
 z ^ c = q <------- z is fed to the next calculation
 q ^ d = e <------- q is fed to the next calculation

 This is interesting because it gives us access to the xor sum over a sequence of numbers for each index starting from 0.
 In fact, the sequence generated by xor sum over the natural numbers is precisely that, a record of the xor sum 
 at each index over the natural numbers starting from 0.
 Except, instead of recording the xor sum over natural numbers again we will record the xor sum of the xor sum over the natural 
 numbers. Hence, we need to analyze a new sequence to see if there are any patterns that will allow for fast access. Spoiler 
 alert, there is.

 The new sequence along with the patterns are shown below in C.

 A = natural numbers, B = xor sum over natural numbers, C = xOr sum over B

 A: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 ...
 B: 0, 1, 3, 0, 4, 1, 7, 0, 8, 1,11, 0,12, 1,15, 0,16, 1,19, 0,20, 1,23, 0,24 ...
 C: 0, 1, 2, 2, 6, 7, 0, 0, 8, 9, 2, 2,14,15, 0, 0,16,17, 2, 2,22,23, 0, 0,24 ...
    +  +  ^  ^  *  *  -  -  +  +  ^  ^  *  *  -  -  +  +  ^  ^  *  *  -  -  +
 1  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__| ...
 3        |__|__|__|__|__|________|__|__|__|__|__|________|__|__|__|__|__|__  ...
 2              |__|__|__|______________|__|__|__|______________|__|__|__|__  ...
 0                    |__|____________________|__|____________________|__|__  ...

 #	 pattern				output

 0	[ 6, 7,14,15] | 0
 1	[ 0, 1, 8, 9] | sample input value
 2	[ 4, 5,12,13] | add 2 to sample input value
 3	[ 2, 3,10,11] | 2

 Great. So now we have quick access to the xor sum value over [0-index] for each index in the xor sum over natural numbers sequence
 using the bit patterns shown above.
 
 So how do we compute the xor sum over a range with a starting index other than 0?

 Well, lets try and formalize the problem a bit.

 Using our diagram of the sequences above where A = natural numbers, B = xor sum over natural numbers,
 and C = xOr sum over B.

 lets also remind ourselves of the problem: compute the xor sum, s, over a range [a,c] in the xor sum over natural numbers sequence.
 
 lets use some variables to represent known values we can access in constant time:
 
 a = value at the start range index in B
 b = value at the start range index in C
 c = value at the end range index in C

 So, imagine, x, is the xor sum over the range [a + 1, c].

 then b ^ x = c

 and a ^ x = s;

 so, to solve for x,

 c ^ b = x.

 now we have both a and x, hence, a ^ x = s.

 example:
 
 C = 0 1 2 2 6 7 0 0 8 9
 B = 0 1 3 0 4 1 7 0 8 1
 A = 0 1 2 3 4 5 6 7 8 9 
 
 range = [3,9]

 a = 0
 b = 2
 c = 9
 x = ?
 s = ?

 2 ^ x = 9, 
 0 ^ x = s,

 x = 2 ^ 9 = 11
 0 ^ 11 = 11.

 answer: 11.

 Let's verify.

 0 ^ 4 = 4 ^ 1 = 5 ^ 7 = 2 ^ 0 = 2 ^ 8 = 10 ^ 1 = 11

 A few more tests reveal this is indeed the correct solution.

  */

/*
Computes the value of the xor sum over the xor sum over natural numbers sequence starting at 0 to
in_index.

@map - a reference to map of type, t, of bit patterns to values.
@in_index - index from sample input.
*/
long comp_xor_val(unordered_map<char, char> & map, long in_index) {
	const char bit_mask = 15, num_patterns = 4;

	long num = in_index & bit_mask; 
	num = map[num] / num_patterns;
	
	switch (num) {
	case 0:
		num = 0;
		break;
	case 1:
		num = in_index;
		break;
	case 2:
		num = ++in_index;
		if (map['t'] == 'c') 
			++num;
		break;
	case 3:
		num = 1;
		if (map['t'] == 'c')
			++num;
		break;
	}

	return num;
}
/*
Computes the xor sum in the range [start_index, end_index] over the xor sum over natural numbers sequence.

@ xsonn a reference to a map of indices to the xor sum over the natural numbers sequence.
@ xsoxsonn a reference to a map of indices to the xor sum over the xor sum over natural numbers sequence.
*/
long int compute_xor_sum(unordered_map<char, char> & xsonn, unordered_map<char, char> & xsoxsonn, long start_index, long end_index) {

	long seq_start_num = comp_xor_val(xsonn, start_index);
	long sum_start_num = comp_xor_val(xsoxsonn, start_index);
	long sum_end_num = comp_xor_val(xsoxsonn, end_index);

	long sum = seq_start_num ^ sum_start_num;
	sum ^= sum_end_num;

	return sum;
}


/*
Pattern data is transfered from buffers to maps to speed access time.

@ xsonn a reference to a map of indices to the xor sum over the natural numbers sequence.
@ xsoxsonn a reference to a map of indices to the xor sum over the xor sum over natural numbers sequence.
*/
void init_map(unordered_map<char, char> & xsonn, unordered_map<char, char> & xsoxsonn) {
	const char SIZE = 16;
	char sum_data_buff[SIZE] = { 6,7,14,15,0,1,8,9,4,5,12,13,2,3,10,11 };
	char seq_data_buff[SIZE] = { 3,7,11,15,0,4,8,12,2,6,10,14,1,5,9,13 };
	for (char i = 0; i < SIZE; i++) {
		xsonn[seq_data_buff[i]] = i;
		xsoxsonn[sum_data_buff[i]] = i;
	}
	xsonn['t'] = 'n'; 
	xsoxsonn['t'] = 'c'; 
}

/*
 Entry point for program. 
*/
int main() {

	int n = 0;
	cin >> n;
	long int start = 0, end = 0;
	//xsonn = xor_sum_over_natural_numbers, xsoxsonn = xor_sum_over_xor_sum_over_natural_numbers
	unordered_map<char, char> xsonn, xsoxsonn; 
	init_map(xsonn, xsoxsonn);

	for (int i = 0; i < n; ++i) {
		cin >> start;
		cin >> end;
		cout << compute_xor_sum(xsonn, xsoxsonn, start, end) << endl;
	}
	return 0;
}