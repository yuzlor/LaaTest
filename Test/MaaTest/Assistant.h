#pragma once

#include <condition_variable>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "Common/AsstMsg.h"
#include "Common/AsstTypes.h"

struct AsstExtAPI
{
public:
	using TaskId = int;
	using AsyncCallId = int;

	virtual ~AsstExtAPI() = default;

	// 开始执行任务队列
	virtual bool start(bool block = true) = 0;
	// 停止任务队列并清空
	virtual bool stop(bool block = true) = 0;
	// 是否正在运行
	virtual bool running() const = 0;

};

namespace asst
{

	class Assistant : public AsstExtAPI
	{
	public:
		Assistant();
		virtual ~Assistant() override;

		virtual bool start(bool block = true) override;
		virtual bool stop(bool block = true) override;
		virtual bool running() const override;

	private:
		void call_proc();
		void working_proc();
		void msg_proc();

	private:

		std::atomic_bool m_thread_exit = false;


		std::atomic_bool m_thread_idle = true;
		std::atomic_bool m_running = false;
		mutable std::mutex m_mutex;
		std::condition_variable m_condvar;

		std::mutex m_msg_mutex;
		std::condition_variable m_msg_condvar;

		std::mutex m_call_mutex;
		std::condition_variable m_call_condvar;
		std::mutex m_completed_call_mutex;
		std::condition_variable m_completed_call_condvar;

		std::thread m_msg_thread;
		std::thread m_call_thread;
		std::thread m_working_thread;
	};
}