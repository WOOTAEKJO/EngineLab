#pragma once

/*
	키 문자열
	FrameMs, FPS, MemMB, JobsQueued, JobsRunning, JobLatencyMs, SpawnPerSec, ReleasePerSec, ExhaustRatePct,
	GrowCount, AvgHoldMs, HandleInvalid, IOPending, ApplyQueue

*/

// 역할 : 카운터/게이지/윈도우 평균/히스토그램(경량) + HUD 3줄 문자열 생성

void Counter() {}

void Gauge() {}

void Histogram() {}