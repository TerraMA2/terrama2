var fs = require("fs");
var path = require("path");
var Application = require("./../core/Application");

module.exports = function(app) {
  app.get(app.locals.BASE_URL, function(req, res, next) {
    // reading TerraMA² config.json
    var hostInfo = Application.getContextConfig();
    var webmonitorHostInfo = hostInfo.webmonitor;
    var webadminHostInfo = hostInfo.webadmin;
    var message = {
      content: (req.query.message && req.query.message != "" ? req.query.message : "")
    };
    res.render('index', { title: 'Express', webmonitorHostInfo: webmonitorHostInfo, webadminHostInfo: webadminHostInfo, message: message});
  });
};