define([], function() {
  /**
   * This directive handles Pagination on multi-values elements.
   * 
   * @example
   * <ul>
   *   <li ng-repeat="element in (filteredArray = myArray | paginatorLimit: 10 | orderBy: '+')"></li>
   * </ul>
   * <terrama2-paginator-controls max-pages="10" size="filteredArray.length"></terrama2-paginator-controls>
   * 
   * <!-- It will generate number of pages to iterate through. You may specify onChange binding in order to retrieve new elements from server -->
   * @param {PaginatorService} PaginatorService - TerraMA² Paginator Module that generate pages and handle user iteration
   * @returns {angular.IDirective}
   */
  function terrama2PaginatorControls(PaginatorService) {
    return {
      restrict: "E",
      templateUrl: "/dist/templates/paginator/templates/paginator-controls.html",
      scope: {
        /**
         * Max pages binding to delimit
         * @name terrama2PaginatorControls#maxPages
         * @type {number}
         */
        maxPages: "=?",
        /**
         * Size of array binding to generate pages
         * @name terrama2PaginatorControls#size
         * @type {number}
         */
        size: "=",
        /**
         * Binding function to notify when a page has changed.
         * @name terrama2PaginatorControls#onChange
         * @type {Function}
         */
        onChange: "&?"
      },
      controller: ["$scope", PaginatorControlsController],
      controllerAs: "$ctrl"
    };
    /**
     * It handles directive behavior.
     * 
     * @class PaginatorControlsController
     */
    function PaginatorControlsController($scope) {
      var self = this;
      /**
       * Reference to Paginator Service
       * @name PaginatorControlsController#$paginator
       * @type {PaginatorService}
       */
      self.$paginator = PaginatorService;
      self.$paginator.setDataLength($scope.size);
      self.range = {
        lower: 1,
        upper: 1,
        total: 1
      };

      self.$maxPages = $scope.maxPages;
      var paginationRange = Math.max($scope.maxPages, 5);

      $scope.$watch("size", function watchSize(value) {
        if (value) {
          paginationRange = Math.max($scope.maxPages, 5);
          generatePagination();
          self.$paginator.setDataLength(value);
          self.pages = self.$paginator.generatePages(paginationRange);
        }
      });

      $scope.$watch("maxPages", function watchMaxPage(value) {
        if (value) {
          self.$maxPages = value;
          paginationRange = Math.max(self.$maxPages, 5);
          generatePagination();
        }
      });

      $scope.$watch(function() {
        return self.$paginator.currentPage();
      }, function(currentPage, previousPage) {
        if (currentPage !== previousPage) {
          goTo(currentPage);
        }
      });

      function generatePagination() {
        var page = PaginatorService.currentPage();
        self.pages = PaginatorService.generatePages(paginationRange);
        var last = self.pages[self.pages.length - 1];
        if (last < page) {
          setCurrent(last);
        } else {
          updateRangeValues();
        }
      }
      /**
       * It defines page number handlers and dispatchs to Parent module via function bindings
       * 
       * @param {number} pageNumber - Current Page
       */
      function goTo(pageNumber) {
        var old = self.$paginator.currentPage();
        self.pages = self.$paginator.generatePages(paginationRange);
        setCurrent(pageNumber);
        updateRangeValues();

        if ($scope.onChange) {
          $scope.onChange({currentPage: pageNumber, previousPage: old});
        }
      }

      function updateRangeValues() {
        var page = PaginatorService.currentPage();
        var itemsPerPage = PaginatorService.itemsPerPage();
        var totalItems = PaginatorService.sizeOfArray();
        self.range.lower = (page - 1) * itemsPerPage + 1;
        self.range.upper = Math.min(page * itemsPerPage, totalItems);
        self.range.total = totalItems;
      }

      if (!$scope.maxPages) {
        $scope.maxPages = 10;
      }

      self.setCurrent = setCurrent;

      self.pages = self.$paginator.generatePages(paginationRange);
      /**
       * It just sets current page from given number
       * 
       * @param {number} page - Page number
       */
      function setCurrent(page) { 
        self.$paginator.setCurrentPage(page);
      }
    }
  }

  terrama2PaginatorControls.$inject = ["PaginatorService"];

  return terrama2PaginatorControls;
});