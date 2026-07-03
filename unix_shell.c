#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "history.h"
#include "parser.h"

#define SIZE 4096

int main(void)
{
    char line[MAX_LINE];
    char *args[MAX_LINE / 2 + 1]; /* Max arguments */
    int should_run = 1;
    session_buffer = malloc(SIZE);
    if (session_buffer == NULL)
    {
        perror("malloc");
        return 1;
    }
    session_buffer[0] = '\0';

    while (should_run)
    {
        int status;
        pid_t done;
        while ((done = waitpid(-1, &status, WNOHANG)) > 0)
            printf("[background pid %d finished]\n", done);
        
        printf("osh> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
        {
            free(session_buffer);
            break;
        }

        char original_line[MAX_LINE];
        strncpy(original_line, line, sizeof(original_line));
        original_line[sizeof(original_line) - 1] = '\0';

        /* remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        /* skip empty lines */
        if (line[0] == '\0') continue;

        // when the !! command is entered
        if (strcmp(line, "!!") == 0)
        {
            char *last_cmd = get_last_line(session_buffer);

            if (last_cmd == NULL || *last_cmd == '\0')
            {
                printf("No previous command\n");
                continue;
            }

            char temp[MAX_LINE];

            
            strncpy(temp, last_cmd, sizeof(temp));

            /* 
                this line ensures that in case the src string
                is greater than sizeof(temp), strncpy wont null terminate, so this 
                line puts a null terminating character guaranteeing buffer always
                terminates somewhere.
            */
            temp[sizeof(temp) - 1] = '\0';

            /*  
                NOTE: the idx of the above line and the below line
                temp[sizeof(temp) - 1] and temp[tlen - 1] are not
                the same unless the command fills the complete buffer 
            */

            /*
                And this is for actually putting the \0 right after where the
                command ends
            */

            // this prevents execvp : file or command not found error

            size_t tlen = strlen(temp);
            if (tlen > 0 && temp[tlen - 1] == '\n') temp[tlen - 1] = '\0';

            int argc = 0;
            char *token = strtok(temp, " \t");
            str_to_tok(token, args, &argc);

            if (argc == 0) continue;

            size_t cmd_len = strlen(last_cmd);
            if (session_len + cmd_len + 1 < SIZE)
            {
                copy_to_session_buf(temp, cmd_len);

                // memcpy(session_buffer + session_len, temp, cmd_len);
                // session_len += cmd_len;
                // session_buffer[session_len++] = '\n';
                // session_buffer[session_len] = '\0';
                
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork");
                continue;
            }
            if (pid == 0)
            {
                execvp(args[0], args);
                perror("execvp");
                _exit(1);
            }

            int status;
            waitpid(pid, &status, 0);
            continue;
        }
        // ends

        /* built-in exit */
        if (strcmp(line, "exit") == 0)
        {
            free(session_buffer);
            break;
        }

        size_t cmd_len = strlen(line);
        if (session_len + cmd_len + 1 < SIZE)
        {
            copy_to_session_buf(original_line, cmd_len);
            // memcpy(session_buffer + session_len, original_line, cmd_len);
            // session_len += cmd_len;
            // session_buffer[session_len++] = '\n';
            // session_buffer[session_len] = '\0';
        }
        flush_history_to_disk();

        /* tokenize input into args[] */
        int argc = 0;
        // convert words of the complete command to tokens
        char *token = strtok(line, " \t");
        str_to_tok(token, args, &argc);
        int background = 0;

        if (argc == 0)
            continue;

        if (strcmp(args[0], "cd") == 0)
        {
            const char *path = args[1];
            if (path == NULL)
            {
                path = getenv("HOME");
            }

            if (path == NULL || chdir(path) != 0)
            {
                perror("cd");
            }

            continue;
        }

        /* append command to session history buffer */

        /* check for background execution (& as last arg) */
        if (argc > 0 && strcmp(args[argc - 1], "&") == 0)
        {
            background = 1;
            args[argc - 1] = NULL;
            argc--;
            if (argc == 0)
                continue; /* just '&' — ignore */
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            continue;
        }

        if (pid == 0)
        {
            /* child */
            execvp(args[0], args);
            perror("execvp");
            _exit(1);
        }
        else
        {
            /* parent */
            if (!background)
            {
                int status;
                waitpid(pid, &status, 0);
            }
            else
            {
                printf("[background pid %d]\n", pid);
            }
        }
    }

    return 0;
}