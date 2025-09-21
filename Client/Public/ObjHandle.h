#pragma once
#include "Engine_Defines.h"
#include "EngineAPI.h"

BEGIN(Engine)

template<class T = IGameObject>
class objHandle
{
public:
	objHandle() = default;
	objHandle(IObjectService* svc, T* p, string key)
		:m_svc(svc), m_ptr(p), m_key(move(key)) {} // �뿩 ����

	~objHandle() { reset(); } // �Ҹ��ڿ��� �ڵ� �ݳ� ȣ��

	objHandle(const objHandle&) = delete;				// ���� ������ ����
	objHandle& operator=(const objHandle&) = delete;	// ���� ������ ����
	// ���� �ݳ��� ���� ����

	objHandle(objHandle&& o) noexcept { swap(o); }

	objHandle& operator=(objHandle&& o) noexcept
	{
		if (this != &o)
		{
			reset();
			swap(o);
		}
		return *this;
	}// �̵� ���� : �ڱ� ���� ���� ��, ���� ���� reset()���� ���� �ݳ��ϰ� �� ���·� ��ü

	T* release() noexcept {
		T* p = m_ptr;
		m_ptr = nullptr;
		m_svc = nullptr;
		m_key.clear();
		return p;
	}

	void reset()
	{
		if (m_svc && m_ptr) { 
			m_svc->Release(m_ptr, m_key); // Ǯ ��ȯ
			m_ptr = nullptr;
			m_svc = nullptr;
			m_key.clear();
		}
		// �ڵ� Reales ȣ��
	}

	T*			get()			 noexcept { return m_ptr; }					// ���� ������ ����
	const T*	get()			const noexcept { return m_ptr; }
	T& 			operator*()				{ return *m_ptr; }
	const T&	operator*()		const { return *m_ptr; }
	T*			operator->()			{ return m_ptr; }
	const T*	operator->()	const { return m_ptr; }// �� �Լ��� ������ó�� ����ϱ� ����
	explicit	operator bool() const { return m_ptr != nullptr; }	// ��ȿ ���� üũ

	const string& key() const noexcept { return m_key; }

private:
	void swap(objHandle& o) noexcept
	{
		std::swap(m_svc, o.m_svc);
		std::swap(m_key, o.m_key);
		std::swap(m_ptr, o.m_ptr);
	}

private:
	IObjectService* m_svc = nullptr; // �ݳ��� ����ϴ� ����
	string			m_key;	 // Ǯ/������Ÿ�� ���� ��Ŷ �ĺ���.
	T*				m_ptr = nullptr; // ���� ������Ʈ �ּ�. ��ȿ�ϸ� �뿩��
};

END