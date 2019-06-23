
/**
Copyright (c) 2016 Jason Ertel, Codesim LLC

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

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

#include "p37ec.h"

#include <endian.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


FILE* initEc() {
  system("modprobe ec_sys write_support=1");
  return fopen("/sys/kernel/debug/ec/ec0/io", "r+");
}

void closeEc(FILE* ec) {
  fclose(ec);
}

void fail(const char* msg) {
  printf("ERROR: %s\n", msg);
  exit(EXIT_FAILURE);
}



/**
 * Read 16 bits from EC
 */
unsigned short read16(FILE* f, unsigned char offset) {
  fseek(f, offset, SEEK_SET);
  unsigned short s = 0;
  fread(&s, sizeof(unsigned short), 1, f);
  return htobe16(s);
}


unsigned char read8(FILE* f, unsigned char offset) {
  fseek(f, offset, SEEK_SET);
  unsigned char c = 0;
  fread(&c, sizeof(unsigned char), 1, f);
  return c;
}

unsigned char read1(FILE* f, unsigned char offset, unsigned char bit) {
  fseek(f, offset, SEEK_SET);
  unsigned char c = 0;
  fread(&c, sizeof(unsigned char), 1, f);
  return (c & (1 << bit)) > 0 ? 1 : 0;
}

void write8(FILE* f, unsigned char offset, const unsigned char value) {
  fseek(f, offset, SEEK_SET);
  fwrite(&value, sizeof(unsigned char), 1, f);
}

void write1(FILE* f, unsigned char offset, unsigned char bit, const unsigned char value) {
  unsigned char c = read8(f, offset);  
  fseek(f, offset, SEEK_SET);
  if (value > 0) {
    c |= (1 << bit);
  } else {
    c &= ~(1 << bit);
  }
  fwrite(&c, sizeof(unsigned char), 1, f);
}
