
*I strongly inform you that this is not for game hacking purposes (the title is just to attract attention (for publicity)), but for research purposes. You are solely responsible for its use.*

*이는 게임 해킹 목적(제목은 단지 관심을 끌기 위한 목적(홍보용))이 아닌 연구 목적임을 강력히 알려드립니다. 그 사용에 대한 책임은 전적으로 귀하에게 있습니다.*

---

# KernelBased_GAMEHACKING

This is for research purposes. WELCOME! 


커널 기반으로 가상메모리 접근 도구를 만들어 연구하였습니다. ( Beta Version NOW ) 

1인 개발 소규모 프로젝트입니다.

이 사용 책임은 저에게 존재하지 않습니다. 
이것은 오로지 연구 목적으로 개발된 레포지토리입니다. 

---

# *[알림]*

계속 저의 연구를 응원해주신다면, 많은 업데이트로 여러분에게 보답하겠습니다.

[+] 밑에 있는 설명들을 읽어주시면 어떻게 개발했는 지 확인하기 좋습니다.

---

# *[Update]*

[MFC기반 IOCTL 유저모드프로그램 ](https://github.com/lastime1650/KernelBased_GAMEHACKING/tree/main/IOCTL_USERMODE/_NEW_MFC_)은 이제 항시 업데이트됩니다.

이전과 같이 CLI기반은 개발하지 않고, MFC로 GUI로 커널을 컨트롤할 수 있는 것에 재미를 느껴, { MFC개발 + 드라이버개발 } 을 하고 있습니다.

## [2024-08-31]

이제 다음과 같은 기능을 어느정도 사용할 수 있습니다. 

1. 유저모드 기반 프로세스 선택 창
2. PAGE 기반 메모리 스캔 창
3. 임의 가상주소 선택 메모리 스캔 창
4. 메모리 스캐너 ( 하단 테이블 아직 구현안됨 )
5. EXE추출 ( 분석을 위한 exe 추출 -> pe파싱하여 식별할 수 있습니다.)
6. Kernel 기반 DLL 인젝션  

<br>

## [2024-08-26]

지금은 "커널기반 메모리 덤퍼" 만 구현되었습니다. 나머지는 메모리 스캐너 및 뷰를 구현하고 있습니다.



---


# 사용방법 

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/image.png)

이 드라이버는 연구용 드라이버이기 때문에, KDU와 같은 불법 로더를 이용하여 로드하면 됩니다.

( KDU Loader는 DriverObject가 항상 NULL이기 떄문에 IoCreateDriver()를 이용하여 초기에 드라이버를 생성해야 IOCTL를 구현할 수 있습니다. ( for Making a Device )  )

그리고 동봉된 [" MFC 기반 유저모드 프로그램 "](https://github.com/lastime1650/KernelBased_GAMEHACKING/tree/main/IOCTL_USERMODE/_NEW_MFC_) 을 활용하여 보다 쉽게 커널을 GUI상에서 Control 하세요! 

---


# Description 설명 

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/1.PNG)

안티치트는 대부분 ObRegisterCallbacks 을 이용하여 외부 프로세스에서 보호하고 있는 게임 프로세스에 대한 접근 권한을 제거하기 때문에
메모리를 읽을 수 없지만, 직접 커널모드에 치트동작이 존재하면 우회가 가능합니다.

<br>

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/KERNEL_BASED_GAME_CHEAT.png)

대표적으로 제가 개발한 치트 동작은 총 2개의 API를 대표적으로 중요시합니다.

1. ZwQueryVirtualMemory
2. MmCopyVirtualMemory

먼저 *ZwQueryVirtualMemory* 는 타겟 프로세스의 HANDLE 값을 통해서 0x0000000 부터 끝까지 "가상주소"를 조회할 수 있습니다.
여기서는 주로 게임의 재화나 아이템들은 PAGE_READWRITE 메모리 보호수준에 존재할 수 있고, 만약에 DUMP를 원한다면 MEM_IMAGE 또는 MEM_MAPPED 와 같은 메모리 타입을 조회하면 DUMP할 수 있습니다.
즉, 이 API는 게임 프로세스의 메모리는 모두 파악할 수 있습니다. 

다음 *MmCopyVirtualMemory* 는 직접 게임 프로세스에 개입이 가능합니다. 즉, 이는 적극적인 행위를 하는 API이기 때문에 위험도가 높습니다.
ZwQueryVirtualMemory에 의해 접근할 가상주소를 얻었다면, 이 MmCopyVirtualMemory를 통하여 "직잡" 데이터를 가져오거나(READ) 쓸수있습니다(WRITE)

*주의* WRITE, 즉 쓰기작업은 매우 위험합니다!!!!!!!!!!!!!!! 그리고, 커널기반으로 쓰기작업을 하였어도, 안티치트의 솔루션에 의하여 감지될 수 있습니다. 

<br>


![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/3.PNG)

다음은 IOCTL 설명입니다.

IOCTL은 유저모드와 커널모드간 버퍼를 공유하는 매개체입니다. 

사용자가 직접 해킹을 하기 위해서 커널에 접근하기 위해 전용으로 유저모드 프로그램이 존재해야합니다. 

(물론 제가 구현한 프로그래밍 방식은 생각보다 복잡할 수 있지만, 보편적으로 저의 이 프로젝트(커널 드라이버)는 비교적으로 매우 쉬운 로직에 속합니다. ) 

<br>


![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/4.PNG)



<br>


![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/5.PNG)

마지막 현재 구현된 대표적 기능입니다.

1. 커널 기반 덤프 ( 일부러 안티치트를 유저모드에서 우회해서 할 필요가 전혀없습니다. )
2. 스캐닝 ( 최초스캔, 다음스캔 )
3. 재사용 ( 값 변경을 확인할 수 있습니다. )
4. 데이터 변조 ( 매우 위험합니다. ) 

<br>

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/6.PNG)

이렇게 수많은 데이터를 IOCTL 단 한번의 통신으로 가능한 방법은 바로 직접 구조체를 정의하고 구현하는 것입니다. 
또한 서로 전달해야할 데이터가 많은 경우, "동적 연결리스트"를 적극적으로 도입하였습니다. 

<br>

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/7.PNG)

여기서 문제는 커널에서 할당된 데이터를 어떻게 유저모드에게 전달하는 지에 대한 문제입니다.
왜냐하면 유저모드는 커널모드에서의 할당된 주소를 참조할 수 없기 때문입니다.

그래서, ZwAllocateVirtualMemory API를 커널에서 사용하여 미리 유저모드의 HEAP 공간에 동적할당하여 그안에 데이터를 미리 COPY하고,
동적할당된 주소의 시작주소와 크기만 유저모드에게 알려주면됩니다.
