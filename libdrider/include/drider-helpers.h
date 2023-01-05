#ifndef DRIDER_HELPERS_H
#define DRIDER_HELPERS_H
#include <log-abs.h>

#define ASSERT_NOT_REACHED()                                   \
	do {                                                   \
		if (1) {                                       \
			LOG_ERROR("{}", "Assertion is reached."); \
			exit(-1);                              \
		}                                              \
	} while (0)

#endif