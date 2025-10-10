#include "../Public/Logger.h"

Engine::log::Logger::Logger()
{
}

Engine::log::Logger::~Logger()
{
}

void Engine::log::Logger::AddSink(ISink* sink)
{
    std::lock_guard<std::mutex> lk(m_mutex); // ���ؽ� ���

    m_vecSink.push_back(sink);
}

void Engine::log::Logger::SetMinLevel(LEVEL lev)
{
    m_minLevel = lev;
}

void Engine::log::Logger::SetFrame(uint64_t frame)
{
    m_frame = frame;
}

uint64_t Engine::log::Logger::Frame() const
{
    return m_frame;
}

void Engine::log::Logger::Write(LEVEL lev, CATEGORY cat, string msg, Fileds filed)
{
    if (m_minLevel > lev) return;

    auto now = chrono::steady_clock::now(); // ���� �ð� üũ

    RateKey key{lev,cat,msg}; // ���ڷ� ���� ������ Ű�� ����
    _bool suppressed = false;
    size_t suppressedCount = 0;

    lock_guard<mutex> lk(m_mutex); // ���ؽ� �� ���

    auto& st = m_mapRate[key]; // �� �����̳ʿ��� Ű�� �´� ���� ������.
    auto sec = chrono::duration_cast<chrono::seconds>(now - st.last).count(); // ���ݰ� last�� �� ������ ���
    
    if (sec == 0)
        suppressed = true; // ���� ���̸� �̹� �α״� ����
    else
        st.last = now;

    if (suppressed) return;

    Event e;
    e.level = lev;
    e.cat = cat;
    e.msg = move(msg);
    e.fileds = move(filed);
    e.frame = Frame();
    e.threadID = ThreadID();
    e.wall = chrono::system_clock::now();
    //auto s = Format();
}

void Engine::log::Logger::FrameStart(uint64_t frame)
{
}

void Engine::log::Logger::FrameEnd(uint64_t frame, _double cpuMs)
{
}

string Engine::log::Logger::FormatTime(chrono::system_clock::time_point time)
{
    return string();
}

void Engine::log::Logger::Format()
{
}

void Engine::log::Logger::Broadcast()
{
}

uint32_t Engine::log::Logger::ThreadID()
{
    return uint32_t();
}
