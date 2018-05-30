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
rm -rf /packages/${VERSION}/${packages_folder}
rm -f /devel/build-package/*.deb
rm -rf /devel/terrama2/packages/debian-package/terrama2-doc*
rm -f /devel/terrama2/packages/debian-package/terrama2-doc*.deb


cd /devel/terrama2
git pull
GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_REMOTE_NAME=`git config branch.${GIT_BRANCH}.remote`
GIT_REMOTE=`git remote get-url ${GIT_REMOTE_NAME}`
GIT_COMMIT=`git rev-parse HEAD`
GIT_DATE=`git log -1 --format=%cd`

cd packages/debian-package
./deb-terrama2.sh
valid $? "Unable to create the TerraMA2 package."
./deb-helper.sh
valid $? "Unable to create the TerraMA2 doc package."


cd /packages
(
  mkdir -p /packages/${VERSION}/${packages_folder}
  cp /devel/build-package/*.deb /packages/${VERSION}/${packages_folder}
  cp /devel/terrama2/packages/debian-package/*.deb /packages/${VERSION}/${packages_folder}

  echo ${GIT_REMOTE} >> /packages/${VERSION}/${packages_folder}/version.txt
  echo ${GIT_BRANCH} >> /packages/${VERSION}/${packages_folder}/version.txt
  echo ${GIT_COMMIT} >> /packages/${VERSION}/${packages_folder}/version.txt
  echo ${GIT_DATE} >> /packages/${VERSION}/${packages_folder}/version.txt
)
