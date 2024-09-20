# core Flight System (cFS) Picam Application
## Introduction
본 cFS Application은 Raspberry Pi4 Model B에 연결한 Camera Module 2를 제어하기 위한 앱입니다. 수신한 명령에 따라 사진을 촬영하여 지정된 디렉토리에 저장합니다.<br>

다음은 cFS Bundle에 포함된 GroundSystem을 활용해 noop command, reset command, capture command를 테스트한 결과와 촬영된 사진입니다.

<p align="center"><img src="images\images1.jpg" width="400" height="300"></p>

<p align="center"><img src="images\GroundSystem.png" width="650" height="250"></p>


## Required Software
cFS Framework (cFE, OSAL, PSP)를 요구합니다.<br>
cFE, OSAL, PSP를 포함한 통합 번들은 다음을 참고하세요.<br>
[core Flight System](https://github.com/nasa/cFS)


## About cFS
cFS는 나사 고다드 우주 비행 센터(NASA Goddard Space Flight Center)에서 개발된 소프트웨어 프레임 워크로, 기본적으로 인공위성의 비행 소프트웨어(FSW) 또는 다른 임베디드 시스템 계열에 사용이 가능합니다.<br>
cFS는 재사용 가능한 소프트웨어 프레임워크로, 여러 애플리케이션과 cFE, 추상화계층 등을 포함하고 있습니다.<br>
cFS와 관련된 상세 설명은 다음을 참고하세요.<br>
[Details about cFS](http://cfs.gsfc.nasa.gov)