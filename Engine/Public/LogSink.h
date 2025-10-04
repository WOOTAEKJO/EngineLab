#pragma once
#include "LogCategories.h"

// ���� : ��ũ �������̽��� ���� �̺�Ʈ ����

BEGIN(Engine::log)

// ����ȭ�� �ΰ� ���� -> ��谡 ����/�˻��ϱ� ���� ������
struct Fileds
{
	// �ٸ� Ÿ���� ������ �޴� ���
	template <typename T>
	void Add(string Key,const T& value)
	{
		m_kv.emplace_back(move(Key), move(to_string(value)));
	}

	// ���ڿ��� ������ �޴� ���
	void Add(string key, string value)
	{
		//m_kv.push_back({ key,value }); -> pair�� �ӽ� ��ü�� ����� ����� �� ��. ���� ���� ���� �����ϴ� ����� ��
		m_kv.emplace_back(move(key), move(value)); 
		// -> �ӽ� ��ü�� ������ �ʰ� �ٷ� ���� �޸� �ȿ��� ���� ����. move �Լ��� �������� �̵��Ͽ� ���� ����� ����.
		// ��, ����� �鿡�� ȿ���� ����.

	}

	// ���� �����̳� �ȿ�  �ִ� ��� ���ڿ����� �߰�ȣ �ȿ� �־� ��ȯ.
	string Get_Str()
	{
		if (m_kv.empty()) return "{}";

		string str = "{ ";

		int iSize = m_kv.size();

		for (int i = 0; i < iSize; i++)
		{
			str += m_kv[i].first;
			str += " = ";
			str += m_kv[i].second;

			if (i + 1 < iSize)
				str += ", ";
		}

		str += " }";
		return str;
	}

private:
	vector<pair<string, string>>	m_kv;
};

// �� �� �α��� �ϼ��� ���� ����
struct Event
{
	LEVEL								level;		//����ü ����
	CATEGORY							cat;		//����ü ī�װ�
	string								msg;		//�޽��� ���� �뵵
	Fileds								fileds;		// ����ü Fileds
	uint32_t							threadID;	// ������ ���̵� ���� -> 4����Ʈ
	uint64_t							frame;		// ������ ���� -> 8����Ʈ -> ������ ����/���� ����
	chrono::system_clock::time_point	wall;		// ���� �ð��� ǥ���ϱ� ���� ���� -> ����� ���� �ð�
};

// �α� �� ���� ����, ��� ��������� �����ϴ� ��� �÷����� �������̽�
struct ISink
{
protected:
	virtual ~ISink() = default;

public:
	virtual void OnMessage(const Event& e, const string& formatted) = 0;
};

END