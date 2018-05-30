"use strict";

// dependencies
var BaseClass = require("./AbstractData");
var Schedule = require("./Schedule");
var AutomaticSchedule = require("./AutomaticSchedule");
var AnalysisOutputGrid = require("./AnalysisOutputGrid");
var AnalysisDataSeries = require("./AnalysisDataSeries");
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
    this.type = params.type || {id: Utils.isString(params.type_id) ? parseInt(params.type_id) : params.type_id};
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

  this.scheduleType = params.schedule_type;

  if (params.Schedule || params.schedule){
    this.schedule = new Schedule(params.Schedule ? params.Schedule.get() : params.schedule);
  } else {
    this.schedule = {};
  }

  this.automaticSchedule = new AutomaticSchedule(params.AutomaticSchedule ? params.AutomaticSchedule.get() : params.automaticSchedule || {});

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
};

Analysis.prototype = Object.create(BaseClass.prototype);
Analysis.prototype.constructor = Analysis;

/**
 * It appends a analysis data series to the list
 *
 * @param {AnalysisDataSeries} analysisDataSeries - An analysis data series object
 */
Analysis.prototype.addAnalysisDataSeries = function(analysisDataSeries) {
  if (analysisDataSeries instanceof BaseClass) {
    this.analysis_dataseries_list.push(analysisDataSeries);
  } else {
    this.analysis_dataseries_list.push(new AnalysisDataSeries(analysisDataSeries));
  }
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

Analysis.prototype.setAutomaticSchedule = function(automaticSchedule) {
  if (automaticSchedule.AutomaticSchedule) {
    this.automaticSchedule = new AutomaticSchedule(automaticSchedule.AutomaticSchedule.get() || {});
  } else {
    this.automaticSchedule = automaticSchedule || {};
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

  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    script: this.script,
    script_language: this.script_language.id,
    type: this.type.id,
    name: this.name,
    description: this.description || null,
    active: this.active,
    output_dataseries_id: this.dataSeries.id,
    output_dataset_id: this.dataset_output,
    metadata: this.metadata,
    'analysis_dataseries_list': outputDataSeriesList,
    schedule: this.schedule instanceof BaseClass ? this.schedule.toObject() : this.schedule,
    automatic_schedule: this.automaticSchedule instanceof BaseClass ? this.automaticSchedule.toObject() : this.automaticSchedule,
    service_instance_id: this.instance_id,
    output_grid: this.outputGrid instanceof BaseClass ? this.outputGrid.toObject() : this.outputGrid,
    schedule_type: this.scheduleType
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

  obj.automatic_schedule = this.automaticSchedule instanceof BaseClass ? this.automaticSchedule.rawObject() : this.automaticSchedule;
  obj.schedule = this.schedule instanceof BaseClass ? this.schedule.rawObject() : this.schedule;
  obj.dataSeries = this.dataSeries instanceof BaseClass ? this.dataSeries.rawObject() : this.dataSeries;
  obj.analysis_dataseries_list = outputDataSeriesList;
  obj.output_grid = this.outputGrid instanceof BaseClass ? this.outputGrid.rawObject() : this.outputGrid;
  obj.type = this.type;
  obj.schedule_type = this.scheduleType;
  return obj;
};
