/*
 * Virtio Cryptodev Device
 *

 *
 * Implementation of virtio-cryptodev qemu backend device.
 *
 * Dimitris Siakavaras <jimsiak@cslab.ece.ntua.gr>
 * Stefanos Gerangelos <sgerag@cslab.ece.ntua.gr>
 * Konstantinos Papazafeiropoulos <kpapazaf@cslab.ece.ntua.gr>
 *
 */
 
#include "qemu/osdep.h"
#include "qemu/iov.h"
#include "hw/qdev.h"
#include "hw/virtio/virtio.h"
#include "standard-headers/linux/virtio_ids.h"
#include "hw/virtio/virtio-cryptodev.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>
 
#define uu "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x "
#define unpack(str) str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8], str[9], str[10], str[11], str[12], str[13], str[14], str[15]
 
static uint64_t get_features(VirtIODevice *vdev, uint64_t features,
                             Error **errp)
{
    DEBUG_IN();
    return features;
}
 
static void get_config(VirtIODevice *vdev, uint8_t *config_data)
{
    DEBUG_IN();
}
 
static void set_config(VirtIODevice *vdev, const uint8_t *config_data)
{
    DEBUG_IN();
}
 
static void set_status(VirtIODevice *vdev, uint8_t status)
{
    DEBUG_IN();
}
 
static void vser_reset(VirtIODevice *vdev)
{
    DEBUG_IN();
}
 
static void vq_handle_output(VirtIODevice *vdev, VirtQueue *vq)
{
    VirtQueueElement *elem;
    unsigned int *syscall_type;
    int *host_fd;
 
    // DEBUG_IN();
 
    elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
    if (!elem) {
        DEBUG("No item to pop from VQ :(");
        return;
    }
 
    // DEBUG("I have got an item from VQ :)");
    syscall_type = elem->out_sg[0].iov_base;
 
    //EDITED
    if (*syscall_type == VIRTIO_CRYPTODEV_SYSCALL_TYPE_OPEN) {
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_OPEN");
        /* ?? */
        //EDITED
        host_fd = elem->in_sg[0].iov_base;
        *host_fd = open("/dev/crypto", O_RDWR);
 
    } else if (*syscall_type == VIRTIO_CRYPTODEV_SYSCALL_TYPE_CLOSE) {
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_CLOSE");
        /* ?? */
        //EDITED
 
        host_fd = (int*)elem->out_sg[1].iov_base;
        close(*host_fd);
 
    } else if (*syscall_type == VIRTIO_CRYPTODEV_SYSCALL_TYPE_IOCTL) {
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_IOCTL");
        /* ?? */
        //EDITED
 
        unsigned int *ioctl_cmd = elem->out_sg[2].iov_base;
        host_fd = (int*)elem->out_sg[1].iov_base;
 
        if (*ioctl_cmd == CIOCGSESSION) {
            struct session_op session;
 
            int *host_return_val = (int*)elem->in_sg[1].iov_base;
            __u32 *session_ses = (__u32*)elem->in_sg[0].iov_base;
 
            memset(&session, 0, sizeof(session));
 
            // typecasting is necessary in order to properly copy
            session.key = (__u8*)elem->out_sg[3].iov_base;
            session.keylen = *((__u32*)elem->out_sg[4].iov_base);
            session.cipher = CRYPTO_AES_CBC;
 
            *host_return_val = ioctl(*host_fd, *ioctl_cmd, &session);
            *session_ses = session.ses;
            printf("CIOCGSESSION:\nReturn Value: %d\nSession id: %d\n", *host_return_val, *session_ses);
 
        } else if (*ioctl_cmd == CIOCFSESSION) {
            int *host_return_val = (int*)elem->in_sg[0].iov_base;
 
            // typecasting is necessary in order to properly copy
            __u32 session_ses = *((__u32*)elem->out_sg[3].iov_base);
 
            *host_return_val = ioctl(*host_fd, *ioctl_cmd, &session_ses);
 
        } else if (*ioctl_cmd == CIOCCRYPT) {
            struct crypt_op cryp;
 
            int *host_return_val = (int*)elem->in_sg[1].iov_base;
 
            memset(&cryp, 0, sizeof(cryp));
 
            // typecasting is necessary in order to properly copy
            cryp.ses = *((__u32*)elem->out_sg[3].iov_base);
            cryp.op = *((__u16*)elem->out_sg[4].iov_base);
            cryp.len = *((__u32*)elem->out_sg[5].iov_base);
            cryp.src = (__u8*)elem->out_sg[6].iov_base;
            cryp.iv = (__u8*)elem->out_sg[7].iov_base;
            cryp.dst = elem->in_sg[0].iov_base;
 
            *host_return_val = ioctl(*host_fd, *ioctl_cmd, &cryp);
 
            printf("CIOCCRYPT (after):\nreturn value: %d\nop: %d\nlen: %d\nraw src: " uu "\nsrc: " uu "\ndst: " uu "\nin_sg[0]: "uu"\n\n-----------------\n\n\n",
                *host_return_val, cryp.op, cryp.len, unpack(((unsigned char*)elem->out_sg[6].iov_base)), unpack(cryp.src), unpack(cryp.dst), unpack(((unsigned char*)elem->in_sg[0].iov_base)));
 
        } else {
            DEBUG("UNKNOWN IOCTL CMD");
 
        }
 
    } else {
        DEBUG("UNKNOWN SYSCALL TYPE");
    }
    // DEBUG("FINISHING");
    virtqueue_push(vq, elem, 0);
    virtio_notify(vdev, vq);
    g_free(elem);
}
 
static void virtio_cryptodev_realize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
 
    DEBUG_IN();
 
    virtio_init(vdev, "virtio-cryptodev", VIRTIO_ID_CRYPTODEV, 0);
    virtio_add_queue(vdev, 128, vq_handle_output);
}
 
static void virtio_cryptodev_unrealize(DeviceState *dev, Error **errp)
{
    DEBUG_IN();
}
 
static Property virtio_cryptodev_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};
 
static void virtio_cryptodev_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *k = VIRTIO_DEVICE_CLASS(klass);
 
    DEBUG_IN();
    dc->props = virtio_cryptodev_properties;
    set_bit(DEVICE_CATEGORY_INPUT, dc->categories);
 
    k->realize = virtio_cryptodev_realize;
    k->unrealize = virtio_cryptodev_unrealize;
    k->get_features = get_features;
    k->get_config = get_config;
    k->set_config = set_config;
    k->set_status = set_status;
    k->reset = vser_reset;
}
 
static const TypeInfo virtio_cryptodev_info = {
    .name          = TYPE_VIRTIO_CRYPTODEV,
    .parent        = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VirtCryptodev),
    .class_init    = virtio_cryptodev_class_init,
};
 
static void virtio_cryptodev_register_types(void)
{
    type_register_static(&virtio_cryptodev_info);
}
 
type_init(virtio_cryptodev_register_types)
