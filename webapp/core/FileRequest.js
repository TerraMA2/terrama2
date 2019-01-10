'use strict';

var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var fs = require('fs');
var Form = require("./Enums").Form;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");
var Application = require("../core/Application");

var FileRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FileRequest.prototype = Object.create(AbstractRequest.prototype);
FileRequest.prototype.constructor = FileRequest;

FileRequest.prototype.request = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    // TODO: validate remote

    fs.stat(self.params[self.syntax().PATHNAME], function(err, stat) {
      if(err == null) {
        if(stat.isDirectory()) {
          fs.readdir(self.params[self.syntax().PATHNAME], function(err, list) {
            if(self.params.list) {
              var items = [];

              if(!err) {
                list = list.filter(function(a) { return a.indexOf('/') === -1 && a.indexOf('\\') === -1 });

                if(self.params[self.syntax().PATHNAME] == self.params.basePath) {
                  var lastChar = self.params[self.syntax().PATHNAME].substr(self.params[self.syntax().PATHNAME].length - 1);
                  self.params[self.syntax().PATHNAME] = (lastChar == '/' ? self.params[self.syntax().PATHNAME].slice(0, -1) : self.params[self.syntax().PATHNAME]);
                }

                for(var i = 0, listLength = list.length; i < listLength; i++) {
                  var fullPath = self.params[self.syntax().PATHNAME] + '/' + list[i];

                  try {
                    var isDirectory = fs.statSync(fullPath).isDirectory();
                  } catch(error) {
                    var isDirectory = false;
                  }

                  if(isDirectory && list[i].charAt(0) != '.')
                    items.push({
                      name: list[i],
                      fullPath: fullPath,
                      children: [],
                      childrenVisible: false
                    });
                }

                items.sort(function(a, b) {
                  if(a.name < b.name) return -1;
                  if(a.name > b.name) return 1;
                  return 0;
                });
              }

              return resolve({
                list: items
              });
            } else {
              return resolve();
            }
          });
        } else return reject(new Exceptions.ConnectionError("Invalid path"));
      } else return reject(new Exceptions.ConnectionError("Invalid path"));
    });
  });
};

FileRequest.fields = function() {
  var properties = {};

  var configFile = Application.getContextConfig();

  // configFile.defaultFilePathList - The first list position returns the default file path.

  properties[UriPattern.PATHNAME] = {
    title: "Path",
    type: Form.Field.TEXT,
    default: configFile.defaultFilePathList[0]
  };

  properties['file_explorer_button'] = {};

  return {
    "name": "FILE",
    properties: properties,
    required: [UriPattern.PATHNAME],
    display: [
      {
        key: UriPattern.PATHNAME,
        type: Form.Field.TEXT,
        htmlClass: 'col-md-11'
      },
      {
        key: 'file_explorer_button',
        type: 'button',
        htmlClass: 'col-md-1 terrama2-schema-form',
        style: 'btn-primary pull-right button-inline-form fa fa-folder',
        onClick: 'openFileExplorer(forms.connectionForm);'
      }
    ]
  };
};


module.exports = FileRequest;
