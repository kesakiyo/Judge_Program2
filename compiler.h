#ifndef	_COMPILER_H_
#define _COMPILER_H_

static const char* languages[] = {"C", "CC", "CC11", nullptr};
typedef enum {C, CC, CC11} langCode;

class Compiler {
	// compile option for language
private:
	int lang;
	char dir[110];

public:
	Compiler(const char*, const char*);
	~Compiler();
	int get_lang();
	int compile();
};

#endif
