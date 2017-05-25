// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "test/librbd/mock/cache/file/MockAioFile.h"
#include "test/librbd/test_mock_fixture.h"
#include "gtest/gtest.h"

struct Context;

namespace librbd {
namespace cache {
namespace file {

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::InSequence;

struct TestMockMetaStore : public TestMockFixture {

  void expect_aio_file_open(MockAioFile &mock_aio_file) {
    EXPECT_CALL(mock_aio_file, open(_)).Times(AtLeast(1));
  }

  void expect_aio_file_close(MockAioFile &mock_aio_file) {
    EXPECT_CALL(mock_aio_file, close(_)).Times(AtLeast(1));
  }

  void expect_aio_file_read(MockAioFile &mock_aio_file, uint64_t offset, uint64_t length) {
    EXPECT_CALL(mock_aio_file, read(offset, length, _, _)).Times(AtLeast(1));
  }

  void expect_aio_file_write(MockAioFile &mock_aio_file, uint64_t offset, bool fdatasync) {
    EXPECT_CALL(mock_aio_file, write(offset, _, fdatasync, _)).Times(AtLeast(1));
  }

  void expect_aio_file_truncate(MockAioFile &mock_aio_file, uint64_t length, bool fdatasync) {
    EXPECT_CALL(mock_aio_file, truncate(length, fdatasync, _)).Times(AtLeast(1));
  }
};

TEST_F(TestMockMetaStore, MetaStoreInit) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile t_mock_aio_file(*ictx, "test");
  expect_aio_file_open(t_mock_aio_file);
  Context *t_on_finish = nullptr;
  t_mock_aio_file.mock_open(t_on_finish);
}

TEST_F(TestMockMetaStore, MetaStoreShutDown) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile t_mock_aio_file(*ictx, "test");
  expect_aio_file_close(t_mock_aio_file);
  Context *t_on_finish = nullptr;
  t_mock_aio_file.mock_close(t_on_finish);
}

TEST_F(TestMockMetaStore, MetaStoreReadBlock) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t t_offset = 0;
  uint64_t t_length = 1;
  Context *t_on_finish = nullptr;
  ceph::bufferlist *t_bl = nullptr;
  MockAioFile t_mock_aio_file(*ictx, "test");
  expect_aio_file_read(t_mock_aio_file, t_offset, t_length);
  t_mock_aio_file.mock_read(t_offset, t_length, t_bl, t_on_finish);
}

TEST_F(TestMockMetaStore, MetaStoreWriteBlock) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t t_offset = 0;
  bool t_fdatasync = false;
  Context *t_on_finish = nullptr;
  bufferlist t_bl;
  MockAioFile t_mock_aio_file(*ictx, "test");
  expect_aio_file_write(t_mock_aio_file, t_offset, t_fdatasync);
  t_mock_aio_file.mock_write(t_offset, std::move(t_bl), t_fdatasync, t_on_finish);
}

TEST_F(TestMockMetaStore, MetaStoreReset) {
  librbd::ImageCtx *ictx = nullptr;
  uint64_t t_length = 1;
  bool t_fdatasync = false;
  Context *t_on_finish = nullptr;
  MockAioFile t_mock_aio_file(*ictx, "test");
  expect_aio_file_truncate(t_mock_aio_file, t_length, t_fdatasync);
  t_mock_aio_file.mock_truncate(t_length, t_fdatasync, t_on_finish);
}

} // namespace file
} // namespace cache
} // namespace librbd
