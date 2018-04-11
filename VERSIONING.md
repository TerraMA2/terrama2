# Versioning

When generating a new version of TerraMA2 one should update the files:

TerraMA2:
- build/cmake/CMakeLists.txt
- LINUX.md
- README.md
- packages/debian-package/deb-terrama2.sh

Web Applications:
- webapp/config/sample_instances/default.json
- webapp/package.json
- webapp/package-lock.json
- webapp/locales/en_US.json
- webapp/locales/es_ES.json
- webapp/locales/pt_BR.json
- webcomponents/package-lock.json
- webcomponents/package.json
- webmonitor/package.json
- webmonitor/locales/en.json
- webmonitor/locales/es.json
- webmonitor/locales/pt.json

Manual:
- helper/indice_manual.html
- helper/introduction.html
- packages/debian-package/deb-helper.sh

VMime
- packages/debian-package/deb-vmime.sh
- build/cmake/modules/FindVMime.cmake

GTest
- packages/debian-package/deb-gtest.sh

## Versioning TerraLib

When updating the version of the `TerraLib`, one should update the files:

TerraMA2:
- build/cmake/terrama2-cpack-options.cmake.in
- README.md
- DEPENDENCIES
- LINUX.md
- packages/debian-package/deb-terrama2.sh
- packages/debian-package/terrama2.conf.cmake
- packages/dmg-package/dmg-terrama2.sh
- packages/install.sh
- packages/uninstall.sh
- packages/nsis-package/nsis-terrama2-msvc14.bat

Manual:
- helper/indice_manual.html
- helper/introduction.html

Web Applications:
- webapp/locales/en_US.json
- webapp/locales/es_ES.json
- webapp/locales/pt_BR.json
- webmonitor/locales/en.json
- webmonitor/locales/es.json
- webmonitor/locales/pt.json
