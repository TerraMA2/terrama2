angular.module("terrama2.components.messagebox", [])
  .directive("terrama2MessageBox", function() {
    return {
      restrict: "AE",
      replace: true,
      transclude: true,
      templateUrl: "/javascripts/angular/alert-box/templates/alert-box.html",
      scope: {
        errorFound: "=errorFound",
        message: "=",
        title: "=",
        close: "&"
      },
    }
  });