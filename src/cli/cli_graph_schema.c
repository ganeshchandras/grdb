#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"

void cli_graph_schema_add(schema_type_t st, char *cmdline, int *pos);

void
cli_graph_schemas_print(char *gname)
{
	char s[BUFSIZE];
	DIR *dirfd;
	struct dirent *de;
	int fd;

	memset(s, 0, BUFSIZE);
	sprintf(s, "%s/%s", grdbdir, gname);
	if ((dirfd = opendir(s)) == NULL)
		return;

	for (;;) {
		de = readdir(dirfd);
		if (de == NULL)
			break;

		if (strcmp(de->d_name, ".") != 0 &&
		    strcmp(de->d_name, "..") != 0) {
			schema_t sv, se;

			schema_init(&sv);
			memset(s, 0, BUFSIZE);
			sprintf(s, "%s/%s/%s/sv", grdbdir, gname, de->d_name);
			fd = open(s, O_RDWR);
			if (fd >= 0) {
				sv = schema_read(fd);
				close(fd);
				if (sv != NULL)
					schema_print(sv);
			}
			schema_init(&se);
			memset(s, 0, BUFSIZE);
			sprintf(s, "%s/%s/%s/se", grdbdir, gname, de->d_name);
			fd = open(s, O_RDWR);
			if (fd >= 0) {
				se = schema_read(fd);
				close(fd);
				if (se != NULL)
					schema_print(se);
			}

		}
	}
	closedir(dirfd);
}

void
cli_graph_schema(char *cmdline, int *pos)
{
	char s[BUFSIZE];
        DIR *dirfd;
        struct dirent *de;

	memset(s, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s);

	if (strcmp(s, "vertex") == 0 || strcmp(s, "v") == 0) {
		cli_graph_schema_add(VERTEX, cmdline, pos);
		return;
	}
	if (strcmp(s, "edge") == 0 || strcmp(s, "e") == 0) {
		cli_graph_schema_add(EDGE, cmdline, pos);
		return;
	}
        /* Loop over directories in grdb directory to display each graph */
        if ((dirfd = opendir(grdbdir)) == NULL)
                return;

        for (;;) {
                de = readdir(dirfd);
                if (de == NULL)
                        break;

                if (strcmp(de->d_name, ".") != 0 &&
                    strcmp(de->d_name, "..") != 0) {
			cli_graph_schemas_print(de->d_name);
                }
        }
        closedir(dirfd);
}
