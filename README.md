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
|초기 화면 : light == 0|<img src="https://github.com/2023-SMU-Capstone-design/gles3jni/assets/87518434/3b344e18-2a58-4cca-a3db-d1ed61e3485c"></img>|
|다수 light 적용 화면|<img src="https://github.com/2023-SMU-Capstone-design/gles3jni/assets/87518434/b0153a1b-81c8-40b6-bee7-a7e7e89f5509"></img>|
|Unity 비교|<img src="https://github.com/2023-SMU-Capstone-design/gles3jni/assets/87518434/a5db96bd-eb39-40b4-802b-9205975f1914"></img>|
