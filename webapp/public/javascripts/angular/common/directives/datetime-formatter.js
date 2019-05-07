define(function() {
  function terrama2DatetimeFormatter($window) {
    return {
      restrict: 'A',
      require: 'ngModel',
      link: function (scope, element, attrs, ngModel) {
        var moment = $window.moment;
        var fmt = scope.$eval(attrs.formatDatetime);
        
        ngModel.$formatters.push(formatter);
        ngModel.$parsers.push(parser);

        element.on('change', function (e) {
          var element = e.target;
          element.value = formatter(ngModel.$modelValue);
        });

        /**
         * It performs a parser string to check if it is a valid date
         * @param {string | Date} value - A date value
         * @return {boolean} 
         */
        function parser(value) {
          var m = moment(value);
          var valid = m.isValid();
          // ngModel.$setValidity('datetime', valid);
          if (valid) {
            return m.valueOf();
          } else {
            return value;
          }
        }

        /**
         * It performs a input formatter
         * @param {string | Date} value - A date value
         * @return {string}
         */
        function formatter(value) {
          var m = moment(value, fmt || 'LLLL');
          var valid = m.isValid();
          if (valid) {
            return m.format(fmt || "LLLL");
          } else {
            return value;
          }
        }
      } //link
    };
  }

  terrama2DatetimeFormatter.$inject = ["$window"];

  return terrama2DatetimeFormatter;
});