#ifndef  PE_PARSER
#define PE_PARSER

#include <ntifs.h>

#define IMAGE_DOS_SIGNATURE 0x5A4D   // 'MZ'
#define IMAGE_NT_SIGNATURE  0x00004550 // 'PE00'
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x010b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x020b

#define IMAGE_SUBSYSTEM_WINDOWS_GUI   2
#define IMAGE_SUBSYSTEM_WINDOWS_CUI   3

#define IMAGE_FILE_EXECUTABLE_IMAGE 0x0002
#define IMAGE_FILE_DLL 0x2000

typedef struct _IMAGE_DOS_HEADER {
    USHORT e_magic;                  // Magic number
    USHORT e_cblp;                   // Bytes on last page of file
    USHORT e_cp;                     // Pages in file
    USHORT e_crlc;                   // Relocations
    USHORT e_cparhdr;                // Size of header in paragraphs
    USHORT e_minalloc;               // Minimum extra paragraphs needed
    USHORT e_maxalloc;               // Maximum extra paragraphs needed
    USHORT e_ss;                     // Initial (relative) SS value
    USHORT e_sp;                     // Initial SP value
    USHORT e_csum;                   // Checksum
    USHORT e_ip;                     // Initial IP value
    USHORT e_cs;                     // Initial (relative) CS value
    USHORT e_lfarlc;                // File address of relocation table
    USHORT e_ovno;                   // Overlay number
    USHORT e_res[4];                 // Reserved words
    USHORT e_oemid;                  // OEM identifier
    USHORT e_oeminfo;                // OEM information
    USHORT e_res2[10];               // Reserved words
    LONG   e_lfanew;                 // File address of new exe header
} IMAGE_DOS_HEADER__, * PIMAGE_DOS_HEADER__;

typedef struct _IMAGE_FILE_HEADER {
    USHORT  Machine;                 // Machine type
    USHORT  NumberOfSections;        // Number of sections
    ULONG   TimeDateStamp;           // Time and date stamp
    ULONG   PointerToSymbolTable;    // File pointer to symbol table
    ULONG   NumberOfSymbols;         // Number of symbols
    USHORT  SizeOfOptionalHeader;    // Size of optional header
    USHORT  Characteristics;         // Characteristics
} IMAGE_FILE_HEADER__, * PIMAGE_FILE_HEADER__;

typedef struct _IMAGE_OPTIONAL_HEADER__ {
    USHORT  Magic;                   // Optional header magic
    UCHAR   MajorLinkerVersion;      // Major linker version
    UCHAR   MinorLinkerVersion;      // Minor linker version
    ULONG   SizeOfCode;             // Size of code
    ULONG   SizeOfInitializedData;  // Size of initialized data
    ULONG   SizeOfUninitializedData;// Size of uninitialized data
    ULONG   AddressOfEntryPoint;    // Address of entry point
    ULONG   BaseOfCode;             // Base of code
    ULONG   BaseOfData;             // Base of data
    ULONG   ImageBase;              // Image base address
    ULONG   SectionAlignment;       // Section alignment
    ULONG   FileAlignment;          // File alignment
    USHORT  MajorOperatingSystemVersion; // Major OS version
    USHORT  MinorOperatingSystemVersion; // Minor OS version
    USHORT  MajorImageVersion;      // Major image version
    USHORT  MinorImageVersion;      // Minor image version
    USHORT  MajorSubsystemVersion; // Major subsystem version
    USHORT  MinorSubsystemVersion; // Minor subsystem version
    ULONG   Win32VersionValue;      // Reserved
    ULONG   SizeOfImage;            // Size of image
    ULONG   SizeOfHeaders;          // Size of headers
    ULONG   CheckSum;               // Checksum
    USHORT  Subsystem;              // Subsystem
    USHORT  DllCharacteristics;     // DLL characteristics
    ULONG   SizeOfStackReserve;     // Size of stack reserve
    ULONG   SizeOfStackCommit;      // Size of stack commit
    ULONG   SizeOfHeapReserve;      // Size of heap reserve
    ULONG   SizeOfHeapCommit;       // Size of heap commit
    ULONG   LoaderFlags;            // Loader flags
    ULONG   NumberOfRvaAndSizes;    // Number of RVA and sizes
} IMAGE_OPTIONAL_HEADER__, * PIMAGE_OPTIONAL_HEADER__;

typedef struct _IMAGE_NT_HEADERS__ {
    ULONG               Signature;      // PE signature
    IMAGE_FILE_HEADER__   FileHeader;     // File header
    IMAGE_OPTIONAL_HEADER__ OptionalHeader; // Optional header
} IMAGE_NT_HEADERS__, * PIMAGE_NT_HEADERS__;

BOOLEAN is_this_EXE_file(PUCHAR EXE_start, ULONG32* OUTPUT_SizeOfImage);

#endif // ! PE_PARSER
