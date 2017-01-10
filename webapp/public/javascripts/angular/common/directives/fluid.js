define(function() {
  function terrama2Fluid($window) {
    return {
      restrict: "A",
      link: function(scope, element, attrs) {
        var windowElement = angular.element($window);

        /**
         * Helper to handle window size and add/remove class depending resolution
         * 
         * @return {void}
         */
        function resizeComponent() {
          if ($window.pageYOffset > 200) {
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