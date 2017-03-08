# university-portfolio  
------------------------------
대학 시절에 수행했던 프로젝트에 대한 포트폴리오입니다.  
이 저장소에는 포트폴리오 내 프로젝트들의 소스코드와 프로젝트에 대한 설명이 담긴 프리젠테이션 자료가 있습니다.  
다음 목차를 참고하여 저장소를 살펴보시기 바랍니다.  

- 각 프로젝트의 소스코드는 당시의 흔적을 최대한 보존하기 위해 수정을 하지 않았습니다.  
현재의 코딩 스타일은 과거의 것과 많이 다를 수 있으며, 나아지기 위해 지속적으로 노력하고 있습니다.
- 각 프로젝트에 대한 자세한 설명은 프리젠테이션 자료에 있습니다.
- 일부 프로젝트는 실행환경을 갖추지 못해 소스코드만 제공합니다.  
코딩스타일을 확인하는 용도로 이용해 주시고, 실행화면은 프리젠테이션 자료에서 확인하실 수 있습니다.

## 1. '손으로 움직이는 세상' 프로젝트 - 'HandControlInterface' 디렉토리
적외선, 웹캠, 오픈소스 OpenCV를 이용한 C++ 기반 손동작 인식 인터페이스를 개발했습니다.  
적외선 인식용 웹캠을 통해 손동작을 인식해서 마우스 및 키보드의 일부 키를 제어할 수 있습니다.  

### 디렉토리 구성
- **handcontrolinterface.cpp** : 본 프로젝트의 소스코드로 실행환경을 조성하기 어려워 소스코드만 제공합니다.

## 2. 'Fast Shopper' 프로젝트 - 'FastShopper' 디렉토리
오픈소스 Zbar를 이용한 바코드 인식 안드로이드 앱과 간단한 Java Swing 기반 서버를 개발했습니다.  
안드로이드 앱을 이용해 상품들의 바코드를 찍고, 서버와 연동해 상품목록 확인 후 결제하는 서비스를 제안한 프로젝트입니다.  
안드로이드 앱은 클라이언트 역할을, Java Swing 서버는 서버 역할을 하며, 클라이언트와 서버는 TCP를 이용해 통신합니다.

### 디렉토리 구성
- **FastShopper** : 클라이언트에 해당하는 안드로이드 앱의 소스코드입니다.
- **FastShopperServer** : 서버에 해당하는 Java Swing 기반 서버의 소스코드입니다.
- **fs_product_data.txt** : 재고상품 목록에 해당하는 DB 파일입니다.  
서버에서 사용하는 파일로 파일에 재고상품 목록을 작성 후, C드라이브의 절대경로(C:\fs_product_data.txt)에 위치시킵니다.

## 3. 'Mini-CPL 인터프리터' 프로젝트 - 'Mini-CPLInterpreter' 디렉토리
간단한 수식 계산용 언어를 설계하고, 그것을 해석하는 C++ 기반 인터프리터를 개발했습니다.  
이 수식 계산용 언어는 매우 간단한 문법으로 설계되었으며, 변수 선언 및 기본 수식 계산 기능만 가지고 있습니다.  
이 인터프리터는 해당 언어에 대한 구문 검사 등의 기본적인 인터프리터 기능을 갖추고 있습니다.

### 디렉토리 구성
- **Mini-CPLInterpreter 소스** : 본 프로젝트의 소스코드와 테스트 파일인 *test.mcpl* 파일입니다.  
일부 문법을 이용하기 위해 C++을 사용했으나, 주로 C 스타일로 VS 2008에서 개발했습니다. main 소스는 *mcpli.cpp* 파일입니다.
- **Mini-CPLInterpreter 실행파일** : 실행파일인 *Mini-CPLInterpreter.exe* 파일과 테스트 파일인 *test.mcpl* 파일입니다.  
원하신다면 Mini-CPL 문법에 맞춰 직접 mcpl 파일을 작성 후 테스트를 할 수 있습니다.

## 4. 'Cocoa-Unix 기반 채팅 시스템' 프로젝트 - 'SimpleChatting' 디렉토리
Objective-C Cocoa 기반 채팅 클라이언트와 Unix C 기반 채팅 서버를 개발했습니다.  
TCP 통신 기반이며, 상호 배제를 이용해 최대한 안정적인 서버를 만드는 데 목표를 두고 개발했습니다.  
간단한 로비-방 구조의 채팅 시스템이고, 편의상 사용자 ID들을 미리 설정해놨습니다.  

### 디렉토리 구성
- **SimpleChatClient** : Objective-C Cocoa 기반 채팅 클라이언트에 해당하는 소스코드입니다.
- **SimpleChatServer** : Unix C 기반 채팅 서버에 해당하는 소스코드입니다.  
MacOS 환경에서 실행하는 것이 유리하며, main 소스는 *simplechat_server.c* 파일입니다.

## 5. '사진 관리 프로그램 갈무리' 프로젝트 - 'PhotoGalmuri' 디렉토리
이미지 파일 관리 및 편집용 MFC 프로그램을 개발했습니다.  
본 프로그램에 다수의 이미지를 불러와 검색 및 정렬을 할 수 있으며, 간단한 이미지 편집 기능 또한 제공합니다.  
SDI 프로젝트로 진행했으며, 간단한 영상처리를 사용하고, 파일 DB를 이용해 관리 대상 이미지 목록을 보존합니다.  
'갈무리'는 물건 따위를 잘 정리해 간수한다는 의미이며, 전체적으로 한글이나 순우리말을 이용해 외관을 디자인했습니다.

### 디렉토리 구성
- **PhotoGalmuri 소스** : 본 프로젝트의 소스코드와 DB 파일인 *phogaldata.dat* 파일입니다.
- **PhotoGalmuri 실행파일** : 실행파일인 *PhotoGalmuri.exe* 파일, DB 파일인 *phogaldata.dat* 파일, 그리고 예시 이미지를 모아놓은 *examples* 디렉토리입니다. 이 예시 자료를 이용하거나 다른 이미지 파일을 이용해 테스트할 수 있습니다.
