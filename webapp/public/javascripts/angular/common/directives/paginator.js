define([], function() {
  function terrama2Paginator(PaginatorService) {
    return {
      restrict: "E",
      link: PaginatorLink
    };

    function PaginatorLink(scope, element, attrs) {
      
    }
  }

  terrama2Paginator.$inject = ["PaginatorService"];

  return terrama2Paginator;
});