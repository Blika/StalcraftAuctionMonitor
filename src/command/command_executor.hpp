#pragma once

#include <queue>
#include <string>

namespace auc_monitor{

    class CommandExecutor{
        public:
            CommandExecutor();
            ~CommandExecutor();

            static void execute(const std::wstring& cmd);
    };
    
}