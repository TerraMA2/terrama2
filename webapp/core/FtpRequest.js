'use strict';

var AbstractRequest = require('./AbstractRequest');
var Client = require('ftp');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var Form = require("./Enums").Form;
var FormField = Form.Field;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


var FtpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FtpRequest.prototype = Object.create(AbstractRequest.prototype);
FtpRequest.prototype.constructor = FtpRequest;

FtpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    var host = self.params[self.syntax().HOST];

    while(host.charAt(host.length - 1) == '/')
      host = host.substr(0, host.length - 1);

    var config = {
      user: self.params[self.syntax().USER],
      password: self.params[self.syntax().PASSWORD],
      host: host,
      port: self.params[self.syntax().PORT]
    };

    var client = new Client();

    client.on('ready', function() {
      client.list(self.params[self.syntax().PATHNAME], function(err, list) {
        if(err) {
          var error;
          switch (err.code) {
            case 450:
              error = new Exceptions.ConnectionError("Invalid path");
              break;
            default:
              error = new Exceptions.ConnectionError("Error in connection");
          }

          client.end();
          return reject(error);
        } else {
          client.end();
          return resolve();
        }
      });
    });

    client.on('error', function(err) {
      var error;
      var syntax = self.syntax();
      switch (err.code) {
        case "ENOTFOUND":
        case 421:
          error = new Exceptions.ConnectionError("Host not found");
          break;
        case "ECONNREFUSED":
          error = new Exceptions.ConnectionError("Error in connection, check the host and the port");
          break;
        case 530:
          error = new Exceptions.ConnectionError("Username or password does not match");
          break;
        default:
          error = new Exceptions.ConnectionError("Error in connection");
      }

      client.end();
      return reject(error);
    });

    client.connect(config);
  });
};

FtpRequest.fields = function() {
  var fieldProperties = {};
  fieldProperties[UriPattern.HOST] = {
    title: "Address",
    type: FormField.TEXT
  }

  fieldProperties[UriPattern.PORT] = {
    title: "Port",
    type: FormField.NUMBER,
    default: 21
  };

  fieldProperties['timeout'] = {
    title: "Data server Timeout",
    type: FormField.NUMBER,
    default: 8
  };

  fieldProperties['active_mode'] = {
    title: "Active mode",
    type: 'boolean'
  };

  fieldProperties[UriPattern.USER] = {
    title: "Username",
    type: FormField.TEXT
  };

  fieldProperties[UriPattern.PASSWORD] = {
    title: "Password",
    type: FormField.TEXT
  };

  fieldProperties[UriPattern.PATHNAME] = {
    title: "Path",
    type: FormField.TEXT
  };

  var orderFields = [
    {
      key: UriPattern.HOST,
      type: FormField.TEXT,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.PORT,
      type: FormField.NUMBER,
      htmlClass: 'col-md-2 terrama2-schema-form'
    },
    {
      key:'timeout',
      type: FormField.NUMBER,
      htmlClass: 'col-md-2 terrama2-schema-form'
    },
    {
      key: 'active_mode',
      type: 'checkbox',
      htmlClass: 'col-md-2 terrama2-schema-form-checkbox'
    },
    {
      key: UriPattern.USER,
      type: FormField.TEXT,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.PASSWORD,
      type: FormField.PASSWORD,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.PATHNAME,
      type: Form.Field.TEXT,
      htmlClass: 'col-md-12 terrama2-schema-form'
    }
  ];

  var fieldsObject = {
    name: "FTP",
    properties: fieldProperties,
    required: [UriPattern.HOST, UriPattern.PORT],
    display: orderFields
  };

  return fieldsObject;
};

module.exports = FtpRequest;
