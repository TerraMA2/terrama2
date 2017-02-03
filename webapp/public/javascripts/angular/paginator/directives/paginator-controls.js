define([], function() {
  function terrama2PaginatorControls(PaginatorService) {
    return {
      restrict: "E",
      templateUrl: "/dist/templates/paginator/templates/paginator-controls.html",
      scope: {
        maxPages: "=?",
        size: "=",
        onChange: "&?"
      },
      controller: PaginatorControlsController,
      controllerAs: "$ctrl"
    };

    function PaginatorControlsController($scope) {
      var self = this;

      self.$paginator = PaginatorService;
      self.$paginator.setDataLength($scope.size);

      self.$maxPages = $scope.maxPages;
      var paginationRange = Math.max($scope.maxPages, 5);

      $scope.$watch("size", function watchSize(value) {
        console.log("Value ", value);
        if (value) {
          self.$paginator.setDataLength(value);
          self.pages = self.$paginator.generatePages(paginationRange);
        }
      });

      if (!$scope.maxPages) {
        $scope.maxPages = 10;
      }

      self.setCurrent = setCurrent;

      self.pages = self.$paginator.generatePages(paginationRange);

      function setCurrent(page) {
        self.$paginator.setCurrentPage(page);
      }
    }

    PaginatorControlsController.$inject = ["$scope"];
  }

  terrama2PaginatorControls.$inject = ["PaginatorService"];

  return terrama2PaginatorControls;
});