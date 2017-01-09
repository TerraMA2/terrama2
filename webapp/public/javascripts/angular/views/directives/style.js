define([], function() {
  function terrama2StyleDirective() {
    return {
      restrict: "E",
      templateUrl: "/dist/templates/views/templates/style.html",
      controller: StyleController,
      controllerAs: "ctrl"
    };

    function StyleController() {

    }
  }

  return terrama2StyleDirective;
});