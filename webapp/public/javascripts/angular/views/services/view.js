define([], function() {
  /**
   * It defines a TerraMA² View Service DAO.
   * 
   * @class ViewService
   * 
   * @param {BaseService} BaseService - Angular TerraMA² base service module
   * @param {angular.IQ} $q - Angular promiser module 
   */
  function ViewService(BaseService, $q) {
    var self = this;
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = BASE_URL + "api/View";
    this.model = [];

    /**
     * It retrieves all data series semantics and cache them in model.
     * 
     * @param {Object} restriction
     * @returns {angular.IPromise<Object[]>}
     */
    this.init = function(restriction) {
      var defer = self.BaseService.$q.defer();

      self.BaseService.$request(self.$baseUrl, "GET", {params: restriction})
        .then(function(response) {
          self.model = response.data;
          return defer.resolve(response.data);
        });

      return defer.promise;
    };

    /**
     * It performs a view creation on API call and stores in cache
     * 
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.create = function(viewObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl, "POST", {
        data: viewObject,
        headers: {
          "Content-Type": "application/json"
        }
      })
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        var errMessage = "Error creating View. \n" + (err.data ? err.data.message : "");
        return defer.reject(errMessage);
      })

      return defer.promise;
    };

    /**
     * It performs a view creation on API call.
     * 
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.list = function(restriction) {
      return self.BaseService.$filter('filter')(self.model, restriction);
    };
    
    /**
     * It performs a view creation on API call.
     * 
     * @param {number} viewId - A view identifier
     * @param {Object} viewObject - A view values
     * @returns {ng.IPromise}
     */
    this.get = function(viewId, restriction) {
      return self.BaseService.get(self.model, restriction);
    };

    /**
     * It performs a view update on API call
     * 
     * @param {number} viewId - A view identifier
     * @param {Object} viewObject - A view values to update
     * @returns {ng.IPromise}
     */
    this.update = function(viewId, viewObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl + "/" + viewId, "PUT", {
        data: viewObject
      }).then(function(response) {
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        var errMessage = "Error updating View. \n" + (err.data ? err.data.message : "");
        return defer.reject(errMessage);
      });

      return defer.promise;
    };
  } // end ViewService

  ViewService.$inject = ["BaseService", "$q"];

  return ViewService;
});