define(function() {
  function terrama2Fluid($window) {
    return {
      restrict: "A",
      link: function(scope, element, attrs) {
        var windowElement = angular.element($window);
        var bodyElement = angular.element("body");
        /**
         * Helper to handle window size and add/remove class depending resolution
         * 
         * @return {void}
         */
        function resizeComponent() {
          var screenY = $window.innerHeight;
          var scrollHeight = bodyElement[0].scrollHeight;
          // var elementHeight = element[0].clientHeight;
          var currentOffSet = $window.pageYOffset;
          var scrollScreenFactor = currentOffSet / scrollHeight; // percentage
          var factor = 200 / scrollHeight;

          if (scrollScreenFactor + factor > 0.3) {
            element.addClass("terrama2-fluid");
          } else {
            element.removeClass("terrama2-fluid");
          }
        }

        // Auto call to resize at first time
        resizeComponent();

        // Performs resize component on page scroll
        windowElement.bind('scroll', function() {
          return resizeComponent();
        });
      }
    };
  }

  terrama2Fluid.$inject = ["$window"];

  return terrama2Fluid;
});