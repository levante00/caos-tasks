#include <arpa/inet.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 30

typedef struct StorageItem {
  char key[PATH_MAX];
  char value[PATH_MAX];
  struct StorageItem *next;
} StorageItem;

typedef struct Storage {
  struct StorageItem *head;
} Storage;

StorageItem *find(Storage *storage, char *key) {
  StorageItem *current = storage->head;
  while (current != NULL) {
    if (strncmp(current->key, key, PATH_MAX - 1) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void set(Storage *storage, char *key, char *value) {
  StorageItem *element = find(storage, key);
  if (element == NULL) {
    element = malloc(sizeof(StorageItem));
    strncpy(element->key, key, PATH_MAX - 1);
    element->key[PATH_MAX - 1] = '\0';
    element->next = storage->head;
    storage->head = element;
  }
  strncpy(element->value, value, PATH_MAX - 1);
  element->value[PATH_MAX - 1] = '\0';
}

char *get(Storage *storage, char *key) {
  StorageItem *element = find(storage, key);
  if (element == NULL) {
    return "\n";
  } else {
    return element->value;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 1;
  }

  Storage *storage = malloc(sizeof(Storage));
  storage->head = NULL;
  int server_port = atoi(argv[1]);

  int server_fd, new_fd;
  struct sockaddr_in server_addr;
  char buffer[1024] = {0};
  fd_set active_fds, read_fds;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    return 1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server_port);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    return 1;
  }

  if (listen(server_fd, 3) < 0) {
    return 1;
  }

  FD_ZERO(&active_fds);
  FD_SET(server_fd, &active_fds);

  while (1) {
    read_fds = active_fds;

    if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
      return 1;
    }

    for (int i = 0; i < FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == server_fd) {
          new_fd = accept(server_fd, NULL, NULL);
          if (new_fd < 0) {
            perror("Accept failed");
            continue;
          }
          FD_SET(new_fd, &active_fds);
        } else {
          memset(buffer, 0, sizeof(buffer));
          int read_bytes = read(i, buffer, sizeof(buffer) - 1);
          if (read_bytes <= 0) {
            close(i);
            FD_CLR(i, &active_fds);
          } else {
            char command[5], key[PATH_MAX], value[PATH_MAX];
            sscanf(buffer, "%s %s %s", command, key, value);

            if (strcmp(command, "set") == 0) {
              set(storage, key, value);
            } else if (strcmp(command, "get") == 0) {
              char *val = get(storage, key);
              if (strcmp(val, "\n") == 0) {
                send(i, val, strlen(val), 0);
              } else {
                char full_response[PATH_MAX + 2];
                snprintf(full_response, sizeof(full_response), "%s\n", val);
                send(i, full_response, strlen(full_response), 0);
              }
            }
          }
        }
      }
    }
  }

  return 0;
}
