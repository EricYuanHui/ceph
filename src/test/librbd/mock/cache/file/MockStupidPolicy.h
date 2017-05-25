// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_TEST_LIBRBD_MOCK_CACHE_FILE_STUPID_POLICY_H
#define CEPH_TEST_LIBRBD_MOCK_CACHE_FILE_STUPID_POLICY_H

#include "librbd/cache/file/Policy.h"
#include "librbd/cache/file/Types.h"
#include "librbd/ImageCtx.h"
#include "include/buffer.h"
#include "include/lru.h"
#include "gmock/gmock.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#define BLOCK_SIZE 4096

namespace librbd {

struct ImageCtx;

namespace cache {
namespace file {

using namespace stupid_policy;

class MockStupidPolicy : public Policy {
public:

  MockStupidPolicy(ImageCtx &m_image_ctx, uint64_t ssd_cache_size ):t_m_image_ctx(m_image_ctx), t_ssd_cache_size(ssd_cache_size){
    t_m_entries.resize(t_ssd_cache_size / BLOCK_SIZE);
    for (auto &entry : t_m_entries) {
      t_m_free_lru.insert_tail(&entry);
    }
  }

  void test_set_write_mode(uint8_t write_mode) {
    t_m_write_mode = write_mode;
  }

  uint8_t test_get_write_mode() {
    return t_m_write_mode;
  }

  void test_set_block_count(uint64_t block_count) {
    t_m_block_count = block_count;
  }

  int test_invalidate(uint64_t block) {
    auto entry_it = t_m_block_to_entries.find(block);
    if (entry_it == t_m_block_to_entries.end()) {
      return -1;
    }

    t_Entry *entry = entry_it->second;
    t_m_block_to_entries.erase(entry_it);

    LRUList *lru;
    if (entry->dirty) {
      lru = &t_m_dirty_lru;
    } else {
      lru = &t_m_clean_lru;
    }
    lru->remove(entry);

    t_m_free_lru.insert_tail(entry);
    return 0;
  }

  bool test_contains_dirty() const {
    return t_m_dirty_lru.get_tail() != nullptr;
  }

  bool test_is_dirty(uint64_t block) const {
    auto entry_it = t_m_block_to_entries.find(block);
    //assert(entry_it != t_m_block_to_entries.end());
    bool dirty = entry_it->second->dirty;
    return dirty;
  }

  int test_map(uint64_t block) {
    if (block >= t_m_block_count) {
      return -1;
    }
    t_Entry *entry;
    auto entry_it = t_m_block_to_entries.find(block);
    if (entry_it != t_m_block_to_entries.end()) {

      entry = entry_it->second;
      LRUList *lru;
      if (entry->dirty) {
        lru = &t_m_dirty_lru;
      } else {
        lru = &t_m_clean_lru;
      }

      lru->remove(entry);
      lru->insert_head(entry);
      return 0;
    }

    entry = reinterpret_cast<t_Entry*>(t_m_free_lru.get_head());
    if (entry != nullptr) {
      t_m_free_lru.remove(entry);

      entry->block = block;
      t_m_block_to_entries[block] = entry;
      t_m_clean_lru.insert_head(entry);
      return 0;
    }

    /*entry = reinterpret_cast<t_Entry*>(t_m_clean_lru.get_tail());
    if (entry != nullptr) {
      int r = m_block_guard.detain(entry->block, nullptr);
      if (r >= 0) {

        t_m_block_to_entries.erase(entry->block);
        t_m_clean_lru.remove(entry);

        entry->block = block;
        t_m_block_to_entries[block] = entry;
        t_m_clean_lru.insert_head(entry);
        return 0;
      }
    }*/
    return 0;
  }

  uint64_t test_bufferlist_to_entry(bufferlist &bl) {
    uint64_t entry_index = 0;
    Entry_t entry;
    for (bufferlist::iterator it = bl.begin(); it != bl.end(); ++it) {
      std::cout << "entry_index = " << entry_index << std::endl;
      entry.decode(it);
      std::cout << "---finish entry --"  << std::endl;
      auto entry_it = t_m_entries[entry_index++];
      entry_it.block = entry.block;
      entry_it.dirty = entry.dirty;
    }
    return entry_index;
  }

  MOCK_METHOD1(set_write_mode, void(uint8_t));

  MOCK_METHOD0(get_write_mode, uint8_t());

  MOCK_METHOD1(set_block_count, void(uint64_t));

  MOCK_METHOD1(invalidate, int(uint64_t));

  MOCK_METHOD1(set_dirty, void(uint64_t));

  MOCK_CONST_METHOD0(contains_dirty, bool());

  MOCK_CONST_METHOD1(is_dirty, bool(uint64_t));

  MOCK_METHOD1(clear_dirty, void(uint64_t));

  MOCK_METHOD1(get_writeback_block, int(uint64_t *));

  MOCK_METHOD5(map, int(IOType, uint64_t, bool, PolicyMapResult *, uint64_t *));

  MOCK_METHOD0(tick, void());

  MOCK_METHOD0(get_entry_size, int());

  MOCK_METHOD2(entry_to_bufferlist, void(uint64_t, bufferlist *));

  MOCK_METHOD1(bufferlist_to_entry, void(bufferlist &));

private:

  struct t_Entry : public LRUObject {
    uint64_t block = 0;
    bool dirty = false;
  };

  typedef std::vector<t_Entry> t_Entries;
  typedef std::unordered_map<uint64_t, t_Entry*> t_BlockToEntries;

  ImageCtx &t_m_image_ctx;

  t_Entries t_m_entries;
  t_BlockToEntries t_m_block_to_entries;

  uint64_t t_ssd_cache_size = 0;
  uint64_t t_m_block_count = 0;
  uint8_t t_m_write_mode = 0;

  LRUList t_m_free_lru;
  LRUList t_m_clean_lru;
  mutable LRUList t_m_dirty_lru;




};

} // namespace file
} // namespace cache
} // namespace librbd

#endif // CEPH_TEST_LIBRBD_MOCK_CACHE_FILE__H

