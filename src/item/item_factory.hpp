#pragma once

#include <string>
#include <unordered_map>

namespace auc_monitor{

    class Item;
    class ItemFactory{
        public:
            ItemFactory();
            ~ItemFactory();
			ItemFactory(const ItemFactory&) = delete;
			ItemFactory& operator = (const ItemFactory&) = delete;

            void init();
            void addItem(const std::string& id, const std::wstring& name);
            Item* getItem(const std::string& id);
            Item* getItem(const std::wstring& id);

        private:
            std::unordered_map<std::wstring, Item*> name_to_item;
            std::unordered_map<std::string, Item*> id_to_item;
    };

}