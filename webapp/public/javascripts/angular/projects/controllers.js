angular.module('terrama2.projects')
  .controller('Registration', ['$scope', '$http', '$window',
  function($scope, $http, $window) {
    $scope.forms = {};
    $scope.isSubmiting = false;
    $scope.errorFound = "";
    $scope.formName = "form";

    $scope.project = configuration.project || {};
    $scope.project.version = 4;

    $scope.save = function(frm) {
      $scope.$broadcast('formFieldValidation');

      if ($scope.forms.projectForm.$valid) {
        $scope.isSubmiting = true;
        $http({
          method: configuration.method,
          url: configuration.url,
          data: $scope.project
        }).success(function(project) {
          console.log(project);
          $scope.errorFound = "";
          $window.location.href = "/configuration/projects?token=" + project.token;
        }).error(function(err) {
          $scope.errorFound = err.message;
          console.log(err);
          $scope.form.name.$invalid = true;

        }).finally(function(){
          $scope.isSubmiting = false;
        });
      }
    };
  }])