#pragma once

#include <string>
#include <unordered_map>

namespace auc_monitor{

    class Item{
        public:
            Item(const std::string& id, const std::wstring& easy_name);
            ~Item();
            Item(const Item&) = delete;
            Item& operator = (const Item&) = delete;
            Item(Item&&) = delete;
            Item& operator=(Item&&) = delete;

            static std::wstring getFullName(const std::wstring& name, const int& qlt, const int& ptn);
            std::wstring getName();
            std::string getId();

        private:
            std::string id;
            std::wstring name;

    };
}