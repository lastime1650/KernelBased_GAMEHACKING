#ifndef API_LOADER_
#define API_LOADER_

#include <ntifs.h>
typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation = 0x00,
    SystemProcessorInformation = 0x01,
    SystemPerformanceInformation = 0x02,
    SystemTimeOfDayInformation = 0x03,
    SystemPathInformation = 0x04,
    SystemProcessInformation = 0x05,
    SystemCallCountInformation = 0x06,
    SystemDeviceInformation = 0x07,
    SystemProcessorPerformanceInformation = 0x08,
    SystemFlagsInformation = 0x09,
    SystemCallTimeInformation = 0x0A,
    SystemModuleInformation = 0x0B,
    SystemLocksInformation = 0x0C,
    SystemStackTraceInformation = 0x0D,
    SystemPagedPoolInformation = 0x0E,
    SystemNonPagedPoolInformation = 0x0F,
    SystemHandleInformation = 0x10,
    SystemObjectInformation = 0x11,
    SystemPageFileInformation = 0x12,
    SystemVdmInstemulInformation = 0x13,
    SystemVdmBopInformation = 0x14,
    SystemFileCacheInformation = 0x15,
    SystemPoolTagInformation = 0x16,
    SystemInterruptInformation = 0x17,
    SystemDpcBehaviorInformation = 0x18,
    SystemFullMemoryInformation = 0x19,
    SystemLoadGdiDriverInformation = 0x1A,
    SystemUnloadGdiDriverInformation = 0x1B,
    SystemTimeAdjustmentInformation = 0x1C,
    SystemSummaryMemoryInformation = 0x1D,
    SystemMirrorMemoryInformation = 0x1E,
    SystemPerformanceTraceInformation = 0x1F,
    SystemObsolete0 = 0x20,
    SystemExceptionInformation = 0x21,
    SystemCrashDumpStateInformation = 0x22,
    SystemKernelDebuggerInformation = 0x23,
    SystemContextSwitchInformation = 0x24,
    SystemRegistryQuotaInformation = 0x25,
    SystemExtendServiceTableInformation = 0x26,
    SystemPrioritySeperation = 0x27,
    SystemVerifierAddDriverInformation = 0x28,
    SystemVerifierRemoveDriverInformation = 0x29,
    SystemProcessorIdleInformation = 0x2A,
    SystemLegacyDriverInformation = 0x2B,
    SystemCurrentTimeZoneInformation = 0x2C,
    SystemLookasideInformation = 0x2D,
    SystemTimeSlipNotification = 0x2E,
    SystemSessionCreate = 0x2F,
    SystemSessionDetach = 0x30,
    SystemSessionInformation = 0x31,
    SystemRangeStartInformation = 0x32,
    SystemVerifierInformation = 0x33,
    SystemVerifierThunkExtend = 0x34,
    SystemSessionProcessInformation = 0x35,
    SystemLoadGdiDriverInSystemSpace = 0x36,
    SystemNumaProcessorMap = 0x37,
    SystemPrefetcherInformation = 0x38,
    SystemExtendedProcessInformation = 0x39,
    SystemRecommendedSharedDataAlignment = 0x3A,
    SystemComPlusPackage = 0x3B,
    SystemNumaAvailableMemory = 0x3C,
    SystemProcessorPowerInformation = 0x3D,
    SystemEmulationBasicInformation = 0x3E,
    SystemEmulationProcessorInformation = 0x3F,
    SystemExtendedHandleInformation = 0x40,
    SystemLostDelayedWriteInformation = 0x41,
    SystemBigPoolInformation = 0x42,
    SystemSessionPoolTagInformation = 0x43,
    SystemSessionMappedViewInformation = 0x44,
    SystemHotpatchInformation = 0x45,
    SystemObjectSecurityMode = 0x46,
    SystemWatchdogTimerHandler = 0x47,
    SystemWatchdogTimerInformation = 0x48,
    SystemLogicalProcessorInformation = 0x49,
    SystemWow64SharedInformationObsolete = 0x4A,
    SystemRegisterFirmwareTableInformationHandler = 0x4B,
    SystemFirmwareTableInformation = 0x4C,
    SystemModuleInformationEx = 0x4D,
    SystemVerifierTriageInformation = 0x4E,
    SystemSuperfetchInformation = 0x4F,
    SystemMemoryListInformation = 0x50,
    SystemFileCacheInformationEx = 0x51,
    SystemThreadPriorityClientIdInformation = 0x52,
    SystemProcessorIdleCycleTimeInformation = 0x53,
    SystemVerifierCancellationInformation = 0x54,
    SystemProcessorPowerInformationEx = 0x55,
    SystemRefTraceInformation = 0x56,
    SystemSpecialPoolInformation = 0x57,
    SystemProcessIdInformation = 0x58,
    SystemErrorPortInformation = 0x59,
    SystemBootEnvironmentInformation = 0x5A,
    SystemHypervisorInformation = 0x5B,
    SystemVerifierInformationEx = 0x5C,
    SystemTimeZoneInformation = 0x5D,
    SystemImageFileExecutionOptionsInformation = 0x5E,
    SystemCoverageInformation = 0x5F,
    SystemPrefetchPatchInformation = 0x60,
    SystemVerifierFaultsInformation = 0x61,
    SystemSystemPartitionInformation = 0x62,
    SystemSystemDiskInformation = 0x63,
    SystemProcessorPerformanceDistribution = 0x64,
    SystemNumaProximityNodeInformation = 0x65,
    SystemDynamicTimeZoneInformation = 0x66,
    SystemCodeIntegrityInformation = 0x67,
    SystemProcessorMicrocodeUpdateInformation = 0x68,
    SystemProcessorBrandString = 0x69,
    SystemVirtualAddressInformation = 0x6A,
    SystemLogicalProcessorAndGroupInformation = 0x6B,
    SystemProcessorCycleTimeInformation = 0x6C,
    SystemStoreInformation = 0x6D,
    SystemRegistryAppendString = 0x6E,
    SystemAitSamplingValue = 0x6F,
    SystemVhdBootInformation = 0x70,
    SystemCpuQuotaInformation = 0x71,
    SystemNativeBasicInformation = 0x72,
    SystemErrorPortTimeouts = 0x73,
    SystemLowPriorityIoInformation = 0x74,
    SystemBootEntropyInformation = 0x75,
    SystemVerifierCountersInformation = 0x76,
    SystemPagedPoolInformationEx = 0x77,
    SystemSystemPtesInformationEx = 0x78,
    SystemNodeDistanceInformation = 0x79,
    SystemAcpiAuditInformation = 0x7A,
    SystemBasicPerformanceInformation = 0x7B,
    SystemQueryPerformanceCounterInformation = 0x7C,
    SystemSessionBigPoolInformation = 0x7D,
    SystemBootGraphicsInformation = 0x7E,
    SystemScrubPhysicalMemoryInformation = 0x7F,
    SystemBadPageInformation = 0x80,
    SystemProcessorProfileControlArea = 0x81,
    SystemCombinePhysicalMemoryInformation = 0x82,
    SystemEntropyInterruptTimingInformation = 0x83,
    SystemConsoleInformation = 0x84,
    SystemPlatformBinaryInformation = 0x85,
    SystemPolicyInformation = 0x86,
    SystemHypervisorProcessorCountInformation = 0x87,
    SystemDeviceDataInformation = 0x88,
    SystemDeviceDataEnumerationInformation = 0x89,
    SystemMemoryTopologyInformation = 0x8A,
    SystemMemoryChannelInformation = 0x8B,
    SystemBootLogoInformation = 0x8C,
    SystemProcessorPerformanceInformationEx = 0x8D,
    SystemCriticalProcessErrorLogInformation = 0x8E,
    SystemSecureBootPolicyInformation = 0x8F,
    SystemPageFileInformationEx = 0x90,
    SystemSecureBootInformation = 0x91,
    SystemEntropyInterruptTimingRawInformation = 0x92,
    SystemPortableWorkspaceEfiLauncherInformation = 0x93,
    SystemFullProcessInformation = 0x94,
    SystemKernelDebuggerInformationEx = 0x95,
    SystemBootMetadataInformation = 0x96,
    SystemSoftRebootInformation = 0x97,
    SystemElamCertificateInformation = 0x98,
    SystemOfflineDumpConfigInformation = 0x99,
    SystemProcessorFeaturesInformation = 0x9A,
    SystemRegistryReconciliationInformation = 0x9B,
    SystemEdidInformation = 0x9C,
    SystemManufacturingInformation = 0x9D,
    SystemEnergyEstimationConfigInformation = 0x9E,
    SystemHypervisorDetailInformation = 0x9F,
    SystemProcessorCycleStatsInformation = 0xA0,
    SystemVmGenerationCountInformation = 0xA1,
    SystemTrustedPlatformModuleInformation = 0xA2,
    SystemKernelDebuggerFlags = 0xA3,
    SystemCodeIntegrityPolicyInformation = 0xA4,
    SystemIsolatedUserModeInformation = 0xA5,
    SystemHardwareSecurityTestInterfaceResultsInformation = 0xA6,
    SystemSingleModuleInformation = 0xA7,
    SystemAllowedCpuSetsInformation = 0xA8,
    SystemDmaProtectionInformation = 0xA9,
    SystemInterruptCpuSetsInformation = 0xAA,
    SystemSecureBootPolicyFullInformation = 0xAB,
    SystemCodeIntegrityPolicyFullInformation = 0xAC,
    SystemAffinitizedInterruptProcessorInformation = 0xAD,
    SystemRootSiloInformation = 0xAE,
    SystemCpuSetInformation = 0xAF,
    SystemCpuSetTagInformation = 0xB0,
    SystemWin32WerStartCallout = 0xB1,
    SystemSecureKernelProfileInformation = 0xB2,
    SystemCodeIntegrityPlatformManifestInformation = 0xB3,
    SystemInterruptSteeringInformation = 0xB4,
    SystemSuppportedProcessorArchitectures = 0xB5,
    SystemMemoryUsageInformation = 0xB6,
    SystemCodeIntegrityCertificateInformation = 0xB7,
    SystemPhysicalMemoryInformation = 0xB8,
    SystemControlFlowTransition = 0xB9,
    SystemKernelDebuggingAllowed = 0xBA,
    SystemActivityModerationExeState = 0xBB,
    SystemActivityModerationUserSettings = 0xBC,
    SystemCodeIntegrityPoliciesFullInformation = 0xBD,
    SystemCodeIntegrityUnlockInformation = 0xBE,
    SystemIntegrityQuotaInformation = 0xBF,
    SystemFlushInformation = 0xC0,
    SystemProcessorIdleMaskInformation = 0xC1,
    SystemSecureDumpEncryptionInformation = 0xC2,
    SystemWriteConstraintInformation = 0xC3,
    SystemKernelVaShadowInformation = 0xC4,
    SystemHypervisorSharedPageInformation = 0xC5,
    SystemFirmwareBootPerformanceInformation = 0xC6,
    SystemCodeIntegrityVerificationInformation = 0xC7,
    SystemFirmwarePartitionInformation = 0xC8,
    SystemSpeculationControlInformation = 0xC9,
    SystemDmaGuardPolicyInformation = 0xCA,
    SystemEnclaveLaunchControlInformation = 0xCB,
    SystemWorkloadAllowedCpuSetsInformation = 0xCC,
    SystemCodeIntegrityUnlockModeInformation = 0xCD,
    SystemLeapSecondInformation = 0xCE,
    SystemFlags2Information = 0xCF,
    SystemSecurityModelInformation = 0xD0,
    SystemCodeIntegritySyntheticCacheInformation = 0xD1,
    MaxSystemInfoClass = 0xD2
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER KernelTime;        // 커널 모드에서 소비한 CPU 시간
    LARGE_INTEGER UserTime;          // 사용자 모드에서 소비한 CPU 시간
    LARGE_INTEGER CreateTime;        // 스레드 생성 시간
    ULONG WaitTime;                  // 대기 시간
    ULONG Priority;                  // 스레드 우선 순위
    LONG BasePriority;               // 기본 우선 순위
    CLIENT_ID PID_and_THread_ID;             // 소속 프로세스 and 스레드
    ULONG ThreadState;               // 스레드 상태
    ULONG WaitReason;                // 대기 이유
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

/* PID 로 프로세스 정보 다 얻기 */
typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset; // 다음 프로세스 정보 구조체까지의 바이트 오프셋
    ULONG NumberOfThreads; // 이 프로세스의 스레드 수
    LARGE_INTEGER WorkingSetPrivateSize; // 프라이빗 작업 세트 크기
    ULONG HardFaultCount; // 하드 페이지 폴트 수
    ULONG NumberOfThreadsHighWatermark; // 스레드의 최대 수
    ULONGLONG CycleTime; // 이 프로세스의 사이클 시간
    LARGE_INTEGER CreateTime; // 프로세스 생성 시간
    LARGE_INTEGER UserTime; // 사용자 모드에서 사용된 시간
    LARGE_INTEGER KernelTime; // 커널 모드에서 사용된 시간
    UNICODE_STRING ImageName; // 프로세스의 이미지 이름
    ULONG BasePriority; // 기본 우선순위
    HANDLE UniqueProcessId; // 프로세스 식별자
    HANDLE InheritedFromUniqueProcessId; // 부모 프로세스 식별자
    ULONG HandleCount; // 프로세스가 소유한 핸들 수
    ULONG SessionId; // 프로세스가 속한 세션 ID
    ULONG_PTR UniqueProcessKey; // 프로세스의 고유 키
    SIZE_T PeakVirtualSize; // 가상 메모리 최대 크기
    SIZE_T VirtualSize; // 현재 가상 메모리 크기
    ULONG PageFaultCount; // 페이지 폴트 수
    SIZE_T PeakWorkingSetSize; // 작업 세트의 최대 크기
    SIZE_T WorkingSetSize; // 현재 작업 세트 크기
    SIZE_T QuotaPeakPagedPoolUsage; // 페이지 풀 사용량의 최대 크기
    SIZE_T QuotaPagedPoolUsage; // 현재 페이지 풀 사용량
    SIZE_T QuotaPeakNonPagedPoolUsage; // 비페이징 풀 사용량의 최대 크기
    SIZE_T QuotaNonPagedPoolUsage; // 현재 비페이징 풀 사용량
    SIZE_T PagefileUsage; // 페이지 파일 사용량
    SIZE_T PeakPagefileUsage; // 페이지 파일 사용량의 최대 크기
    SIZE_T PrivatePageCount; // 프로세스의 프라이빗 페이지 수
    LARGE_INTEGER ReadOperationCount; // 읽기 작업 수
    LARGE_INTEGER WriteOperationCount; // 쓰기 작업 수
    LARGE_INTEGER OtherOperationCount; // 기타 작업 수
    LARGE_INTEGER ReadTransferCount; // 읽기 전송량
    LARGE_INTEGER WriteTransferCount; // 쓰기 전송량
    LARGE_INTEGER OtherTransferCount; // 기타 전송량
    SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

// 함수 포인터 구조체
typedef NTSTATUS(*Bring_ZwQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(*Bring_ZwQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(*Bring_ObSetSecurityObjectByPointer)(
    PVOID Object,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR SecurityDescriptor
    );



//전역 함수 포인터 변수
extern Bring_ZwQuerySystemInformation ZwQuerySystemInformation;
extern Bring_ZwQueryInformationProcess ZwQueryInformationProcess;
extern Bring_ObSetSecurityObjectByPointer ObSetSecurityObjectByPointer;

//함수
NTSTATUS Bring_API();

#endif