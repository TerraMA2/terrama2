define([], function() {
  /**
   * This directive handles Pagination on multi-values elements.
   * 
   * @example
   * <ul>
   *   <li ng-repeat="element in (filteredArray = myArray | pagingLimit: 10 | orderBy: '+')"></li>
   * </ul>
   * <terrama2-paging-tool-controls max-pages="10" size="filteredArray.length"></terrama2-paging-tool-controls>
   * 
   * <!-- It will generate number of pages to iterate through. You may specify onChange binding in order to retrieve new elements from server -->
   * @param {PagingToolService} PagingToolService - TerraMA² Paging Tool Module that generate pages and handle user iteration
   * @returns {angular.IDirective}
   */
  function terrama2PagingToolControls(PagingToolService) {
    return {
      restrict: "E",
      templateUrl: "/dist/templates/paging-tool/templates/paging-tool-controls.html",
      scope: {
        /**
         * Max pages binding to delimit
         * @name terrama2PagingToolControls#maxPages
         * @type {number}
         */
        maxPages: "=?",
        /**
         * Size of array binding to generate pages
         * @name terrama2PagingToolControls#size
         * @type {number}
         */
        size: "=",
        /**
         * Binding function to notify when a page has changed.
         * @name terrama2PagingToolControls#onChange
         * @type {Function}
         */
        onChange: "&?"
      },
      controller: ["$scope", PagingToolControlsController],
      controllerAs: "$ctrl"
    };
    /**
     * It handles directive behavior.
     * 
     * @class PagingToolControlsController
     */
    function PagingToolControlsController($scope) {
      var self = this;
      /**
       * Reference to Paging Tool Service
       * @name PagingToolControlsController#$paging
       * @type {PagingToolService}
       */
      self.$paging = PagingToolService;
      self.$paging.setDataLength($scope.size);
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
          self.$paging.setDataLength(value);
          self.pages = self.$paging.generatePages(paginationRange);
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
        return self.$paging.currentPage();
      }, function(currentPage, previousPage) {
        if (currentPage !== previousPage) {
          goTo(currentPage);
        }
      });

      function generatePagination() {
        var page = PagingToolService.currentPage();
        self.pages = PagingToolService.generatePages(paginationRange);
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
        var old = self.$paging.currentPage();
        self.pages = self.$paging.generatePages(paginationRange);
        setCurrent(pageNumber);
        updateRangeValues();

        if ($scope.onChange) {
          $scope.onChange({currentPage: pageNumber, previousPage: old});
        }
      }

      function updateRangeValues() {
        var page = PagingToolService.currentPage();
        var itemsPerPage = PagingToolService.itemsPerPage();
        var totalItems = PagingToolService.sizeOfArray();
        self.range.lower = (page - 1) * itemsPerPage + 1;
        self.range.upper = Math.min(page * itemsPerPage, totalItems);
        self.range.total = totalItems;
      }

      if (!$scope.maxPages) {
        $scope.maxPages = 10;
      }

      self.setCurrent = setCurrent;

      self.pages = self.$paging.generatePages(paginationRange);
      /**
       * It just sets current page from given number
       * 
       * @param {number} page - Page number
       */
      function setCurrent(page) { 
        self.$paging.setCurrentPage(page);
      }
    }
  }

  terrama2PagingToolControls.$inject = ["PagingToolService"];

  return terrama2PagingToolControls;
});