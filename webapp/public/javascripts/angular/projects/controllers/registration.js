define(function() {
  "use strict";

  /**
   * TerraMA² Controller for handling Project Registration and Update
   * 
   * @class RegisterController
   * @param {angular.IScope} $scope - Angular Scope
   * @param {angular.IHttpService} $http - Angular HTTP module
   * @param {angular.IWindowService} $window - Angular Main Window Service
   * @param {any} i18n - TerraMA² Internationalization module
   */
  function RegisterController($scope, $http, $window, i18n) {
    $scope.i18n = i18n;
    $scope.forms = {};
    $scope.isSubmiting = false;
    $scope.errorFound = "";
    $scope.formName = "form";

    $scope.project = configuration.project || {};
    $scope.project.version = 4;

    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.close = function() {
      $scope.display = false;
    };

    var makeDialog = function(level, bodyMessage, show, title) {
      $scope.alertBox.title = i18n.__(title || "Project Registration");
      $scope.alertBox.message = bodyMessage;
      $scope.alertLevel = level;
      $scope.display = show;
    };

    $scope.save = function(frm) {
      $scope.close();
      $scope.$broadcast('formFieldValidation');

      if ($scope.forms.projectForm.$invalid){
        makeDialog("alert-danger", i18n.__("There are invalid fields on form"), true);
      }

      if ($scope.forms.projectForm.$valid) {
        $scope.isSubmiting = true;
        $http({
          method: configuration.method,
          url: configuration.url,
          data: $scope.project
        }).success(function(project) {
          console.log(project);
          $scope.errorFound = "";

          //$window.location.href = "/configuration/projects?token=" + project.token;
          $window.location.href = "/configuration/projects/" + $scope.project.name + "/activate/" + project.token;
        }).error(function(err) {
          $scope.errorFound = err.message;
          console.log(err);
          $scope.form.name.$invalid = true;

        }).finally(function(){
          $scope.isSubmiting = false;
        });
      }
    };
  }
  RegisterController.$inject = ['$scope', '$http', '$window', 'i18n'];

  return RegisterController;
})