/*
 * test_crypto.c
 *
 * Performs a simple encryption-decryption
 * of random data from /dev/urandom with the
 * use of the cryptodev device.
 *
 * Stefanos Gerangelos <sgerag@cslab.ece.ntua.gr>
 * Vangelis Koukis <vkoukis@cslab.ece.ntua.gr>
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <crypto/cryptodev.h>


#define DATA_SIZE       256
#define BLOCK_SIZE      16
#define KEY_SIZE	16  /* AES128 */


/* Insist until all of the data has been read */
ssize_t insist_read(int fd, void *buf, size_t cnt)
{
        ssize_t ret;
        size_t orig_cnt = cnt;

        while (cnt > 0) {
                ret = read(fd, buf, cnt);
                if (ret < 0)
                        return ret;
                buf += ret;
                cnt -= ret;
        }

        return orig_cnt;
}
struct session_op sess;
struct crypt_op cryp;
struct {
  unsigned char 	in[DATA_SIZE],
  encrypted[DATA_SIZE],
  decrypted[DATA_SIZE],
  iv[BLOCK_SIZE],
  key[KEY_SIZE];
} data;

static int fill_urandom_buf(unsigned char *buf, size_t cnt)
{
  int crypto_fd;
  int ret = -1;

  crypto_fd = open("/dev/urandom", O_RDONLY);
  if (crypto_fd < 0)
  return crypto_fd;

  ret = insist_read(crypto_fd, buf, cnt);
  close(crypto_fd);

  return ret;
}


static int init_crypto(void ){
  int i = -1;

  memset(&sess, 0, sizeof(sess));
  memset(&cryp, 0, sizeof(cryp));
  memset(data.iv,'\0',16);
  strcpy(data.iv,"123456789012345");


  memset(data.key,'\0',16);
  strcpy(data.key,"123456789012345");


}

static char* encrypt(int cfd, char* buf)
{
  memcpy(data.in, buf, length(buf));

	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
	sess.key = data.key;

	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return 1;
	}

	/*
	 * Encrypt data.in to data.encrypted
	 */
	cryp.ses = sess.ses;
	cryp.len = sizeof(data.in);
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;

	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}

  return data.encrypted;
	/*
	 * Decrypt data.encrypted to data.decrypted
	 */

static char* decrypt(int cfd, char* buf)
{
  memcpy(data.in, buf, length(buf));


	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}

	printf("\nDecrypted data:\n");
	for (i = 0; i < DATA_SIZE; i++) {
		printf("%x", data.decrypted[i]);
	}
	printf("\n");

	/* Verify the result */
	if (memcmp(data.in, data.decrypted, sizeof(data.in)) != 0) {
		fprintf(stderr, "\nFAIL: Decrypted and original data differ.\n");
		return 1;
	} else
		fprintf(stderr, "\nTest passed.\n");

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return 1;
	}

	return 0;
}

int main(void)
{
	int fd;

	fd = open("/dev/crypto", O_RDWR);
	if (fd < 0) {
		perror("open(/dev/crypto)");
		return 1;
	}

	if (test_crypto(fd) < 0) {
		return 1;
	}

	if (close(fd) < 0) {
		perror("close(fd)");
		return 1;
	}

	return 0;
}
