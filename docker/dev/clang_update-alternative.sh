#!/bin/bash
#
# Register all clang-6.0 and llvm-6.0 software as default alternatives.
# i.e.: clang++ -> clang++6.0
#

VERSION="6.0"

for entry in "/usr/bin/clang"*"-${VERSION}" "/usr/bin/llvm"*"-${VERSION}" "/usr/bin/lldb"*"-${VERSION}"
do
  if [ -f "$entry" ];then
    new_name=${entry%-${VERSION}}
    new_name=${new_name#$"/usr/bin/"}

    echo ${new_name}
    sudo update-alternatives --install /usr/bin/${new_name} ${new_name} ${entry} 100
  fi
done

sudo update-alternatives --install /usr/bin/run-clang-tidy run-clang-tidy /usr/bin/run-clang-tidy-${VERSION} 100
sudo update-alternatives --install /usr/bin/run-clang-tidy.py run-clang-tidy.py /usr/bin/run-clang-tidy-${VERSION}.py 100
sudo update-alternatives --install /usr/bin/clang-tidy-diff.py clang-tidy-diff.py /usr/bin/clang-tidy-diff-${VERSION}.py 100
