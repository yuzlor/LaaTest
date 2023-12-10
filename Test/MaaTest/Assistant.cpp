#include "Assistant.h"

#include "Utils/Logger.hpp"

using namespace asst;

Assistant::Assistant()
{
	LogTraceFunction;

	m_msg_thread = std::thread(&Assistant::msg_proc, this);
	m_call_thread = std::thread(&Assistant::call_proc, this);
	m_working_thread = std::thread(&Assistant::working_proc, this);
}

Assistant::~Assistant()
{
	m_thread_exit = true;
	m_thread_idle = true;

	{
		std::unique_lock <std::mutex> lock(m_call_mutex);
		m_call_condvar.notify_all();
	}
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condvar.notify_all();
	}
	{
		std::unique_lock<std::mutex> lock(m_msg_mutex);
		m_msg_condvar.notify_all();
	}

	if (m_working_thread.joinable()) {
		m_working_thread.join();
	}
	if (m_call_thread.joinable()) {
		m_call_thread.join();
	}
	if (m_msg_thread.joinable()) {
		m_msg_thread.join();
	}
}

bool Assistant::start(bool block)
{
	LogTraceFunction;
	Log.info("Start |", block ? "block" : "non block");

	if (!m_thread_idle) {
		return false;
	}
	std::unique_lock<std::mutex> lock;
	if (block) {
		lock = std::unique_lock<std::mutex>(m_mutex);
	}
	m_thread_idle = false;
	m_running = true;
	m_condvar.notify_one();

	return true;
}

bool Assistant::stop(bool block)
{
	LogTraceFunction;
	Log.info("Stop |", block ? "block" : "non block");

	m_thread_idle = true;

	std::unique_lock<std::mutex> lock;
	if (block) { // 外部调用
		lock = std::unique_lock<std::mutex>(m_mutex);
	}
	//m_tasks_list.clear();

	//clear_cache();
	return true;
}

bool Assistant::running() const
{
	return m_running;
}

void Assistant::working_proc()
{
	LogTraceFunction;

	//std::vector<TaskId> finished_tasks;
	while (true) {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_thread_exit) {
			m_running = false;
			return;
		}

		if (m_thread_idle) {
			m_thread_idle = true;
			m_running = false;
			Log.flush();
			m_condvar.wait(lock);
			continue;
		}

		m_running = true;
		std::cout << "this is work proc" << std::endl;
	}
}

void Assistant::msg_proc()
{

}

void Assistant::call_proc()
{

}