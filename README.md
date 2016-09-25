# gamebusdriver
KPU 
졸업작품용 협업 System

branch 합치기 이전의 최종 commit 본
===============
`16.09.26` [ **김형준** ] - iocp 복잡한 함수들을, 유지보수 편하게 하기 위해 class 형식으로 모두 변경. protocol.h 의 몇몇 함수와 전역 변수가 class화 하여 바꾼 서버 클래스와 충돌이 많아서, 아예 client send packet 클래스에 합쳐버렸음. 차후 문제가 발생할 여지가 있기 때문에 북구 링크남김
* 위 버전을 복구하기 위한 링크
* https://github.com/korleinster/gamebusdriver/tree/e6689c40b5a5f11265545538135003353d99b8d8


서버 변경 사항
===============

`16.08.18` [ **김형준** ] - *protocol.h & ServerMain.cpp 수정*, class SendPacket 의 경우 메모리 해제와 비동기 작동에 관련하여 문제가 발생할 것 같아 전역 buf_send 배열로 변경.
buf_recv 와 queue 의 경우 나중에 하나로 합칠 예정.
OVLP_EX, PLAYER_INFO 구조체 두개를 protocol.h 에서 ServerMain.cpp 로 선언을 옮김.
error_display, error_quit 함수를 protocol.h 로 옮김.
기본 서버 통신 확인용 TEST process protocol define
class SendPacket 에 Send_default_test() 함수 추가
* 위 버전을 복구하기 위한 링크
* https://github.com/korleinster/gamebusdriver/tree/cd800086b82bc6bc8af297514da69ad89cdf8c10

`16.08.16` [ **김형준** ] - *protocol.h 수정*, 클라이언트에서 패킷 보내기 편하기 하기 위해 클래스를 한번 만들어 봤는데, 동적할당으로는 처음 작성해 보았기 때문에, 비동기로 작동할 경우 자기 맘대로 메모리 해제를 할 가능성이 다분히 있다고 생각이 된다.
오류가 발생할 가능성이 있으니 문제없는지 꼭 확인해 보고 사용하자.
* 위 버전을 복구하기 위한 링크
* https://github.com/korleinster/gamebusdriver/tree/1d979606895b9cdec5ab708ffe2c42a2044671a8

`16.07.06` [ **김형준** ] - *서버 기본 틀 마련*, 서버 기본 틀. 클라이언트를 저장하는 clients 가 원래 array 었으나, vector 포인터 형태로 처음 변경해 보았기 때문에, 버그가 발생할 소지가 있다.	클라이언트에서 정보를 잘 전달하기 위해, 기본 프로토콜을 같이 적용하여 추후 만들 생각 이다.
* 위 버전을 복구하기 위한 링크
* https://github.com/korleinster/gamebusdriver/tree/64f9ddd196d45a0782d4a4d1861a07c27ef53a09	

클라이언트 변경 사항
===============

`16.09.06` [ **김형준** ] - *클라 기본 틀*, 통신 관련 코드는 없다. 윈도우 창으로 생성이 된다. 추후 통신 코드 수정할 예정
* 위 버전을 복구하기 위한 링크
* https://github.com/korleinster/gamebusdriver/tree/2dbb9f0d3b385690a9b16c9d18eec950ca8163b4


여기서부턴 README 및 GitHub 사용법
================================
기본적으로 텍스트 예쁘게 만드는 방법은 아래와 같다.
*Italic*, **bold**, and  `monospace`

fork 를 눌러서, 자기 github 으로 프로젝트를 복사해 오게 된다.
  * 마음대로 수정해도, 일단 내꺼에만 저장하게 되고, 원본 전체 마스터 프로젝트에는 변동이 없다.
  * 이렇게 점도 찍을 수 있고

> 왼쪽 앞에 만들 수 있고
>
> 선이 주륵주륵

전부다 수정하고 왼쪽 위의 New Pull Request 를 눌러 원본에 이걸 적용해 달라고 요청한다.
원본 소유자는 이 요청을 적용하고 수정적용한다.

* 이후 내용은 Pull requests 에서 close 된 내용을 살펴보자! 간단한 노잼 설명이 있다!
