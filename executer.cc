#include "executer.h"
#include "compiler.h"
#include "utility.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/ptrace.h>

Executer::Executer(int _problem_id, int _lang, int _special, int _time_limit, int _memory_limit, const char* _dir, const char* _judge_input, const char* _user_output) {
	problem_id = _problem_id;
	lang = _lang;
	special = _special;
	time_limit = _time_limit;
	memory_limit = _memory_limit;
    strcpy(dir, _dir);
    strcpy(judge_input, _judge_input);
    strcpy(user_output, _user_output);

    result = -1;
    max_time = 0;
    max_memory = 0;
}

Executer::~Executer() {

}

bool Executer::isSplecial() {
    return special;
}

int Executer::get_result() {
    return result;
}

int Executer::get_max_time() {
    return max_time;
}

int Executer::get_max_memory() {
    return max_memory;
}

int Executer::running() {
    // change working directory
    chdir(dir);

    // redirection input, ouput, error file...
    freopen(judge_input, "r", stdin);
    freopen(user_output, "w+", stdout);
    freopen("runtime_error.out", "a+", stderr);

    // set the limit.
    struct rlimit LIM;

    // set time limit.
    // give time limit one second extra.
    // because max_time isn't exactly.
    LIM.rlim_cur = time_limit + 1;
    LIM.rlim_max = time_limit + 1;
    setrlimit(RLIMIT_CPU, &LIM);

    // set file limit.
    LIM.rlim_cur = 64 << 20;
    LIM.rlim_max = 64 << 20;
    setrlimit(RLIMIT_FSIZE, &LIM);

    // set stack limit.
    LIM.rlim_cur = 64 << 20;
    LIM.rlim_max = 64 << 20;
    setrlimit(RLIMIT_STACK, &LIM);


    // set momey limit.
    LIM.rlim_cur = (memory_limit << 20) / 2 * 3;
    LIM.rlim_max = (memory_limit << 20) * 2;
    setrlimit(RLIMIT_AS, &LIM);

    // trace me
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    switch(lang) {
        case C: case CC: case CC11: {
            execl("./Main", "./Main", (char *)NULL);
            break;
        }
    }

    exit(0);
}

int Executer::watching(const pid_t& pid) {
    // check the child process
    Utility util;
    int ret, status, sig, exit_code;
    struct rusage ruse;

    while(1) {
        // wait awhile for getting resource of child process
        wait4(pid, &status, 0, &ruse);

        // getting the memory that is used by child process
        int cur_memory = util.get_proc_status(pid, "VmPeak:") << 10;

        // if child memory is bigger than max memory, update max memory.
        if(cur_memory > max_memory) max_memory = cur_memory;

        // if max memory is bigger than memory limit, return value is updated to time limit flag.
        if(max_memory > (memory_limit << 20)) {
            ret = ML;

            // before exit while, kill child process
            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }

        // if child process was terminated for normal
        if(WIFEXITED(status)) break;

        //  get child process status.
        exit_code = WEXITSTATUS(status);

        // if child process was terminated for unusal
        if(exit_code != 0x05 && exit_code != 0x00) {

            // print runtime error message
            util.print_runtime_error(strsignal(exit_code));

            // update return value.
            switch(exit_code) {
                case SIGKILL: case SIGXCPU: {
                    ret = TL;
                    break;
                }
                case SIGXFSZ: {
                    ret = OL;
                    break;
                }
                default: {
                    ret = RE;
                    break;
                }
            }

            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }

        // if child process was terminated by siganl
        if(WIFSIGNALED(status)) {
            // getting signal from status.
            sig = WTERMSIG(status);

            // print runtime error message
            util.print_runtime_error(strsignal(sig));

            // update return value.
            switch(sig) {
                case SIGKILL: case SIGXCPU: {
                    ret = TL;
                    break;
                }
                case SIGXFSZ: {
                    ret = OL;
                    break;
                }
                default: {
                    ret = RE;
                    break;
                }
            }

            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }

        /* please write the program for checking system call.... */
        /*                        CODE AREA                      */
        /* ----------------------------------------------------- */

        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }

    // check that child process using time
    max_time += (ruse.ru_utime.tv_sec * 1000 + ruse.ru_utime.tv_usec / 1000);
    max_time += (ruse.ru_stime.tv_sec * 1000 + ruse.ru_stime.tv_usec / 1000);

    return ret;
}

void Executer::execute() {
    pid_t pid = fork();

    // child process runs user program.
    if(pid == 0) running();
    // parent process watches child process
    else result = watching(pid);
}