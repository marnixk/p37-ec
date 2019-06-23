#include <stdio.h>

#ifndef _P37EC_HEADER
#define _P37EC_HEADER

	/**
	 * Read a word from EC
	 * 
	 * @param  f      incoming EC file descriptor
	 * @param  offset location to read from
	 * 
	 * @return        the 16 bit value at this location.
	 */
	unsigned short read16(FILE* f, unsigned char offset);

	/**
	 * Read a byte from EC
	 * 
	 * @param  f      incoming EC file descriptor
	 * @param  offset location to read from
	 * @return        the 8bit value at this location.
	 */	
	unsigned char read8(FILE* f, unsigned char offset);

	/**
	 * Read a bit from EC
	 * 
	 * @param  f      incoming EC file descriptor
	 * @param  offset location to read from
	 * @param  bit 	  the bit within the byte to return
	 * @return        the bit value at this location.
	 */
	unsigned char read1(FILE* f, unsigned char offset, unsigned char bit);

	/**
	 * Write a byte to the EC
	 * @param f      The EC file descriptor.
	 * @param offset The address offset to write to
	 * @param value  The bytes to write
	 */
	void write8(FILE* f, unsigned char offset, const unsigned char value);

	/**
	 * Write a byte to the EC
	 * @param f      The EC file descriptor.
	 * @param offset The address offset to write to
	 * @param value  The bytes to write
	 */
	void write1(FILE* f, unsigned char offset, unsigned char bit, const unsigned char value);

	/**
	 * Initialise.
	 */
	FILE* initEc();

	/**
	 * Close EC handler
	 * @param ec handler to close.
	 */
	void closeEc(FILE* ec);

	/**
	 * Fail and kill programme.
	 * @param msg what to say before going.
	 */
	void fail(const char* msg);

#endif