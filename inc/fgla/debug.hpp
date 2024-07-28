#pragma once

extern unsigned int nMessages;

#ifndef NDEBUG
#include <cstdio>

#define FGLA_DB_PRINTF(...) do { printf("%05u: FGLA -- INFO: ", ++nMessages);printf(__VA_ARGS__); } while (0)
#define FGLA_WN_PRINTF(...) do { printf("%05u: FGLA -- WARN: ", ++nMessages);printf(__VA_ARGS__); } while (0)
#define FGLA_ER_PRINTF(...) do { printf("%05u: FGLA -- ERROR: ", ++nMessages);printf(__VA_ARGS__); } while (0)

#else

#define FGLA_DB_PRINTF(...) do { } while (0)
#define FGLA_WN_PRINTF(...) do { } while (0)
#define FGLA_ER_PRINTF(...) do { } while (0)

#endif