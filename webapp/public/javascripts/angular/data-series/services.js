(function() {
  'use strict';

  angular.module("terrama2.dataseries.services", ["terrama2"])
    /**
     * It defines a TerraMA² Data Series Type available.
     * 
     * @readonly
     * @enum {string}
     */
    .constant("DataSeriesType", {
      DCP: 'DCP',
      OCCURRENCE: 'OCCURRENCE',
      GRID: 'GRID',
      ANALYSIS_MONITORED_OBJECT: 'ANALYSIS_MONITORED_OBJECT',
      GEOMETRIC_OBJECT: 'GEOMETRIC_OBJECT',
      POSTGIS: 'POSTGIS'
    })
    .service("DataSeriesService", DataSeriesService)
    .factory("SemanticsParserFactory", SemanticsParserFactory)
    .service("DataSeriesSemanticsService", DataSeriesSemanticsService);
  
  /**
   * Data Series service DAO
   * 
   * @class DataSeriesService
   * @param {BaseService} BaseService - A TerraMA² base service request
   * @param {DataSeriesType} DataSeriesType - A const TerraMA² enum for handling data series type
   * @param {angular.IFilter} $filter - An angular $filter module.
   */
  function DataSeriesService(BaseService, DataSeriesType, $filter, $q) {
    this.BaseService = BaseService;
    this.url = "/api/DataSeries";
    this.DataSeriesType = DataSeriesType;
    this.$filter = $filter;
    /**
     * It defines a cached data series objects
     * @type {DataSeries[]}
     */
    this.model = [];
    this.$q = $q;
  }
  /**
   * It retrieves data series from remote host. You can specify a query restriction.
   * 
   * @param {Object} restriction - A query restriction
   * @returns {angular.IPromise<DataSeries[]>}
   */
  DataSeriesService.prototype.init = function(restriction) {
    var defer = this.$q.defer();
    var self = this;

    this.BaseService
      .$request(this.url, "GET", {params: restriction})
      .then(function(data) {
        self.model = data;
        return defer.resolve(data);
      })
      
      .catch(function(err) {
        return defer.reject(err);
      });
    return defer.promise;
  };
  /**
   * It retrieves all data series from API
   * 
   * @param {Object} restriction - a query restriction
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.list = function(restriction) {
    return this.$filter('filter')(this.model, restriction);
  };
  /**
   * It creates a Data Series on remote API
   * 
   * @param {Object} dataSeriesObject - A data series object values to save
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.create = function(dataSeriesObject) {
    return this.BaseService.$request(this.url, "POST", {
      data: dataSeriesObject
    });
  };

  // Angular Injecting Dependency
  DataSeriesService.$inject = ["BaseService", "DataSeriesType", "$filter", "$q"];

  
  
  /**
   * Data Series Semantics service DAO
   * 
   * @class DataSeriesSemanticsService
   * @param {BaseService} BaseService - A TerraMA² base service request
   * @param {DataSeriesType} DataSeriesType - A const TerraMA² enum for handling data series type
   */
  function DataSeriesSemanticsService(BaseService, $q) {
    this.BaseService = BaseService;
    this.url = "/api/DataSeriesSemantics/";
    /**
     * It defines a cached data series objects
     * @type {DataSeries[]}
     */
    this.model = [];
  }

  /**
   * It retrieves all data series semantics and cache them in model.
   * 
   * @param {Object} restriction
   * @returns {angular.IPromise<Object[]>}
   */
  DataSeriesSemanticsService.prototype.init = function(restriction) {
    var defer = this.BaseService.$q.defer();
    var self = this;

    this.BaseService
      .$request(this.url, "GET", {params: restriction})
      .then(function(data) {
        self.model = data;
        return defer.resolve(data);
      })
      .catch(function(err) {
        return defer.reject(err);
      });

    return defer.promise;
  };

  DataSeriesSemanticsService.prototype.list = function(restriction) {
    return this.BaseService.$list(this.model, restriction);
  };

  DataSeriesSemanticsService.prototype.get = function(restriction) {
    return this.BaseService.get(this.model, restriction);
  };


  /**
   * Class responsibles for processing semantics. Use it whenever you need format semantics before send to server
   * 
   * @returns {Object}
   */
  function SemanticsParserFactory() {
    return {
      /**
       * It parses input data series semantics, removing all keys that starts with "output_"
       *  
       * @param {Object} semanticsFormat - DataSeries Semantics input with keys
       * @returns {Object} Parse semantics
       */
      parseKeys: function(semanticsFormat) {
        var parsedKeys = {};
        Object.keys(semanticsFormat)
          .forEach(function(key) {
            if (key.startsWith("output_")) {
              parsedKeys[key.replace("output_", "")] = semanticsFormat[key];
            } else {
              parsedKeys[key] = semanticsFormat[key];
            }
          });
        return parsedKeys;
      }
    };
  }
} ());