define(
  function() {
    /**
     * A generic directive for handling datetime field
     * 
     */
    function terrama2Datetime($timeout) {
      return {
        restrict: 'A',
        require : 'ngModel',
        link: function(scope, element, attrs, ngModelCtrl) {
          var options = angular.extend({}, options, scope.$eval(attrs.options));
          scope.options = options;

          // Watchers
          scope.$watch('options', function (newValue) {
            var data = element.data('DateTimePicker');
            $.map(newValue, function (value, key) {
              data[key](value);
            });
          });

          ngModelCtrl.$render = function () {
            if (!!ngModelCtrl.$viewValue) {
              element.data('DateTimePicker').date(ngModelCtrl.$viewValue);
            } else {
              element.data('DateTimePicker').date(null);
            }
          };

          // Digesting scope
          element.on('dp.change', function (e) {
            $timeout(function () {
              if (!!e.date) {
                scope.$apply(function () {
                  ngModelCtrl.$setViewValue(e.date);
                });
              }
            });
          });

          element.datetimepicker(options);

          $timeout(function () {
            if (!!ngModelCtrl.$viewValue) {
              if (!(ngModelCtrl.$viewValue instanceof moment)) {
                const { options } = scope;
                let fmt = options ? (options.format || 'LLL') : 'LLL';

                ngModelCtrl.$setViewValue(moment(scope.dateTime, fmt));
              }
              element.data('DateTimePicker').date(ngModelCtrl.$viewValue);
            }
          });
        }
      };
    }

    terrama2Datetime.$inject = ["$timeout"];

    return terrama2Datetime;
  }
);