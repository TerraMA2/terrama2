(function() {
  angular.module("terrama2.views.services", ["terrama2"])
    .service("ViewService", ViewService);
  
  /**
   * 
   * @class ViewService
   */
  function ViewService($q, $http) {
    this.$q = $q;
    this.$http = $http;
    this.$baseUrl = "/api/Views";
  }

  /**
   * It performs a web request to the Views API
   * 
   * @param {string} url - A well-formed URL to make request
   * @param {method} method - Request method.
   * @param {Object} options - A extra request options
   * @returns {ng.IPromise}
   */
  ViewService.prototype.$request = function(url, method, options) {
    var self = this;
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
  }

  /**
   * It performs a view creation on API call.
   * 
   * @param {Object} viewObject - A view values
   * @returns {ng.IPromise}
   */
  ViewService.prototype.create = function(viewObject) {
    return this.$request(this.$baseUrl, "POST", {
      data: viewObject
    });
  };

  /**
   * It performs a view creation on API call.
   * 
   * @param {Object} viewObject - A view values
   * @returns {ng.IPromise}
   */
  ViewService.prototype.list = function(restriction) {
    return this.$request(this.$baseUrl, "GET", {
      params: restriction
    });
  };
  
  /**
   * It performs a view creation on API call.
   * 
   * @param {Object} viewObject - A view values
   * @returns {ng.IPromise}
   */
  ViewService.prototype.get = function(viewId, restriction) {
    return this.$request(this.$baseUrl + "/" + viewId, "GET", {
      params: restriction
    });
  };
} ());