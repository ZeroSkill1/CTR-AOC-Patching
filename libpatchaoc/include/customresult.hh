#ifndef __customresult_hh__
#define __customresult_hh__

#include <3ds/result.h>	

// 0xD8A3FBEB
#define ARCHIVE_CLOSED MAKERESULT(RL_PERMANENT, RS_INVALIDSTATE, RM_APPLICATION, RD_ALREADY_DONE)

// 0xD8A3FBE6
#define CANNOT_CLOSE_ARCHIVE_FILES_OPEN MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_ARCHIVE_FILES_OPEN)

// 0xD8A3FBE7
#define TITLE_VERSION_MISMATCH MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_TITLE_VERSION_MISMATCH)

// 0xD8A3FBED
#define INVALID_PATCH_TYPE MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_INVALID_ENUM_VALUE)

// 0xD8A3FBF6
#define INVALID_POINTER MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_INVALID_POINTER)

// 0xD8A3FBF7
#define INVALID_HANDLE MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_INVALID_HANDLE)

// 0xD8A3FBFA
#define FILE_NOT_FOUND MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_NOT_FOUND)

// 0xD8A3FBE5
#define INCOMPLETE_IO MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_INCOMPLETE_IO)

// 0xD8A3FBE4
#define TITLES_NOT_FOUND MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_TITLES_NOT_FOUND)

// 0xD8A3FBEC
#define INVALID_SAVE_SIZE MAKERESULT(RL_PERMANENT,RS_INVALIDSTATE,RM_APPLICATION,RD_INVALID_SIZE)

enum
{
	RD_TITLE_VERSION_MISMATCH = RD_INVALID_RESULT_VALUE - 24,
	RD_ARCHIVE_FILES_OPEN = RD_INVALID_RESULT_VALUE - 25,
	RD_INCOMPLETE_IO = RD_INVALID_RESULT_VALUE - 26,
	RD_TITLES_NOT_FOUND = RD_INVALID_RESULT_VALUE - 27,
};

#endif