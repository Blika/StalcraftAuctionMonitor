#include "item.hpp"
#include <sstream>

namespace auc_monitor{

    Item::Item(const std::string& i, const std::wstring& n): id{i}, name{n}{

    }

    Item::~Item(){

    }

    std::wstring Item::getFullName(const std::wstring& name, const int& qlt, const int& ptn){
        std::wstringstream str;
        switch(qlt){
            case 0:
                str << L"Обычный ";
                break;
            case 1:
                str << L"Необычный ";
                break;
            case 2:
                str << L"Особый ";
                break;
            case 3:
                str << L"Редкий ";
                break;
            case 4:
                str << L"Исключительный ";
                break;
            case 5:
                str << L"Легендарный ";
                break;
            case 6:
                str << L"Уникальный ";
                break;
        }
        str << name << " +" << ptn;
        return str.str();
    }

    std::wstring Item::getName(){
        return name;
    }

    std::string Item::getId(){
        return id;
    }

}