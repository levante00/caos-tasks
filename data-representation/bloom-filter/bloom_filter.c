#include <stdlib.h>
#include <string.h>

#include "bloom_filter.h"

uint64_t calc_hash(const char *str, uint64_t modulus, uint64_t seed) {
  // TODO: Write any hash function for further usage
  uint64_t hash = 0;
  uint64_t string_length = strlen(str);
  uint64_t power = 1;

  for (uint64_t i = 0; i < string_length; i++) {
    hash = (hash + str[i] * power % modulus) % modulus;
    power = (power * seed) % modulus;
  }

  return hash;
}

void bloom_init(struct BloomFilter *bloom_filter, uint64_t set_size,
                hash_fn_t hash_fn, uint64_t hash_fn_count) {
  // TODO: Initialize bloom filter
  bloom_filter->set = (uint64_t *)calloc(set_size / 64 + 1, sizeof(uint64_t));
  bloom_filter->set_size = set_size;
  bloom_filter->hash_fn = hash_fn;
  bloom_filter->hash_fn_count = hash_fn_count;
}

void bloom_destroy(struct BloomFilter *bloom_filter) {
  // TODO: Free memory if needed
  free(bloom_filter->set);
  bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter *bloom_filter, Key key) {
  // TODO: Insert key into set
  uint64_t const kSeed = 7741; // Random big prime number
  uint64_t hash = calc_hash(key, bloom_filter->set_size, kSeed);
  bloom_filter->set[hash / 64] |= ((uint64_t)1 << (hash % 64));
}
bool bloom_check(struct BloomFilter *bloom_filter, Key key) {
  //   TODO: Check if key exists in set
  uint64_t const kSeed = 7741; // Random big prime number
  uint64_t hash = calc_hash(key, bloom_filter->set_size, kSeed);
  return (bloom_filter->set[hash / 64] & ((uint64_t)1 << (hash % 64))) ? true
                                                                       : false;
}
