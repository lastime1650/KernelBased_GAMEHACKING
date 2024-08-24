# KernelBased_GAMEHACKING
This is my tiny game hacking project for development WELCOME! 


커널 기반으로 게임 치트를 만들어 연구하였습니다. ( Beta Version NOW ) 


---

# *[주의]*

IOCTL 프로그램은 직접 작성하셔야합니다. 

이것은 연구목적이기 때문에 편리함은 고려하지 않았습니다.

또한 지금 현재, 이 연구는 지속적인 업데이트가 필요합니다.

계속 저의 연구를 응원해주신다면, 많은 업데이트로 여러분을 보답하겠습니다.

[+] 밑에 있는 설명은 모두 읽어주셔야합니다.


---


# Description 설명 

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/1.PNG)

안티치트는 대부분 ObRegisterCallbacks 을 이용하여 외부 프로세스에서 보호하고 있는 게임 프로세스에 대한 접근 권한을 제거하기 때문에
메모리를 읽을 수 없지만, 직접 커널모드에 치트동작이 존재하면 우회가 가능합니다.

<br>

![initial](https://github.com/lastime1650/KernelBased_GAMEHACKING/blob/main/Images/2.PNG)

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

<br>
