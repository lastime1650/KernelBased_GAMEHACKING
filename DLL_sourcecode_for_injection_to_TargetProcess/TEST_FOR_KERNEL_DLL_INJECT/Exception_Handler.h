#ifndef Exception_Hander_H
#define Exception_Hander_H

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

#include "IOCTL_functions.h"


#ifdef __cplusplus
extern "C" {
#endif

	LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);
	BOOLEAN SEND_Exception(PCONTEXT context);

#ifdef __cplusplus
}
#endif

#endif // !Exception_Hander_H
