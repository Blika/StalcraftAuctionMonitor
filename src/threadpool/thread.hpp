#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

namespace auc_monitor{

    class Thread{
        public:
            Thread();
            ~Thread();
            Thread(const Thread&) = delete;
            Thread &operator=(const Thread&) = delete;
            Thread(Thread&&) = delete;
            Thread &operator=(Thread&&) = delete;

            int32_t getWeight();
            void addToQueue(const std::function<void()>& func, const int32_t& weight);
            bool busy();

        private:
            std::thread worker;
            // pairs of functions and their weight
            // weight determines how much resources the function is going to need
            // negative weight means function will take forever (loop)
            // thus we don't pick such threads for new jobs
            std::queue<std::pair<std::function<void()>, int32_t>> tasks;
            std::mutex tasksMutex;
            std::condition_variable condition;
            bool terminated = false;
            int32_t weight = 0;
            std::mutex weightMutex;

            // loop function
            void run();
    };
}