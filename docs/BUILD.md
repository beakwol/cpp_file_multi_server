# 빌드 방법

## 요구사항
- CMake 3.20 이상
- Visual Studio 2026 (MSVC)

## 빌드

```bash
cmake -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Debug
```

빌드 완료 시 `build/Debug/` 에 세 실행 파일이 생성됩니다.
```
build/Debug/
├── ParentServer.exe
├── ChildServer.exe
└── Client.exe
```

## 실행 순서

**1. 패런츠 서버 실행** (포트 56000 대기)
```
ParentServer.exe
```

**2. 클라이언트 실행**
```
Client.exe <서버IP> <파일경로>
```
예시:
```
Client.exe 127.0.0.1 test.txt
```

**3. 결과 확인**

`test_modified.txt` 생성 — 원본 파일에 헤더·타임스탬프 추가됨

## 포트
| 역할 | 포트 |
|------|------|
| 패런츠 서버 | 56000 |
| 차일드 서버 (1번째) | 56001 |
| 차일드 서버 (2번째) | 56002 |
| ... | +1씩 증가 |

