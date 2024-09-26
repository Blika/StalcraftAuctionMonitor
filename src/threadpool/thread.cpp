#include "thread.hpp"

namespace auc_monitor{

    Thread::Thread(){
        worker = std::thread(&Thread::run, this);
    }

    Thread::~Thread(){
        {
            std::lock_guard<std::mutex> lock(tasksMutex);
            terminated = true;
            condition.notify_one();
        }
        worker.join();
    }

    int32_t Thread::getWeight(){
        int32_t w;
        {
            std::lock_guard<std::mutex> lock(weightMutex);
            w = weight;
        }
        return w;
    }

    void Thread::addToQueue(const std::function<void()>& func, const int32_t& w){
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.push(std::make_pair(std::move(func), w));
        condition.notify_one();
        std::lock_guard<std::mutex> lock1(weightMutex);
        weight += w;
    }

    bool Thread::busy(){
        bool v;
        {
            std::lock_guard<std::mutex> lock(tasksMutex);
            v = tasks.empty();
        }
        return !v;
    }

    void Thread::run(){
        while(true){
            std::pair<std::function<void()>, int32_t> pair;
            {
                std::unique_lock<std::mutex> lock(tasksMutex);
                condition.wait(lock, [this]{
                    return !tasks.empty() || terminated;
                });
                if(terminated){
                    break;
                }
                pair = tasks.front();
            }
            pair.first();
            {
                std::unique_lock<std::mutex> lock(tasksMutex);
                tasks.pop();
                condition.notify_one();
            }
            std::unique_lock<std::mutex> lock1(weightMutex);
            weight -= pair.second;
        }
    }
}