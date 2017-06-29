'use strict';

var AbstractRequest = require('./AbstractRequest');
var Client = require('ssh2').Client;
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var Form = require("./Enums").Form;
var FormField = Form.Field;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


var SftpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

SftpRequest.prototype = Object.create(AbstractRequest.prototype);
SftpRequest.prototype.constructor = SftpRequest;

SftpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    var host = self.params[self.syntax().HOST];

    while(host.charAt(host.length - 1) == '/')
      host = host.substr(0, host.length - 1);

    var config = {
      username: self.params[self.syntax().USER],
      password: self.params[self.syntax().PASSWORD],
      host: host,
      port: self.params[self.syntax().PORT]
    };

    var client = new Client();

    client.on('ready', function() {
      client.sftp(function(err, sftp) {
        if(err) {
          var error;
          switch (err.code) {
            case 2:
              error = new Exceptions.ConnectionError("Invalid path");
              break;
            default:
              error = new Exceptions.ConnectionError(err.message);
          }

          client.end();
          return reject(error);
        } else {
          sftp.readdir(self.params[self.syntax().PATHNAME], function(err, list) {
            if(err) {
              var error;
              switch (err.code) {
                case 2:
                  error = new Exceptions.ConnectionError("Invalid path");
                  break;
                default:
                  error = new Exceptions.ConnectionError(err.message);
              }

              client.end();
              return reject(error);
            } else {
              if(self.params.list) {
                var items = [];

                list = list.filter(function(a) { return a.filename.indexOf('/') === -1 && a.filename.indexOf('\\') === -1 });

                if(self.params[self.syntax().PATHNAME] == self.params.basePath) {
                  var lastChar = self.params[self.syntax().PATHNAME].substr(self.params[self.syntax().PATHNAME].length - 1);
                  self.params[self.syntax().PATHNAME] = (lastChar == '/' ? self.params[self.syntax().PATHNAME].slice(0, -1) : self.params[self.syntax().PATHNAME]);
                }

                for(var i = 0, listLength = list.length; i < listLength; i++) {
                  if(list[i].longname.charAt(0) == 'd' && list[i].filename.charAt(0) != '.')
                    items.push({
                      name: list[i].filename,
                      fullPath: self.params[self.syntax().PATHNAME] + '/' + list[i].filename,
                      children: [],
                      childrenVisible: false
                    });
                }

                items.sort(function(a, b) {
                  if(a.name < b.name) return -1;
                  if(a.name > b.name) return 1;
                  return 0;
                });

                client.end();
                return resolve({
                  list: items
                });
              } else {
                client.end();
                return resolve();
              }
            }
          });
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
          error = new Exceptions.ConnectionError(err.message);
      }

      client.end();
      return reject(error);
    });

    try {
      client.connect(config);
    } catch(err) {
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
          error = new Exceptions.ConnectionError(err.message);
      }

      client.end();
      return reject(error);
    }
  });
};

SftpRequest.fields = function() {
  var fieldProperties = {};
  fieldProperties[UriPattern.HOST] = {
    title: "Address",
    type: FormField.TEXT
  }

  fieldProperties[UriPattern.PORT] = {
    title: "Port",
    minimum: 2,
    maximum: 65535,
    type: FormField.NUMBER,
    default: 22
  };

  fieldProperties['timeout'] = {
    title: "Data server Timeout (sec)",
    minimum: 1,
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

  fieldProperties['file_explorer_button'] = {};

  var orderFields = [
    {
      key: UriPattern.HOST,
      type: FormField.TEXT,
      htmlClass: 'col-md-5 terrama2-schema-form'
    },
    {
      key: UriPattern.PORT,
      type: FormField.NUMBER,
      htmlClass: 'col-md-2 terrama2-schema-form'
    },
    {
      key:'timeout',
      type: FormField.NUMBER,
      htmlClass: 'col-md-3 terrama2-schema-form'
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
      htmlClass: 'col-md-11 terrama2-schema-form'
    },
    {
      key: 'file_explorer_button',
      type: 'button',
      htmlClass: 'col-md-1 terrama2-schema-form',
      style: 'btn-primary pull-right button-inline-form fa fa-folder',
      onClick: 'openFileExplorer(forms.connectionForm);'
    }
  ];

  var fieldsObject = {
    name: "SFTP",
    properties: fieldProperties,
    required: [UriPattern.HOST, UriPattern.PORT, UriPattern.USER, UriPattern.PASSWORD],
    display: orderFields
  };

  return fieldsObject;
};

module.exports = SftpRequest;
