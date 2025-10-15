# 🧭 C++ - Object-Oriented Program Project — A* Pathfinding Sandbox

**무작위/이동형 장애물**이 있는 격자 맵에서, 캐릭터가 **A\*** 알고리즘으로 목적지까지 **실시간 경로 탐색 및 이동**합니다.  
클래스·상속·오버라이딩·오버로딩 등 **객체지향 설계**와 **그래픽스(SFML)**, **파일 입출력**을 통합한 프로젝트입니다.

---

## 🧱 Create Walls at Random Locations
<img width="768" height="505" alt="image" src="https://github.com/user-attachments/assets/6ceefd64-7652-47d1-a5c5-eb46ab55964b" />

## 🧠 Pathfinding to the Destination with the A* Algorithm
<img width="739" height="418" alt="image" src="https://github.com/user-attachments/assets/1313fceb-047d-4083-bc75-f08abed157dc" />

---

## 🕹️ 작동법 (Controls)
- **좌클릭:** 목적지 설정  
- 캐릭터가 **A\***로 최단 경로 탐색 → **프레임마다 재탐색**하며 이동  
- **장애물이 움직여도** 경로를 즉시 갱신 (visit/path 셀 시각화)

---

## ✨ 주요 기능 (Features)
- **무작위 Bar형 장애물 10개 생성** 및 **상/하 왕복 이동**  
- A\* (대각선 비용 √2) + 휴리스틱(**Octile distance**)  
- **프레임 제한(약 60 FPS)**, 프레임당 탐색 스텝(`CHECKS_PER_FRAME`) 조절  
- **입력 복구/검증** 및 **실시간 그리기**(SFML)  
- **파일 로그**: 이동 시 `Count.txt`에 `"move count: N"` 누적 기록  
- **템플릿/람다/가상함수/상속** 등 C++ 언어 요소 폭넓게 사용

---

## 🧩 코드 구조 (OOP 설계 개요)
- `namespace gbl::{SCREEN, MAP, PATHFINDING}`: 전역 상수·타입(맵/좌표/유틸)  
- **렌더링**: `draw_map()`가 셀 상태(Empty/Visited/Path/Wall/Start/Finish)에 따라 색/스프라이트 처리  
- **A\***:
  - `calculate_h_score()` — **Octile** 휴리스틱  
  - `get_adjacent_cells()` — 대각 포함 인접 셀(벽 제외)  
  - `astar_search()` — f=g+h 최소 셀 선택 → 갱신/추적/역추적(stack)  
  - `astar_reset()` — 점수 맵 초기화, 시작 셀 push
- **장애물**:
  - `Block::barblock()` — **세로 Bar** 시작점 무작위, 겹침/가까운 열 피하기, 빈 칸 보장  
  - 메인 루프에서 **왕복 이동** 및 맵 상태 업데이트
- **파일 I/O**:
  - `File`(추상) → `IFile`(구현): `addcount()`/`Count()` **오버로딩**  
  - 이동 목적지 지정 시 카운트 증가(람다 `add`) 및 특정 줄 출력 기능

---

## 🎯 프로젝트 의의 & 소감

- **C++ 핵심기술 통합**: 클래스·상속·가상함수·템플릿·람다·네임스페이스·파일 I/O·SFML 렌더링을 하나의 실시간 시스템으로 엮어 **설계—구현—시각화** 전 과정을 경험.  
- **A\*** **시각화 학습**: `Visited/Path` 표시로 휴리스틱·우선순위 탐색 개념을 **직관적으로 이해**, 프레임 단위 재탐색과 장애물 갱신으로 **실시간 시스템 감각** 습득.  
- **확장 가능 구조**: 우선순위 큐 최적화, 가중치 지형, 여러 에이전트, 맵 에디터 등으로의 **기능 확장 기반** 확보.  
- **협업·테스트 친화**: 모듈화된 네임스페이스/함수/클래스 구조로 유지보수·코드 리뷰·단위 검증 용이.
- 눈앞에서 경로가 그려지고 캐릭터가 움직이니 추상적이던 알고리즘과 OOP 개념이 **손에 잡혔고**, 코드를 더 **구조적으로 생각**하게 됨.
- 게임형식으로 구현하니 재미있었음.



