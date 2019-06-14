var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var WfsClient = require('wfs-client');
var Exceptions = require("./Exceptions");
// var NodeUtils = require('util');
// var Requester = require('request');
var Form = require('./Enums').Form;
var FormField = Form.Field;
var UriPattern = require("./Enums").Uri;

class WFSRequest extends AbstractRequest{
  request(){
    return new Promise(async (resolve, reject) => {
      this.uri = this.uri.replace("wfs:", "http:");

      try {
        const clientWfs = new WfsClient(this.uri, {"version":"1.1.0"});
        await clientWfs.capabilities();
        return resolve();
      }catch(e){
        reject(new Exceptions.ConnectionError(e.message));
      }
    })
  }

  static fields()
  {
    var properties = {};
    properties[UriPattern.HOST] = {
      title: "Address",
      type: FormField.TEXT
    };

    properties[UriPattern.PORT] = {
      title: "Port",
      minimum: 2,
      maximum: 65535,
      type: FormField.NUMBER,
      default: 80
    };

    properties['timeout'] = {
      title: "Data server Timeout (sec)",
      minimum: 1,
      type: FormField.NUMBER,
      default: 8
    };

    properties[UriPattern.USER] = {
      title: "Username",
      type: FormField.TEXT
    };

    properties[UriPattern.PASSWORD] = {
      title: "Password",
      type: FormField.TEXT
    };

    properties[UriPattern.PATHNAME] = {
      title: "Path",
      type: FormField.TEXT
    };

    return {
      name: "WFS",
      properties: properties,
      required: [UriPattern.HOST, UriPattern.PORT],
      display: [
        {
          key: UriPattern.HOST,
          type: FormField.TEXT,
          htmlClass: 'col-md-6 terrama2-schema-form',
          ngModelOptions: { "updateOn": "blur" }
        },
        {
          key: UriPattern.PORT,
          type: FormField.NUMBER,
          htmlClass: 'col-md-3 terrama2-schema-form'
        },
        {
          key:'timeout',
          type: FormField.NUMBER,
          htmlClass: 'col-md-3 terrama2-schema-form'
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
          htmlClass: 'col-md-12'
        }
      ]
    };
  }
}

module.exports = WFSRequest;