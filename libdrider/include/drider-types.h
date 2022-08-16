#ifndef DRIDER_TYPES_H
#define DRIDER_TYPES_H

#define BUF_SIZE_8 8
#define BUF_SIZE_16 16
#define BUF_SIZE_32 32
#define BUF_SIZE_64 64
#define BUF_SIZE_128 128
#define BUF_SIZE_256 256
#define BUF_SIZE_512 512
#define BUF_SIZE_1K 1024
#define BUF_SIZE_2K 2048
#define BUF_SIZE_4K 4096
#define BUF_SIZE_8K 8192

namespace drider {
enum PAC_TYPE {
	SUBS = 0,
	REGS,
	UNSUB,
	UNREG,
	ERR,
	ACK,
};
} // namespace drider
#endif