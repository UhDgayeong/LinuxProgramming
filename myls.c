#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

char type(mode_t);
char *perm(mode_t);
void printStat(char*, char*, struct stat*);

int main(int argc, char **argv)
{
	DIR *dp;
	char *dir;
	struct stat st;
	struct dirent *d;
	char path[BUFSIZ+1];
	char op[3];
	
	if (argc == 1 ) {
		strcpy(op, "x");
		dir = ".";
	}
		
	else {
		if (strcmp(argv[1], "-l") == 0) {
			strcpy(op, "l");
			if (argc > 2) {
				dir = argv[2];
			}
			else
				dir = ".";
			
		}
		else if (strcmp(argv[1], "-a") == 0) {
			strcpy(op, "a");
			if (argc > 2) {
				dir = argv[2];
			}
			else
				dir = ".";
			
		}
		else if (strcmp(argv[1], "-al") == 0) {
			strcpy(op, "al");
			if (argc > 2) {
				dir = argv[2];
			}
			else
				dir = ".";
			
		}
		else {
			strcpy(op, "x");
			dir = argv[1];
		}
	}
	
	if ((dp = opendir(dir)) == NULL) {
		if (lstat(dir, &st) < 0 )
			perror(dir);
		printStat(op, dir, &st);
		exit(0);
		//perror(dir);
	}
	
	while ((d = readdir(dp)) != NULL) {
		sprintf(path, "%s/%s", dir, d->d_name);
		if (lstat(path, &st) < 0) {
			perror(path);
		}
		printStat(op, d->d_name, &st);
	}
	if (strcmp(op, "a") == 0 || strcmp(op, "x") == 0)
		putchar('\n');
	
	closedir(dp);
	exit(0);
}

void printStat(char *op, char *file, struct stat *st) {
	
	if (strcmp(op, "al") == 0) {
		printf("%c%s ", type(st->st_mode), perm(st->st_mode)); //file type & permission
		printf("%2ld ", st->st_nlink);
		printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
		printf("%5ld ", st->st_size);
		printf("%.12s ", ctime(&st->st_mtime)+4);
		printf("%s", file);
		putchar('\n');
	}
	else if (strcmp(op, "l") == 0) {
		if (strncmp(".", file, 1) == 0)
			return;
		printf("%c%s ", type(st->st_mode), perm(st->st_mode)); //file type & permission
		printf("%2ld ", st->st_nlink);
		printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
		printf("%5ld ", st->st_size);
		printf("%.12s ", ctime(&st->st_mtime)+4);
		printf("%s", file);
		putchar('\n');
	}
	else if (strcmp(op, "a") == 0) {
		printf("%s  ", file);
	}
	else if (strcmp(op, "x") == 0) {
		if (strncmp(".", file, 1) == 0)
				return;
		printf("%s  ", file);
	}
}

char type(mode_t mode) {
	if (S_ISREG(mode))
		return('-');
	if (S_ISDIR(mode))
		return ('d');
	if (S_ISCHR(mode))
		return ('c');
	if (S_ISBLK(mode))
		return ('b');
	if (S_ISLNK(mode))
		return ('l');
	if (S_ISFIFO(mode))
		return ('p');
	if (S_ISSOCK(mode))
		return ('s');
}

char* perm(mode_t mode) {
	int i;
	static char perms[10];
	
	strcpy(perms, "---------");
	for (i=0; i < 3; i++) {
		if (mode & (S_IREAD >> i*3))
			perms[i*3] = 'r';
		if (mode & (S_IWRITE >> i*3))
			perms[i*3+1] = 'w';
		if (mode & (S_IEXEC >> i*3))
			perms[i*3+2] = 'x';
	}
	return(perms);
}
