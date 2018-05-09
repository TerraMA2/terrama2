#! /bin/bash

#
# Valid parameter val or abort script
#
function valid()
{
  if [ $1 -ne 0 ]; then
    printf "\n$2\n\n"
    exit 1
  fi
}

now=$(date '+%Y%m%d')
ubuntu=$(lsb_release -rs)
packages_folder=${ubuntu}-${now}


# delete old packages
rm -rf /${packages_folder}
rm /devel/build-package/*.deb

(
  cd /devel/terrama2
  git pull

  cd packages/debian-package
  ./deb-terrama2.sh
  valid $? "Unable to create the TerraMA2 package."
  ./deb-helper.sh
  valid $? "Unable to create the TerraMA2 doc package."
)

cd packages
(
  mkdir ${packages_folder}
  cp /devel/build-package/*.deb ${packages_folder}/
  cp /devel/terrama2/packages/debian-package/*.deb ${packages_folder}/
)

tail -f /dev/null
