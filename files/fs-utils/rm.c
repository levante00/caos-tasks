#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int remove_file(const char *file_path, struct stat st) {
  if (S_ISLNK(st.st_mode)) {
    if (remove(file_path) == -1) {
      return 1;
    }
  } else if (S_ISREG(st.st_mode)) {
    if (remove(file_path) == -1) {
      return 1;
    }
  }

  return 0;
}

int remove_directory(const char *dir_path, int recursive) {
  if (recursive) {
    DIR *dir;
    if ((dir = opendir(dir_path)) == NULL) {
      return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        char entry_path[PATH_MAX];
        strcpy(entry_path, dir_path);
        strcat(entry_path, "/");
        strcat(entry_path, entry->d_name);

        struct stat st;
        if (lstat(entry_path, &st) == -1) {
          return 1;
        }

        if (S_ISLNK(st.st_mode) || S_ISREG(st.st_mode)) {
          if (remove_file(entry_path, st) == 1) {
            return 1;
          }
        } else if (S_ISDIR(st.st_mode)) {
          remove_directory(entry_path, recursive);
        }
      }
    }

    closedir(dir);

    if (rmdir(dir_path) == -1) {
      return 1;
    }
  } else {
    return 1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int recursive = 0;
  int opt;

  while ((opt = getopt(argc, argv, "r")) != -1) {
    switch (opt) {
    case 'r':
      recursive = 1;
      break;
    default:
      return 1;
    }
  }

  for (int i = optind; i < argc; i++) {
    const char *path = argv[i];
    struct stat st;

    if (lstat(path, &st) == -1) {
      return 1;
    }

    if (S_ISLNK(st.st_mode) || S_ISREG(st.st_mode)) {
      if (remove_file(path, st) == 1) {
        return 1;
      }
    } else if (S_ISDIR(st.st_mode)) {
      if (recursive) {
        if (remove_directory(path, recursive) == 1) {
          return 1;
        };
      } else {
        return 1;
      }
    }
  }

  return 0;
}
