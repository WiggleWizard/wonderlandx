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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ipc_allocator.o \
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
LDLIBSOPTIONS=-Llibs/zeromq-4.0.5/dist/lib -lzmq

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so

${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/CoD4X17
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/CoD4X17/wonderlandx.so ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/src/ipc_allocator.o: src/ipc_allocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ipc_allocator.o src/ipc_allocator.cpp

${OBJECTDIR}/src/logger.o: src/logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/logger.o src/logger.cpp

${OBJECTDIR}/src/wonderlandx.o: src/wonderlandx.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Ilibs/zeromq-4.0.5/dist/include -Ilibs/CoD4X17a/plugins -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/wonderlandx.o src/wonderlandx.cpp

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
