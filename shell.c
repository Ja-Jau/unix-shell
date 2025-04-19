/*
Made as test and introduction to Linux and shell programming with C

Compile:
gcc shell.c ./lib/linenoise.c -o shell
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "./lib/linenoise.h"


#define PROMPT "$ "
#define HISTORY_LENGTH 1024
#define MAX_ARGS 1024
#define TOKEN_SEP " \t"
#define PATH_MAX 4096

char CWD[PATH_MAX]; // Cur. working directory


int s_read(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, TOKEN_SEP);
    while (token != NULL && i < (MAX_ARGS - 1)) {
        args[i++] = token;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL; // Null-terminate array of arguments
    return i; // Return number of arguments parsed
}

int s_execute(char *cmd, char **cmd_args) {
    if (strcmp(cmd, "calc") == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process (using fork): Executes program
            execl("/mnt/c/TEST/Shell/tools/calc", "./calc", NULL);
            perror("!execl failed");
            exit(1);
        } else if (pid < 0) {
            perror("!fork failed");
            return -1;
        }
        // Parent process: Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
    
    fprintf(stdout, "Executing command: `%s`!\n", cmd);

    int status;
    // Creates child process
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return -1;
    }
    if (pid == 0) {
        // Child process
        execvp(cmd, cmd_args);
        perror("execvp failed");
        exit(1);
    } else {
        // Parent process
        if (waitpid(pid, &status, 0) != pid) {
            fprintf(stderr, "Waitpid failed");
            return -1;
        }
    }

    return status; // Returns status of child process
}

// Builtin cmds
typedef enum Builtin {
    CD,
    PWD,
    INVALID
}   Builtin;

void builtin_impl_cd(char **args, size_t n_args);
void builtin_impl_pwd(char **args, size_t n_args);

void (*BUILTIN_TABLE[]) (char **args, size_t n_args) = {
    [CD] = builtin_impl_cd,
    [PWD] = builtin_impl_pwd,
};

Builtin builtin_code(char *cmd) {
    if (!strncmp(cmd, "cd", 2)) {
        return CD;
    } else if (!strncmp(cmd, "pwd", 3)) {
        return PWD;
    } else {
        return INVALID;
    }
}

int is_builtin(char *cmd) {
    return builtin_code(cmd) != INVALID;
}

// cmd -> builtin
void s_execute_builtin(char *cmd, char **args, size_t n_args) {
    BUILTIN_TABLE[builtin_code(cmd)](args, n_args);
}

void refresh_cwd(void) {
    if (getcwd(CWD, sizeof(CWD)) == NULL) {
        fprintf(stderr, "ERROR: couldn't get working dir");
        exit(1);
    }
}

void builtin_impl_cd(char **args, size_t n_args) {
    if (n_args == 0 || args[0] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "ERROR: HOME env. var. not set\n");
            return;
        }
        if (chdir(home) != 0) {
            fprintf(stderr, "ERROR: couldn't change dir to %s\n", home);
            return;
        }
    } else {
        char *new_dir = args[0];
        if (chdir(new_dir) != 0) {
            fprintf(stderr, "ERROR: couldn't change dir to %s\n", new_dir);
            return;
        }
    }
    refresh_cwd(); // update cur. working directory
}

void builtin_impl_pwd(char **args, size_t n_args) {
    fprintf(stdout, "%s\n", CWD);
}


int main(void) 
{
    refresh_cwd();

    if (!linenoiseHistorySetMaxLen(HISTORY_LENGTH)) {
        fprintf(stderr, "Failed to set history length\n");
        exit(1);
    }

    char *line;
    char *args[MAX_ARGS];

    // REPL
    while((line = linenoise(PROMPT)) != NULL) {

        //READ
        int args_read = s_read(line, args);
        
        fprintf(stdout, "Read %d args\n", args_read);
        for (int i = 0; i < args_read; i++) {
            fprintf(stdout, "arg[%d] = %s\n", i, args[i]);
        }
        if (args_read == 0) {
            linenoiseFree(line);
            continue; // Skip empty lines
        }

        char *cmd = args[0];
        char **cmd_args = args;

        if (is_builtin(cmd)) {
            s_execute_builtin(cmd, (cmd_args+1), args_read-1);
        } else {
            s_execute(cmd, cmd_args);
        }
        
        

        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
    
    return 0;
}