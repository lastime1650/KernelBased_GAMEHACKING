#include "PE_parsing.h"

BOOLEAN is_this_EXE_file(PUCHAR EXE_start, ULONG32* OUTPUT_SizeOfImage) {
	if (EXE_start == NULL) return FALSE;


	BOOLEAN find_exe_file = FALSE;

	PIMAGE_DOS_HEADER__ dosHeader = (PIMAGE_DOS_HEADER__)EXE_start;
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
		PIMAGE_NT_HEADERS__ ntHeaders = (PIMAGE_NT_HEADERS__)((PUCHAR)EXE_start + dosHeader->e_lfanew);
		if (ntHeaders->Signature == IMAGE_NT_SIGNATURE) {

			// Optional 헤더 검증
			if (ntHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC || ntHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {

				if (ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI || ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) {

					// Characteristics 검증
					if (!(ntHeaders->FileHeader.Characteristics & IMAGE_FILE_DLL)) {
						if (ntHeaders->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) {

							*OUTPUT_SizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
							find_exe_file = TRUE;
						}
					}

				}
				else {
					// Characteristics 검증
					if (!(ntHeaders->FileHeader.Characteristics & IMAGE_FILE_DLL)) {
						if (ntHeaders->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) {
							*OUTPUT_SizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
							find_exe_file = TRUE;
						}
					}
				}
			}
		}
	}


	return find_exe_file;
}