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
   * @param {MessageBoxService} MessageBoxService - TerraMA² MessageBox service handler
   */
  function RegisterController($scope, $http, $window, i18n, MessageBoxService) {
    $scope.i18n = i18n;
    $scope.forms = {};
    $scope.isSubmiting = false;
    $scope.formName = "form";
    $scope.MessageBoxService = MessageBoxService;
    var title = "Project Registration";

    $scope.project = configuration.project || {canProtect: true};
    $scope.project.version = 4;

    $scope.css = {
      boxType: "box-solid"
    }
    
    $scope.close = function() {
      MessageBoxService.reset();
    };

    $scope.save = function(frm) {
      $scope.close();
      $scope.$broadcast('formFieldValidation');

      if ($scope.forms.projectForm.$invalid){
        return MessageBoxService.danger(i18n.__(title), i18n.__("There are invalid fields on form"));
      }

      $scope.isSubmiting = true;
      $http({
        method: configuration.method,
        url: configuration.url,
        data: $scope.project
      }).then(function(response) {
        $window.location.href = BASE_URL + "configuration/projects/" + $scope.project.name + "/activate/" + response.data.token;
      }).catch(function(response) {
        MessageBoxService.danger(i18n.__(title), i18n.__(response.data.message));
      }).finally(function(){
        $scope.isSubmiting = false;
      });
    };
  }
  RegisterController.$inject = ['$scope', '$http', '$window', 'i18n', 'MessageBoxService'];

  return RegisterController;
})