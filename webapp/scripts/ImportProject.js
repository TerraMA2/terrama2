"use strict";

var fs = require('fs');
var ImportProjectMember = require("./../core/ImportProject");

var configFilePath = process.argv[2];
if (!configFilePath){
  console.log("Invalid database configuration file path");
  return;
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
    ImportProjectMember(projectJson, function(result){
      if (result.err){
        console.log("Error to import the Project. Error: " + result.err);
      } else {
        console.log("Project successfully imported!");
      }
    });
  }
  catch(err){
    console.log("Error: " + err.message);
  }
});
