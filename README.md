# xor-sequence
Bit manipulation algorithm to compute the xor sum segment over the natural numbers in a given range.

## Problem Statement

ref: https://www.hackerrank.com/challenges/xor-se/problem

To determine the xor sum over a given range [0, quadrillion] in a sequence of numbers
requires fast indexing via bit pattern recognition, leverage of certain properties of the xor operation, 
and caching xor sums over a sequence.</br>

The problem statement requires us to generate a sequence using the xor sum over natural numbers. 
So, for instance, if we look at numbers 0,1,2,3,4 and 5..the xor sum over these numbers is as follows.

0 ^ 1 = 1 ^ 2 = 3 ^ 3 = 0 ^ 4 = 4 ^ 5 = 1 ...

The xor sum over [0-24] is shown in B below.

A= indices, B=input sequence (xOr sum over natural numbers)

|B| 0| 1| 3| 0| 4| 1| 7| 0| 8| 1|11| 0|12| 1|15| 0|16| 1|19| 0|20| 1|23| 0|24| 
|-|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--| 
|A| 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24| 



We can see some patterns beginning to emerge. 
For instance, a repeating '0' at index 3,7,11,15,19... and
a repeating '1' at 1,5,9,13,17...

Not all numbers in the sequence repeat, but all numbers have a relation to their index.
For instance, notice at indices 2,6,10,14,18... values in the sequence are 3,7,11,15,19.
All +1 over the index.

In total there are 4 patterns ([0-3]) of numbers that can be segmented from the xor sum over natural numbers.

|\#	 |	  pattern		 |        output             |
|----|---------------|---------------------------|
| 0  | [ 3, 7,11,15] | 0                         |
| 1  | [ 0, 4, 8,12] | index                     |
| 2  | [ 2, 6,10,14] | index + 1                 | 
| 3  | [ 1, 5, 9,13] | 1                         |

Notice that this pattern holds true for any number in the sequence, but only the number's 4 least significant bits
are necessary to determine its pattern number. Once we have the pattern number we can easily retrieve the output.

Great, now we can determine a value in the sequence of a given index in constant time by masking its 4 least significant
bits, and accessing the output based on its pattern number.

Therefore, if given a starting range index, we can easily determine the starting value in the sequence; however, we
still need to calculate the  xor sum over a range, and accessing the value in the xor sum over natural numbers
sequence at the end range index gives us the xor sum starting from 0, but the start range index may be a number different from 0. 

So how do we get the value of the xor sum over a range?
Clearly, with a worse case range of quadrillion we cannot xor each number in the range.
So, we can start by looking at the properties of the xor operation itself.

For instance, notice this property, a ^ b ^ c ^ d = a ^ e where e is the xor sum over b,c, and d.

starting value: 5</br>

set: {2 6 4 3 1 9}</br>

5 ^ 2 ^ 6 ^ 4 ^ 3 ^ 1 ^ 9  = 12</br>
(2 ^ 6 ^ 4 ^ 3 ^ 1 ^ 9) = 11</br>
5 ^ (11) = 12</br>

Interesting, so we only need to xor the starting range index with a single number, x,
to get our answer. But how do we get x?

We can again think about how the xor sum works.

When we xor a sequence of numbers together, the output of one calculation feeds as input to the next calculation in the 
sequence. 

For instance, 

a ^ b ^ c ^ d = e</br>
a ^ b = z <------- z is a temp value fed to the next calculation </br>
z ^ c = q <------- q is a temp value fed to the next calculation</br>
q ^ d = e

z and q are temp values representing the xor sum over the sequence starting from 0 to their index.
In fact, the sequence generated by the xor sum over the natural numbers is precisely that -- a record of the xor sum 
at each index over the natural numbers starting from 0.
 
Ok, so if we generate another xor sum sequence using the xor sum over natural numbers sequence as input then each of 
the values in the sequence will be the xor sum starting from 0 of the xor sum over natural numbers sequence.

Hence, we need to analyze a new sequence to see if there are any patterns that will allow for fast access. Spoiler 
alert, there is.

The new sequence along with the patterns are shown below in C.

A = natural numbers, B = xor sum over natural numbers, C = xOr sum over B

|C| 0| 1| 2| 2| 6| 7| 0| 0| 8| 9| 2| 2|14|15| 0| 0|16|17| 2| 2|22|23| 0| 0|24| 
|-|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--| 
|B| 0| 1| 3| 0| 4| 1| 7| 0| 8| 1|11| 0|12| 1|15| 0|16| 1|19| 0|20| 1|23| 0|24|
|A| 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24| 
| | +| +| ^| ^| *| *| -| -| +| +| ^| ^| *| *| -| -| +| +| ^| ^| *| *| -| -| +| 


|S|\#	 |	  pattern		 |        output             |
|-|----|---------------|---------------------------|
|-| 0  | [ 6, 7,14,15] | 0                         |
|+| 1  | [ 0, 1, 8, 9] | index                     |
|*| 2  | [ 4, 5,12,13] | index + 2                 | 
|^| 3  | [ 2, 3,10,11] | 2                         |

Great. So now we have quick access to the xor sum value over [0-index] for each index in the xor sum over natural numbers sequence
using the bit patterns shown above.

So how do we compute the xor sum over a range with a starting index other than 0?

Well, lets try and formalize the problem a bit.

Using our diagram of the sequences above where A = natural numbers, B = xor sum over natural numbers,
and C = xOr sum over B.

lets also remind ourselves of the problem: compute the xor sum, s, over a range [a,c] in the xor sum over natural numbers sequence.

lets use some variables to represent known values we can access in constant time:

a = value at the start range index in B</br>
b = value at the start range index in C</br>
c = value at the end range index in C</br>

So, imagine, x, is the xor sum over the range [a + 1, c].

then b ^ x = c

and a ^ x = s;

so, to solve for x,

c ^ b = x.

now we have both a and x, hence, a ^ x = s.

example:

C = 0 1 2 2 6 7 0 0 8 9</br>
B = 0 1 3 0 4 1 7 0 8 1</br>
A = 0 1 2 3 4 5 6 7 8 9</br>

range = [3,9]</br>

a = 0</br>
b = 2</br>
c = 9</br>
x = ?</br>
s = ?</br>

2 ^ x = 9,</br> 
0 ^ x = s,</br>

x = 2 ^ 9 = 11</br>
0 ^ 11 = 11.</br>

answer: 11.</br>

Let's verify.</br>

0 ^ 4 = 4 ^ 1 = 5 ^ 7 = 2 ^ 0 = 2 ^ 8 = 10 ^ 1 = 11</br>

A few more tests reveal this is indeed the correct solution.</br>
