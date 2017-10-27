#ifndef CEPH_LIBRBD_CACHEIMAGECTX_H
#define CEPH_LIBRBD_CACHEIMAGECTX_H

#include "librbd/ImageCtx.h"
#include "librbd/AioCompletion.h"
#include <hdcs/libhdcs.hpp>

namespace librbd {
/*class C_CacheVolumeRead : public C_AioRequest {
public:
    C_CacheVolumeRead( CephContext *cct, AioCompletion *completion, char* data, uint64_t off, size_t len )
        : C_AioRequest(cct, completion), m_data(data), m_off(off), m_len(len){}
    virtual ~C_CacheVolumeRead() {}
    virtual void finish(int r);
  private:
    char* m_data;
    uint64_t m_off;
    size_t len;
}*/

struct CacheImageCtx : ImageCtx {
    std::string cached_image_name;
    libhdcs* cache_inst;

    CacheImageCtx(const std::string &image_name, const std::string &image_id,
            const char *snap, const std::string &real_image_name, IoCtx& p, bool read_only);
    void delete_CacheImageCtx();
    void aio_write( AioCompletion *c,
        uint64_t off, size_t len, const char *buf,
        int op_flags );
    void aio_read( AioCompletion *c,
        uint64_t off, size_t len, char *buf,
        int op_flags );


};
}


#endif
