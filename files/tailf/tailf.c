#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    return 1;
  }

  const int BYTE_READING_CHUNK_SIZE = 4096;
  const char *path = argv[1];
  ssize_t read_bytes;
  char buf[BYTE_READING_CHUNK_SIZE];

  int fd = open(path, O_RDONLY);

  if (fd == -1) {
    return 1;
  }


  while (1) {
    read_bytes = read(fd, buf, BYTE_READING_CHUNK_SIZE);

    if (read_bytes == -1) {
      close(fd);
      return 1;
    } else if (read_bytes > 0) {
      fwrite(buf, 1, read_bytes, stdout);
    }
    fflush(stdout);
    usleep(0.1);
  }

  close(fd);

  return 0;
}
