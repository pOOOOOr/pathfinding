#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include <curses.h>
#include <unistd.h>

#include "pq.h"

#define PROGRAM_NAME    "ucs"

#define SQ_FLAG_VISITED    0x01
#define SQ_FLAG_ENQUEUED   0x02
#define SQ_FLAG_IMPASSABLE 0x04
#define SQ_FLAG_START      0x08
#define SQ_FLAG_GOAL       0x10
#define SQ_FLAG_PATH       0x20

struct square
{
    unsigned int flags;
    char glyph;
    int cost;
    int dist;
    int pre;
    int post;
};

struct map
{
    struct square *grid;
    int width;
    int height;
};

int g_curses_interval = 0;

struct map *map_init (int width, int height);
void map_destroy (struct map *map);
struct map *load_map (char *fname);
void print_map (struct map *map);
void set_square (struct square *sq, char c);
void make_path (struct map *map, int x0, int y0, int x1, int y1);
void print_path (struct map *map, int x0, int y0, int x1, int y1);
void init_curses (void);
void end_curses (void);
void curses_draw_map (struct map *map);

void
make_path (struct map *map, int x0, int y0, int x1, int y1)
{
    int w = map->width, h = map->height,
        start = y0 * w + x0, end = y1 * w + x1;

    pq_t *pq = pq_create ();

    map->grid[start].flags |= SQ_FLAG_START;
    map->grid[end].flags |= SQ_FLAG_GOAL;

    map->grid[start].flags |= SQ_FLAG_VISITED;
    map->grid[start].pre = start;
    map->grid[start].dist = 0;
    pq_enqueue (pq, start, map->grid[start].dist);

    int index, cost, found = 0;
    int j, n_index, n_cost, n_dist;

    while (pq->size != 0)
    {
        pq_dequeue (pq, &index, &cost);
        map->grid[index].flags &= ~SQ_FLAG_ENQUEUED;

        if (index == end)
        {
            found = 1;
            break;
        }
        else
        {
            map->grid[index].flags |= SQ_FLAG_VISITED;
        }

        int neighbors[4] = { index - w, index + w, index - 1, index + 1 };

        for (j = 0; j < 4; j++)
        {
            n_index = neighbors[j];
            if (!(map->grid[n_index].flags & SQ_FLAG_VISITED)
                    && !(map->grid[n_index].flags & SQ_FLAG_IMPASSABLE)
                    && n_index < h*w && n_index > 0)
            {
                n_cost = map->grid[n_index].cost;
                n_dist = cost + n_cost;
                if (map->grid[n_index].flags & SQ_FLAG_ENQUEUED)
                {
                    if (n_dist < map->grid[n_index].dist)
                    {
                        map->grid[n_index].dist = n_dist;
                        map->grid[n_index].pre = index;
                    }
                }
                else
                {
                    pq_enqueue (pq, n_index, n_dist);
                    map->grid[n_index].dist = n_dist;
                    map->grid[n_index].flags |= SQ_FLAG_ENQUEUED;
                    map->grid[n_index].pre = index;
                }
            }

            curses_draw_map (map);
        }
    }

    if (found)
    {
        int pre_index, current_index = end;
        while (current_index != start)
        {
            pre_index = map->grid[current_index].pre;
            map->grid[pre_index].post = current_index;
            current_index = pre_index;
            map->grid[current_index].flags |= SQ_FLAG_PATH;
            curses_draw_map (map);
        }
    }

    pq_destroy(pq);
}

void
print_path (struct map *map, int x0, int y0, int x1, int y1)
{
    int cost = 0;
    int success = 1;
    int w = map->width, start = y0 * w + x0, end = y1 * w + x1;
    struct square *sq;

    int r_index = end;
    while (r_index != start)
    {
        r_index = map->grid[r_index].pre;

        if (r_index == 0)
        {
            success = 0;
            break;
        }

        sq = map->grid + r_index;
        sq->glyph = 'o';
        cost += sq->cost;
    }

    if (success)
    {
        map->grid[start].glyph = 'A';
        map->grid[end].glyph = 'B';

        print_map (map);

        int index = start;
        while (index != end)
        {
            printf ("(%d, %d) -> ", index % w, index / w);
            index = map->grid[index].post;
        }
        printf ("(%d, %d)\n", index % w, index / w);

        printf ("Path cost: %d\n", cost);
    }
    else
    {
        printf ("No path found.\n");
    }
}

void
set_square (struct square *sq, char c)
{
    sq->glyph = c;
    sq->flags = 0;
    switch (c)
    {
    case '.':
        sq->cost = 1;
        break;
    case ':':
        sq->cost = 2;
        break;
    case '=':
        sq->cost = 5;
        break;
    case '+':
        sq->cost = 12;
        break;
    case '#':
    default:

        sq->cost = -1;
        sq->flags |= SQ_FLAG_IMPASSABLE;
        break;
    }

    sq->dist = 0;
    sq->pre = 0;
    sq->post = 0;
}

struct map *
map_init (int width, int height)
{
    struct map *new_map;
    struct square *sq;
    int i;

    new_map = malloc (sizeof (struct map));
    if (!new_map)
    {
        return NULL;
    }

    new_map->grid = malloc (sizeof (struct square) * width * height);
    if (!new_map->grid)
    {
        return NULL;
    }

    new_map->width = width;
    new_map->height = height;

    sq = new_map->grid;
    for (i = 0; i < width * height; i++)
    {
        set_square (sq, '#');
        sq++;
    }

    return new_map;
}

void
map_destroy (struct map *map)
{
    if (map)
    {
        free (map->grid);
        free (map);
    }
}

struct map *
load_map (char *fname)
{
    struct map *map = NULL;
    struct square *sq;
    FILE *fp;
    int width, height;
    int n;
    char c;

    if ((fp = fopen (fname, "r")) == NULL)
    {
        fprintf (stderr, "Error: cannot open file %s.\n", fname);
        exit (EXIT_FAILURE);
    }

    width = height = n = 0;
    while ((c = getc (fp)) != EOF)
    {
        if (c == '\n')
        {
            height++;
            if (n > width)
            {
                width = n;
            }
            n = 0;
        }
        else
        {
            n++;
        }
    }

    if (fseek (fp, SEEK_SET, 0))
    {
        return NULL;
    }

    width += 2;
    height += 2;

    map = map_init (width, height);
    if (!map)
    {
        fprintf (stderr, "Cannot allocate memory for map structure.\n");
        exit (EXIT_FAILURE);
    }

    sq = map->grid + width + 1;
    while ((c = getc (fp)) != EOF)
    {
        if (c == '\n')
        {
            sq += 2;
        }
        else
        {
            set_square (sq, c);
            sq++;
        }
    }

    fclose (fp);

    return map;
}

void
print_map (struct map *map)
{
    int x, y, i;
    struct square *sq;

    if (!map)
    {
        return;
    }

    i = map->width;
    for (y = 1; y < map->height - 1; y++)
    {
        i++;
        for (x = 1; x < map->width - 1; x++)
        {
            sq = map->grid + i;
            putchar (sq->glyph);
            i++;
        }
        putchar ('\n');
        i++;
    }
}

void
init_curses (void)
{
    if (!g_curses_interval)
    {
        return;
    }
    initscr ();
    start_color ();
    init_pair (0, COLOR_WHITE, COLOR_BLACK);
    init_pair (1, COLOR_RED, COLOR_BLACK);
    init_pair (2, COLOR_CYAN, COLOR_BLACK);
    init_pair (3, COLOR_GREEN, COLOR_BLACK);
    init_pair (4, COLOR_YELLOW, COLOR_BLACK);
    init_pair (5, COLOR_BLUE, COLOR_BLUE);
    init_pair (6, COLOR_GREEN, COLOR_BLACK);

    atexit ((void (*)()) endwin);
}

void
end_curses (void)
{
    if (!g_curses_interval)
    {
        return;
    }
    getchar ();

}

void
curses_draw_map (struct map *map)
{
    int i, j;
    struct square *sq;
    if (!g_curses_interval)
    {
        return;
    }
    if (map == NULL)
    {
        return;
    }
    sq = map->grid + map->width;
    for (j = 1; j < map->height - 1; j++)
    {
        move (j - 1, 0);
        sq++;
        for (i = 1; i < map->width - 1; i++)
        {
            char ch = sq->glyph;
            int color = 0;

            if (sq->flags & SQ_FLAG_IMPASSABLE)
            {
                color = 5;
            }
            if (sq->flags & SQ_FLAG_VISITED)
            {
                color = 1;
            }
            if (sq->flags & SQ_FLAG_ENQUEUED)
            {
                color = 2;
                ch = 'o';
            }
            if (sq->flags & SQ_FLAG_PATH)
            {
                color = 6;
                ch = 'o';
            }
            if (sq->flags & SQ_FLAG_START)
            {
                color = 3;
                ch = 'A';
            }
            if (sq->flags & SQ_FLAG_GOAL)
            {
                color = 4;
                ch = 'B';
            }

            if (sq->flags & ~SQ_FLAG_IMPASSABLE)
            {
                attron (A_BOLD);
            }

            attron (COLOR_PAIR (color));
            addch (ch);
            attroff (COLOR_PAIR (color));

            if (sq->flags & ~SQ_FLAG_IMPASSABLE)
            {
                attroff (A_BOLD);
            }

            sq++;
        }
        sq++;
    }
    move (j, 0);
    refresh ();
    usleep (g_curses_interval);
}

int
main (int argc, char **argv)
{
    struct map *map;
    int x0, y0, x1, y1;

    if (argc < 6)
    {
        fprintf (stderr, "%s needs at least 5 arguments (%d were given)\n",
                 PROGRAM_NAME, argc - 1);
        fprintf (stderr,
                 "usage: %s mapfile x0 y0 x1 y1 (time_interval - optional)\n",
                 PROGRAM_NAME);
        exit (EXIT_FAILURE);
    }

    x0 = atoi (argv[2]);
    y0 = atoi (argv[3]);
    x1 = atoi (argv[4]);
    y1 = atoi (argv[5]);
    g_curses_interval = 0;
    if (argc > 6)
    {
        g_curses_interval = atoi (argv[6]);
    }

    map = load_map (argv[1]);
    if (!map)
    {
        fprintf (stderr, "Error loading map.\n");
        exit (EXIT_FAILURE);
    }
    if ((x0 <= 0 || x0 >= map->width - 1) ||
            (x1 <= 0 || x1 >= map->width - 1) ||
            (y0 <= 0 || y0 >= map->height - 1) ||
            (y1 <= 0 || y1 >= map->height - 1))
    {
        fprintf (stderr, "Coordinates are not in range.\n");
        exit (EXIT_FAILURE);
    }


    if (g_curses_interval > 0)
    {
        init_curses ();
    }

    make_path (map, x0, y0, x1, y1);

    if (g_curses_interval > 0)
    {
        end_curses ();
    }
    else
    {
        print_path (map, x0, y0, x1, y1);
    }

    map_destroy (map);

    return EXIT_SUCCESS;
}
