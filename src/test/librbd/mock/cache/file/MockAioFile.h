// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_TEST_LIBRBD_MOCK_CACHE_FILE_AIO_FILE_H
#define CEPH_TEST_LIBRBD_MOCK_CACHE_FILE_AIO_FILE_H

#include "librbd/cache/file/AioFile.h"
#include "librbd/ImageCtx.h"
#include "include/Context.h"
#include "include/buffer.h"
#include "gmock/gmock.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <errno.h>

struct Context;

namespace librbd {

struct ImageCtx;

namespace cache {
namespace file {

class MockAioFile {
public:

  MockAioFile(ImageCtx &image_ctx, const std::string &name):t_m_image_ctx(image_ctx) {
    t_m_name = "/tmp/rbd_cache." + name;
  }

  ~MockAioFile() {
    if (t_m_fd != -1) {
      ::close(t_m_fd);
    }
  }

  int test_open() {
    while (true) {
      t_m_fd = ::open(t_m_name.c_str(), O_CREAT | O_NOATIME | O_RDWR,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
      if (t_m_fd == -1) {
        int r = -errno;
        if (r == -EINTR) {
          continue;
        }
        break;
      }
      break;
    }
    return t_m_fd;
  }

  int test_write(uint64_t offset, const ceph::bufferlist &bl, bool sync) {
    int r = bl.write_fd(t_m_fd, offset);
    if (r < 0) {
      return r;
    }

    if (sync) {
      r = test_fdatasync();
      return r;
    }
    return r;
  }

  int test_read(uint64_t offset, uint64_t length, ceph::bufferlist *bl) {
    bufferptr bp = buffer::create(length);
    bl->push_back(bp);
    int r = 0;
    char *buffer = reinterpret_cast<char *>(bp.c_str());
    uint64_t count = 0;
    while (count < length) {
      ssize_t ret_val = pread64(t_m_fd, buffer, length - count, offset + count);
      if (ret_val == 0) {
        break;
      } else if (ret_val < 0) {
        r = -errno;;
        if (r == -EINTR) {
          continue;
        }
        return r;
      }
      count += ret_val;
      buffer += ret_val;
    }
    return count;
  }

  int test_discard(uint64_t offset, uint64_t length, bool sync) {
    int r;
    while (true) {
#if !defined(DARWIN) && !defined(__FreeBSD__)
# ifdef CEPH_HAVE_FALLOCATE
#  ifdef FALLOC_FL_KEEP_SIZE
      r = fallocate(t_m_fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
                    offset, length);
      if (r == -1) {
        r = -errno;
        if (r == -EINTR) {
          continue;
        }
        return r;
      }
      goto out;
#  endif
# endif
#endif
    {
      bufferlist bl;
      bl.append_zero(length);
      r = ::lseek64(t_m_fd, offset, SEEK_SET);
      if (r < 0) {
        r = -errno;
        goto out;
      }
      r = bl.write_fd(t_m_fd);
    }

    }

    if (sync) {
      r = test_fdatasync();
    }
  out:
    return r;

  }

  int test_truncate(uint64_t length, bool sync) {
    int r;
    while (true) {
      r = ftruncate(t_m_fd, length);
      if (r == -1) {
        r = -errno;
        if (r == -EINTR) {
          continue;
        }
        return r;
      }
      break;
    }

    if (sync) {
      r = test_fdatasync();
    }
    return r;
  }

  int test_fdatasync() {
    int r = ::fdatasync(t_m_fd);
    if (r == -1) {
      r = -errno;
      return r;
    }
    return 0;
  }

  uint64_t test_filesize() {
    struct stat file_st;
    memset(&file_st, 0, sizeof(file_st));
    fstat(t_m_fd, &file_st);
    return file_st.st_size;
  }

  MOCK_METHOD1(open, void(Context *));
  void mock_open(Context *on_finish) {
    open(on_finish);
  }

  MOCK_METHOD1(close, void(Context *));
  void mock_close(Context *on_finish) {
    close(on_finish);
  }

  MOCK_METHOD4(read, void(uint64_t, uint64_t, ceph::bufferlist *, Context *));
  void mock_read(uint64_t offset, uint64_t length, ceph::bufferlist *bl, Context *on_finish) {
    read(offset, length, bl, on_finish);
  }

  MOCK_METHOD4(write, void(uint64_t, const ceph::bufferlist &, bool, Context *));
  void mock_write(uint64_t offset, const ceph::bufferlist &bl, bool fdatasync, Context *on_finish) {
    write(offset, bl, fdatasync, on_finish);
  }

  MOCK_METHOD4(discard, void(uint64_t, uint64_t, bool, Context *));
  void mock_discard(uint64_t offset, uint64_t length, bool fdatasync, Context *on_finish) {
    discard(offset, length, fdatasync, on_finish);
  }

  MOCK_METHOD3(truncate, void(uint64_t, bool, Context *));
  void mock_truncate(uint64_t length, bool fdatasync, Context *on_finish) {
    truncate(length, fdatasync, on_finish);
  }

  MOCK_METHOD1(fsync, void(Context *));
  void mock_fsync(Context *on_finish) {
    fsync(on_finish);
  }

  MOCK_METHOD1(fdatasync, void(Context *));
  void mock_fdatasync(Context *on_finish) {
    fdatasync(on_finish);
  }

  MOCK_METHOD0(filesize, uint64_t());
  void mock_filesize() {
    filesize();
  }

private:
  ImageCtx &t_m_image_ctx;
  std::string t_m_name;
  int t_m_fd = -1;
};
} // namespace file
} // namespace cache
} // namespace librbd

#endif // CEPH_TEST_LIBRBD_MOCK_CACHE_FILE_AIO_FILE_H


