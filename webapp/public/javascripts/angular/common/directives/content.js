define(function() {
  function terrama2Content() {
    return {
      restrict: "E",
      transclude: true,
      template: "<div ng-class='divClass' ng-transclude></div>",
      link: function(scope, element, attrs) {
        scope.divClass = attrs.class || "row";
      }
    };
  }

  return terrama2Content;
});