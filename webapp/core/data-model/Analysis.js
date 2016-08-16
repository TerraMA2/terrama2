"use strict";

var BaseClass = require("./AbstractData");
var Schedule = require("./Schedule");
var AnalysisOutputGrid = require("./AnalysisOutputGrid");
var isObject = require("./../Utils").isObject;

var Analysis = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Analysis'});
  this.id = params.id;
  this.project_id = params.project_id;
  this.script = params.script;

  if (params.ScriptLanguage) {
    this.script_language = params.ScriptLanguage.get();
  } else {
    this.script_language = params.script_language || {};
  }

  if (params.AnalysisType) {
    this.type = params.AnalysisType.get();
  } else {
    this.type = params.type || {};
  }

  this.name = params.name;
  this.description = params.description;
  this.active = params.active;
  this.dataset_output = params.dataset_output;

  if (params.AnalysisMetadata) {
    this.setMetadata(params.AnalysisMetadata);
  } else {
    this.metadata = params.metadata || {};
  }

  this.analysis_dataseries_list = [];

  if (params.Schedule) {
    this.schedule = new Schedule(params.Schedule.get() || {});
  } else {
    this.schedule = params.schedule;
  }

  this.instance_id = params.instance_id;
  this.dataSeries = {};

  if (params.AnalysisOutputGrid) {
    this.setAnalysisOutputGrid(params.AnalysisOutputGrid);
  } else {
    this.setAnalysisOutputGrid(params.outputGrid || {});
  }
};

Analysis.prototype = Object.create(BaseClass.prototype);
Analysis.prototype.constructor = Analysis;

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

  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    script: this.script,
    script_language: this.script_language.id,
    type: this.type.id,
    name: this.name,
    description: this.description,
    active: this.active,
    output_dataseries_id: this.dataset_output,
    metadata: this.metadata,
    'analysis_dataseries_list': outputDataSeriesList,
    schedule: this.schedule instanceof BaseClass ? this.schedule.toObject() : this.schedule,
    service_instance_id: this.instance_id,
    output_grid: this.outputGrid instanceof BaseClass ? this.outputGrid.toObject() : this.outputGrid
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

  obj.dataSeries = this.dataSeries instanceof BaseClass ? this.dataSeries.rawObject() : this.dataSeries;
  obj.analysis_dataseries_list = outputDataSeriesList;
  obj.type = this.type;
  return obj;
};
