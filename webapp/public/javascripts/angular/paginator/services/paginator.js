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
    this.setDataLength = setDataLength;
    this.sizeOfArray = sizeOfArray;
    this.setItemsPerPage = setItemsPerPage;
    this.itemsPerPage = itemsPerPage;
    this.generatePages = generatePages;
    this.numberOfPages = numberOfPages;
    this.currentPage = currentPage;
    this.setCurrentPage = setCurrentPage;
    /**
     * It retrieves current array observable
     * 
     * @returns {number}
     */
    function sizeOfArray() {
      return _length;
    }
    /**
     * It handles current page context
     * 
     * @param {number} pageNumber - Page number to set as current
     */
    function goToPage(pageNumber) {
      setCurrentPage(pageNumber);
    }
    /**
     * It go to next page
     */
    function nextPage() {
      this.goToPage(_currentPage + 1);
    }
    /**
     * It retrieves current page number
     * 
     * @returns {number}
     */
    function currentPage() {
      return _currentPage;
    }
    /**
     * It sets current page
     * @param {number} page - Page number
     */
    function setCurrentPage(page) {
      _currentPage = page;
    }
    /**
     * It retrieves how many items should display
     * @returns {number}
     */
    function itemsPerPage() {
      return _itemsPerPage;
    }
    /**
     * Retrieves pages that should display in GUI
     * 
     * @returns {number}
     */
    function numberOfPages() {
      return Math.ceil(_length / _itemsPerPage);
    }
    /**
     * It defines how many items should display in GUI2
     * 
     * @param {number} size - Value
     */
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
    /**
     * It generates all pages based in current page. It is important due it must re-generate whenever a page change in order to hide/show specific pages
     * 
     * @param {number} range - A range to determine interval
     * @returns {number[]}
     */
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
        var pageNumber = calculatePageNumber(i, _currentPage, range, total);

        var openingEllipsesNeeded = (i === 2 && (position === 'middle' || position === 'end'));
        var closingEllipsesNeeded = (i === range - 1 && (position === 'middle' || position === 'start'));
        if (ellipsesNeeded && (openingEllipsesNeeded || closingEllipsesNeeded)) {
          pages.push('...');
        } else {
          pages.push(pageNumber);
        }
        ++i;
      }
      return pages;
    }
    /**
     * It identifies which page number a index determines. For example, when generating array, you must know min and max intervals during iteration in order
     * to hide page number to fit in range (...). This functions retrieves a page number based in index.
     * @param {number} i - Current Element Index
     * @param {number} currentPage - Current Paginator Page
     * @param {number} paginationRange - Range value
     * @param {number} totalPages - Total pages of paginator
     * @returns {number}
     */
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