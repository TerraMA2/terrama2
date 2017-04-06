define([], function(){
  "use strict";

  function AlertList($scope, i18n){
    $scope.i18n = i18n;

    $scope.model = [
      {
        name: "Alert 1",
        color: "blue"
      },
      {
        name: "Alert 2",
        color: "yellow"
      },
      {
        name: "Alert 3",
        color: "red"
      }
    ];

    $scope.fields = ['name'];
    $scope.linkToAdd = "/configuration/alert/new";
    $scope.link = function(object) {
      return "";
    };

    $scope.iconProperties = {
      type: "icon"
    };

    $scope.icon = function(object) {
      if (object.color === 'blue')
        return "fa fa-check label-primary";

      if (object.color === 'yellow')
        return "fa fa-eye label-warning";

      if (object.color === 'red')
        return "fa fa-exclamation label-danger";

      return "";
    }
  }

  AlertList.$inject = ["$scope", "i18n"];

  return AlertList;
});