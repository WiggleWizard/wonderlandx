#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/globals.o \
	${OBJECTDIR}/src/ipc_event.o \
	${OBJECTDIR}/src/ipc_func_man.o \
	${OBJECTDIR}/src/ipc_function.o \
	${OBJECTDIR}/src/ipc_rabbithole.o \
	${OBJECTDIR}/src/ipc_return.o \
	${OBJECTDIR}/src/logger.o \
	${OBJECTDIR}/src/wonderlandx.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -Wattributes
CXXFLAGS=-m32 -Wattributes

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Llibs/zeromq-4.0.5/dist/lib libs/zeromq-4.0.5/dist/lib/libzmq.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so

${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so: libs/zeromq-4.0.5/dist/lib/libzmq.a

${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/CoD4X17
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/src/globals.o: src/globals.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/globals.o src/globals.cpp

${OBJECTDIR}/src/ipc_event.o: src/ipc_event.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_event.o src/ipc_event.cpp

${OBJECTDIR}/src/ipc_func_man.o: src/ipc_func_man.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_func_man.o src/ipc_func_man.cpp

${OBJECTDIR}/src/ipc_function.o: src/ipc_function.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_function.o src/ipc_function.cpp

${OBJECTDIR}/src/ipc_rabbithole.o: src/ipc_rabbithole.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_rabbithole.o src/ipc_rabbithole.cpp

${OBJECTDIR}/src/ipc_return.o: src/ipc_return.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_return.o src/ipc_return.cpp

${OBJECTDIR}/src/logger.o: src/logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/logger.o src/logger.cpp

${OBJECTDIR}/src/wonderlandx.o: src/wonderlandx.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/wonderlandx.o src/wonderlandx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
