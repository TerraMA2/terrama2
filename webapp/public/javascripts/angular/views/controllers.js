(function() {
  'use strict';

  angular.module("terrama2.views.controllers", ["terrama2", "terrama2.views.services", "schemaForm"])
    .controller("ViewRegistration", ViewRegistration);


  function ViewRegistration($scope, ViewService, $log, $http, $timeout) {
    var self = this;

    $http.get("/api/DataProviderType", {}).success(function(data) {
      // Setting HTTP default syntax
      self.httpSyntax = data.find(function(element) {
        return element.name === "HTTP";
      });
    }).error(function(err) {
      $log.info(err);
    });

    self.ViewService = ViewService;
    $scope.forms = {};
    self.css = {
      boxType: "box-solid"
    };

    self.save = function() {
      $scope.$broadcast("formFieldValidation");
      if (self.forms.viewForm.$invalid) {
        return;
      }

      self.ViewService.create(self.view).then(function(response) {
        alert("FOI");
        $log.info(response);
      }).catch(function(err) {
        alert("NUM FOI");
        $log.info(err);
      });
    };

    // configuring Schema form http
    $timeout(function() {
      $scope.schema = {
        type: "object",
        properties: self.httpSyntax.properties,
        required: self.httpSyntax.required || []
      };

      $scope.model = {};
      if (self.httpSyntax.display) {
        $scope.form = self.httpSyntax.display;
      } else {
        $scope.form = ["*"];
      }

      $scope.$broadcast('schemaFormRedraw');
    });
  }

  // Injecting Angular Dependencies
  ViewRegistration.$inject = ["$scope", "ViewService", "$log", "$http", "$timeout"];
}());