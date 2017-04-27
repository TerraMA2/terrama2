define(function(){
  function AlertService(BaseService, $q){
    var self = this;
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = "/api/Alert";
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
     * It performs a alert creation on API call and stores in cache
     * 
     * @param {Object} alertObject - A alert values
     * @returns {ng.IPromise}
     */
    this.create = function(alertObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl, "POST", {
        data: alertObject,
        headers: {
          "Content-Type": "application/json"
        }
      })
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        return defer.reject(err);
      })

      return defer.promise;
    };

    /**
     * It performs a alert creation on API call and stores in cache
     * 
     * @param {number} alertId - An alert identifier
     * @param {Object} alertObject - An alert values
     * @returns {ng.IPromise}
     */
    this.update = function(alertId, alertObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl + "/" + alertId, "PUT", {
        data: alertObject
      }).then(function(response) {
        return defer.resolve(response.data);
      });

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
    
  }

  AlertService.$inject = ["BaseService", "$q"];

  return AlertService;
});