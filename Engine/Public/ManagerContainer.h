#pragma once
#include "Engine_Defines.h"
#include "IManager.h"

BEGIN(Engine)

class ManagerContainer
{
protected:
	ManagerContainer(){};

public:
	template<class M, class... A>
	M* Add(A&&... args)
	{
		static_assert(is_base_of_v<IManager, M>, "M은 IManager와 연관이 없습니다.");

		if (auto* existing = Get<M>())
			return existing;

		auto m = make_unique_enabler_as_builder<IManager,M>().Init(forward<A>(args)...); // 인자를 초기화 함수 안에 넣을 수 있게 만듬.
		
		M* raw = static_cast<M*>(m.get());
		m_Managers[typeid(M)] = move(m); // 매니저 관리 컨테이너 안에 매니저 객체를 새로 만들고 소유권 이전.

		if constexpr (is_base_of_v<ITickable, M>) // tick 가능이면 tick 가능 컨테이너 안에 추가.
			m_Tickables.push_back(raw);

		return raw;
	}

	template<typename M>
	M* Get() noexcept
	{
		auto it = m_Managers.find(type_index(typeid(M)));
		
		if (it == m_Managers.end()) return nullptr;

		return static_cast<M*>(it->second.get()); // ITickable 에서 m으로 다운 캐스팅
	}

	template<typename M>
	void Remove()
	{
		auto it = m_Managers.find(type_index(typeid(M)));

		if (it == m_Managers.end()) return;

		IManager* victim = it->second.get();

		m_Tickables.erase(remove(m_Tickables.begin(), m_Tickables.end(), victim),
			m_Tickables.end());

		victim->Shutdown();
		m_Managers.erase(it);
	}

	void RemoveAll()
	{
		m_Tickables.clear();

		for (auto& [_, m] : m_Managers)
			m->Shutdown();

		m_Managers.clear();
	}

	void TickAll(_double dt){
		for (auto* t : m_Tickables)
			t->Tick(dt);
	}// tick 가능 매니저들 tick 돌리기

	void ShutdownAll()
	{
		for (auto& [_, m] : m_Managers)
			m->Shutdown();
	}// 모두 셧다운

private:
	unordered_map<type_index, unique_ptr<IManager,erased_deleter<IManager>>> m_Managers; // 매니저 관리 컨테이너
	vector<ITickable*> m_Tickables; // tick 가능 관리 컨테이너. 매니저들은 단 하나이기 때문에, 매니저들의 주소만을 저장하여 접근하는 방식
};

END