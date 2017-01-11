define(function() {
  function terrama2AlertBox(AlertLevel) {
    return {
      restrict: "E",
      transclude: true,
      templateUrl: "message-box.html",
      scope: {
        handler: '=handler', // it should be a AlertBox
        close: '&?',
        extra: '=?extra'
      },
      controller: ["$scope", function($scope) {
        $scope.$watch("handler", function(value) {
          if (!value) {
            return;
          }

          var alertBox = value;
          $scope.display = alertBox.display;
          $scope.title = alertBox.title;
          $scope.message = alertBox.message;
          $scope.alertLevel = alertBox.level;
          $scope.extra = alertBox.extra;
        }, true);

        $scope.isAnyExtra = function() {
          return Object.keys($scope.extra || {}).length > 0;
        };
        $scope.alertIcon = function() {
          switch($scope.alertLevel) {
            case AlertLevel.INFO:
              return "fa-info";
            case AlertLevel.WARNING:
              return "fa-exclamation-triangle";
            case AlertLevel.DANGER:
              return "fa-times";
            case AlertLevel.SUCCESS:
              return "fa-check";
            default:
              return "fa-info";
          }
        };
      }]
    }
  }

  terrama2AlertBox.$inject = ["AlertLevel"];

  return terrama2AlertBox;
})