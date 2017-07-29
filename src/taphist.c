/*
 * taphist.c, show histogram of pulses in a TAP file
 * Copyright (C) 2010 Christopher Williams
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tap.h"

char *argv0;

int main(int argc, char **argv)
{
	struct tap tapfile;
	long pulse;
	long pulsehist[256];
	long max;
	const float logmin = -7.;
	int i;
	FILE *infile;
	int width = 80;
	
	argv0 = argv[0];
	for (i = 0; i < 256; ++i) pulsehist[i] = 0;
	if (argc == 2) {
		infile = fopen(argv[1], "rb");
		if (!infile) {
			perror(argv[1]);
			return 1;
		}
	} else if (argc == 1) {
		infile = stdin;
	} else {
		fprintf(stderr, "%s: too many arguments\n", argv0);
		return 1;
	}
	if (tap_read_header(&tapfile, infile)) {
		fprintf(stderr, "%s: error reading TAP file\n", argv0);
		return 1;
	}
	while ((pulse = tap_get_pulse(&tapfile)) >= 0) {
		//printf("pulse: %6ld (%04lx)\n", pulse, pulse);
		if (pulse < 8*256)
			++pulsehist[pulse/8];
	}
	
	max = 0;
	for (i = 0; i < 256; ++i) {
		if (pulsehist[i] > max)
			max = pulsehist[i];
	}
	for (i = 0; i < 256; ++i) {
		int j;
		float x;
		//printf("%4d: %ld\n", i, pulsehist[i]);
		printf("%4d: ", i*8);
		x = logf((float)pulsehist[i] / max);
		/* -inf <= x <= 0 */
		if (x < logmin) x = logmin;
		for (j = (x - logmin) / -logmin * (width - 6); j; --j)
			putchar('#');
		putchar('\n');
	}
	return 0;
}
