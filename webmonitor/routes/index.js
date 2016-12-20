var express = require('express');
var router = express.Router();
var fs = require("fs");
var path = require("path");

/* GET home page. */
router.get('/', function(req, res, next) {
  // reading TerraMA² config.json
  var hostInfo = JSON.parse(fs.readFileSync(path.join(__dirname, "../config/config.terrama2monitor"), "utf-8"));
  var webmonitorHostInfo = hostInfo.webmonitor;
  var webadminHostInfo = hostInfo.webadmin;
  res.render('index', { title: 'Express', webmonitorHostInfo: webmonitorHostInfo, webadminHostInfo: webadminHostInfo});
});

module.exports = router;
