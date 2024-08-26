#ifndef  FILE_IO
#define FILE_IO

#include <stdio.h>
#include <Windows.h>


#ifdef __cplusplus
extern "C" {
#endif

	BOOLEAN File_Create(PUCHAR INPUT_DATA, ULONG32 INPUT_DATA_SIZE, LPCSTR FILE_PATH);


#ifdef __cplusplus
}
#endif

#endif // ! FILE_IO
