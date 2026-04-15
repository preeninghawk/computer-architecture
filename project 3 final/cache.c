#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cache.h"

static int index_bits(int n){
  int cnt = 0;

  while(n) {
    cnt++;
    n = n >> 1;
  }

  return cnt-1;
}

/***************************************************************/
/*                                                             */
/* Procedure : build_cache                                     */
/*                                                             */
/* Purpose   : Initialize cache structure                      */
/*                                                             */
/***************************************************************/
void build_cache(struct l1cache** cache, int block_size, int n_ways, int capacity)
{
    *cache = (struct l1cache*)malloc(sizeof(struct l1cache));
    
    (*cache)->n_data_bits = index_bits(block_size);
    int n_sets = capacity / (n_ways * block_size);
    (*cache)->n_set_bits = index_bits(n_sets);
    (*cache)->n_ways = n_ways;
    
    (*cache)->sets = (struct cache_set*)malloc(n_sets * sizeof(struct cache_set));
    
    for (int i = 0; i < n_sets; i++) {
        (*cache)->sets[i].lines = (struct cache_line*)malloc(n_ways * sizeof(struct cache_line));
        
        for (int j = 0; j < n_ways; j++) {
            (*cache)->sets[i].lines[j].valid = false;
            (*cache)->sets[i].lines[j].dirty = false;
            (*cache)->sets[i].lines[j].tag = 0;
            (*cache)->sets[i].lines[j].age = 0;
        }
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : access_cache                                    */
/*                                                             */
/* Purpose   : Update cache stat and content                   */
/*                                                             */
/***************************************************************/
void access_cache(struct l1cache* cache, int op, uint32_t addr, int* hit, int* miss, int* wb)
{
    uint32_t block_offset = addr & ((1 << cache->n_data_bits) - 1);
    uint32_t set_index = (addr >> cache->n_data_bits) & ((1 << cache->n_set_bits) - 1);
    uint32_t tag = addr >> (cache->n_data_bits + cache->n_set_bits);
    
    struct cache_set* target_set = &(cache->sets[set_index]);
    int hit_way = -1;
    
    for (int way = 0; way < cache->n_ways; way++) {
        if (target_set->lines[way].valid && target_set->lines[way].tag == tag) {
            hit_way = way;
            break;
        }
    }
    
    if (hit_way != -1) {
        (*hit)++;
        
        for (int way = 0; way < cache->n_ways; way++) {
            if (target_set->lines[way].valid) {
                target_set->lines[way].age++;
            }
        }
        target_set->lines[hit_way].age = 0;
        
        if (op == 1) {
            target_set->lines[hit_way].dirty = true;
        }
    } else {
        (*miss)++;
        
        int replace_way = -1;
        uint32_t oldest_age = 0;
        
        for (int way = 0; way < cache->n_ways; way++) {
            if (!target_set->lines[way].valid) {
                replace_way = way;
                break;
            }
        }
        
        if (replace_way == -1) {
            for (int way = 0; way < cache->n_ways; way++) {
                if (target_set->lines[way].age >= oldest_age) {
                    oldest_age = target_set->lines[way].age;
                    replace_way = way;
                }
            }
            
            if (target_set->lines[replace_way].dirty) {
                (*wb)++;
            }
        }
        
        for (int way = 0; way < cache->n_ways; way++) {
            if (target_set->lines[way].valid) {
                target_set->lines[way].age++;
            }
        }
        
        target_set->lines[replace_way].valid = true;
        target_set->lines[replace_way].tag = tag;
        target_set->lines[replace_way].age = 0;
        target_set->lines[replace_way].dirty = (op == 1);
    }
}
