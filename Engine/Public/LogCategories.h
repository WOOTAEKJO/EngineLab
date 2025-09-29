#pragma once

/*
	카테고리 목록
	CORE, TIME, JOBS, RES, POOL, HANDLE, GFX, IO, INPUT, AUDIO, PHYS, SCENE, PERF

	로그 한 줄 포맷
	HH:MM:SS.mmm | F=<frame> | T=<thread> | <LEVEL> | <CATEGORY> | <MESSAGE> | {k=v,...}

	레이트 리밋 규칙 주석 한 줄
	같은 (Level + Category + Message)는 1초에 1회만 상세, 나머지는 suppressed 집계

*/
// 역할 : 레벨,카테고리 열거 + 문자열 변환만 제공