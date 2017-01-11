define(function() {
  function terrama2Button() {
    return {
      restrict: "E",
      template: "<button ng-class='class' ng-transclude></button>",
      scope: true,
      replace: true,
      transclude: true,
      link: function(scope, element, attrs, tranclude) {
        scope.class = attrs.class;
      }
    };
  }

  return terrama2Button;
});