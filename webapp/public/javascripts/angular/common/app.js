define([
    "TerraMA2WebApp/common/services/index",
    "TerraMA2WebApp/common/controllers/i18n",
    "TerraMA2WebApp/common/directives/index"
  ],
  function(commonServicesModule, i18nController, commonDirectivesModule) {
    var moduleName = "terrama2.common";
    angular.module(moduleName, ["i18n", commonServicesModule, commonDirectivesModule])
      .controller("TerraMA2Controller", i18nController)
      .run(["$rootScope", "$locale", function($rootScope, $locale) {
        $rootScope.locale = $locale.localeID;
      }]);
    return moduleName;
  });