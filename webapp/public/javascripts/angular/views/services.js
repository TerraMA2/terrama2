(function() {
  angular.module("terrama2.views.services", ["terrama2"])
    .service("ViewService", ["BaseService", "$q", ViewService]);
  
  /**
   * It defines a TerraMA² View Service DAO.
   * 
   * @class ViewService
   * 
   * @param {BaseService} BaseService - Angular TerraMA² base service module
   * @param {angular.IQ} $q - Angular promiser module 
   */
  function ViewService(BaseService, $q) {
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = "/api/View";
    this.model = [];

    /**
     * It retrieves all data series semantics and cache them in model.
     * 
     * @param {Object} restriction
     * @returns {angular.IPromise<Object[]>}
     */
    this.init = function(restriction) {
      var defer = this.BaseService.$q.defer();
      var self = this;

      this.BaseService
        .$request(this.$baseUrl, "GET", {params: restriction})
        .then(function(data) {
          self.model = data;
          return defer.resolve(data);
        })
        .catch(function(err) {
          return defer.reject(err);
        })

      return defer.promise;
    };

    /**
     * It performs a view creation on API call.
     * 
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.create = function(viewObject) {
      return this.BaseService.$request(this.$baseUrl, "POST", {
        data: viewObject,
        headers: {
          "Content-Type": "application/json"
        }
      });
    };

    /**
     * It performs a view creation on API call.
     * 
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.list = function(restriction) {
      return this.BaseService.$filter('filter')(this.model, restriction);
    };
    
    /**
     * It performs a view creation on API call.
     * 
     * @param {number} viewId - A view identifier
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.get = function(viewId, restriction) {
      return this.BaseService.get(this.model, restriction);
    };

    /**
     * It performs a view update on API call
     * 
     * @param {number} viewId - A view identifier
     * @param {Object} viewObject - A view values to update
     * @returns {ng.IPromise}
     */
    this.update = function(viewId, viewObject) {
      return this.BaseService.$request(this.$baseUrl + "/" + viewId, "PUT", {
        data: viewObject
      });
    };
  }
} ());