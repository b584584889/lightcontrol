LIBRARY_NAME		= amqpcpp
CPP	        		= g++
RM	        		= rm -f
CPPFLAGS			= -Wall -c -I./include -std=c++11 -MD -Wno-class-conversion -I/opt/homebrew/opt/openssl@1.1/include -I/Users/smims/MySQL/mysql-connector-c++-8.0/include/jdbc -I/opt/homebrew/opt/boost/include
LD	        		= g++
LD_FLAGS			= -Wall -shared
SHARED_LIB			= lib$(LIBRARY_NAME).so
STATIC_LIB			= lib$(LIBRARY_NAME).a
SOURCES				= $(wildcard *.cpp) $(wildcard linux_tcp/*.cpp)
SHARED_OBJECTS		= $(SOURCES:%.cpp=%.o)
STATIC_OBJECTS		= $(SOURCES:%.cpp=%.s.o)
DEPENDENCIES		= $(SOURCES:%.cpp=%.d)
STATIC_DEPENDENCIES		= $(SOURCES:%.cpp=%.s.d)
PURE_SHARED_OBJECTS	= $(filter-out tcpconnection.o, $(SOURCES:%.cpp=%.o))
PURE_STATIC_OBJECTS	= $(filter-out tcpconnection.s.o, $(SOURCES:%.cpp=%.s.o))


ifeq ($(shell uname -s),Darwin)
    SONAMEPARAMETER = -install_name
else
    SONAMEPARAMETER = -soname
endif

-include ${DEPENDENCIES}

all:			CPPFLAGS += -g
all:			LD_FLAGS += -g
all:			static removeobj
#all:			shared static removeobj

pure:			CPPFLAGS += -g
pure:			LD_FLAGS += -g
pure:			shared_pure static_pure

release:		CPPFLAGS += -O2
release:		LD_FLAGS += -O2
release:		shared static

shared:			${SHARED_OBJECTS} ${SHARED_LIB}
shared_pure:	${PURE_SHARED_OBJECTS} ${SHARED_LIB}

static:			${STATIC_OBJECTS} ${STATIC_LIB}
static_pure:	${PURE_STATIC_OBJECTS} ${STATIC_LIB}

${SHARED_LIB}: ${SHARED_OBJECTS}
	${LD} ${LD_FLAGS} -Wl,${SONAMEPARAMETER},lib$(LIBRARY_NAME).so -o $@ ${SHARED_OBJECTS}

${STATIC_LIB}: ${STATIC_OBJECTS}
	ar rcs ${STATIC_LIB} ${STATIC_OBJECTS}

clean:
	${RM} *.obj *~* ${SHARED_OBJECTS} ${STATIC_OBJECTS} ${SHARED_LIB} ${STATIC_LIB} ${DEPENDENCIES} ${STATIC_DEPENDENCIES}

removeobj:
	${RM} *.obj *.d *~* ${SHARED_OBJECTS} ${STATIC_OBJECTS} ${DEPENDENCIES} ${STATIC_DEPENDENCIES}

${SHARED_OBJECTS}:
	${CPP} ${CPPFLAGS} -fpic -o $@ ${@:%.o=%.cpp}

${STATIC_OBJECTS}:
	${CPP} ${CPPFLAGS} -o $@ ${@:%.s.o=%.cpp}
