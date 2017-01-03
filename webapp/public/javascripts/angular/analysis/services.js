define([
  "TerraMA2WebApp/common/services/index"
], function(commonServiceApp) {
  'use strict';

  var moduleName = "terrama2.analysis.services";
    
  angular.module(moduleName, [commonServiceApp])
    .constant("AnalysisType", {
      DCP: 1,
      MONITORED: 2,
      GRID: 3
    })
    .service("AnalysisOperators", ["BaseService", "$log", function(BaseService, $log) {
      var self = this;
      this.$data = {};

      this.init = function() {
        return BaseService.$request("/javascripts/angular/analysis/data/operators.json", "GET", {})
          .then(function(response) {
            return self.$data = response.data;
          })
          .catch(function(err) {
            $log.log(err);
            throw error;
          });
      }
    }])
    .service("AnalysisService", ["BaseService", "AnalysisType", AnalysisService]);
  
  /**
   * It handles Analysis DAO using API calls
   * @class AnalysisService
   */
  function AnalysisService(BaseService, AnalysisType) {
    /**
     * Self reference
     * @type {AnalysisService}
     */
    var self = this;
    /**
     * It defines a cached analysis list retrieved from remote host or even created
     * @type {Analysis[]}
     */
    this.model = [];
    /**
     * It defines target API URL
     * @type {string} 
     */
    this.url = "/api/Analysis";
    /**
     * It defines enum for Analysis 
     * @type {AnalysisType}
     */
    this.types = AnalysisType;

    this.BaseService = BaseService;

    /**
     * It initializes Analysis DAO service, retrieving analysis instances and caching them
     * 
     * @param {Object} restriction - A query restriction
     * @returns {angular.Promise<Analysis[]>}
     */
    this.init = function(restriction) {
      var defer = self.BaseService.$q.defer();

      self.BaseService.$request(self.url, "GET", {params: restriction})
        .then(function(response) {
          self.model = response.data;
          return defer.resolve(response.data);
        });

      return defer.promise;
    };

    /**
     * It retrives cached analysis. You can filter given query restriction.
     * 
     * @example
     * AnalysisService
     *   .init()
     *   .then((data) => {
     *     console.log(AnalysisService.list({type_id: AnalysisType.GRID})); // it will display all grid analysis 
     *   })
     * 
     *  
     * @param {Object} restriction - A query restriction
     * @returns {Analysis[]}
     */
    this.list = function(restriction) {
      return self.BaseService.$list(self.model, restriction);
    };

    this.get = function(restriction) {
      return self.BaseService.get(self.model, restriction);
    };

    /**
     * It creates a analysis resource on remote host using API POST and then retrieve the resource created.
     * 
     * @param {Object} analysisObject - A javascript object with analysis values
     * @returns {angular.IPromise<Analysis>}
     */
    this.create = function(analysisObject) {
      var defer = self.BaseService.$q.defer();
      self.BaseService
        .$request(self.url, "POST", {data: analysisObject})
        .then(function(response) {
          self.model.push((response.data || {}).result);
          return defer.resolve(response.data);
        })
        .catch(function(err) {
          return defer.reject(err);
        });
      return defer.promise;
    };

    /**
     * It performs update analysis on remote host using TerraMA² PUT API
     * @todo Update cached analysis
     */
    this.update = function(analysisId, analysisObject) {
      return self.BaseService.$request(self.url + "/" + analysisId, "PUT", {data: analysisObject});
    };
  }

  return moduleName;
});