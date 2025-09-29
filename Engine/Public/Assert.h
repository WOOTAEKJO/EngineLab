#pragma once

// 역할 : Dev/Soft/Hard 3단계 어서션 + 모드(Dev/Release) 정책 + 실패 콜백
// 역할 : 실패 시 Logger로 표준 포맷 ERROR|CORE|"... assert failed: ..." 남기고, 모드/정책에 따라 Break(디버그 브레이크) / Continue / Exit 수행.

void DevAssert() {}

void SoftAssert() {}

void HardAssert() {}