define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/data-series/services/cemaden"
], (commonServices, CemadenService) => {
  'use strict';

  var moduleName = "terrama2.dataseries.services";

  function ConfigureServiceModule($httpProvider) {
    delete $httpProvider.defaults.headers.common['X-Requested-With'];
  }
  ConfigureServiceModule.$inject = ['$httpProvider'];

  angular.module(moduleName, [commonServices])
    /**
     * It defines a TerraMA² Data Series Type available.
     *
     * @readonly
     * @enum {string}
     */
    .config(ConfigureServiceModule)
    .constant("DataSeriesType", {
      DCP: 'DCP',
      OCCURRENCE: 'OCCURRENCE',
      GRID: 'GRID',
      ANALYSIS_MONITORED_OBJECT: 'ANALYSIS_MONITORED_OBJECT',
      GEOMETRIC_OBJECT: 'GEOMETRIC_OBJECT',
      POSTGIS: 'POSTGIS',
      VECTOR_PROCESSING_OBJECT: 'VECTOR_PROCESSING_OBJECT'
    })
    .service("DataSeriesService", ["BaseService", "DataSeriesType", "$filter", "$q", DataSeriesService])
    .service("DataSeriesSemanticsService", ["BaseService", "$q", DataSeriesSemanticsService])
    .service("CemadenService", CemadenService)
    .factory("SemanticsParserFactory", [SemanticsParserFactory])
    .factory("SemanticsHelpers", ["StringDiff", WrapSemanticsHelpers])
    .factory("SemanticsLibs", ["SemanticsHelpers", "SemanticsParserFactory", SemanticsLibs]);

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
    this.url = BASE_URL + "api/DataSeries";
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
      .then(function(response) {
        self.model = response.data;
        return defer.resolve(response.data);
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
   * It performs DataSeries update over API
   *
   * @param {number} dataSeriesId - Data Series ID to update
   * @param {Object} obj - Values to update. It may contain input/output data series values
   * @returns {angular.IPromise}
   */
  DataSeriesService.prototype.update = function(dataSeriesId, obj) {
    var defer = this.$q.defer();
    var self = this;
    self.BaseService.$request(self.url + "/" + dataSeriesId, "PUT", {data: obj})
      .then(function(response) {
        return defer.resolve(response.data);
      }).catch(function(err) {
        return defer.reject(err.data);
      });

    return defer.promise;
  };
  /**
   * It creates a Data Series on remote API
   *
   * @param {Object} dataSeriesObject - A data series object values to save
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.create = function(dataSeriesObject) {
    var defer = this.$q.defer();
    var self = this;
    this.BaseService.$request(this.url, "POST", {
        data: dataSeriesObject
      })
      .then(function(response) {
        self.model.push(response.data);
        return defer.resolve(response.data);
      }).catch(function(err) {
        return defer.reject(err.data);
      });
    return defer.promise;
  };

  /**
   *
   */
  DataSeriesService.prototype.validateView = async function(tableName, provider, queryBuilder) {
    const { BaseService, url } = this;

    const bodyData = { provider, tableName, whereCondition: queryBuilder };

    try {
      const response = await BaseService.$request(`${url}/validateView`, "POST", { data: bodyData });

      return response.status === 200;
    } catch (errResponse) {
      throw new Error(`View Error: ${errResponse.data.error}`);
    }

  };

  /**
   * Retrieves icon information of DataSeries based in semantics
   *
   * @param {object} dataSeries - DataSeries metadata to compose icon
   * @returns {string} icon path
   */
  DataSeriesService.prototype.getIcon = function(dataSeries) {
    const dataSeriesTypeName = dataSeries.data_series_semantics.data_series_type_name;
    const { DataSeriesType } = this;

    switch(dataSeriesTypeName) {
      case DataSeriesType.ANALYSIS_MONITORED_OBJECT:
        return `${BASE_URL}images/analysis/monitored-object/monitored-object_analysis.png`;
      case DataSeriesType.VECTOR_PROCESSING_OBJECT:
        return `${BASE_URL}images/analysis/vectorial-processing/vector-processing.svg`;
      case DataSeriesType.GRID:
        if (dataSeries.isAnalysis){
          return `${BASE_URL}images/analysis/grid/grid_analysis.png`;
        }

        if (dataSeries.isInterpolator) {
          return `${BASE_URL}images/interpolation/grid/grid.png`;
        }

        return `${BASE_URL}images/dynamic-data-series/grid/grid.png`;
      case DataSeriesType.OCCURRENCE:
        return `${BASE_URL}images/dynamic-data-series/occurrence/occurrence.png`;
      case DataSeriesType.GEOMETRIC_OBJECT:
        if (dataSeries.data_series_semantics.temporality === 'STATIC') {
          return `${BASE_URL}images/static-data-series/vetorial/vetorial.png`;
        }
        return `${BASE_URL}images/dynamic-data-series/geometric-object/geometric-object.png`;
      default:
        return `${BASE_URL}images/dynamic-data-series/dcp/dcp.png`;
    }
  };

  /**
   * It retrieves all dynamic data series from API
   *
   * @param {Object} restriction - a query restriction
   * @returns {ng.IPromise}
   */

  DataSeriesService.prototype.dynamicDataSeries = function() {
    return this.list({data_series_semantics:{temporality:"DYNAMIC", "format": "POSTGIS"}});
  }

  /**
   * It retrieves all static data series from API
   *
   * @param {Object} restriction - a query restriction
   * @returns {ng.IPromise}
   */

  DataSeriesService.prototype.staticDataSeries = function() {
    return this.list({data_series_semantics:{temporality:"STATIC", "format": "POSTGIS"}});
  }

  /**
   * It duplicates a Data Series of another project on remote API
   *
   * @param {Object} dataSeriesObject - A data series object values to duplicate
   * @returns {ng.IPromise}
   */
  DataSeriesService.prototype.duplicate = function(dataSeriesObject) {
    var defer = this.$q.defer();
    var self = this;
    this.BaseService.$request(this.url + "/duplicate", "POST", {
        data: dataSeriesObject
      })
      .then(function(response) {
        return defer.resolve(response.data);
      }).catch(function(err) {
        return defer.reject(err.data);
      });
    return defer.promise;
  };
  /**
   * Data Series Semantics service DAO
   *
   * @class DataSeriesSemanticsService
   * @param {BaseService} BaseService - A TerraMA² base service request
   * @param {DataSeriesType} DataSeriesType - A const TerraMA² enum for handling data series type
   */
  function DataSeriesSemanticsService(BaseService, $q) {
    this.BaseService = BaseService;
    this.url = BASE_URL + "api/DataSeriesSemantics/";
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
      .then(function(response) {
        self.model = response.data;
        return defer.resolve(response.data);
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
    var semantics = this.BaseService.get(this.model, restriction);

    if (semantics.metadata) {
      semantics.metadata.form.forEach(function(form) {
        if(form.htmlClass.indexOf("validate-mask") !== -1) {
          form.$validators = {
            validateMask: function(value) {
              if(value) {
                for(var i = 0, valueLength = value.length; i < valueLength; i++) {
                  var charCode = value.charCodeAt(i);

                  if(value[i] === "%") {
                    if(
                      value.substr(i, 5) === "%YYYY" ||
                      value.substr(i, 4) === '%JJJ' ||
                      value.substr(i, 3) === "%YY" || value.substr(i, 3) === "%MM" || value.substr(i, 3) === "%DD" ||
                      value.substr(i, 3) === "%hh" || value.substr(i, 3) === "%mm" || value.substr(i, 3) === "%ss" ||
                      value.substr(i, 2) === "%("
                    )
                      continue;

                    return false;
                  }

                  if(value[i] == "(" && i > 0 && value[i-1] === "%")
                  {
                    let j = value.indexOf(")%", i);
                    if(j === -1)
                      return false;

                    i = j+1;
                    continue;
                  }

                  if(value[i] === "*" || value[i] === "%" || value[i] === "." || value[i] === "-" || value[i] === "_" || value[i] === "/")
                    continue;

                  if((charCode < 48) || ((charCode > 57) && (charCode < 65)) || ((charCode > 90) && (charCode < 97)) || (charCode > 122))
                    return false;
                }

                return true;
              } else
                return false;
            }
          };

          form.validationMessage.validateMask = "Invalid mask";
        }
      });
    }

    return semantics;
  };

  function SemanticsLibs(SemanticsHelpers, SemanticsParserFactory) {
    return {
      utility: SemanticsHelpers,
      parsers: SemanticsParserFactory
    };
  }

  /**
   * It defines availables methods used in semantics.json
   * Remember to call initialize whenever you want to use it even when resetting model due model reference
   *
   * @class SemanticsHelpers
   */
  function WrapSemanticsHelpers(StringDiff) {
    function SemanticsHelpers() {
      /**
       * Defines ngmodel reference
       * @type {Object}
       */
      var _model = null;

      /**
       * It injects the available functions into scope variable.
       *
       * @param {any} formModel - Angular ngmodel
       */
      this.init = function(formModel) {
        _model = formModel;
      };

      /**
       * It forces the user to match with regex expression. Normally, you may specify expression with only valid values
       * in order to block the un-match keys
       *
       * @param {string} key - NgModel key
       * @param {any} value - NgModel value
       * @param {string} expression - Regex expression
       */
      this.only = function(key, value, expression) {
        var regex = new RegExp(expression);
        var results = regex.exec(value);
        if (results && results[0] !== results.input) {
          var diff = StringDiff(results[0], value);
          _model[key[0]] = value.replace(diff, "");
        }
      };
    }

    return SemanticsHelpers;
  }

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

  return moduleName;
});