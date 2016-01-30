#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

#define MAX_LINE_LEN		128
#define MAX_SHAPE_WIDTH		MAX_LINE_LEN
#define MAX_SHAPE_HEIGHT	MAX_LINE_LEN
#define MAX_SHAPES		22
#define WALL			MAX_SHAPES + 2
#define TAB_WIDTH		8

struct shape {
	char shape[MAX_SHAPE_HEIGHT][MAX_SHAPE_WIDTH];
	int height;
	int width;
};

struct puzzle {
	struct shape board;
	struct shape tiles[MAX_SHAPES];
	int no_tiles;
};

int
line_empty(char *line)
{
	int i;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] != ' ' && line[i] != '\n' && line[i] != '\r' && line[i] != '\t') {
			return (0);
		}
	}

	return (1);
}

int
optimalize_shape(const struct shape *src, struct shape *dst)
{
	int x, y, x2, i;
	int ch;
	struct shape tmp_shape;
	int minx, maxx;

	/*
	 * Replace whitespace characters by space ' ' (or tab by TAB_SIZE spaces) and non whitespace
	 * characters by dot '.'
	 */
	for (y = 0; y < src->height; y++) {
		for (x = 0, x2 = 0; x < strlen(src->shape[y]); x++) {
			ch = src->shape[y][x];

			if (ch == ' ' || ch == '\n' || ch == '\r') {
				tmp_shape.shape[y][x2] = ' ';
			} else if (ch == '\t') {
				for (i = 0; i < TAB_WIDTH; i++) {
					tmp_shape.shape[y][x2] = ' ';

					if (i < TAB_WIDTH - 1)
						x2++;
				}
			} else {
				tmp_shape.shape[y][x2] = '.';
			}

			x2++;
		}
		tmp_shape.shape[y][x2] = '\0';
	}
	tmp_shape.height = src->height;

	/*
	 * Find minimal and maximal x
	 */
	minx = MAX_SHAPE_WIDTH + 1;
	maxx = 0;

	for (y = 0; y < tmp_shape.height; y++) {
		for (x = 0; x < strlen(tmp_shape.shape[y]); x++) {
			ch = tmp_shape.shape[y][x];

			if (ch != ' ' && x < minx) {
				minx = x;
			}

			if (ch != ' ' && x > maxx) {
				maxx = x;
			}
		}
	}

	/*
	 * Copy relevant data to dst shape
	 */
	for (y = 0; y < tmp_shape.height; y++) {
		for (x = minx, x2 = 0; x <= maxx; x++, x2++) {
			if (x < strlen(tmp_shape.shape[y])) {
				dst->shape[y][x2] = tmp_shape.shape[y][x];
			} else {
				dst->shape[y][x2] = ' ';
			}
		}
		dst->shape[y][maxx + 1] = '\0';
	}
	dst->height = tmp_shape.height;
	dst->width = (maxx - minx + 1);

	return (0);
}

int
load(FILE *f, struct puzzle *dst)
{
	char line[MAX_LINE_LEN];
	int state;
	struct shape raw, optimalized;
	int line_no;
	char *res;
	int board_loaded;

	state = 0;
	line_no = 0;
	dst->no_tiles = 0;
	board_loaded = 0;

	while (((res = fgets(line, MAX_LINE_LEN, f)) != NULL) || state != 0) {
		line_no++;

		if (res == NULL) {
			line[0] = '\0';
		} else {
			if (line[strlen(line) - 1] != '\n') {
				errx(1, "Error on line %u: Line too long or file does not end with newline", line_no);
			}
		}

		switch (state) {
		case 0:
			if (!line_empty(line)) {
				raw.height = 0;
				if (strlen(line) > MAX_SHAPE_WIDTH) {
					errx(1, "Error on line %u: Line too long", line_no);
				}

				strcpy(raw.shape[raw.height], line);
				raw.height++;
				state = 1;
			}
			break;
		case 1:
			if (line_empty(line)) {
				/*
				 * Shape loaded
				 */
				optimalize_shape(&raw, &optimalized);

				if (board_loaded) {
					if (dst->no_tiles >= MAX_SHAPES) {
						errx(1, "Error on line %u: Too much tiles", line_no);
					}
					memcpy(&dst->tiles[dst->no_tiles], &optimalized, sizeof(optimalized));
					dst->no_tiles++;
				} else {
					memcpy(&dst->board, &optimalized, sizeof(optimalized));
					dst->no_tiles = 0;
					board_loaded = 1;
				}
				state = 0;
			} else {
				if (strlen(line) > MAX_SHAPE_WIDTH) {
					errx(1, "Error on line %u: Line too long", line_no);
				}

				strcpy(raw.shape[raw.height], line);
				raw.height++;
			}
			break;
		}
	}

	if (!board_loaded || dst->no_tiles == 0) {
		errx(1, "Board not defined or no tiles is defined in file");
	}

	return (0);
}

int
dump_shape(const struct shape *s, char empty, char full)
{
	int x, y;

	for (y = 0; y < s->height; y++) {
		for (x = 0; x < s->width; x++) {
			if (s->shape[y][x] == ' ') {
				printf("%c", empty);
			} else {
				printf("%c", full);
			}
		}
		printf("\n");
	}

	return (0);
}

int
dump_puzzle(const struct puzzle *p)
{
	int i;

	printf("Board:\n");
	dump_shape(&p->board, ' ', 'X');
	printf("\n");

	for (i = 0; i < p->no_tiles; i++) {
		printf("Tile %u (%c):\n", i + 1, 'A' + i);

		dump_shape(&p->tiles[i], ' ', 'A' + i);
		printf("\n");
	}

	return (0);
}

#define SVG_TILE_WIDTH	50
#define SVG_TILE_HEIGHT 50

char *svg_colors[MAX_SHAPES] = {
    "rgb(255,0,0)",
    "rgb(0,255,0)",
    "rgb(255,255,0)",
    "rgb(0,255,255)",
    "rgb(255,0,255)",
    "rgb(128,0,0)",
    "rgb(0,128,0)",
    "rgb(128,128,0)",
    "rgb(0,128,128)",
    "rgb(128,0,128)",
    "rgb(255,128,128)",
    "rgb(128,255,128)",
    "rgb(255,255,128)",
    "rgb(128,255,255)",
    "rgb(255,128,255)",
    "rgb(255,128,0)",
    "rgb(255,0,128)",
    "rgb(128,255,0)",
    "rgb(0,255,128)",
    "rgb(0,128,255)",
    "rgb(64,0,0)",
    "rgb(0,64,0)",
};

int
dump_board_svg(const struct shape *board)
{
	int x, y;
	FILE *f;
	char fname[512];
	char cmd[512];
	int fd;

	snprintf(fname, sizeof(fname), "cbsolve-XXXXXX");

	fd = mkstemp(fname);
	if (fd == 0) {
		err(1, "Can't create temp file name");
	}

	f = fdopen(fd, "wt");
	if (f == NULL) {
		err(1, "Can't open file");
	}

	fprintf(f, "<svg version=\"1.1\" baseProfile=\"full\" width=\"%u\" "
	    "height=\"%u\" xmlns=\"http://www.w3.org/2000/svg\">\n",
	    board->width * SVG_TILE_WIDTH, board->height * SVG_TILE_HEIGHT);

	for (y = 0; y < board->height; y++) {
		for (x = 0; x < board->width; x++) {
			fprintf(f, "<rect x=\"%u\" y=\"%u\" width=\"%u\" height=\"%u\" fill=\"",
				x * SVG_TILE_WIDTH, y * SVG_TILE_HEIGHT, SVG_TILE_WIDTH,
				SVG_TILE_HEIGHT);

			if (board->shape[y][x] == 0) {
				fprintf(f, "%s", "black");
			} else if (board->shape[y][x] == WALL) {
				fprintf(f, "%s", "none");
			} else {
				fprintf(f, "%s\" stroke=\"black\" stroke-width=\"1", svg_colors[board->shape[y][x] - 1]);
			}
			fprintf(f, "\" />\n");
		}
	}
	fprintf(f, "</svg>");
	fclose(f);

	snprintf(cmd, sizeof(cmd), "display svg:%s\n", fname);

	system(cmd);
	unlink(fname);

	return (0);
}

int
dump_board(const struct shape *board, int show_wall)
{
	int x, y;

	for (y = 0; y < board->height; y++) {
		for (x = 0; x < board->width; x++) {
			if (board->shape[y][x] == 0) {
				printf("%c", ' ');
			} else if (board->shape[y][x] == WALL) {
				if (show_wall) {
					printf("%c", 'X');
				} else {
					printf("%c", ' ');
				}
			} else {
				printf("%c", 'A' + board->shape[y][x] - 1);
			}
		}
		printf("\n");
	}

	return (0);

}

int
insert_tile(struct shape *board, int tile_no, const struct shape *tile, int x, int y)
{
	int tx, ty;

	for (ty = 0; ty < tile->height; ty++) {
		for (tx = 0; tx < tile->width; tx++) {
			if (tile->shape[ty][tx] != ' ') {
				if (board->shape[ty + y][tx + x] != 0) {
					errx(1, "Can't insert tile into board!");
				} else {
					board->shape[ty + y][tx + x] = tile_no;
				}
			}
		}
	}

	return (0);
}

int
remove_tile(struct shape *board, int tile_no)
{
	int x, y;

	for (y = 0; y < board->height; y++) {
		for (x = 0; x < board->width; x++) {
			if (board->shape[y][x] == tile_no) {
				board->shape[y][x] = 0;
			}
		}
	}

	return (0);
}

int
can_insert_tile(const struct shape *board, const struct shape *tile, int x, int y)
{
	int tx, ty;

	for (ty = 0; ty < tile->height; ty++) {
		for (tx = 0; tx < tile->width; tx++) {
			if (tile->shape[ty][tx] != ' ') {
				if (ty + y >= board->height) {
					return (0);
				}

				if (tx + x >= board->width) {
					return (0);
				}

				if (board->shape[ty + y][tx + x] != 0) {
					return (0);
				}
			}
		}
	}

	return (1);
}

int
solve_rec(struct shape *board, const struct puzzle *p, int current_tile)
{
	int x, y;
	const struct shape *current_shape;

	if (current_tile == p->no_tiles) {
		/*
		 * Found solution
		 */
		printf("Solution:\n");
		dump_board(board, 0);
		dump_board_svg(board);
		printf("\n");

		return (0);
	}

	current_shape = &p->tiles[current_tile];

	for (y = 0; y < board->height - current_shape->height + 1; y++) {
		for (x = 0; x < board->width - current_shape->width + 1; x++) {
			if (can_insert_tile(board, &p->tiles[current_tile], x, y)) {
				insert_tile(board, current_tile + 1, &p->tiles[current_tile], x, y);

				solve_rec(board, p, current_tile + 1);

				remove_tile(board, current_tile + 1);
			}
		}
	}

	return (0);
}

int
solve(const struct puzzle *p)
{
	struct shape board;
	int x, y;

	for (y = 0; y < p->board.height; y++) {
		for (x = 0; x < p->board.width; x++) {
			if (p->board.shape[y][x] == ' ') {
				board.shape[y][x] = WALL;
			} else {
				board.shape[y][x] = 0;
			}
		}
	}
	board.width = p->board.width;
	board.height = p->board.height;

	solve_rec(&board, p, 0);

	return (0);
}

int
main(void)
{
	struct puzzle p;

	load(stdin, &p);
	dump_puzzle(&p);
	solve(&p);

	return (0);
}
