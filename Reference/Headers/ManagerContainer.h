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
		static_assert(is_base_of_v<IManager, M>, "M�� IManager�� ������ �����ϴ�.");

		if (auto* existing = Get<M>())
			return existing;

		auto m = make_unique_enabler_as_builder<IManager,M>().Init(forward<A>(args)...); // ���ڸ� �ʱ�ȭ �Լ� �ȿ� ���� �� �ְ� ����.
		
		M* raw = static_cast<M*>(m.get());
		m_Managers[typeid(M)] = move(m); // �Ŵ��� ���� �����̳� �ȿ� �Ŵ��� ��ü�� ���� ����� ������ ����.

		if constexpr (is_base_of_v<ITickable, M>) // tick �����̸� tick ���� �����̳� �ȿ� �߰�.
			m_Tickables.push_back(raw);

		return raw;
	}

	template<typename M>
	M* Get() noexcept
	{
		auto it = m_Managers.find(type_index(typeid(M)));
		
		if (it == m_Managers.end()) return nullptr;

		return static_cast<M*>(it->second.get()); // ITickable ���� m���� �ٿ� ĳ����
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
	}// tick ���� �Ŵ����� tick ������

	void ShutdownAll()
	{
		for (auto& [_, m] : m_Managers)
			m->Shutdown();
	}// ��� �˴ٿ�

private:
	unordered_map<type_index, unique_ptr<IManager,erased_deleter<IManager>>> m_Managers; // �Ŵ��� ���� �����̳�
	vector<ITickable*> m_Tickables; // tick ���� ���� �����̳�. �Ŵ������� �� �ϳ��̱� ������, �Ŵ������� �ּҸ��� �����Ͽ� �����ϴ� ���
};

END