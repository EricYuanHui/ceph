#include "librbd/AioCompletion.h"
#include "librbd/ImageState.h"
#include "librbd/CacheImageCtx.h"

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::CacheImageCtx: "

namespace librbd {

struct hdcs_io_unit{
    Context* onfinish;
    hdcs_completion_t comp;
    hdcs_io_unit( Context* onfinish ):onfinish(onfinish){}
    ~hdcs_io_unit(){
    }
};

static void _finish_hdcs_aio(int r, void *data){
    hdcs_io_unit* io_u = (hdcs_io_unit*) data;
    io_u->onfinish->complete(r);
    hdcs_aio_release( io_u->comp );
    delete io_u;
}

CacheImageCtx::CacheImageCtx(const std::string &image_name,
        const std::string &image_id, const char *snap,
        const std::string &real_image_name, IoCtx& p,
        bool read_only):ImageCtx(image_name, "", "", p, read_only){
    this->cached_image_name = real_image_name;
    this->is_cache_volume = true;
    this->cache_inst = new libhdcs(real_image_name.c_str());
    ldout(cct, 1) << "Open CacheImage:" << image_name << dendl;
    //this->cached_image_snap_name = snap;
}

void CacheImageCtx::delete_CacheImageCtx(){
    delete cache_inst;
}

void CacheImageCtx::aio_write( AioCompletion *c,
        uint64_t off, size_t len, const char *buf,
        int op_flags ){
    ldout(cct, 1) << "do aio_write off:" << off << " len:" << len << dendl;
    c->init_time(this, librbd::AIO_TYPE_WRITE);
    c->start_op();
    c->get();
    c->set_request_count(1);
    C_AioRequest* onfinish = new C_AioRequest( c );
    hdcs_io_unit *io_u = new hdcs_io_unit( onfinish );
    hdcs_aio_create_completion( (void*)io_u, _finish_hdcs_aio, &(io_u->comp) );
    cache_inst->hdcs_aio_write(cached_image_name.c_str(), buf, off, len, io_u->comp);
    c->put();
    return;
}

void CacheImageCtx::aio_read( AioCompletion *c,
        uint64_t off, size_t len, char *buf,
        int op_flags ){
    ldout(cct, 1) << "do aio_read off:" << off << " len:" << len << dendl;
    c->init_time(this, librbd::AIO_TYPE_READ);
    c->start_op();
    c->get();
    c->set_request_count(1);
    C_AioRequest* onfinish = new C_AioRequest( c );
    //todo: add check for backend,
    //if not hyperstash, then use default aio_write
    hdcs_io_unit *io_u = new hdcs_io_unit( onfinish );
    hdcs_aio_create_completion( (void*)io_u, _finish_hdcs_aio, &(io_u->comp) );
    cache_inst->hdcs_aio_read(cached_image_name.c_str(), buf, off, len, io_u->comp);
    c->put();
    return;
}
}

