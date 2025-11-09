#include <dirent.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void grep_file(const char *path, pcre *re) {
  int fd;
  struct stat stat;
  char *file_content;

  if ((fd = open(path, O_RDONLY)) == -1) {
    fprintf(stderr, "File openning failed\n");
    return;
  }

  if (fstat(fd, &stat) == -1) {
    fprintf(stderr, "Stat failed\n");
    close(fd);
    return;
  }

  file_content = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file_content == MAP_FAILED) {
    fprintf(stderr, "Mmap failed\n");
    close(fd);
    return;
  }

  int line_number = 1;
  char *line_start = file_content;
  char *next_newline;

  while (line_start < file_content + stat.st_size) {
    next_newline = strchr(line_start, '\n');

    int line_length = (next_newline)
                          ? (next_newline - line_start)
                          : (file_content + stat.st_size - line_start);
    int match_count;
    int ovector[30];
    match_count =
        pcre_exec(re, NULL, line_start, line_length, 0, 0, ovector, 30);

    if (match_count >= 0) {
      printf("%s:%d: ", path, line_number);
      fwrite(line_start, 1, line_length, stdout);
      printf("\n");
    }

    if (next_newline && *next_newline == '\n') {
      line_start = next_newline + 1;
      line_number++;
    } else {
      break;
    }
  }

  munmap(file_content, stat.st_size);
  close(fd);
}

void grep_dir(const char *dir_path, pcre *re) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(dir_path))) {
    fprintf(stderr, "Opendir failed\n");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
      grep_dir(path, re);
    } else if (entry->d_type == DT_REG) {
      char path[1024];
      snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
      grep_file(path, re);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Wrong Usage");
    return 1;
  }

  const char *regex = argv[1];
  const char *dir = argv[2];

  const char *error;
  int erroffset;
  pcre *re = pcre_compile(regex, 0, &error, &erroffset, NULL);
  if (re == NULL) {
    fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset,
            error);
    return 1;
  }

  grep_dir(dir, re);

  pcre_free(re);

  return 0;
}
