#ifndef	_EXECUTER_H_
#define _EXECUTER_H_

#define WAIT 		0
#define JUDGING		1
#define AC			2
#define WA 			3
#define CE 			4
#define RE 			5
#define ML 			6
#define TL 			7
#define OL			8

#include <unistd.h>

class Executer {
private:
	// initialize value
	int problem_id;
	int lang;
	int special;
	int time_limit;
	int memory_limit;
	char judge_input[110];
	char user_output[110];
	char dir[110];

	// this value will be used at runtime.
	int result;
	int max_time;
	int max_memory;

public:
	Executer(int, int, int, int, int, const char*, const char*, const char*);
	~Executer();
	bool isSplecial();
	int get_result();
	int get_max_time();
	int get_max_memory();
	int running();
	int watching(const pid_t&);
	void execute();
};

#endif
