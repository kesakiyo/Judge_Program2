INCLUDEDIRS = \

LIBDIRS = \

LIBS = \

CPPFLAGS = -Wall -O2 -std=c++11 -g 
LDFLAGS =  $(LIBDIRS) $(LIBS)

bin = judge

t1 = judge
t2 = compiler
t3 = executer
t4 = utility

obj = $(t1).o $(t2).o $(t3).o $(t4).o

all: $(bin)
$(bin): $(obj)
		$(CXX) $(CPPFLAGS) $(obj) $(LDFLAGS) -o $@

clean:
		rm -f $(bin) *.o
