var AbstractClass = require("./AbstractData");
var Utils = require("./../Utils");

function Service(params) {
  AbstractClass.call(this, {'class': 'Service'});

  this.id = params.id;
  this.name = params.name;
  this.host = params.host;
  this.sshUser = params.sshUser;
  this.sshPort = params.sshPort;
  this.port = params.port;
  this.pathToBinary = params.pathToBinary;
  this.runEnviroment = params.runEnviroment;
  this.numberOfThreads = params.numberOfThreads;
  this.description = params.description;
  this.log = params.log || {};
  this.service_type_id = params.service_type_id;
  this.maps_server_uri = params.maps_server_uri;
  this.setMetadata(params.metadata);
}

Service.prototype = Object.create(AbstractClass.prototype);
Service.prototype.constructor = Service;

/**
 * It populates service metadata
 *
 * @param {any} meta - Metadata to set
*/
Service.prototype.setMetadata = function setMetadata(meta) {
  var out = {};
  if (meta) {
    if (meta instanceof Array) {
      out = Utils.formatMetadataFromDB(meta);
    } else {
      out = meta || out;
    }
  }
  this.metadata = out;
};

Service.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    instance_id: this.id,
    instance_name: this.name,
    listening_port: this.port,
    number_of_threads: this.numberOfThreads,
    log_database: this.log instanceof AbstractClass ? this.log.toObject() : this.log,
    additional_info: this.metadata
  });
};

Service.prototype.rawObject = function() {
  return {
    id: this.id,
    name: this.name,
    host: this.host,
    sshUser: this.sshUser,
    sshPort: this.sshPort,
    port: this.port,
    pathToBinary: this.pathToBinary,
    runEnviroment: this.runEnviroment,
    numberOfThreads: this.numberOfThreads,
    description: this.description,
    service_type_id: this.service_type_id,
    log: this.log instanceof AbstractClass ? this.log.rawObject() : this.log,
    metadata: this.metadata
  };
}

module.exports = Service;
