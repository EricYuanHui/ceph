// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "test/librbd/mock/cache/file/MockStupidPolicy.h"
#include "test/librbd/test_mock_fixture.h"
#include "gtest/gtest.h"
#include <iostream>

namespace librbd {
namespace cache {
namespace file {

struct TestStupidPolicy : public TestMockFixture {};

TEST_F(TestStupidPolicy, WriteModeSet) {

  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 8192;
  uint8_t write_mode = 1;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  t_mock_stupid_policy->test_set_write_mode(write_mode);
  ASSERT_EQ(1, t_mock_stupid_policy->test_get_write_mode());
  write_mode = 0;
  t_mock_stupid_policy->test_set_write_mode(write_mode);
  ASSERT_EQ(0, t_mock_stupid_policy->test_get_write_mode());
  delete t_mock_stupid_policy;
}

TEST_F(TestStupidPolicy, InvalidateBlock) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 8192;
  uint64_t block = 1;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  ASSERT_EQ(-1, t_mock_stupid_policy->test_invalidate(block));
  delete t_mock_stupid_policy;
}

TEST_F(TestStupidPolicy, BufferlistToEntry) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 40960;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  bufferlist t_bl;
  t_bl.push_back('a');
  t_bl.push_back('b');
  t_bl.push_back('c');
  uint64_t block_count = 20;
  t_mock_stupid_policy->test_set_block_count(block_count);
  ASSERT_EQ(3, t_mock_stupid_policy->test_bufferlist_to_entry(t_bl));
  delete t_mock_stupid_policy;
}

TEST_F(TestStupidPolicy, MapBlockCount) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 40960;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  uint64_t block_count = 20;
  uint64_t block = 100;
  t_mock_stupid_policy->test_set_block_count(block_count);
  ASSERT_EQ(-1, t_mock_stupid_policy->test_map(block));
  delete t_mock_stupid_policy;
}

TEST_F(TestStupidPolicy, ContainsDirty) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 40960;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  ASSERT_FALSE(t_mock_stupid_policy->test_contains_dirty());
  delete t_mock_stupid_policy;
}

TEST_F(TestStupidPolicy, IsDirty) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t ssd_cache_size = 40960;
  uint64_t block = 1;
  MockStupidPolicy *t_mock_stupid_policy = new MockStupidPolicy(*ictx, ssd_cache_size);
  ASSERT_FALSE(t_mock_stupid_policy->test_is_dirty(block));
  delete t_mock_stupid_policy;
}

} // namespace file
} // namespace cache
} // namespace librbd
