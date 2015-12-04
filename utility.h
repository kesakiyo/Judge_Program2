#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <unistd.h>

class Utility {
private:

public:
	Utility();
	~Utility();
	int get_proc_status(pid_t, const char*);
	int get_file_size(const char*);
	int stoi(const char*);
	void print_runtime_error(const char*);
	bool isSameFile(const char*, const char*);
};

#endif