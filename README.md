# OpenGL을 이용한 태양계 모델

## 개요 및 환경설정

JoeyDeVries 의 Learn OpenGL 을 참고 및 응용 <https://github.com/JoeyDeVries/LearnOpenGL>

OpenGL 사용을 용이하게 하는 GLFW, GLAD 라이브러리 이용 <https://github.com/glfw/glfw>

Assimp(Open Asset Import Library) 를 통해 태양과 태양계 행성들, 소행성 obj 파일을 렌더링 <https://github.com/assimp/assimp>

Assimp의 라이브러리 생성 및 관리를 위한 CMAKE 이용 <https://cmake.org/>
--- -
플랫폼 도구 집합 Visual Studio 2019 (v142)

프로젝트-디버깅-환경 | 해당 프로젝트의 assimp 라이브러리 경로 (\...\dlls)

프로젝트-링커-입력-추가 종속성 | glfw3.lib;opengl32.lib;assimp-vc143-mtd.lib


## 구현 기능
###processInput 메소드
1. WASD를 이용하여 카메라 이동
2. 방향키 UP : 공전 멈추기 | 방향키 DOWN :  공전 다시 시작
3. 방향키 LEFT : 자전 속도 느리게 | RIGHT : 자전 속도 빠르게
###scroll_callback 메소드
마우스 휠 업/다운 : 카메라 줌 인/아웃

## 스크린샷
<img src="https://github.com/DialMyNumber/KDKSolSys/blob/master/asteroid/resources/screenshots/example1.png"></img>

수많은 소행성을 각각 렌더링 하면 프레임 드랍이 일어날 수 있음
그렇기 때문에 소행성 모델링 하나를 렌더링 하여 Instancing 기법을 사용함과 동시에 약간의 변위를 적용하여 최소한의 성능으로 자연스러운 소행성대 구현
--- -
<img src="https://github.com/DialMyNumber/KDKSolSys/blob/master/asteroid/resources/screenshots/example3.png"></img>

태양의 좌표에 Directional Light 적용하여 태양을 보고 있는 방향에선 밝게, 그렇지 않는 쪽은 어둡게 처리
--- -
<img src="https://github.com/DialMyNumber/KDKSolSys/blob/master/asteroid/resources/screenshots/example2.png"></img>

지구의 경우 태양 빛을 받지 않는 뒷면이라 하더라도 도시의 불빛이라는 독특한 특징을 가지고 있음
그렇기 때문에 지구 모델링을 반으로 나눠 태양을 향하는 앞면의 모델링과 뒷면 모델링을 각각 적용하여 렌더링 수행
