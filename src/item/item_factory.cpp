#include "item_factory.hpp"
#include "item.hpp"
#include "../utils/utils.hpp"

namespace auc_monitor{

    ItemFactory::ItemFactory(){

    }

    ItemFactory::~ItemFactory(){
        for(auto& [id, it]: id_to_item){
            delete id_to_item[id];
        }
        id_to_item.clear();
        name_to_item.clear();
    }

    void ItemFactory::init(){
        if(!id_to_item.empty()){
            for(auto& [id, it]: id_to_item){
                delete id_to_item[id];
                id_to_item.erase(id);
            }
            for(auto& [id, it]: name_to_item){
                name_to_item.erase(id);
            }
        }
        nlohmann::json map;
	    try{
            std::ifstream mf("data/items/item_map.json");
            map = nlohmann::json::parse(mf);
            mf.close();
        }catch(const std::exception& e){
            throw std::runtime_error("failed to initialize items data");
	    }
        for(auto& [k,v]: map["id_to_ru"].items()){
            std::string vv = v.get<std::string>();
            addItem(k, to_item_name(toWstring(vv)));
        }
        send_debug("initialized items");
    }

    void ItemFactory::addItem(const std::string& id, const std::wstring& name){
        id_to_item[id] = new Item(id, name);
        name_to_item[name] = id_to_item[id];
    }

    Item* ItemFactory::getItem(const std::string& id){
        if(id_to_item.contains(id)){
            return id_to_item[id];
        }else if(name_to_item.contains(toWstring(id))){
            return name_to_item[toWstring(id)];
        }else{
            return nullptr;
        }
    }

    Item* ItemFactory::getItem(const std::wstring& id){
        if(id_to_item.contains(toString(id))){
            return id_to_item[toString(id)];
        }else if(name_to_item.contains(id)){
            return name_to_item[id];
        }else{
            return nullptr;
        }
    }

}