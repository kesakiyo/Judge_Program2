#include "compiler.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>

using namespace std;

Compiler::Compiler(const char* _lang, const char* _dir) {
    lang = 0;
    while(strcmp(_lang, languages[lang])) ++lang;
    strcpy(dir, _dir);
}

Compiler::~Compiler() {

}

int Compiler::get_lang() {
    return lang;
}

int Compiler::compile() {
    // change working directory.
    chdir(dir);

    const char * CP_C[] = {"gcc", "Main.c", "-o", "Main", "-O2","-Wall", "-lm", "--static", "-std=c99", NULL};
    const char * CP_CC[] = {"g++", "Main.cc", "-o", "Main","-O2", "-Wall", "-lm", "--static", NULL};
    const char * CP_CC11[] = {"g++-4.8", "Main.cc", "-o", "Main", "-O2", "-Wall", "-lm", "--static", "-std=c++11", NULL};
    pid_t pid = fork();

    // child process.
    // excute compile....
    if(pid == 0) {
        // rlimit is used for the restricting the resource
        struct rlimit LIM;

        // setting cpu timelimit
        LIM.rlim_max = 60;
        LIM.rlim_cur = 60;
        setrlimit(RLIMIT_CPU, &LIM);

        // setting file size
        LIM.rlim_max = 90 << 20;
        LIM.rlim_cur = 90 << 20;
        setrlimit(RLIMIT_FSIZE, &LIM);

        // setting virtual memory
        LIM.rlim_max = 1024 << 20;
        LIM.rlim_cur = 1024 << 20;
        setrlimit(RLIMIT_AS, &LIM);

        freopen("compile_error.txt", "a+", stderr);
        switch(lang) {
            case C: {
                execvp(CP_C[0], (char * const *)CP_C);
                break;
            }
            case CC: {
                execvp(CP_CC[0], (char * const *)CP_CC);
                break;
            }
            case CC11: {
                execvp(CP_CC11[0], (char * const *)CP_CC11);
                break;
            }
            break;
        }
        exit(0);
    }
    else {
        int status = 0;

        // parent process waiting child process with status value.
        waitpid(pid, &status, 0);

        // return status value.
        // if the compile is success that return value is zero.
        // Other cases return value isn't zero....
        return status;
    }
}