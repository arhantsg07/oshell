#include "history.h"

char *session_buffer;
size_t session_len = 0;

char *get_last_line(char *buffer)
{
    // Safety check for empty or NULL buffers
    if (buffer == NULL || *buffer == '\0') { return buffer; }

    int len = strlen(buffer);
    int i = len - 1;

    // 1. Ignore the absolute last character if it happens to be a newline.
    // (Otherwise, a trailing newline makes it look like the last line is empty)
    if (i > 0 && buffer[i] == '\n') { i--; }

    // 2. Walk backwards through the memory until we hit another newline
    while (i >= 0 && buffer[i] != '\n') { i--; }

    // 3. 'i' is now the index of the newline (or -1 if no newline was found).
    // The actual text of the last line starts exactly one byte after 'i'.
    return &buffer[i + 1];
}

/* 
 I need to store the history of commands as well for the shell
 I have used the following approach:
 first allocate space in primary memory and store the commands
 in that allocated space and then write it to disk when session exits
 and then free up the allocated space
*/

void flush_history_to_disk()
{
    if (session_buffer != NULL)
    {
        int fd = open(".osh_history", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd != -1)
        {
            write(fd, session_buffer, session_len);
            close(fd);
        }
        // free(session_buffer);                // this would continually free up the memory
        // printf("Session flushed to disk.\n");
    }
}

void copy_to_session_buf(char *temp, size_t len) {
    memcpy(session_buffer + session_len, temp, len);
    session_len += len;
    session_buffer[session_len++] = '\n';
    session_buffer[session_len] = '\0';
}

