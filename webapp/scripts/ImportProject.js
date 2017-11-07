"use strict";

var fs = require('fs');
var ImportProjectMember = require("./../core/ImportProject");

var configFile = process.argv[2];
var configFilePath = "";
if (!configFile){
  console.log("Invalid database configuration");
  return;
} else {
  configFilePath = "./../config/instances/" + configFile + ".json";
}

var projectFilePath = process.argv[3];
if (!projectFilePath) {
  console.log("Invalid project path");
  return;
}

var DataManager = require("./../core/DataManager");
DataManager.init(configFilePath, function(err){
  if (err){
    console.log("Error to load database. Error: " + err);
  }
  try {
    var projectJson = JSON.parse(fs.readFileSync(projectFilePath));
    DataManager.listServiceInstances({})
      .then(function(servicesResult){
        var services = {
          COLLECT: [],
          ANALYSIS: [],
          VIEW: [],
          ALERT: []
        };
        for(var j = 0, servicesLength = servicesResult.length; j < servicesLength; j++) {
          switch(servicesResult[j].service_type_id) {
            case 1:
              services.COLLECT.push(servicesResult[j]);
              break;
            case 2:
              services.ANALYSIS.push(servicesResult[j]);
              break;
            case 3:
              services.VIEW.push(servicesResult[j]);
              break;
            case 4:
              services.ALERT.push(servicesResult[j]);
              break;
            default:
              break;
          }
        }
        if(projectJson.Collectors !== undefined && projectJson.Collectors.length > 0 && services.COLLECT.length === 0) {
          throw "To import this file you need to have at least one collector service";
        } else if(projectJson.Analysis !== undefined && projectJson.Analysis.length > 0 && services.ANALYSIS.length === 0) {
          throw "To import this file you need to have at least one analysis service";
        } else if(projectJson.Views !== undefined && projectJson.Views.length > 0 && services.VIEW.length === 0) {
          throw "To import this file you need to have at least one view service";
        } else if(projectJson.Alerts !== undefined && projectJson.Alerts.length > 0 && services.ALERT.length === 0) {
          throw "To import this file you need to have at least one alert service";
        }

        if(projectJson.Collectors !== undefined && projectJson.Collectors.length > 0)
          projectJson['servicesCollect'] = services.COLLECT[0].id.toString();

        if(projectJson.Analysis !== undefined && projectJson.Analysis.length > 0)
          projectJson['servicesAnalysis'] = services.ANALYSIS[0].id.toString();

        if(projectJson.Views !== undefined && projectJson.Views.length > 0)
          projectJson['servicesView'] = services.VIEW[0].id.toString();

        if(projectJson.Alerts !== undefined && projectJson.Alerts.length > 0)
          projectJson['servicesAlert'] = services.ALERT[0].id.toString();

        ImportProjectMember(projectJson).then(function(result){
          console.log("Project successfully imported!");
        }).catch(function(result){
          throw "Error to import the Project. Error: " + result.err;
        });
      })
  }
  catch(err){
    var errorMessage = err.message ? err.message : err;
    console.log("Error: " + errorMessage);
  }
});
