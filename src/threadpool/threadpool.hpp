#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace auc_monitor{
    class Thread;
    class ThreadPool{
        public:
            ThreadPool();
            ~ThreadPool();
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool &operator=(const ThreadPool&) = delete;
            ThreadPool(ThreadPool&&) = delete;
            ThreadPool &operator=(ThreadPool&&) = delete;

            void assignNewTask(const std::function<void()>& func, const int32_t& weight);
            void wait();

        private:
            std::vector<Thread*> threads;

            uint32_t getLeastBusyThread();
    };
}