#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

enum { MAX_ARGS_COUNT = 256, MAX_CHAIN_LINKS_COUNT = 256 };

typedef struct {
  char *command;
  uint64_t argc;
  char *argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
  uint64_t chain_links_count;
  chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void create_chain(char *command, chain_t *chain) {
  char *token = strtok(command, "|");
  uint64_t i = 0;

  while (token != NULL && i < MAX_CHAIN_LINKS_COUNT) {
    chain->chain_links[i].command = strdup(token);
    chain->chain_links[i].argc = 0;
    token = strtok(NULL, "|");
    ++i;
  }
  chain->chain_links_count = i;

  for (uint64_t j = 0; j < chain->chain_links_count; ++j) {
    char *arg_token = strtok(chain->chain_links[j].command, " ");
    while (arg_token != NULL && chain->chain_links[j].argc < MAX_ARGS_COUNT) {
      chain->chain_links[j].argv[chain->chain_links[j].argc++] =
          strdup(arg_token);
      arg_token = strtok(NULL, " ");
    }
  }
}

void run_chain(chain_t *chain) {
  int pipe_fd[2];
  pid_t child_pid;
  int prev_pipe_read = -1;

  for (uint64_t i = 0; i < chain->chain_links_count; ++i) {
    if (i < chain->chain_links_count - 1) {
      if (pipe(pipe_fd) == -1) {
        return;
      }
    }

    if ((child_pid = fork()) == -1) {
      return;
    }

    if (child_pid == 0) {
      if (i > 0) {
        if (dup2(prev_pipe_read, STDIN_FILENO) == -1) {
          return;
        }

        close(prev_pipe_read);
      }

      if (i < chain->chain_links_count - 1) {
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
          return;
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
      }

      execvp(chain->chain_links[i].argv[0], chain->chain_links[i].argv);
    } else {
      if (prev_pipe_read != -1) {
        close(prev_pipe_read);
      }

      if (i < chain->chain_links_count - 1) {
        close(pipe_fd[1]);
        prev_pipe_read = pipe_fd[0];
      }
    }
  }

  int status;
  if (waitpid(child_pid, &status, 0) == -1) {
    return;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }

  chain_t my_chain;
  create_chain(argv[1], &my_chain);
  run_chain(&my_chain);

  return 0;
}
