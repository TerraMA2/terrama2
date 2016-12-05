"use strict";

// dependencies
var BaseClass = require("./AbstractData");
var Schedule = require("./Schedule");
var AnalysisOutputGrid = require("./AnalysisOutputGrid");
var ReprocessingHistoricalData = require("./ReprocessingHistoricalData");
var Utils = require("./../Utils");

var isObject = Utils.isObject;

/**
 * Analysis model representation
 * 
 * @param {Analysis | Object} params - A javascript object with values to build an analysis
 */
var Analysis = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Analysis'});
  /**
   * @name Analysis#id
   * @type {number}
   */
  this.id = params.id;
  /**
   * @name Analysis#project_id
   * @type {number}
   */
  this.project_id = params.project_id;
  /**
   * @name Analysis#script
   * @type {string}
   */
  this.script = params.script;

  if (params.ScriptLanguage) {
    /**
     * @name Analysis#script_language
     * @type {Object}
     */    
    this.script_language = params.ScriptLanguage.get();
  } else {
    /**
     * @name Analysis#script_language
     * @type {Object}
     */
    this.script_language = params.script_language || {};
  }

  if (params.AnalysisType) {
    this.type = params.AnalysisType.get();
  } else {
    this.type = params.type || {};
  }

  /**
   * @name Analysis#name
   * @type {string}
   */
  this.name = params.name;
  /**
   * @name Analysis#description
   * @type {string=}
   */
  this.description = params.description;
  /**
   * @name Analysis#active
   * @type {boolean}
   */
  this.active = params.active;
  /**
   * @name Analysis#dataset_output
   * @type {number}
   */
  this.dataset_output = params.dataset_output;

  if (params.AnalysisMetadata) {
    this.setMetadata(params.AnalysisMetadata);
  } else {
    this.metadata = params.metadata || {};
  }
  /**
   * @name Analysis#analysis_dataseries_list
   * @type {Array<Object>}
   */
  this.analysis_dataseries_list = [];

  if (params.Schedule) {
    this.schedule = new Schedule(params.Schedule.get() || {});
  } else {
    this.schedule = params.schedule;
  }

  /**
   * @name Analysis#instance_id
   * @type {number}
   */
  this.instance_id = params.instance_id;
  /**
   * @type {DataSeries}
   */
  this.dataSeries = {};

  if (params.AnalysisOutputGrid) {
    this.setAnalysisOutputGrid(params.AnalysisOutputGrid);
  } else {
    this.setAnalysisOutputGrid(params.outputGrid || {});
  }

  /**
   * @type {ReprocessingHistoricalData}
   */
  this.historicalData = null;

  if (params.ReprocessingHistoricalDatum) {
    this.setHistoricalData(params.ReprocessingHistoricalDatum);
  } else {
    this.setHistoricalData(params.historicalData);
  }
};

Analysis.prototype = Object.create(BaseClass.prototype);
Analysis.prototype.constructor = Analysis;

/**
 * It appends a analysis data series to the list
 * 
 * @param {AnalysisDataSeries} analysisDataSeries - An analysis data series object
 */
Analysis.prototype.addAnalysisDataSeries = function(analysisDataSeries) {
  this.analysis_dataseries_list.push(analysisDataSeries);
};

Analysis.prototype.setAnalysisOutputGrid = function(outputGrid) {
  var output = {};
  if (outputGrid.get) {
    output = new AnalysisOutputGrid(outputGrid.get());
  } else {
    if (isObject(outputGrid) && Object.keys(outputGrid).length === 0) {
      output = null;
    } else {
      output = new AnalysisOutputGrid(outputGrid);
    }
  }
  this.outputGrid = output;
};

/**
 * It creates and sets a ReprocessingHistoricalData to historicalData
 * 
 * @param {Object | ReprocessingHistoricalData}
 */
Analysis.prototype.setHistoricalData = function(historicalData) {
  if (historicalData instanceof BaseClass) {
    this.historicalData = historicalData;
  } else {
    if (historicalData && !Utils.isEmpty(historicalData)) {
      this.historicalData = new ReprocessingHistoricalData(historicalData);
    } else {
      this.historicalData = {};
    }
  }
};

Analysis.prototype.setDataSeries = function(dataSeries) {
  this.dataSeries = dataSeries;
};

Analysis.prototype.setScriptLanguage = function(scriptLanguage) {
  if (scriptLanguage.get) {
    this.script_language = scriptLanguage.get() || {};
  } else {
    this.script_language = scriptLanguage || {};
  }
};

Analysis.prototype.setSchedule = function(schedule) {
  if (schedule.Schedule) {
    this.schedule = new Schedule(schedule.Schedule.get() || {});
  } else {
    this.schedule = schedule || {};
  }
};

Analysis.prototype.setMetadata = function(metadata) {
  var meta = {};
  if (metadata instanceof Array) {
    // array of sequelize model
    metadata.forEach(function(element) {
      meta[element.key] = element.value;
    });
  } else {
    for(var key in metadata) {
      if (metadata.hasOwnProperty(key)) {
        meta[key] = metadata[key];
      }
    }
  }

  this.metadata = meta;
};

Analysis.prototype.getOutputDataSeries = function() {
  var outputDataSeriesList = [];
  this.analysis_dataseries_list.forEach(function(analysisDataSeries) {
    if (analysisDataSeries instanceof BaseClass) {
      outputDataSeriesList.push(analysisDataSeries.toObject());
    } else {
      outputDataSeriesList.push(analysisDataSeries);
    }
  });
  return outputDataSeriesList;
};

Analysis.prototype.toObject = function() {
  var outputDataSeriesList = this.getOutputDataSeries();
  var historicalData = this.historicalData instanceof BaseClass ? this.historicalData.toObject() : this.historicalData;

  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    script: this.script,
    script_language: this.script_language.id,
    type: this.type.id,
    name: this.name,
    description: this.description,
    active: this.active,
    output_dataseries_id: this.dataSeries.id,
    output_dataset_id: this.dataset_output,
    metadata: this.metadata,
    'analysis_dataseries_list': outputDataSeriesList,
    schedule: this.schedule instanceof BaseClass ? this.schedule.toObject() : this.schedule,
    service_instance_id: this.instance_id,
    output_grid: this.outputGrid instanceof BaseClass ? this.outputGrid.toObject() : this.outputGrid,
    reprocessing_historical_data: Utils.isEmpty(historicalData) ? null : historicalData
  });
};

Analysis.prototype.rawObject = function() {
  var outputDataSeriesList = [];
  this.analysis_dataseries_list.forEach(function(analysisDataSeries) {
    if (analysisDataSeries instanceof BaseClass) {
      outputDataSeriesList.push(analysisDataSeries.rawObject());
    } else {
      outputDataSeriesList.push(analysisDataSeries);
    }
  });
  var obj = this.toObject();

  var historicalData = this.historicalData instanceof BaseClass ? this.historicalData.rawObject() : this.historicalData;

  obj.reprocessing_historical_data = historicalData;
  obj.schedule = this.schedule instanceof BaseClass ? this.schedule.rawObject() : this.schedule;
  obj.dataSeries = this.dataSeries instanceof BaseClass ? this.dataSeries.rawObject() : this.dataSeries;
  obj.analysis_dataseries_list = outputDataSeriesList;
  obj.output_grid = this.outputGrid instanceof BaseClass ? this.outputGrid.rawObject() : this.outputGrid;
  obj.type = this.type;
  return obj;
};
