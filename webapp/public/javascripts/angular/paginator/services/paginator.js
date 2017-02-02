define([], function() {
  /**
   * It defines a Angular Paginator Service that handles dynamic data loading in GUI interface
   * 
   * @class PaginatorService
   */
  function PaginatorService(Utility) {
    /**
     * It defines current page number
     * @private
     * @name PaginatorService#_currentPage
     * @type {number}
     * @default 1
     */
    var _currentPage = 1;
    /**
     * It defines how many items will display per page
     * @private
     * @name PaginatorService#_itemsPerPage
     * @type {number}
     * @default 20
     */
    var _itemsPerPage = 20;
    /**
     * It defines size of data. It is important due this service does not need to know raw data but only length
     * @private
     * @name PaginatorService#_length
     * @type {number}
     * @default 0
     */
    var _length = 0;

    // functions bindings
    this.goToPage = goToPage;
    this.nextPage = nextPage;
    this.numberOfPages = numberOfPages;
    this.setDataLength = setDataLength;
    this.currentPage = currentPage;
    this.setItemsPerPage = setItemsPerPage;
    this.generatePages = generatePages;
    this.setCurrentPage = setCurrentPage;

    function goToPage(pageNumber) {
      _currentPage = pageNumber;
    }

    function nextPage() {
      this.goToPage(_currentPage + 1);
    }
    function currentPage() {
      return _currentPage;
    }
    function setCurrentPage(page) {
      _currentPage = page;
    }
    /**
     * Retrieves pages that should display in GUI
     * 
     * @returns {number}
     */
    function numberOfPages() {
      return Math.ceil(_length / _itemsPerPage);
    }

    function setItemsPerPage(size) {
      if (Utility.isNumber(size)) {
        _itemsPerPage = size;
      } else {
        throw new Error(Utility.format("Invalid number 'size' got: {0}", size));
      }
    }
    /**
     * It sets data length that paginator will use to split page generators
     * 
     * @throws {Error} When invalid size is set
     * @param {number} size - Data Length
     */
    function setDataLength(size) {
      if (Utility.isNumber(size)) {
        _length = size;
      } else {
        throw new Error(Utility.format("Invalid number 'size' got: {0}", size));
      }
    }

    function generatePages(range) {
      var pages = [];
      var total = Math.ceil(_length / _itemsPerPage);
      var half = Math.ceil(range / 2);

      var position = null;
      if (_currentPage < half) {
        position = "start";
      } else if (total - half < _currentPage) {
        position = "end";
      } else {
        position = "middle";
      }

      var ellipsesNeeded = range < total;
      var i = 1;
      while (i <= total && i <= range) {
        var pageNumber = calculatePageNumber(i, currentPage, range, total);

        var openingEllipsesNeeded = (i === 2 && (position === 'middle' || position === 'end'));
        var closingEllipsesNeeded = (i === range - 1 && (position === 'middle' || position === 'start'));
        if (ellipsesNeeded && (openingEllipsesNeeded || closingEllipsesNeeded)) {
          pages.push('...');
        } else {
          pages.push(pageNumber);
        }
        i++;
      }
      console.log(pages);
      return pages;
    }

    function calculatePageNumber(i, currentPage, paginationRange, totalPages) {
            var halfWay = Math.ceil(paginationRange/2);
            if (i === paginationRange) {
                return totalPages;
            } else if (i === 1) {
                return i;
            } else if (paginationRange < totalPages) {
                if (totalPages - halfWay < currentPage) {
                    return totalPages - paginationRange + i;
                } else if (halfWay < currentPage) {
                    return currentPage - halfWay + i;
                } else {
                    return i;
                }
            } else {
                return i;
            }
        }
  }

  PaginatorService.$inject = ["Utility"];

  return PaginatorService;
});