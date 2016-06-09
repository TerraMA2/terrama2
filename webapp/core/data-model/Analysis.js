var BaseClass = require("./AbstractData");

var Analysis = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Analysis'});
  this.id = params.id;
  this.project_id = params.project_id;
  this.script = params.script;

  if (params.ScriptLanguage)
    this.script_language = params.ScriptLanguage.get();
  else
    this.script_language = params.script_language || {};

  if (params.AnalysisType)
    this.type = params.AnalysisType.get();
  else
    this.type = params.type || {};

  this.name = params.name;
  this.description = params.description;
  this.active = params.active;
  this.dataset_output = params.dataset_output;

  if (params.AnalysisMetadata)
    this.setMetadata(params.AnalysisMetadata);
  else
    this.metadata = params.metadata || {};

  this.analysis_dataseries_list = [];

  if (params.Schedule)
    this.schedule = params.Schedule.get() || {};
  else
    this.schedule = params.schedule;

  this.instance_id = params.instance_id;
};

Analysis.prototype = Object.create(BaseClass.prototype);
Analysis.prototype.constructor = Analysis;

Analysis.prototype.addAnalysisDataSeries = function(analysisDataSeries) {
  this.analysis_dataseries_list.push(analysisDataSeries);
};

Analysis.prototype.setScriptLanguage = function(scriptLanguage) {
  if (scriptLanguage.get)
    this.script_language = scriptLanguage.get() || {};
  else
    this.script_language = scriptLanguage || {};
};

Analysis.prototype.setSchedule = function(schedule) {
  if (schedule.Schedule)
    this.schedule = schedule.Schedule.get() || {};
  else
    this.schedule = schedule || {};
}

Analysis.prototype.setMetadata = function(metadata) {
  var meta = {};
  if (metadata instanceof Array) {
    // array of sequelize model
    metadata.forEach(function(element) {
      meta[element.key] = element.value;
    })
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
    if (analysisDataSeries instanceof BaseClass)
      outputDataSeriesList.push(analysisDataSeries.toObject());
    else
      outputDataSeriesList.push(analysisDataSeries);
  })
  return outputDataSeriesList;
}

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
    output_dataseries_id: this['dataset_output'],
    metadata: this.metadata,
    'analysis_dataseries_list': outputDataSeriesList,
    schedule: this['schedule'] instanceof BaseClass ? this['schedule'].toObject() : this['schedule'],
    service_instance_id: this.instance_id
  });
};

Analysis.prototype.rawObject = function() {
  var obj = this.toObject();

  obj.type = this.type;
  return obj;
}
