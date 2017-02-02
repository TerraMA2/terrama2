define([], function() {
  function terrama2Paginator($parse, PaginatorService) {
    return {
      restrict: "A",
      terminal: true,
      multiElement: true,
      priority: 100,
      compile: PaginatorCompiler
    };

    function PaginatorCompiler(tElement, tAttrs) {
      var expression = tAttrs.terrama2Paginator;
      tAttrs.ngRepeat = expression;

      return PaginatorPostLink;

      function PaginatorPostLink(scope, element, attrs) {

      }
    }
  }

  terrama2Paginator.$inject = ["$parse", "PaginatorService"];

  return terrama2Paginator;
});