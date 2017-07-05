define(function(){
  function LegendService(BaseService, $q){
    var self = this;
    this.BaseService = BaseService;
    this.$q = $q;
    this.$baseUrl = BASE_URL + "api/Legend";
    this.model = [];

    /**
     * It retrieves all legends and cache them in model.
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
     * It performs a legend creation on API call and stores in cache
     * 
     * @param {Object} legendObject - A legend values
     * @returns {ng.IPromise}
     */
    this.create = function(legendObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl, "POST", {
        data: legendObject,
        headers: {
          "Content-Type": "application/json"
        }
      })
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        var errMessage = "Error creating Legend. \n" + (err.data ? err.data.message : "");
        return defer.reject(errMessage);
      })

      return defer.promise;
    };

    /**
     * It performs a legend update on API call and stores in cache
     * 
     * @param {number} legendId - An legend identifier
     * @param {Object} legendObject - An legend values
     * @returns {ng.IPromise}
     */
    this.update = function(legendId, legendObject) {
      var defer = self.$q.defer();
      self.BaseService.$request(self.$baseUrl + "/" + legendId, "PUT", {
        data: legendObject
      }).then(function(response) {
        return defer.resolve(response.data);
      })
      .catch(function(err) {
        var errMessage = "Error updating Legend. \n" + (err.data ? err.data.message : "");
        return defer.reject(errMessage);
      });

      return defer.promise;
    };

    /**
     * It lists the legends.
     * 
     * @param {Object} restriction - The restriction
     * @returns {ng.IPromise}
     */
    this.list = function(restriction) {
      return self.BaseService.$filter('filter')(self.model, restriction);
    };
    
  }

  LegendService.$inject = ["BaseService", "$q"];

  return LegendService;
});