#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

using LayerID = _ubyte; //uint8_t; -> 1����Ʈ -> 8��Ʈ 
						// ������Ʈ�� ���� ���̾� id, �ִ� 32�� ����(��Ʈ����ũ�� 32��Ʈ�̱� ����)
						/*
							������ 8��Ʈ�� ������ 0 ~ 255 (2�� 8���� ����)������,
							AND ���� �浹 ����� �����ϸ� 32��Ʈ �߿��� 1�� �� �ϳ��� �����ؾ� �� -> �׷��� ���̾ 32��
							�׷��� 32���� ������ ����ϰ� �ִ� �浹 ��İ��� ���� �ʰ� �ȴ�.
						*/
using TagID = _uint; // uint32_t; -> 4����Ʈ -> 32��Ʈ  // ���ڿ� �±׸� ���� id�� ġȯ�� ������ �� ���� id

struct CollisionFilter // �浹 �з�/ ����ũ (�ڽ� 2D ��Ÿ��)
{
	_uint categoryBits = 1u; // � ī�װ�����. 1u -> 0��° ��Ʈ�� 1
	_uint maskBits = ~0u;
	/*
		- � ī�װ��� �浹�� ������.
		~0u -> ��� ��Ʈ�� 1-> ��� ī�װ��� �浹
			�̴� AND �����ڸ� ����ϱ� ����. ��� ��Ʈ�� 1�̸� ��� TRUE�̱� �����̴�.
		
		��, ���� �浹 ������ ��뵵 ���� ����ؾ� �ϹǷ� ����� maskBits�� �� categoryBits�� ������ �浹���� �ʴ´�.

		��)
			bit0: defalut	-> 0001b
			bit1: player	-> 0010b
			bit2: enemy		-> 0100b
			bit3: bullet	-> 1000b
				
			����		ī�װ� bit0, ����ũ ~0u(������)
			����		ī�װ� bit2, ����ũ ~0u(������)
			�÷��̾�	ī�װ� bit1, ����ũ 1110b (default�� ����)

			���̿� ���� �浹 �˻�
				����	ī�װ� 0001b , ����		����ũ 1111b => 0001b and 1111b => true
				����	ī�װ� 0100b , ����		����ũ 1111b => 0100b and 1111b => true
				����� �浹 ����

			���̿� �÷��̾� �浹 �˻�
				����		ī�װ� 0001b , �÷��̾�	����ũ 1110b => 0001b and 1110b => false
				�÷��̾�	ī�װ� 0010b , ����		����ũ 1111b => 0010b and 1111b => true
				����� �浹 ����
	*/
};

struct ObjectMeta
{
	LayerID layerID = 0;
	TagID	tagID = 0;

	CollisionFilter filter{};

	_bool	visible = true; // ���� �� ǥ�� ����
	_bool	paused = false; // ������Ʈ ����
	_double timescale = 1.f; // ���̾�/ ���� �ð� ������
};

struct SvHash
{
	using is_transparent = void;
	size_t operator()(string_view sv) const noexcept
	{
		return std::hash<std::string_view>{}(sv);
	}

	size_t operator()(const string& s) const noexcept
	{
		return (*this)(std::string_view{ s });
	}

	size_t operator()(const char* s) const noexcept
	{
		return (*this)(std::string_view{ s });
	}
};

struct SvEq
{
	using is_transparent = void;
	bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
};
// �� �� ����ü�� ���� lookup

class TagDB
{
public:
	TagID GetOrCreate(std::string_view name) // ID ��ȯ OR ID ���� �Լ�
	{
		auto it = m_NameToTagID.find(name);
		if(it != m_NameToTagID.end()) return it->second;

		TagID id = static_cast<TagID>(m_Names.size() + 1); // ������ + 1 ��ŭ �±�id ����
		m_Names.emplace_back(name);
		m_NameToTagID.emplace(m_Names.back(), id);
		return id;
	}

	TagID Find(std::string_view name) const // ã�Ƽ� ID�� ��ȯ
	{
		auto it = m_NameToTagID.find(name);
		if (it == m_NameToTagID.end()) return TagID(0);
		return it->second;
	}

	std::string_view Name(TagID id) const // �̸�(���ڿ�) ��ȯ
	{
		if (id == 0 || id > m_Names.size()) return {};
		return m_Names[id - 1];
	}

	/*
		string_view�� ����ϴ� ����
			- �Է� ���� �Ķ���� : �б⸸ �� ���� �Ҵ� ���� �޴´�. -> ���� �������� ����
			- �±װ� �̹� �����ϴ��� ��ȸ�� �� �� �ӽ� string ����/ �Ҵ��� ���� �� �ִ�.
				- ���� lookup ���� -> ��¥ ���Ҵ��� �� �� �ִ�.
					-> �����ؽ�, �����
			��, ���ɰ� ������鿡�� ����.
	*/

private:
	unordered_map<string, TagID,SvHash,SvEq> m_NameToTagID; // ���ڿ����� ID�� �ٲ� �͵��� ����
	vector<string> m_Names; // ���ڿ��� ����
};

END