'use strict';

var AbstractRequest = require('./AbstractRequest');
var Client = require('ftp');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var Form = require("./Enums").Form;
var FormField = Form.Field;
var UriPattern = require("./Enums").Uri;
const isString = require('./Utils').isString;
const logger = require('./Logger');

/**
 * Parse FTP directory and retrieves content structure
 *
 * @throws Error could not parse file correctly
 * @param {string} fileList File paths
 */
function parseField(file) {
  const artifacts = file.split(' ');

  // Retrieve permission mode
  const permission = artifacts[0];

  return {
    name: artifacts[artifacts.length - 1],
    type: permission[0]
  };
}

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
          if(self.params.list) {
            try {
              var items = [];

              /**
               * Checking if the output is array of string paths
               *
               * It is important since the FTP provider may use different configurations to list directory.
               * Sometimes, got string or object.
               */

              list = list.map(elm => {
                if (isString(elm))
                  return parseField(elm);
                if (elm.name.indexOf('/') === -1 && elm.name.indexOf('\\') === -1)
                  return elm;
                return {};
              });

              if(self.params[self.syntax().PATHNAME] == self.params.basePath) {
                var lastChar = self.params[self.syntax().PATHNAME].substr(self.params[self.syntax().PATHNAME].length - 1);
                self.params[self.syntax().PATHNAME] = (lastChar == '/' ? self.params[self.syntax().PATHNAME].slice(0, -1) : self.params[self.syntax().PATHNAME]);
              }

              for(var i = 0, listLength = list.length; i < listLength; i++) {
                if(list[i].type == 'd' && list[i].name.charAt(0) != '.')
                  items.push({
                    name: list[i].name,
                    fullPath: self.params[self.syntax().PATHNAME] + '/' + list[i].name,
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
              return resolve({ list: items });
            } catch (error) {
              logger.error(`An unexpected error occurred while list FTP ${host}: ${error}`);
              // Close connection
              client.end();
              return reject(error);
            }
          } else {
            client.end();
            return resolve();
          }
        }
      });
    });

    client.on('error', function(err) {
      var error;
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
    minimum: 2,
    maximum: 65535,
    type: FormField.NUMBER,
    default: 21
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
      htmlClass: 'col-md-5 terrama2-schema-form',
      validationMessage: {
        "202": "Invalid address"
      },
      ngModelOptions: { "updateOn": "blur" }
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
    name: "FTP",
    properties: fieldProperties,
    required: [UriPattern.HOST, UriPattern.PORT],
    display: orderFields
  };

  return fieldsObject;
};

module.exports = FtpRequest;
