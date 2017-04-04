define(function() {
  function ListController($scope, $http, DataProviderService, i18n, $window, MessageBoxService, $q) {
    var config = $window.configuration;
    $scope.loading = true;
    $scope.model = [];

    $q.all([
        i18n.ensureLocaleIsLoaded(),
        DataProviderService.init()
      ])
    
      .then(function() {
        var title = i18n.__("Data Server");
        $scope.i18n = i18n;
        $scope.model = DataProviderService.list();
        $scope.linkToAdd = "/configuration/providers/new";
        $scope.MessageBoxService = MessageBoxService;
        $scope.fields = [
          {key: "name", as: i18n.__("Name")},
          {key: "timeout", as: i18n.__("Data Server Timeout")},
          {key: "description", as: i18n.__("Description")}
        ];
        $scope.remove = function(object) {
          return "/api/DataProvider/" + object.id + "/delete";
        };
        $scope.link = function(object) {
          return "/configuration/providers/edit/" + object.id;
        };
        $scope.close = function() {
          MessageBoxService.reset();
        };
        $scope.iconProperties = {};

        $scope.errorFound = false;
        $scope.extra = {
          removeOperationCallback: function(err, data) {
            if (err) {
              MessageBoxService.danger(title, err.message);
              return;
            }
            MessageBoxService.danger(title, data.name + i18n.__(" removed"));
          }
        };

        $scope.iconFn = config.iconFn || null;

        $scope.iconProperties = config.iconProperties || {};
        
        $scope.method = "{[ method ]}";
        if (config.message ) {
          MessageBoxService.success(title, config.message);
        }
      })
      
      .finally(function() {
        $scope.loading = false;
      }); // end then
  } 
  ListController.$inject = ["$scope", "$http", "DataProviderService", "i18n", "$window", "MessageBoxService", "$q"];

  return ListController;
})