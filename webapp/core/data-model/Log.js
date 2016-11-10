var AbstractClass = require("./AbstractData");

function Log(params) {
  AbstractClass.call(this, {'class': 'Log'});
  
  this.id = params.id;
  this.host = params.host;
  this.port = params.port;
  this.user = params.user;
  this.password = params.password;
  this.database = params.database;
  this.service_instance_id = params.service_instance_id;
}

Log.prototype = Object.create(AbstractClass.prototype);
Log.prototype.constructor = Log;

Log.prototype.rawObject = function() {
  return {
    id: this.id,
    host: this.host,
    port: this.port,
    user: this.user,
    password: this.password,
    database: this.database,
    service_instance_id: this.service_instance_id
  };
};

Log.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    PG_HOST: this.host,
    PG_PORT: this.port,
    PG_USER: this.user,
    PG_PASSWORD: this.password,
    PG_DB_NAME: this.database
  });
};

module.exports = Log;