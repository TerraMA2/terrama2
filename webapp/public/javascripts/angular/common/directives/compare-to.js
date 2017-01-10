define([], function() {
  function terrama2CompareTo() {
    return {
      restrict: 'A',
      require: 'ngModel',
      scope: {
        compare: '=terrama2CompareTo'
      },
      link: function(scope, element, attrs, ngModel) {
        ngModel.$validators.compareTo = function(modelValue) {
          return modelValue === scope.compare;
        };

        scope.$watch("compare", function() {
          ngModel.$validate();
        });
      }
    };
  }

  return terrama2CompareTo;
});