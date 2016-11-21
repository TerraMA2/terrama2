(function() {
  'use strict';

  angular.module("terrama2.administration.services.iservices", ["terrama2", "terrama2.services"])
    .service("Service", Service)
    .constant("ServiceType", {
      COLLECTOR: 1,
      ANALYSIS: 2,
      VIEW: 3  
    });

  /**
   * It handles TerraMA² service like dao.
   * 
   * @param {ServiceInstanceFactory} ServiceInstanceFactory - TerraMA² factory to build http requests
   * @param {i18n} i18n - Internationalization module
   * @param {ng.IPromise} $q - Angular promiser 
   */
  function Service(ServiceInstanceFactory, i18n, $q, ServiceType, $filter) {
    /**
     * Cached TerraMA² services
     * @type {Object[]}
     */
    this.model = [];
    /**
     * Angular promiser
     * @type {ng.IPromise}
     */
    this.$q = $q;
    /**
     * @type {Object}
     */
    this.factory = ServiceInstanceFactory;
    /**
     * TerraMA² Service Types Supported
     * @type {Object}
     */
    this.types = ServiceType;
    /**
     * Angular Filter module. Used to filter services from given restriction
     * 
     * @type {angular.IFilter}
     */
    this.$filter = $filter;
  }

  /**
   * It performs a service initialization, loading cache services
   * @returns {Promise}
   */
  Service.prototype.init = function() {
    /**
     * @type {Service}
     */
    var self = this;
    var promiser = self.$q.defer();
    self.$request()
      .then(function(){
        return promiser.resolve();
      })
      .catch(function(err) {
        return promiser.reject(err);
      });
    return promiser.promise;
  };

  /**
   * It performs a get operation in order to retrieve services from server.
   * @returns {Promise}
   */
  Service.prototype.$request = function() {
    /**
     * @type {Service}
     */
    var self = this;
    var promiser = self.$q.defer();
    self.factory.get()
      .success(function(services) {
        self.model = services;
        return promiser.resolve(self.model);
      })
      .error(function(err) {
        return promiser.reject(new Error("Could not retrieve services from host " + err.toString()));
      });
    return promiser.promise;
  }

  /**
   * It retrieves a list of services
   * @returns {Object[]}
   */
  Service.prototype.list = function(restriction) {
    return this.$filter('filter')(this.model, restriction || {});
  };

  /**
   * It retrieves a single service from given id
   * 
   * @param {number} serviceId - A TerraMA² service identifier
   * @returns {Object}
   */
  Service.prototype.get = function(serviceId) {
    var output = null;
    this.model.some(function(instance) {
      if (instance.id === serviceId) {
        output = instance;
        return true;
      }
    });
    return output;
  };

  /**
   * It performs a update operation in ServiceInstanceFactory.
   * 
   * @param {number} serviceId - TerraMA² Service identifier 
   * @param {Object} serviceObject - a javascript object with service values
   * @returns {Object}
   */
  Service.prototype.update = function(serviceId, serviceObject) {
    return this.factory.put(serviceId, serviceObject);
  };

  /**
   * It performs a save operation in ServiceInstanceFactory.
   * 
   * @param {Object} serviceObject - a javascript object with service values
   * @returns {Object}
   */
  Service.prototype.create = function(serviceObject) {
    return this.factory.post(serviceObject);
  };

}());