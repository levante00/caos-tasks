#include "lca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

const size_t MAX_READ_LENGTH = 512;

pid_t get_parent_pid(pid_t pid) {
  FILE *file;
  char path[MAX_READ_LENGTH];
  char line[MAX_READ_LENGTH];

  snprintf(path, sizeof(path), "/proc/%d/status", pid);

  if ((file = fopen(path, "r")) == NULL) {
    return -1;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    if (strncmp(line, "PPid:", 5) == 0) {
      fclose(file);
      return atoi(line + 6);
    }
  }

  fclose(file);
  return -1;
}

pid_t find_lca(pid_t x, pid_t y) {
  pid_t ancestors_x[MAX_TREE_DEPTH];
  pid_t ancestors_y[MAX_TREE_DEPTH];
  size_t depth_x = 0, depth_y = 0;

  while (x > 1 && depth_x < MAX_TREE_DEPTH) {
    ancestors_x[depth_x++] = x;
    x = get_parent_pid(x);
  }

  while (y > 1 && depth_y < MAX_TREE_DEPTH) {
    ancestors_y[depth_y++] = y;
    y = get_parent_pid(y);
  }

  for (size_t i = 0; i < depth_x; ++i) {
    for (size_t j = 0; j < depth_y; ++j) {
      if (ancestors_x[i] == ancestors_y[j]) {
        return ancestors_x[i];
      }
    }
  }

  return -1;
}
