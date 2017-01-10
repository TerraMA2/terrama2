define(function() {
  function TerraMA2Controller($scope, i18n) {
    $scope.i18n = i18n;
  }

  TerraMA2Controller.$inject = ["$scope", "i18n"];

  return TerraMA2Controller;
});