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

# delete old packages
rm -rf /packages
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

mkdir packages
cp /devel/build-package/*.deb packages/
cp /devel/terrama2/packages/debian-package/*.deb packages/

bash
