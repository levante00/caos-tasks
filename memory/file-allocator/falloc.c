#include "falloc.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const size_t BITS_COUNT = 64;

void falloc_init(file_allocator_t *allocator, const char *filepath,
                 uint64_t allowed_page_count) {

  int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    return;
  }

  off_t file_size = allowed_page_count * PAGE_SIZE;
  if (ftruncate(fd, file_size) == -1) {
    close(fd);
    return;
  }

  void *base_addr =
      mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (base_addr == MAP_FAILED) {
    close(fd);
    return;
  }

  size_t page_mask_size = (allowed_page_count + BITS_COUNT - 1) / BITS_COUNT;
  if (page_mask_size > PAGE_MASK_SIZE) {
    close(fd);
    return;
  }

  static uint64_t page_mask[PAGE_MASK_SIZE];
  for (size_t i = 0; i < page_mask_size; ++i) {
    page_mask[i] = 0;
  }

  allocator->fd = fd;
  allocator->base_addr = base_addr;
  allocator->page_mask = page_mask;
  if (allocator->base_addr != filepath &&
      allocator->allowed_page_count != allowed_page_count) {
    allocator->curr_page_count = 0;
  }
  allocator->allowed_page_count = allowed_page_count;
}

void falloc_destroy(file_allocator_t *allocator) {
  munmap(allocator->base_addr, allocator->allowed_page_count * PAGE_SIZE);
  allocator->base_addr = NULL;
  allocator->page_mask = NULL;
  close(allocator->fd);
}

void *falloc_acquire_page(file_allocator_t *allocator) {
  for (uint64_t i = 0; i < allocator->allowed_page_count; ++i) {
    if ((allocator->page_mask[i / BITS_COUNT] & (1ULL << (i % BITS_COUNT))) ==
        0) {
      allocator->page_mask[i / BITS_COUNT] |= (1ULL << (i % BITS_COUNT));
      allocator->curr_page_count++;
      return (char *)allocator->base_addr + i * PAGE_SIZE;
    }
  }

  return NULL;
}

void falloc_release_page(file_allocator_t *allocator, void **addr) {
  uint64_t index = ((char *)(*addr) - (char *)allocator->base_addr) / PAGE_SIZE;

  allocator->page_mask[index / BITS_COUNT] &= ~(1ULL << (index % BITS_COUNT));
  allocator->curr_page_count--;
  *addr = NULL;
}
