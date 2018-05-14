#!/bin/bash
#
# Register all clang-6.0 and llvm-6.0 software as default alternatives.
# i.e.: clang++ -> clang++6.0
#

for entry in "/usr/bin/clang"*"-6.0" "/usr/bin/llvm"*"-6.0" "/usr/bin/lldb"*"-6.0"
do
  if [ -f "$entry" ];then
    new_name=${entry%-6.0}
    new_name=${new_name#$"/usr/bin/"}

    echo ${new_name}
    sudo update-alternatives --install /usr/bin/${new_name} ${new_name} ${entry} 100
  fi
done
