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
    this.getNumberOfPages = getNumberOfPages;
    this.setDataLength = setDataLength;

    function goToPage(pageNumber) {
      _currentPage = pageNumber;
    }

    function nextPage() {
      this.goToPage(_currentPage + 1);
    }
    /**
     * Retrieves pages that should display in GUI
     * 
     * @returns {number}
     */
    function getNumberOfPages() {
      return Math.ceil(_length / _itemsPerPage);
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
      }
      throw new Error(Utility.format("Invalid number 'size' got: {0}", size));
    }
  }

  PaginatorService.$inject = ["Utility"];

  return PaginatorService;
});