#include "threadpool.hpp"
#include "thread.hpp"
#include <stdexcept>
#include <thread>

namespace auc_monitor{

    ThreadPool::ThreadPool(){
        const uint32_t max_threads = std::thread::hardware_concurrency();
        threads.resize(max_threads);
        for(uint32_t i = 0; i < max_threads; ++i){
            threads[i] = new Thread();
        }
    }

    ThreadPool::~ThreadPool(){
        for(auto& t: threads){
            delete t;
        }
        threads.clear();
    }

    void ThreadPool::assignNewTask(const std::function<void()>& func, const int32_t& weight){
        uint32_t t = getLeastBusyThread();
        threads[t]->addToQueue(std::move(func),weight);
    }

    void ThreadPool::wait(){
        for(auto& t: threads){
            while(t->getWeight() > -1 && t->busy()){}
        }
    }

    uint32_t ThreadPool::getLeastBusyThread(){
        uint32_t t = 0;
        int32_t w = INT32_MAX;
        size_t size = threads.size();
        for(auto i = 0; i < size; ++i){
            const int32_t tw = threads[i]->getWeight();
            if(tw >= 0 && tw < w){
                t = i;
                w = tw;
            }
            if(w == 0) break;
        }
        return t;
    }

}