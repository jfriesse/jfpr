/*
 * Copyright (c) 2011, Jan Friesse <jfriesse@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Prototypes
 */
static void	usage(void);

/*
 * Sparse tee - duplicate standard input to standard output and file(s)
 */
int
main(int argc, char *const argv[]) {
	int aflag;
	int ch;

	aflag = 0;

	while ((ch = getopt(argc, argv, "ai")) != -1)
		switch (ch) {
		case 'a':
			aflag = 1;
			break;
		case 'i':
			signal(SIGINT, SIG_IGN);
			break;
		default:
			usage();
			/* NOTREACHED */
	}

	argc -= optind;
	argv += optind;

	return (0);
}

static void
usage(void) {
	extern char *__progname;

	fprintf(stderr, "usage: %s [-ai]\n", __progname);
	exit(1);
}
