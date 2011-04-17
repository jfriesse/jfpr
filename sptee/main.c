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

#include <sys/queue.h>

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE	512

/*
 * Structures
 */
struct fl_entry {
	FILE *f;
	SLIST_ENTRY(fl_entry) entries;
};

/*
 * Global variables
 */
SLIST_HEAD(fl_head, fl_entry) fl_head;

/*
 * Prototypes
 */
static void	fl_add(FILE *f);
static void	fl_fseek(long offset);
static void	fl_fwrite(void *ptr, size_t size, size_t nmemb);
static void	usage(void);

/*
 * Sparse tee - duplicate standard input to standard output and file(s)
 */
int
main(int argc, char *const argv[]) {
	char buf[BUF_SIZE];
	FILE *f;
	const char *o_mode;
	struct fl_entry *entry;
	size_t readed;
	int a_flag;
	int ch;
	int i;
	int last_seek;

	a_flag = 0;

	while ((ch = getopt(argc, argv, "ai")) != -1)
		switch (ch) {
		case 'a':
			a_flag = 1;
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

	if (a_flag) {
		/*
		 * Don't use a, because it looks like "a" simply ignores any
		 * fseek beyond end of file
		 */
		o_mode = "r+b";
	} else {
		o_mode = "wb";
	}

	SLIST_INIT(&fl_head);

	fl_add(stdout);
	for (i = 0; i < argc; i++) {
		f = fopen(argv[i], o_mode);
		if (f == NULL && a_flag) {
			/*
			 * Try to create file
			 */
			f = fopen(argv[i], "wb");
		}

		if (f == NULL) {
			err(1, "Can't open file %s\n", argv[i]);
		}

		if (a_flag) {
			fseek(f, 0, SEEK_END);
		}

		fl_add(f);
	}

	last_seek = 0;

	while ((readed = fread(buf, 1, sizeof(buf), stdin)) != 0) {
		last_seek = 0;

		if (readed == sizeof(buf)) {
			for (i = 0; i < sizeof(buf) && buf[i] == 0; i++) ;

			if (i == sizeof(buf)) {
				fl_fseek(sizeof(buf));
				last_seek = 1;
			} else {
				fl_fwrite(buf, 1, readed);
			}
		} else {
			fl_fwrite(buf, 1, readed);
		}
	}

	if (last_seek) {
		fl_fseek(-1);
		buf[0] = 0;
		fl_fwrite(buf, 1, 1);
	}

	while (!SLIST_EMPTY(&fl_head)) {
		entry = SLIST_FIRST(&fl_head);
		SLIST_REMOVE_HEAD(&fl_head, entries);
		if (fclose(entry->f) != 0) {
			err(1, "Can't close file");
		}
		free(entry);
	}

	return (0);
}

static void
fl_add(FILE *f) {
	struct fl_entry *entry;

	entry = malloc(sizeof(*entry));
	if (entry == NULL) {
		errx(1, "Out of memory");
	}

	entry->f = f;
	SLIST_INSERT_HEAD(&fl_head, entry, entries);
}

static void
fl_fseek(long offset)
{
	struct fl_entry *entry;

	SLIST_FOREACH(entry, &fl_head, entries) {
		if (fseek(entry->f, offset, SEEK_CUR) != 0) {
			if (errno != ESPIPE && errno != EBADF && errno != EINVAL) {
				err(1, "Can't seek");
			}
		}
	}
}

static void
fl_fwrite(void *ptr, size_t size, size_t nmemb)
{
	struct fl_entry *entry;

	SLIST_FOREACH(entry, &fl_head, entries) {
		if (fwrite(ptr, size, nmemb, entry->f) != nmemb) {
			err(1, "fwrite != nmem");
		}
	}
}

static void
usage(void) {
	extern char *__progname;

	fprintf(stderr, "usage: %s [-ai]\n", __progname);
	exit(1);
}
