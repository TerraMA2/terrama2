"use strict";

var fs = require('fs');
var ImportProjectMember = require("./../core/ImportProject");
var projectFilePath = process.argv[2];
if (!projectFilePath) {
  console.log("Invalid path");
  return;
}
var DataManager = require("./../core/DataManager");
DataManager.init(function(err){
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
