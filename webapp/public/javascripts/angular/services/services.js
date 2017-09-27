define([
  "TerraMA2WebApp/common/services/index"
], function(commonServiceApp) {
  'use strict';

  var moduleName = "terrama2.administration.services.iservices";

  angular.module(moduleName, [commonServiceApp])
    .service("Service", ["BaseService", "i18n", "$q", "ServiceType", Service])
    .constant("ServiceType", {
      COLLECTOR: 1,
      ANALYSIS: 2,
      VIEW: 3,
      ALERT: 4,
      INTERPOLATION: 5
    });

  /**
   * It handles TerraMA² service like dao.
   * 
   * @param {BaseService} BaseService - TerraMA² factory to build http requests
   * @param {i18n} i18n - Internationalization module
   * @param {ng.IPromise} $q - Angular promiser 
   */
  function Service(BaseService, i18n, $q, ServiceType) {
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
    this.BaseService = BaseService;
    /**
     * TerraMA² Service Types Supported
     * @type {Object}
     */
    this.types = ServiceType;

    this.url = BASE_URL + "api/Service";
  }

  /**
   * It performs a service initialization, loading cache services
   * @returns {Promise}
   */
  Service.prototype.init = function(params) {
    /**
     * @type {Service}
     */
    var self = this;
    var promiser = self.$q.defer();
    self.BaseService.$request(self.url, "GET", params)
      .then(function(response){
        self.model = response.data;
        return promiser.resolve(response.data);
      })
      .catch(function(err) {
        return promiser.reject(err);
      });
    return promiser.promise;
  };

  /**
   * It retrieves a list of services
   * @returns {Object[]}
   */
  Service.prototype.list = function(restriction) {
    return this.BaseService.$list(this.model, restriction);
  };

  /**
   * It retrieves a single service from given id
   * 
   * @param {number} serviceId - A TerraMA² service identifier
   * @returns {Object}
   */
  Service.prototype.get = function(serviceId) {
    return this.BaseService.get(this.model, {id: serviceId});
  };

  /**
   * It performs a update operation in BaseService.
   * 
   * @param {number} serviceId - TerraMA² Service identifier 
   * @param {Object} serviceObject - a javascript object with service values
   * @returns {Object}
   */
  Service.prototype.update = function(serviceId, serviceObject) {
    var defer = this.$q.defer();

    this.BaseService.$request(this.url + "/" + serviceId, "PUT", {data: serviceObject})
      .then(function(response) {
        return defer.resolve(response.data);
      }).catch(function(err) {
        return defer.reject(err.data);
      });;

    return defer.promise;
  };

  /**
   * It performs a save operation in BaseService.
   * 
   * @param {Object} serviceObject - a javascript object with service values
   * @returns {Object}
   */
  Service.prototype.create = function(serviceObject) {
    var self = this;
    var defer = self.$q.defer();
    self.BaseService.$request(self.url, "POST", {data: serviceObject})
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      })
      .catch(function(response) {
        return defer.reject(response.data);
      });

    return defer.promise;
  };

  return moduleName;

});
