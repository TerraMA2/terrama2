define([], function() {
  "use strict";
  /**
   * It executes a paging filter delimiting a limit of elements in each page cycle
   * 
   * @example
   * <element ng-repeat="elm in arr | pagingToolLimit: 10">
   *   <sub-element ng-bind="elm"></sub-element>
   * </element>
   * 
   * @param {PagingToolService} PagingToolService - TerraMA² Paging Tool module to handle page cycle
   * @param {Utility} Utility - TerraMA² Utils module
   * @returns {angular.IFilter}
   */
  function PagingToolLimit(PagingToolService, Utility) {
    return _WrapPagingToolLimit;
    
    /**
     * It performs the filter execution
     * 
     * @param {Array<?>} collection - Array of elements to filter
     * @param {number} itemsPerPage - Number of elements per page
     * @returns {Array<?>|any}
     */
    function _WrapPagingToolLimit(collection, itemsPerPage) {
      if (Utility.isObject(collection)) {
        // do
        itemsPerPage = parseInt(itemsPerPage) || 20;
        var currentPageNumber = PagingToolService.currentPage();

        var startPage = (currentPageNumber - 1) * itemsPerPage;
        var endPage = startPage + itemsPerPage;

        PagingToolService.setItemsPerPage(itemsPerPage);

        if (collection instanceof Array) {
          return collection.slice(startPage, endPage);
        }
        var output = {};
        Object.keys(collection).slice(startPage, endPage).forEach(function(k) {
          output[k] = collection[k];
        });
        return output;
      } else {
        return collection;
      }
    }
  }

  PagingToolLimit.$inject = ["PagingToolService", "Utility"];

  return PagingToolLimit;
});