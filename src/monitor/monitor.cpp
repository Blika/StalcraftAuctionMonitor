#include "monitor.hpp"
#include "../command/command_executor.hpp"
#include "../item/item_factory.hpp"
#include "../item/item.hpp"
#include "../utils/utils.hpp"
#include "../threadpool/threadpool.hpp"
#include <curl/curl.h>
#include <chrono>
#include <thread>
#include <random>

namespace auc_monitor{

	Monitor* Monitor::instance = nullptr;

    Monitor::Monitor(){
        instance = this;
        nlohmann::json properties;
	    try{
            std::ifstream mf("data/client.json");
            properties = nlohmann::json::parse(mf);
            mf.close();
        }catch(const std::exception& e){
            throw std::runtime_error("failed to initialize data");
	    }
        runtimeSleep = properties["runtime_sleep"].get<uint32_t>();
        inputSleep = properties["input_sleep"].get<uint32_t>();
        client.client_id = properties["client_id"].get<std::string>();
        client.client_secret = properties["client_secret"].get<std::string>();
        getToken();
        items = new ItemFactory();
        items->init();
        initThreadpool();
    }

    Monitor::~Monitor(){
        if(!stopped) shutdown();
    }

    Monitor* Monitor::getInstance(){
        return instance;
    }

    ItemFactory* Monitor::getItemFactory(){
        return items;
    }
    
    size_t writeCallback(char* contents, size_t size, size_t nmemb, void* userp){
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    void Monitor::getToken(){
        CURL *curl = curl_easy_init();
        if(curl){
            send_log("POSTing to exbo.net");
            std::string data = (std::string)"client_id="+client.client_id+"&client_secret="+client.client_secret+"&grant_type=client_credentials&scope=";
            std::string json;
            curl_easy_setopt(curl, CURLOPT_URL, "https://exbo.net/oauth/token");
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data.c_str()));
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
	        try{
                nlohmann::json t = nlohmann::json::parse(json);
                token.token_type = t["token_type"].get<std::string>();
                token.access_token = t["access_token"].get<std::string>();
                token.expires = t["expires_in"].get<int64_t>();
                authorized = true;
                send_log("Authorized");
            }catch(const std::exception& e){
                send_error("Failed to authorize, using demo api");
                send_error(toWstring(json));
	        }
        }
        curl_global_cleanup();
    }

    void Monitor::initThreadpool(){
        threadpool = new ThreadPool();
        threadpool->assignNewTask(std::bind(&Monitor::checkInput, this),-1);
    }

    void Monitor::shutdownThreadpool(){
        delete threadpool;
    }

    ThreadPool* Monitor::getThreadPool(){
        return threadpool;
    }

    void Monitor::checkInput(){
        #ifdef _WIN32
            _setmode(_fileno(stdin), _O_U16TEXT);
        #endif
        while(true){
            if(stopped) break;
            std::wstring in;
            std::getline(std::wcin, in);
            #ifdef _WIN32
                std::wcin.ignore();
            #endif
            CommandExecutor::execute(in);
            std::this_thread::sleep_for(std::chrono::milliseconds(inputSleep));
        }
    }

    void Monitor::shutdown(){
        if(stopped) return;
        stopped = true;
        delete items;
        send_debug("Shutting down threadpool...");
        shutdownThreadpool();
    }

    void Monitor::checkSellHistory(Item* item, const int& qlt, const int& ptn){
        int offset = 0;
        int found = 0;
        while(true){
            if(found >= lots) break;
            CURL *hnd = curl_easy_init();
            if(hnd){
                std::string json;
                curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
                curl_easy_setopt(hnd, CURLOPT_URL, ("https://"+(std::string)(authorized ? "e" : "d")+"api.stalcraft.net/ru/auction/"+item->getId()+"/history?offset="+std::to_string(offset)+"&additional=true&limit=200").c_str());
                curl_easy_setopt(hnd, CURLOPT_VERBOSE, 0L);
                curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &json);
                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, ("Authorization: Bearer "+(authorized ? token.access_token : demo_token)).c_str());
                curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
                CURLcode ret = curl_easy_perform(hnd);
                curl_easy_cleanup(hnd);
                try{
                    nlohmann::json t = nlohmann::json::parse(json);
                    for(auto& [k,v]: t["prices"].items()){
                        int q,p;
                        if(!v["additional"].contains("qlt") && qlt > 0){
                            continue;
                        }else if(!v["additional"].contains("qlt") && qlt <= 0){
                            q = 0;
                        }else{
                            q = v["additional"]["qlt"].get<int>();
                            if(q != qlt && qlt > -1) continue;
                        }
                        if(!v["additional"].contains("ptn") && ptn > 0){
                            continue;
                        }else if(!v["additional"].contains("ptn") && ptn <= 0){
                            p = 0;
                        }else{
                            p = v["additional"]["ptn"].get<int>();
                            if(p != ptn && ptn > -1) continue;
                        }
                        found++;
                        uint64_t price = v["price"].get<uint64_t>();
                        std::string time = v["time"].get<std::string>();
                        send_debug(Item::getFullName(item->getName(), q, p)," ",price," ",toWstring(time));
                        if(found >= lots) break;
                    }
                }catch(const std::exception& e){
                }
            }
            curl_global_cleanup();
            offset += auc_offset;
            if(offset >= sell_offset){
                break;
            }
        }
        if(found == 0){
            send_debug("no such item in history (", sell_offset, " sells)");
        }
    }

    void Monitor::checkActiveLots(Item* item, const int& qlt, const int& ptn){
        int offset = 0;
        int found = 0;
        while(true){
            CURL *hnd = curl_easy_init();
            if(hnd){
                std::string json;
                curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
                curl_easy_setopt(hnd, CURLOPT_URL, ("https://"+(std::string)(authorized ? "e" : "d")+"api.stalcraft.net/ru/auction/"+item->getId()+"/lots?additional=true&limit=200&offset="+std::to_string(offset)+"&order=desc&sort=time_created").c_str());
                curl_easy_setopt(hnd, CURLOPT_VERBOSE, 0L);
                curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &json);

                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, ("Authorization: Bearer "+(authorized ? token.access_token : demo_token)).c_str());
                curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

                CURLcode ret = curl_easy_perform(hnd);
                curl_easy_cleanup(hnd);
                try{
                    nlohmann::json t = nlohmann::json::parse(json);
                    for(auto& [k,v]: t["lots"].items()){
                        int q,p;
                        if(!v["additional"].contains("qlt") && qlt > 0){
                            continue;
                        }else if(!v["additional"].contains("qlt") && qlt <= 0){
                            q = 0;
                        }else{
                            q = v["additional"]["qlt"].get<int>();
                            if(q != qlt && qlt > -1) continue;
                        }
                        if(!v["additional"].contains("ptn") && ptn > 0){
                            continue;
                        }else if(!v["additional"].contains("ptn") && ptn <= 0){
                            p = 0;
                        }else{
                            p = v["additional"]["ptn"].get<int>();
                            if(p != ptn && ptn > -1) continue;
                        }
                        found++;
                        uint64_t price = v["buyoutPrice"].get<uint64_t>();
                        send_debug(Item::getFullName(item->getName(), q, p)," ",price);
                    }
                }catch(const std::exception& e){
                }
            }
            curl_global_cleanup();
            offset += auc_offset;
            if(offset >= sell_offset){
                break;
            }
        }
        if(found == 0){
            send_debug("no such item on auction");
        }
    }

	float Monitor::rnd(const float& min, const float& max){
        std::default_random_engine rnd_eng{std::random_device{}()};
		std::uniform_real_distribution<float> rnd_dist(min,max);
		return rnd_dist(rnd_eng);
	}

    void Monitor::run(){
        while(!stopped){
        }
    }

}