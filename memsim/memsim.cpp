
//memory allocation
#include "memsim.h"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <list>
#include <set>
#include <iterator>
#include <climits>

//given in doc and code
struct Partition {
  int tag;
  int64_t addr;
  int64_t size;
  //default tag is 0, for free
  Partition(int64_t addr, int64_t size, int tag = -1) {
    this->tag = tag;
    this->addr = addr;
    this->size = size;
  }
  //returns true if tag is not zero, false if 0
  bool is_occupied() {
    return this->tag != 0;
  }
};

//given in hints
typedef std::list<Partition>::iterator PartitionRef;

//given in hints
struct scmp {
  bool operator()(const PartitionRef & c1, const PartitionRef & c2) const {
    if (c1->size == c2->size)
      return c1->addr < c2->addr;
    else
      return c1->size > c2->size;
  }
};

//given in doc/hints. 
//Allocate and Deallocate follows the doc A6_hints by Anshdeep,week13,tutorials 
struct Simulator {
  //variables for structure
  int64_t page_size;
  int64_t page_requested;
  std::list<Partition> all_blocks;                                      
  std::set<PartitionRef, scmp> free_blocks;              
  std::unordered_map<int, std::list<PartitionRef>> tagged_blocks;  
  
  //constructor initialize 
  Simulator(int64_t page_size) {
    this->page_size = page_size;
    this->page_requested = 0;
    this->all_blocks = std::list<Partition>();
    this->tagged_blocks = std::unordered_map<int, std::list<PartitionRef>>();
  }

  //function to quickly find number of pages to add based on pages size
  int64_t pages_needed(int64_t size) {
    int64_t page_size = this->page_size;

    if (size % page_size == 0) {
      return size / page_size;
    } else {
      return size / page_size + 1;
    }
  }

  //allocateion
  void allocate(int tag, int64_t size) {
    // this is the initial case when we have no partitions at all
    // create a new partition with the size to be multiple of the page size
    // update page requested in results
    // push back this partition to the all_blocks and free_blocks
    if (all_blocks.empty()) {
      int64_t new_pages = this->pages_needed(size);
      Partition head = Partition(0, new_pages * this->page_size);
      this->page_requested += new_pages;
      this->all_blocks.push_back(head);
      this->free_blocks.insert(this->all_blocks.begin());
    }
    PartitionRef current;
    // this is the initial case when we have no partitions at all
    // create a new partition with the size to be multiple of the page size
    // update page requested in results
    // push back this partition to the all_blocks and free_blocks
    if (!this->free_blocks.empty()) { 
      PartitionRef first_free = *this->free_blocks.begin();
      //first free partition will fit, add it and return
      if (first_free->size >= size) {
      
        current = first_free;
        this->free_blocks.erase(this->free_blocks.begin());

        int64_t leftover_size = current->size - size;

        current->tag = tag;
        current->size = size;
        this->tagged_blocks[tag].push_back(current);
        //free block isnt all taken up, split and create new free
        if (leftover_size > 0) {
          Partition leftover = Partition(current->addr + size, leftover_size);

          this->all_blocks.insert(std::next(current), leftover);
          this->free_blocks.insert(std::next(current));
        }
        //nothing left to be done
        return;
      }
    }

    //free partition is not found
    PartitionRef last_part = std::prev(this->all_blocks.end());
    //last block is free
    if (!last_part->is_occupied()) {
      // load pages, create an empty partition where the size is of the order of page size
      //  and merge with the last partition
      // change the address and size of this partition based on the last partition
      int64_t last_part_size = last_part->size;
      //find number of new pages needed
      int64_t new_pages = this->pages_needed(size - last_part->size);
      last_part_size += new_pages * this->page_size;
      this->page_requested += new_pages;
      //erase
      if (this->free_blocks.find(last_part) != this->free_blocks.end()) {
        this->free_blocks.erase(this->free_blocks.find(last_part));
      }
      last_part->tag = tag;
      last_part->size = size;
      this->tagged_blocks[tag].push_back(last_part);
      last_part_size -= size;
      //create free if avalble space
      if (last_part_size > 0) {
        Partition new_last_part = Partition(last_part->addr + last_part->size, last_part_size);
        this->all_blocks.push_back(new_last_part);
        this->free_blocks.insert(std::prev(this->all_blocks.end()));
      }

    } 
    else {
      // simply add a new partition by loading from the memory 
      // size is of the order of page size
      //need to add new pages, find amount and create free partition
      int64_t new_pages = this->pages_needed(size);
      this->page_requested += new_pages;
      int64_t leftover_size = new_pages * this->page_size - size;

      Partition new_last_part = Partition(last_part->addr + last_part->size, size, tag);
      this->all_blocks.push_back(new_last_part);
      this->tagged_blocks[tag].push_back(std::prev(this->all_blocks.end()));
      //create free if avalibale space
      if (leftover_size > 0) {
        Partition leftover = Partition(last_part->addr + last_part->size + size, leftover_size);
        this->all_blocks.push_back(leftover);
        this->free_blocks.insert(std::prev(this->all_blocks.end()));
      }

    }
  }

  void deallocate(int tag) {
  
    std::list<PartitionRef> all_blocks = this->tagged_blocks[tag];
    //loop trough all tagged blocks
    for (PartitionRef partition : all_blocks) {
      //free the block
      partition->tag = 0;
       // find the previous of the current block
      // do a left merge
      // erase the previous of the current block from all blocks
      if (partition != this->all_blocks.begin()) {
        PartitionRef prev = std::prev(partition);

        if (!prev->is_occupied()) {
          partition->addr = prev->addr;
          partition->size += prev->size;
          this->all_blocks.erase(prev);
          if (this->free_blocks.find(prev) != this->free_blocks.end()) {
            this->free_blocks.erase(this->free_blocks.find(prev));
          }
        }
      }
      //// find the next of the current block
      // do a right merge
      // erase next of the current block from all blocks 
      if (partition != std::prev(this->all_blocks.end())) {
        PartitionRef next = std::next(partition);

        if (!next->is_occupied()) {
          partition->size += next->size;
          this->all_blocks.erase(next);

          if (this->free_blocks.find(next) != this->free_blocks.end()) {
            this->free_blocks.erase(this->free_blocks.find(next));
          }
        }
      }
      //insert current block to free
      this->free_blocks.insert(partition);
    }
    //clear blocks with tag
    this->tagged_blocks[tag].clear();
  }

  MemSimResult getStats() {
    MemSimResult result;
    //no free blocks
    if (this->free_blocks.empty()) {
      result.max_free_partition_size = 0;
      result.max_free_partition_address = 0;
    } 
    //free blocks, get first stored
    else {
      PartitionRef first_free = *this->free_blocks.begin();
      result.max_free_partition_size = first_free->size;
      result.max_free_partition_address = first_free->addr;
    }
    //return
    result.n_pages_requested = this->page_requested;
    return result;
  }

  void check_consistency() {
  }
};

//given in code
MemSimResult mem_sim(int64_t page_size, const std::vector<Request> & requests)
{
  // if you decide to use the simulator class above, you probably do not need
  // to modify below at all
  Simulator sim(page_size);
  for (const auto & req : requests) {
    if (req.tag < 0) {
      sim.deallocate(-req.tag);
    } else {
      sim.allocate(req.tag, req.size);
    }
    sim.check_consistency();
  }
  return sim.getStats();
}
