angular.module("terrama2.providers", [
      "terrama2",
      "terrama2.services",
      "terrama2.components.messagebox",
      "terrama2.table"
    ])
    .controller("DataProviderController", [
      "$scope",
      "$http",
      "DataProviderFactory",
      "i18n",
      "$window",
      function($scope, $http, DataProviderFactory, i18n, $window) {
        var config = $window.configuration;
        i18n.ensureLocaleIsLoaded()
          .then(function() {
            $scope.i18n = i18n;
            $scope.model = [];
            $scope.linkToAdd = "/configuration/providers/new";
            $scope.fields = [
              {key: "name", as: i18n.__("Name")},
              {key: "description", as: i18n.__("Description")}
            ];
            $scope.remove = function(object) {
              return "/api/DataProvider/" + object.id + "/delete";
            };
            $scope.link = function(object) {
              return "/configuration/providers/edit/" + object.id;
            };
            $scope.iconProperties = {};

            $scope.errorFound = false;
            $scope.extra = {
              removeOperationCallback: function(err, data) {
                $scope.display = true;
                if (err) {
                  $scope.alertLevel = "alert-danger";
                  $scope.alertBox.message = err.message;
                  return;
                }

                $scope.alertLevel = "alert-success";
                $scope.alertBox.message = data.name + i18n.__(" removed");
              }
            };
            $scope.method = "{[ method ]}";
            $scope.alertLevel = "alert-success";
            $scope.alertBox = {
              title: i18n.__("Data Server"),
              message: config.message
            };
            $scope.resetState = function() { $scope.display = false; };
            $scope.display = config.message ? true : false;

            $scope.loading = true;

            DataProviderFactory.get().success(function(dataProviders) {
              $scope.model = dataProviders;
            }).error(function(err) {
              
            }).finally(function() {
              $scope.loading = false;
            });
          }); // end then
    }]);