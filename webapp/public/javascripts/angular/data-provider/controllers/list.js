define(function() {
  function ListController($scope, $http, DataProviderService, i18n, $window, MessageBoxService, $q, $timeout) {
    var config = $window.configuration;
    $scope.loading = true;
    $scope.model = [];

    $q.all([
        i18n.ensureLocaleIsLoaded(),
        DataProviderService.init()
      ])
    
      .then(function() {
        var title = "Data Server";
        $scope.i18n = i18n;
        $scope.model = DataProviderService.list();
        $scope.linkToAdd = "/configuration/providers/new";
        $scope.MessageBoxService = MessageBoxService;
        $scope.fields = [
          {key: "name", as: i18n.__("Name")},
          {key: "data_provider_type.name", as: i18n.__("Type")},
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
            if(err) {
              MessageBoxService.danger(i18n.__(title), err.message);
              return;
            }
            MessageBoxService.danger(i18n.__(title), data.name + i18n.__(" removed"));
          }
        };

        $scope.iconFn = config.iconFn || null;

        $scope.iconProperties = config.iconProperties || {};
        
        $scope.method = "{[ method ]}";
        if(config.message) {
          var messageArray = config.message.split(" ");
          var tokenCodeMessage = messageArray[messageArray.length - 1];
          messageArray.splice(messageArray.length - 1, 1);

          $timeout(function() {
            var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
            MessageBoxService.success(i18n.__(title), finalMessage);
          }, 1000);
        }
      })
      
      .finally(function() {
        $scope.loading = false;
      }); // end then
  } 
  ListController.$inject = ["$scope", "$http", "DataProviderService", "i18n", "$window", "MessageBoxService", "$q", "$timeout"];

  return ListController;
})