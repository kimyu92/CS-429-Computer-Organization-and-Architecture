/* 
 * CS:APP Data Lab 
 * 
 * <Name: Kim Yu Ng   accID: kimyu92    UTEID: kn6254>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  //Ops: 3
  //Shift n * 8 bits and trunscate to 1 byte
  int shifted = (x >> (n << 3));
  return shifted & 0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  //Ops: 6
  //In order to overcome the arithmetic shift
  //Intersect so that it pads with 0s of shift amount
  int mask = ~( ((1 << 31) >> n ) << 1 );
  return (x >> n) & mask;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  //Ops: 36
  //01010101010101010101010101010101
  int prepFirst = 0x55 << 8 | 0x55;
  int first =  prepFirst << 16 | prepFirst;
  
  //00110011001100110011001100110011
  int prepSecond = 0x33 << 8 | 0x33;
  int second = prepSecond << 16 | prepSecond;
  
  //00001111000011110000111100001111
  int prepThird = 0x0f << 8 | 0x0f;
  int third =  prepThird << 16 | prepThird;

  //00000000111111110000000011111111
  int fourth = 0xff << 16 | 0xff;
  
  //00000000000000001111111111111111
  int fifth = fourth >> 8 | 0xff;

  int sum = 0;

  sum = (x & first) + ((x >> 1) & first);
  
  sum = (sum & second) + ((sum >> 2) & second);
  sum = (sum & third) + ((sum >> 4) & third);
  sum = (sum & fourth) + ((sum >> 8) & fourth);
  sum = (sum & fifth) + ((sum >> 16) & fifth);

  return sum;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  int aBit = x >> 31;

  int complementABit = (~x + 1) >> 31;

  return ~(aBit | complementABit ) & 0x1; 
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int aBit = x >> 31;
  int temp = (~x & aBit) + (x & ~aBit);
  
  n = n + ~0;
  return !( temp >> n );
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  //Ops: 7
  //return 0 or 1
  int rounding = ( (x >> 31) &  ( ~0 + (1 << n)) );

  return ( x + rounding ) >> n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (x ^ ~0 ) + 1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  int temp = (x & (1 << 31));
  
  return !( temp | !x ) ;
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2 
 */
int leastBitPos(int x) {
  return x & (~x + 1);
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x){
  int pos = 0;
  int neg = 0;
  int result = 0;

  //mask
  int aBit = x & 1;
  int mask = x >> 31;
  int sign = mask & 1;
  int offsetMask = sign & aBit;
  int offsetMaskLast = (x >> 2);

  //division
  int first = (x + ( mask + (mask & 0x8) ) ) >> 3;
  int sec = (x + ( mask + (mask & 0x2 ) ) ) >> 1;

  //offset
  pos = ~sign & aBit & offsetMaskLast;
  neg = ~( offsetMask & offsetMaskLast) + 1;
  neg = offsetMask + neg;
  neg = (~neg) + 1;
  
  result = (first + sec) + neg + pos;
  
  return result;
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0, 
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
  int checkX = (x >> 31) & 0x1;
  int checkY = (y >> 31) & 0x1;
  int sum = ((x + y) >> 31 & 0x1);

  return !( ( ~( checkX ^ checkY) ) & (checkX ^ sum));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {

  int result = 0;
  int temp = 0;

  result = !!(x >> 16);
  result = result << 4;

  temp = x >> (result + 8);
  result += (!!temp) << 3;

  temp = x >> (result + 4);
  result += (!!temp) << 2;

  temp = x >> (result + 2);
  result += (!!temp) << 1;

  temp = x >> (result + 1);
  result += !!temp;
   
  result += !!result;
  result += !(1 ^ x);
  result += (1 << 31 >> 31);
  
  return result ;  
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  unsigned copiedofX = x;

  unsigned sigo = 0;
  unsigned expo;
  unsigned fraco; 
  unsigned remaining;

  unsigned countE = 0;
  unsigned mask = 1 << 31;
   
  unsigned temp;  
  int stillZero = 1;

  //If x is negative, take the absoulte value    
  //Return zero if x is zero
  if (x < 0){
    sigo = mask;
    copiedofX = -x;  
  }
  else if (x == 0)
    return 0;

  //Take the copied of X
  //Count how many left shift (E) is required
  while (stillZero){
    temp = copiedofX;  
    
    copiedofX = copiedofX << 1;
    countE++;

    //Check whether the most left is 1
    if (temp & mask)
      stillZero = 0;
  }

  if ( ((copiedofX & 1023) == 768) || (copiedofX & 511 ) > 256)
    remaining = 1;
  else
    remaining = 0;

  //Get exponent and fraction
  expo = copiedofX >> 9;
  fraco = (159 - countE) << 23;
  
  return sigo + expo + fraco + remaining;  
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  unsigned result = uf;
  unsigned mask = 1 << 31;
  unsigned mask4expo = (1 << 31) >> 8 & ~(1 << 31);

  // a bit is shiftted to the left or expo is increased
  if ((result & mask4expo) == 0)
    result = (result & mask) | ((result & ~(1 << 31 >> 8) ) << 1);
  else if ( (mask4expo & result) != mask4expo)
    result = (1 << 23) + result;

  return result;
}
