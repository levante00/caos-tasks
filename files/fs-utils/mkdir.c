#include <asm-generic/errno-base.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int opt;
  const int DEFAULT_MODE = 0755;
  int mode = DEFAULT_MODE;
  int create_parent = 0;

  struct option long_options[] = {{"mode", required_argument, NULL, 'm'},
                                  {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "pm:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'p':
      create_parent = 1;
      break;
    case 'm':
      mode = strtol(optarg, NULL, 8);
      break;
    default:
      return 1;
    }
  }

  for (int i = optind; i < argc; i++) {
    const char *dir_path = argv[i];
    if (mkdir(dir_path, mode) == -1) {
      if (errno == EEXIST) {
        return 1;
      } else if (errno == ENOENT && create_parent == 0) {
        return 1;
      } else if (errno == ENOENT && create_parent == 1) {
        char *token;
        char cp_dir_path[strlen(dir_path) + 1];
        char accumulator[strlen(dir_path) + 1];

        strcpy(cp_dir_path, dir_path);
        char *last_sep = strrchr(cp_dir_path, '/');
        if (last_sep != NULL) {
          *last_sep = '\0';
        } else {
          return 1;
        }
        token = strtok(cp_dir_path, "/");
        accumulator[0] = '/';
        strcpy(accumulator + 1, token);

        while (token != NULL) {
          mkdir(accumulator, DEFAULT_MODE);

          token = strtok(NULL, "/");
          if (token != NULL) {
            strcat(accumulator, "/");
            strcat(accumulator, token);
          }
        }
        mkdir(dir_path, mode);

      } else {
        return 1;
      }
    }
  }

  return 0;
}
