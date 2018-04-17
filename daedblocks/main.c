/*
 * Copyright (c) 2018, Jan Friesse <jfriesse@gmail.com>
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

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BLOCK_SIZE	(1024 * 1024)

#define MAX_BLOCK_SIZE		(1LL * 1024 * 1024 * 1024)
#define BLOCKS_PER_LINE		80
#define BLOCKS_FORMAT		"%zu "

static void
usage(void)
{
	extern char *__progname;

	fprintf(stderr, "usage: %s [-E] [-b block_size] block_device|file\n", __progname);
	exit(1);
}

int
main(int argc, char *const argv[])
{
	char *ep;
	FILE *f;
	unsigned char *buf;
	unsigned char *zero_buf;
	size_t block_size;
	size_t blocks;
	size_t readed;
	long long tmp_ll;
	int ch;
	int erase;

	erase = 0;
	block_size = DEFAULT_BLOCK_SIZE;

	while ((ch = getopt(argc, argv, "Eb:")) != -1) {
		switch (ch) {
		case 'E':
			erase = 1;
			break;
		case 'b':
			errno = 0;
			tmp_ll = strtoll(optarg, &ep, 10);
			if (ep == optarg || *ep != '\0' || errno != 0) {
				errx(1, "Can't parse number %s", optarg);
			}
			if (tmp_ll < 1 || tmp_ll > MAX_BLOCK_SIZE) {
				errx(1, "%lld is not withing allowed range <%lld, %lld>",
				    tmp_ll, 1LL, MAX_BLOCK_SIZE);
			}
			block_size = (size_t)tmp_ll;
			break;
		default:
			usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 1) {
		warnx("Expecting exactly one input file");
		usage();
	}

	buf = zero_buf = NULL;

	if (((buf = calloc(1, block_size)) == NULL) ||
	    ((zero_buf = calloc(1, block_size)) == NULL)) {
		free(buf);
		free(zero_buf);

		errx(2, "Can't alloc memory");
	}

	f = fopen(argv[0], "r+b");
	if (f == NULL) {
		free(buf);
		free(zero_buf);

		err(3, "Can't open input file %s", argv[0]);
	}

	blocks = 0;

	printf(BLOCKS_FORMAT, blocks);

	while ((readed = fread(buf, 1, block_size, f)) != 0) {
		if (memcmp(buf, zero_buf, readed) == 0) {
			if (readed == block_size) {
				printf(".");
			} else {
				printf(",");
			}
		} else {
			if (readed == block_size) {
				printf("#");
			} else {
				printf("+");
			}

			if (erase) {
				if (fseek(f, -readed, SEEK_CUR) != 0) {
					free(buf);
					free(zero_buf);
					fclose(f);

					err(4, "Can't seek back");
				}

				if (fwrite(zero_buf, 1, readed, f) != readed) {
					free(buf);
					free(zero_buf);
					fclose(f);

					err(5, "Can't store zero_buf");
				}
			}
		}

		blocks++;

		if (blocks % BLOCKS_PER_LINE == 0) {
			printf("\n");
			printf(BLOCKS_FORMAT, blocks);
		}

	}

	printf("\n");

	free(buf);
	free(zero_buf);
	fclose(f);

	return (0);
}
