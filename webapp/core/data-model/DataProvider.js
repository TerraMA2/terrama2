var AbstractData = require("./AbstractData");


var DataProvider = function(params) {
  AbstractData.apply(this, [{'class': 'DataProvider'}]);

  this.id = params.id;
  this.project_id = params.project_id;
  this.description = params.description;
  this.data_provider_type_name = params.data_provider_type_name;
  this.data_provider_intent_name = params.data_provider_intent_name;
  this.name = params.name;
  this.active = params.active;
  this.uri = params.uri;
};

DataProvider.prototype = Object.create(AbstractData.prototype);
DataProvider.prototype.constructor = DataProvider;

DataProvider.prototype.toObject = function() {
  return Object.assign(AbstractData.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    data_provider_type_name: this.data_provider_type_name,
    data_provider_intent_name: this.data_provider_intent_name,
    name: this.name,
    description: this.description,
    uri: this.uri,
    active: this.active
  });
};

module.exports = DataProvider;