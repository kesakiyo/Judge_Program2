#include "utility.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <vector>
#include <string>

Utility::Utility() {

}

Utility::~Utility() {

}

int Utility::stoi(const char* str) {
	int ret = 0, len = strlen(str);
	for(int i=0 ; i<len ; ++i) {
		ret *= 10;
		ret += str[i] - '0';
	}
	return ret;
}

int Utility::get_proc_status(pid_t pid, const char* str) {
	FILE* fp;

	const int BUFFER_SIZE = 110;
	char command[BUFFER_SIZE], buffer[BUFFER_SIZE];
	int ret = 0;
	sprintf(command, "/proc/%d/status", pid);
	fp = fopen(command, "r");
	int len = strlen(str);

	while(fp && fgets(buffer, BUFFER_SIZE - 1, fp)) {
		buffer[strlen(buffer) - 1] = 0;
		if(strncmp(buffer, str, len) == 0) {
			sscanf(buffer + len + 1, "%d", &ret);
			break;
		}
	}

	if(fp) fclose(fp);
	return ret;
}

int Utility::get_file_size(const char* file_name) {
	struct stat file_status;
	if(stat(file_name, &file_status) == -1) return 0;

	return file_status.st_size;
}

void Utility::print_runtime_error(const char* error_message) {
    FILE *fp = fopen("error.out","a+");
    fprintf(fp,"Runtime Error: %s\n", error_message);
    if(fp) fclose(fp);
}

bool Utility::isSameFile(const char* user, const char* judge) {
	FILE* user_fp = fopen(user, "r");
	FILE* judge_fp = fopen(judge, "r");
	std::vector<std::string> U, J;

	const int MAX=10000000;
	char* buffer = new char[MAX];

	if(user_fp) {
		while(fgets(buffer, MAX, user_fp)) {
			int len=strlen(buffer);
			if(!len) continue;
			std::string str(buffer);
			while(str.size() && (str[str.size()-1]==' ' || str[str.size()-1]=='\n' || str[str.size()-1]=='\r' || str[str.size()-1]=='\t')) str.pop_back();
			if(str.size()) U.push_back(str);
		}
	}

	if(judge_fp) {
		while(fgets(buffer, MAX, judge_fp)) {
			int len=strlen(buffer);
			if(!len) continue;
			std::string str(buffer);
			while(str.size() && (str[str.size()-1]==' ' || str[str.size()-1]=='\n'  || str[str.size()-1]=='\r' || str[str.size()-1]=='\t')) str.pop_back();
			if(str.size()) J.push_back(str);
		}
	}

	if(U.size()!=J.size()) return false;

	for(int i=0 ; i<(int)U.size() ; ++i) {
		const std::string& u=U[i];
		const std::string& j=J[i];
		if(u!=j) return false;
	}

	if(user_fp) fclose(user_fp);
	if(judge_fp) fclose(judge_fp);
	delete[] buffer;

	return true;
}