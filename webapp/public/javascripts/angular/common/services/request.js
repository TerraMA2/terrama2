define(function() {
  /**
   * TerraMA² Base service dao
   * 
   * @param {angular.IPromise} $q - Angular $q promiser
   * @param {angular.IHTTP} $http - Angular $http module
   * @param {angular.IFilter} $filter - Angular $filter module
   * @param {angular.IParse} $parse - Angular Parser module
   * 
   * @class BaseService
   */
  function BaseService($q, $http, $filter, $parse) {
    this.$q = $q;
    this.$http = $http;
    this.$filter = $filter;
    this.$parse = $parse;

    var self = this;
    /**
     * TerraMA² base request URL. It performs $http operation from given request options
     * 
     * @param {string} url - URL to request
     * @param {string} method - HTTP method
     * @param {Object} options - HTTP options
     * @returns {ng.IPromise}
     */
    this.$request = function(url, method, options) {
      var defer = self.$q.defer();

      self.$http(Object.assign({
        url: url,
        method: method
      }, options)).success(function(data) {
        return defer.resolve(data);
      }).error(function(err) {
        return defer.reject(err);
      });

      return defer.promise;
    };
    /**
     * It applies a angular filter over a array with query restriction.
     * 
     * @param {Array<?>} model - An array of object to filter
     * @param {Object} query - A query restriction
     * @returns {Array<?>}
     */
    this.$list = function(model, query) {
      return self.$filter("filter")(model, query);
    };

    /**
     * It retrieves a first selement from model. If element found, return element. Otherwise, return null.
     * 
     * @param {Array<?>} model - An array of object to filter
     * @param {Object} query - A query restriction
     * @returns {?}
     */
    this.get = function(model, query) {
      var elements = self.$list(model, query);
      if (elements.length === 0) {
        return null;
      }
      return elements[0];
    };
  }
  BaseService.$inject = ["$q", "$http", "$filter", "$parse"];

  return BaseService;
});