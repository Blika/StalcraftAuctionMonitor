#include "command_executor.hpp"
#include "../monitor/monitor.hpp"
#include "../item/item_factory.hpp"
#include "../item/item.hpp"
#include "../utils/utils.hpp"
#include <boost/algorithm/string.hpp>

namespace auc_monitor{

    CommandExecutor::CommandExecutor(){}
    CommandExecutor::~CommandExecutor(){}

    void CommandExecutor::execute(const std::wstring& txt){
        std::vector<std::wstring> cmd;
        boost::split(cmd, txt, boost::is_any_of(" "));
        if(cmd.empty()){
            send_log("unknown command");
            return;
        }
        if(cmd[0] == L"map"){
            map_items();
        }else if(cmd[0] == L"init_items"){
            Monitor::getInstance()->getItemFactory()->init();
        }else if(cmd[0] == L"offset"){
            if(cmd.size() < 2){
                send_log("offset num");
                return;
            }
            int num = std::atoi(toString(cmd[1]).c_str());
            Monitor::getInstance()->sell_offset = num;
            send_debug("set max offset to ",num);
        }else if(cmd[0] == L"lots"){
            if(cmd.size() < 2){
                send_log("lots num");
                return;
            }
            int num = std::atoi(toString(cmd[1]).c_str());
            Monitor::getInstance()->lots = num;
            send_debug("set max lots to ",num);
        }else if(cmd[0] == L"find"){
            if(cmd.size() < 4){
                send_log("find qlt ptn name");
                return;
            }
            int qlt = std::atoi(toString(cmd[1]).c_str());
            int ptn = std::atoi(toString(cmd[2]).c_str());
            cmd.erase(cmd.begin());
            cmd.erase(cmd.begin());
            cmd.erase(cmd.begin());
            std::wstring n = boost::algorithm::join(cmd,L" ");
            Item* item = Monitor::getInstance()->getItemFactory()->getItem(n);
            if(item == nullptr){
                send_log("item not found ",n);
            }else{
                Monitor::getInstance()->checkSellHistory(item,qlt,ptn);
            }
        }else if(cmd[0] == L"check"){
            if(cmd.size() < 4){
                send_log("check qlt ptn name");
                return;
            }
            int qlt = std::atoi(toString(cmd[1]).c_str());
            int ptn = std::atoi(toString(cmd[2]).c_str());
            cmd.erase(cmd.begin());
            cmd.erase(cmd.begin());
            cmd.erase(cmd.begin());
            std::wstring n = boost::algorithm::join(cmd,L" ");
            Item* item = Monitor::getInstance()->getItemFactory()->getItem(n);
            if(item == nullptr){
                send_log("item not found ",n);
            }else{
                Monitor::getInstance()->checkActiveLots(item,qlt,ptn);
            }
        }else{
            send_log("unknown command ",cmd[0]);
        }
    }

}