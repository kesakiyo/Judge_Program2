#include "compiler.h"
#include "executer.h"
#include "utility.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*
#define WAIT 		0
#define JUDGING		1
#define AC			2
#define WA 			3
#define CE 			4
#define RE 			5
#define ML 			6
#define TL 			7
#define OL			8
*/

int main(int argc, char* argv[]) {

	// Class usage
	// Compiler compiler(cosnt char* lang, const char* work_directory);
	// compiler.compile()

	// Executer executer(problem_id, int lang, special, time_limit, memory_limit, const char* work_directory, const char* _judge_input, const char* _user_output)
	// executer.execute()
	Utility util;

	int problem_id = util.stoi(argv[1]);
	const char* lang = argv[2];
	int special = util.stoi(argv[3]);
	int time_limit = util.stoi(argv[4]);
	int memory_limit = util.stoi(argv[5]);
	const char* judge_input = argv[6];
	const char* judge_output = argv[7];
	const char* user_output = "user.out";

	/*
	printf("%d\n", problem_id);
	printf("%s\n", lang);
	printf("%d\n", special);
	printf("%d\n", time_limit);
	printf("%d\n", memory_limit);
	printf("%s\n", judge_input);
	printf("%s\n", judge_output);
	printf("%s\n", user_output);
	*/

	// working directory.
	const char* dir="JudgeArea";
	Compiler compiler(lang, dir);
	int result, max_time, max_memory;

	// compile error
	if(compiler.compile()) {
		result= CE;
		max_time = max_memory = -1;
	}
	else {
		// binary file is executed.
		Executer executer(problem_id, compiler.get_lang(), special, time_limit, memory_limit, dir, judge_input, user_output);
		executer.execute();

		// get result file
		result = executer.get_result();
		max_time = executer.get_max_time(); // unit is milli second
		max_memory = executer.get_max_memory(); // unit is kilo byte

		// output limit exceeded
		if(result == OL) {
			result = OL;
			max_time = max_memory = -1;
		}
		// memory limit exceeded
		else if(result == ML || max_memory > (memory_limit << 20)) {
			result = ML;
			max_time = max_memory = -1;
		}
		// time limit exceeded
		else if(result == TL || max_time > time_limit * 1000) {
			result = TL;
			max_time = max_memory = -1;
		}
		// runtime error
		else if(result == RE) {
			result = RE;
			max_time = max_memory = -1;
		}
		else {
			if(special) {
				// this problem needs special judge

				/* please write the program for special judge */
				/*               code area                    */
				/* -------------------------------------------*/
			}
			else {
				chdir(dir);
				bool isAccept = util.isSameFile(user_output, judge_output);
				result = isAccept ? AC : WA;

				if(result == WA) {
					max_time = max_memory = -1;
				}
			}
		}
	}

	// {"firstName":"John", "lastName":"Doe"}
	// print result to python syntax
	printf("{\"result\":\"%d\", \"maxtime\":\"%d\", \"maxmemory\":\"%d\"}", result, max_time, max_memory >> 10);
}