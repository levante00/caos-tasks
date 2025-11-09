#define _GNU_SOURCE
#include "signalfd.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

int fd[2];

static void signal_handler(int sig) {
  ssize_t bytes_written;
  do {
    bytes_written = write(fd[1], &sig, sizeof(sig));
  } while (bytes_written == -1 && errno == EINTR);
}

int signalfd() {
  if (pipe(fd) == -1) {
    return -1;
  }

  fcntl(fd[0], F_SETFL, O_NONBLOCK);

  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  for (int i = 1; i < 32; ++i) {
    if (i != SIGKILL && i != SIGSTOP) {
      sigaction(i, &sa, NULL);
    }
  }

  return fd[0];
}
