#ifndef HISTORY_H
#define HISTORY_H
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

extern char *session_buffer;
extern size_t session_len;

void history_init(void);

void copy_to_session_buf(char *temp, size_t len);
char *get_last_line(char *buffer);
void flush_history_to_disk(void);

void history_free(void);

#endif