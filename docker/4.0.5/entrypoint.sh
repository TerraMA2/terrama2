#! /bin/bash

(
  cd /devel/terrama2
  git pull

  cd packages/debian-package
  ./deb-terrama2.sh
  ./deb-helper.sh
)

mkdir packages
cp /devel/build-package/*.deb .
cp /devel/terrama2/packages/debian-package/*.deb .

bash
