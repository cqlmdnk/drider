#ifndef DRIDER_TYPES_H
#define DRIDER_TYPES_H
#define BUF_SIZE_16 16
#define BUF_SIZE_1K 1024
#define BUF_SIZE_4K 4096

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