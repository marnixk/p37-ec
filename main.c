#include "p37ec.h"

#include <endian.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * prototype definitions
 */
int executeQuickSettings(FILE *ec, char *command, unsigned short verbose);
void showUsage(FILE *ec, char **args);
void sleepForMs(unsigned int msec);
void keepQuietAlgorithm(FILE *ec);
void executeManualSetting(FILE *ec, char **args);


/**
 * Show the usage for this application.
 * 
 * @param ec  the embedded controller file pointer
 * @param args
 */
void showUsage(FILE *ec, char **args) {


		printf("Usage: sudo %s [<quicksetting>]\n", args[0]);

		printf("Usage: sudo %s [<hex-offset[.bit]> <hex-value>]\n", args[0]);
		printf("   Ex: sudo %s 0x01.6 0x07\n\n", args[0]);
		printf("Current Embedded Controller Values:\n");
		printf("  USB Charge During Sleep     [0x01.5]: %d\n", read1(ec, 0x01, 5));
		printf("  USB Charge During Hibernate [0x07.2]: %d\n", read1(ec, 0x07, 2));
		printf("  Camera Enabled              [0x01.6]: %d\n", read1(ec, 0x01, 6));
		printf("  Bluetooth Enabled           [0x01.7]: %d\n", read1(ec, 0x01, 7));
		printf("  WiFi Enabled                [0x02.6]: %d\n", read1(ec, 0x02, 6));
		printf("  Touchpad Enabled            [0x03.5]: %d\n", read1(ec, 0x03, 5));
		printf("  Ambient Light               [0x66]:   %d%%\n", read8(ec, 0x66));
		printf("  Screen Enabled              [0x09.3]: %d\n", read1(ec, 0x09, 3));
		printf("  Keyboard Backlight Mode     [0xD7]:   %d\n", read8(ec, 0xD7));
		printf("  CPU Temp                    [0x60]:   %d C\n", read8(ec, 0x60));
		printf("  GPU Temp                    [0x61]:   %d C\n", read8(ec, 0x61));
		printf("  MLB Temp                    [0x62]:   %d C\n", read8(ec, 0x62));
		printf("  Fan0 Speed                  [0xFC]:   %d RPM\n", read16(ec, 0xFC));
		printf("  Fan1 Speed                  [0xFE]:   %d RPM\n", read16(ec, 0xFE));
		printf("  Fan Control Enabled         [0x13.3]: %d\n", read1(ec, 0x13, 3));
		printf("  Fan Quiet Mode Enabled      [0x08.6]: %d\n", read1(ec, 0x08, 6));
		printf("  Fan Gaming Mode Enabled     [0x12.4]: %d\n", read1(ec, 0x12, 4));
		printf("  Fan Custom Mode Enabled     [0x13.0]: %d\n", read1(ec, 0x13, 0));
		printf("  Fan0 Custom Speed Setting   [0xB0]:   %d%%\n", (int)round(read8(ec, 0xB0) / 2.55));
		printf("  Fan1 Custom Speed Setting   [0xB1]:   %d%%\n", (int)round(read8(ec, 0xB1) / 2.55));
		printf("  Current Speed Setting       [0x64]:   %d\n", read8(ec, 0x64));

}



void sleepForMs(unsigned int msec) {
	struct timespec ts;
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;
	nanosleep(&ts, NULL);
}


void keepQuietAlgorithm(FILE *ec) {

	unsigned int idx = 0;
	unsigned int highFor = 0;
	unsigned int stepSizeMs = 200;
	unsigned int reduceAfter = 10;
	unsigned int minimiseAfter = 5;
	unsigned int reduceIfUnderTemp = 70;
	unsigned int fanThreshold = 5000;
	
	printf("\e[2J\e[H"); 

	// set initial
	executeQuickSettings(ec, "silent", 0);
	executeQuickSettings(ec, "quiet", 0);

	while (1) {

		// progress update. 
		char progChar = (
				idx == 0 ? '|' : 
				idx == 1 ? '/' : 
				idx == 2 ? '-' : 
				idx == 3 ? '\\' : '*'
		);

		// reset so we can read more 
		write8(ec, 0x01, 0xA3);

		// extract information speeds
		unsigned int 
			fan1 = read16(ec, 0xFC), 
			fan2 = read16(ec, 0xFE),
			cpuTemp = read8(ec, 0x60),
			totalSpeed = fan1 + fan2
		;

		// nothing going on?
		if (totalSpeed == 0) {
			highFor = 0;
		}
		// half way? let's reduce the speed.
		else if (totalSpeed > fanThreshold && cpuTemp < reduceIfUnderTemp && highFor == minimiseAfter) {
			progChar = '*';
			executeQuickSettings(ec, "reduced", 0);
			++highFor;
		}
		// end game? let's turn them back off
		else if (totalSpeed > fanThreshold && cpuTemp < reduceIfUnderTemp && highFor >= reduceAfter) {
			// adjusting.
			highFor = 0;
			progChar = '*';
			// executeQuickSettings(ec, "silent", 0);
			executeQuickSettings(ec, "silent", 0);
		} 

		// want to make sure we cool
		else if (cpuTemp >= reduceIfUnderTemp) {
			highFor = 0;
		}

		// want to make sure we're counting this as a busy period
		else if (totalSpeed > fanThreshold) {
			++highFor;
		}

		printf("[\033[1;34m%c\033[0m] [\x1b[37;1m%02d\033[0m] State: %d rpm, %d rpm, %d 'C                             \r", progChar, highFor, fan1, fan2, cpuTemp);
		fflush(stdout);

		sleepForMs(stepSizeMs); // seems like we might not need this.

		// increase progress bar indicator.
		++idx;
		if (idx >= 4) {
			idx = 0;
		}
	}
}



int executeQuickSettings(FILE *ec, char *command, unsigned short verbose) {

	if (strcmp(command, "silent") == 0) {
		write1(ec, 0x13, 3, 1);   // enable fan control
	
		if (verbose) printf("Command: SILENT\n");
		write1(ec, 0x08, 6, 1);   // enable fan quiet mode
		write1(ec, 0x12, 4, 0);   // disable gaming mode
		write1(ec, 0x13, 0, 1);   // enable custom mode

		// basically disable both fans
		write8(ec, 0x64, 0x0);   
		write8(ec, 0xb0, 0x1);   
		write8(ec, 0xb1, 0x1);
	}
	else if (strcmp(command, "quiet") == 0) {
	
		if (verbose) printf("Command: QUIET\n");
		write1(ec, 0x13, 3, 1);   // enable fan control
		write1(ec, 0x08, 6, 1);   // fan quiet mode enabled
		write1(ec, 0x12, 4, 0);   // disable gaming mode
		write1(ec, 0x13, 0, 0);   // disable custom mode
	}
	else if (strcmp(command, "reduced") == 0) {

		if (verbose) printf("Command: REDUCED\n");

		write1(ec, 0x13, 3, 1);   // enable fan control
		write1(ec, 0x08, 6, 0);   // fan quiet mode enabled
		write1(ec, 0x12, 4, 0);   // disable gaming mode
		write1(ec, 0x13, 0, 1);   // enable custom mode
		write8(ec, 0xb0, 8);
		write8(ec, 0xb1, 8);

	}
	else if (strcmp(command, "normal") == 0) {

		if (verbose) printf("Command: NORMAL\n");
		write1(ec, 0x13, 3, 1);   // enable fan control
		write1(ec, 0x08, 6, 0);   // fan quiet mode enabled
		write1(ec, 0x12, 4, 0);   // disable gaming mode
		write1(ec, 0x13, 0, 0);   // disable custom mode

	}
	else if (strcmp(command, "gaming") == 0) {

		if (verbose) printf("Command: GAMING\n");
		write1(ec, 0x13, 3, 1);   // enable fan control
		write1(ec, 0x08, 6, 0);   // disable quiet mode
		write1(ec, 0x12, 4, 1);   // enable gaming mode
		write1(ec, 0x13, 0, 0);   // disable custom mode

	}  
	else if (strcmp(command, "keep_quiet") == 0) {
		keepQuietAlgorithm(ec);

	}
	else {
		printf("Unknown command: '%s', expected: keep_quiet, silent, quiet, reduced, normal, gaming\n", command);
	}
	return 0;
}



/**
 * Execute the normal hex editing.
 * 
 * @param  ec     is the file pointer to drive
 * @param  args   the arguments to operate on.
 */
void executeManualSetting(FILE *ec, char **args) {
		char* dotIdx = strchr(args[1], '.');
		if (dotIdx != NULL) {
			*dotIdx = '\0';
			dotIdx++;
			unsigned char offset = (unsigned char)strtol(args[1], NULL, 0);
			unsigned char bit = (unsigned char)atoi(dotIdx);
			unsigned char value = (unsigned char)strtol(args[2], NULL, 0);
			write1(ec, offset, bit, value);
		} else {
			unsigned char offset = (unsigned char)strtol(args[1], NULL, 0);
			unsigned char value = (unsigned char)strtol(args[2], NULL, 0);
			write8(ec, offset, value);
		}
}





int main(int argc, char** args) {
	FILE* ec = initEc();
	if (!ec) {
		fail("Unable to initialize embedded controller; did you forget to use sudo?");
	}

	// Setting the following bits causing the EC to activate (or at least activate fan controls)
	write8(ec, 0x01, 0xA3);

	if (argc != 3 && argc != 2) {
		showUsage(ec, args);
	} 
	else if (argc == 2) {
		executeQuickSettings(ec, args[1], 1);
	}
	else if (argc == 3) {
		executeManualSetting(ec, args);  
	}

	// close file pointer to embedded controller.
	closeEc(ec);

	// Exit success
	exit(EXIT_SUCCESS);
}
