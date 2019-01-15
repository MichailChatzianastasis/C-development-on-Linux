/*
 * Virtio Cryptodev Device
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

    DEBUG_IN();

    elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
    if (!elem) {
        DEBUG("No item to pop from VQ :(");
        return;
    } 

    DEBUG("I have got an item from VQ :)");

    syscall_type = elem->out_sg[0].iov_base;
    switch (*syscall_type) {
    case VIRTIO_CRYPTODEV_SYSCALL_TYPE_OPEN:
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_OPEN");
        int *host_fd = elem->in_sg[0].iov_base;
        *host_fd = open("/dev/crypto",O_RDWR);
        break;

    case VIRTIO_CRYPTODEV_SYSCALL_TYPE_CLOSE:
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_CLOSE");
        /* ?? */
        int *host_fd = elem->out_sg[1].iov_base;
        int x;
        if(x = close(*host_fd)<0){
            perror("close");
            return;
        }
        *elem->in_sg[0] = x;
        break;

    case VIRTIO_CRYPTODEV_SYSCALL_TYPE_IOCTL:
        DEBUG("VIRTIO_CRYPTODEV_SYSCALL_TYPE_IOCTL");
        /* ?? */
        int *host_fd = elem->out_sg[1].iov_base;
        unsigned int *cmd = elem->out_sg[2].iov_base;
        printf("We say: %s\n", input_msg);
        // break;
        switch(*cmd) {
        case CIOCGSESSION:
          {  unsigned char *sesskey = elem->out_sg[3].iov_base;
            unsigned char *output_msg = elem->out_sg[4].iov_base;
            printf("Guest says: %s\n", output_msg);
            unsigned char *input_msg = elem->in_sg[0].iov_base;
            memcpy(input_msg, "Host: Welcome to the virtio World!", 35);

            struct session_op *session_sg = elem->in_sg[1].iov_base;
            int *ret_sg = elem->in_sg[2].iov_base;
            session_sg->key = sesskey;
            if(ioctl(*host_fd,CIOCGSESSION,session_sg)){
                *ret_sg = -1;
                perror("CIOCGSESSION ERROR");
            }
            else { *ret = 0;}
            break;
        }
        case CIOCFSESSION: 
    {           unsigned char *output_msg = elem->out_sg[3].iov_base;
            printf("Guest says: %s\n", output_msg);
        
            int *sess_id_sg = elem->out_sg[4].iov_base;
            unsigned char *input_msg = elem->in_sg[0].iov_base;
            memcpy(input_msg, "Host: Welcome to the virtio World!", 35);

            int *ret_sg = elem->in_sg[1].iov_base;
            if (ioctl(*host_fd, CIOCFSESSION, sess_id_sg))
            {
                *ret_sg = -1;
                perror("CIOCFSESSION ERROR");
            }
            else
            {
                *ret = 0;
            }
            break;
    }
        case CIOCCRYPT:
     {       struct crypt_op *crypt = elem->out_sg[3].iov_base;
            unsigned char *output_msg = elem->out_sg[4].iov_base;
            printf("Guest says: %s\n", output_msg);
            unsigned char *src = elem->out_sg[5].iov_base;
            unsigned char *iv = elem->out_sg[6].iov_base;
            int *ret_sg = elem->in_sg[0].iov_base;
            unsigned char *dst = elem->in_sg[1].iov_base;
            unsigned char *input_msg = elem->in_sg[2].iov_base;
            memcpy(input_msg, "Host: Welcome to the virtio World!", 35);

            crypt->src = src;
            crypt->iv = iv;
            crypt->dst = dst;
            if (ioctl(*host_fd, CIOCCRYPT, crypt))
            {
                *ret_sg = -1;
                perror("CIOCCRYPT ERROR");
            }
            else
            {
                *ret = 0;
            }
            break;
             } 
                    default:
            DEBUG("Unknown syscall_type");
            break;
    }
}

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
