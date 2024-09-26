#pragma once

#include <iostream>
#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#ifdef _WIN32
    #include <io.h>
#endif
#include <stdio.h>
#include <fcntl.h>
#include <boost/locale.hpp>
#include <boost/algorithm/string/case_conv.hpp>

template<typename T>
inline void concat_text(std::wstringstream& ss, T&& arg){
    ss << std::forward<T>(arg);
}

template<typename T, typename... Ts>
inline void concat_text(std::wstringstream& ss, T&& arg, Ts&&... args){
    ss << std::forward<T>(arg);
    concat_text(ss, std::forward<Ts>(args)...);
}

inline std::wstring toWstring(const char* str){
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);;
}

inline std::wstring toWstring(const std::string& str){
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);;
}

inline std::string toString(const std::wstring& str){
    return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

inline std::string toString(const wchar_t* str){
    return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

template<typename T, typename... Ts>
inline void send_error(T&& arg, Ts&&... args){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "ERROR: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    stream << '\n';
    wprintf(stream.str().c_str());
}

template<typename T>
inline void send_error(T&& arg){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "ERROR: ";
    stream << arg;
    stream << '\n';
    wprintf(stream.str().c_str());
}

template<typename T, typename... Ts>
inline void send_log(T&& arg, Ts&&... args){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "LOG: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    stream << '\n';
    wprintf(stream.str().c_str());
}

template<typename T>
inline void send_log(T&& arg){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "LOG: ";
    stream << arg;
    stream << '\n';
    wprintf(stream.str().c_str());
}

template<typename T, typename... Ts>
inline void send_debug(T&& arg, Ts&&... args){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "DEBUG: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    stream << '\n';
    wprintf(stream.str().c_str());
}

template<typename T>
inline void send_debug(T&& arg){
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    std::wstringstream stream;
    stream << "DEBUG: ";
    stream << arg;
    stream << '\n';
    wprintf(stream.str().c_str());
}

inline std::wstring to_item_name(const std::wstring& str){
    std::wstring w = boost::locale::to_lower(str);
    std::unordered_map<wchar_t,bool> map{
        {' ', true},
        {'1', true},
        {'2', true},
        {'3', true},
        {'4', true},
        {'5', true},
        {'6', true},
        {'7', true},
        {'8', true},
        {'9', true},
        {'0', true},
        {'-', true},
        {'_', true}
    };
    for(int i = 0; i < w.size(); i++){
        if(!iswalpha(w.at(i)) && !map.contains(w.at(i))) {
            w.replace(i,1,L"");
        }
    }
    return w;
}

inline void map_items(){
    nlohmann::json mapper;
    mapper["id_to_ru"] = {};

    std::string startPath = std::filesystem::current_path().string() + "/stalcraft-database/ru/items/";
    std::unordered_map<std::string, bool> usedPaths{};
    std::queue<std::string> paths{};
    std::string previousPath = "";
    std::string currentPath = "";
    while(true){
        if(paths.empty()){
            currentPath = startPath;
        }else{
            currentPath = paths.front();
            paths.pop();
        }
        if(currentPath == previousPath){
            break;
        }
        for(const auto& items : std::filesystem::directory_iterator(currentPath)){
            std::string path_dir = items.path().string() + "/";
            if(usedPaths.contains(items.path().string()) || usedPaths.contains(path_dir)) continue;
            if(std::filesystem::is_directory(items.path())){
                paths.push(path_dir);
                usedPaths[path_dir] = true;
            }else if(items.path().extension() == ".json"){
                nlohmann::json data;
                try{
                    std::ifstream mf(items.path());
                    data = nlohmann::json::parse(mf);
                    mf.close();
                }catch(const std::exception& e){
                    send_error(items.path().wstring(), " failed to initialize data");
                    usedPaths[items.path().string()] = true;
                    continue;
                }
                try{
                    mapper["id_to_ru"][data["id"].get<std::string>()] = data["name"]["lines"]["ru"].get<std::string>();
                }catch(const std::exception& e){
                    send_error(items.path().wstring(), " failed to access relevant info");
                    usedPaths[items.path().string()] = true;
                    continue;
                }
                usedPaths[items.path().string()] = true;
            }
        }
        previousPath = currentPath;
    }

    std::ofstream file("data/items/item_map.json");
    file << std::setw(4) << mapper;
    file.flush();
    file.close();
    send_debug("sorted items");
}