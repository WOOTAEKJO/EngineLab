#pragma once
#include "MagicBuilder.h"

// ---------- MagicBuilder ---------------
// ������
#define U_NEW(T, ...) (make_unique_enabler_builder<T>(__VA_ARGS__).build())

#define S_NEW(T, ...) (make_shared_enabler_builder<T>(__VA_ARGS__).build())

// �ʱ�ȭ �Լ�
#define U_INIT(T, ...) (make_unique_enabler_builder<T>().Init(__VA_ARGS__))

#define S_INIT(T, ...) (make_shared_enabler_builder<T>().Init(__VA_ARGS__))

// ������ + �ʱ�ȭ �Լ�
#define U_MAKE(T, CTOR, INIT) (make_unique_enabler_builder<T> CTOR.Init INIT)

#define S_MAKE(T, CTOR, INIT) (make_shared_enabler_builder<T> CTOR.Init INIT)

#define UNIQUE_PTR(T)  unique_ptr<T,erased_deleter<T>>

template<typename T>
using u_ptr = unique_ptr<T, erased_deleter<T>>;