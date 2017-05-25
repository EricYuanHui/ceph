// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "test/librbd/mock/cache/file/MockAioFile.h"
#include "test/librbd/test_mock_fixture.h"
#include "gtest/gtest.h"

namespace librbd {
namespace cache {
namespace file {

struct TestAioFile : public TestMockFixture {};

TEST_F(TestAioFile, AioOpenWrite) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  ASSERT_LE(0, t_mock_aio_file->test_open());
  bufferlist t_bl;
  t_bl.push_back('a');
  ASSERT_EQ(0, t_mock_aio_file->test_write(0, std::move(t_bl), false));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenSyncWrite) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  ASSERT_LE(0, t_mock_aio_file->test_open());
  bufferlist t_bl;
  t_bl.push_back('b');
  ASSERT_EQ(0, t_mock_aio_file->test_write(1024, std::move(t_bl), true));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenRead) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('b');
  t_mock_aio_file->test_open();
  ASSERT_EQ(4096, t_mock_aio_file->test_read(4096, 4096, &t_bl));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenReadNull) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('b');
  t_mock_aio_file->test_open();
  ASSERT_EQ(0, t_mock_aio_file->test_read(0, 0, &t_bl));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenTruncate) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('c');
  t_mock_aio_file->test_open();
  ASSERT_EQ(0, t_mock_aio_file->test_write(8192, t_bl, false));
  ASSERT_EQ(0, t_mock_aio_file->test_truncate(4096, false));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenSyncTruncate) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('d');
  t_mock_aio_file->test_open();
  ASSERT_EQ(0, t_mock_aio_file->test_write(8192, t_bl, false));
  ASSERT_EQ(0, t_mock_aio_file->test_truncate(1024, true));
  delete t_mock_aio_file;
}


TEST_F(TestAioFile, AioOpenFilesize) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  t_mock_aio_file->test_open();
  ASSERT_EQ(1024, t_mock_aio_file->test_filesize());
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenDiscard) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('e');
  t_mock_aio_file->test_open();
  t_mock_aio_file->test_write(2048, t_bl, false);
  ASSERT_EQ(0, t_mock_aio_file->test_discard(0, 1024, false));
  delete t_mock_aio_file;
}

TEST_F(TestAioFile, AioOpenSyncDiscard) {
  librbd::ImageCtx *ictx = nullptr;
  MockAioFile *t_mock_aio_file = new MockAioFile(*ictx, "test");
  bufferlist t_bl;
  t_bl.push_back('f');
  t_mock_aio_file->test_open();
  t_mock_aio_file->test_write(8192, t_bl, false);
  ASSERT_EQ(0, t_mock_aio_file->test_discard(0, 4096, true));
  delete t_mock_aio_file;
}

} // namespace file
} // namespace cache
} // namespace librbd
