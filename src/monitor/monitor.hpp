#pragma once

#include <mutex>
#include <stdint.h>
#include <string>
#include <functional>

namespace auc_monitor{
    class ThreadPool;
    class ItemFactory;
    class Item;
    class Monitor{
        public:
            uint64_t sell_offset = 4000;
            uint64_t lots = 20;

			Monitor();
			~Monitor();
			Monitor(const Monitor&) = delete;
			Monitor& operator = (const Monitor&) = delete;

            static Monitor* getInstance();
            ItemFactory* getItemFactory();
            void getToken();
            void initThreadpool();
            void shutdownThreadpool();
            ThreadPool* getThreadPool();
            int getAvailableThread();
            void checkInput();
            void shutdown();
            // ideally these methods need multi-threaded approach
            // but alas multiple requests from the same token/ip get ignored
            void checkSellHistory(Item* item, const int& qlt, const int& ptn);
            void checkActiveLots(Item* item, const int& qlt, const int& ptn);
            float rnd(const float& min, const float& max);
            void run();

        private:
            const char* demo_token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJhdWQiOiIxIiwibmJmIjoxNjczNzk3ODM4LCJleHAiOjQ4MjczOTc4MzgsImlhdCI6MTY3Mzc5NzgzOCwianRpIjoiYXhwbzAzenJwZWxkMHY5dDgzdzc1N2x6ajl1MmdyeHVodXVlb2xsZ3M2dml1YjVva3NwZTJ3eGFrdjJ1eWZxaDU5ZDE2ZTNlN2FqdW16Z3gifQ.ZNSsvwAX72xT5BzLqqYABuH2FGbOlfiXMK5aYO1H5llG51ZjcPvOYBDRR4HUoPZVLFY8jyFUsEXNM7SYz8qL9ePmLjJl6pib8FEtqVPmf9ldXvKkbaaaSp4KkJzsIEMY_Z5PejB2Vr-q-cL13KPgnLGUaSW-2X_sHPN7VZJNMjRgjw4mPiRZTe4CEpQq0BEcPrG6OLtU5qlZ6mLDJBjN2xtK0DI6xgmYriw_5qW1mj1nqF_ewtUiQ1KTVhDgXnaNUdkGsggAGqyicTei0td6DTKtnl3noD5VkipWn_CwSqb2Mhm16I9BPfX_d5ARzWrnrwPRUf6PA_7LipNU6KkkW0mhZfmwEPTm_sXPus0mHPENoVZArdFT3L5sOYBcpqwvVIEtxRUTdcsKp-y-gSzao5muoyPVoCc2LEeHEWx0cIi9spsZ46SPRQpN4baVFp7y5rp5pjRsBKHQYUJ0lTmh1_vyfzOzbtNN2v6W_5w9JTLrN1U6fhmifvKHppFSEqD6DameL1TC59kpIdufRkEU9HE4O-ErEf1GuJFRx-Dew6XDvb_ExhvEqcw31yNvKzpVqLYJfLazqn6tUbVuAiPwpy6rP9tYO2taT1vj5TGn_vxwDu9zoLWe796tFMPS-kmbCglxB5C9L4EbpfWNbWxYjUkTvjT2Ml9OnrB0UbYo1jI";
            const uint64_t auc_offset = 200;
            static Monitor* instance;
            ItemFactory* items;
            struct{
                std::string client_id;
                std::string client_secret;
            } client;
            struct{
                std::string token_type;
                int64_t expires;
                std::string access_token;
            } token;
            bool authorized = false;
            uint32_t runtimeSleep = 50;
            uint32_t inputSleep = 200;
            int inputThread;
            bool stopped = false;
            ThreadPool* threadpool = nullptr;
    };
}