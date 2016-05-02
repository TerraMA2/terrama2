var BaseClass = require("./AbstractData");

var Analysis = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Analysis'});
  this.id = params.id;
  this.project_id = params.project_id;
  this.script_language = params.script_language;
  this.type_id = params.type_id;
  this.name = params.name;
  this.description = params.description;
  this.active = params.active;
  this.dataset_output = params.dataset_output;
  this.metadata = params.metadata || {};
  this.analysis_dataseries_list = [];
  this.schedule_id = params.schedule_id;
};

Analysis.prototype = Object.create(BaseClass.prototype);
Analysis.prototype.constructor = Analysis;

Analysis.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    script_language: this.script_language,
    type: this['type_id'],
    name: this.name,
    description: this.description,
    active: this.active,
    output_dataset: this['dataset_output'],
    metadata: this.metadata,
    'analysis_dataseries_list': this.analysis_dataseries_list,
    schedule: this['schedule_id']
  });
};