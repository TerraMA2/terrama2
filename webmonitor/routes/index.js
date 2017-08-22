var fs = require("fs");
var path = require("path");

module.exports = function(app) {
  app.get(app.locals.BASE_URL, function(req, res, next) {
    // reading TerraMA² config.json
    var hostInfo = JSON.parse(fs.readFileSync(path.join(__dirname, "../config/monitor.json"), "utf-8"));
    var webmonitorHostInfo = hostInfo.webmonitor;
    var webadminHostInfo = hostInfo.webadmin;
    var message = {
      content: (req.query.message && req.query.message != "" ? req.query.message : "")
    };

    var state = JSON.parse(fs.readFileSync(path.join(__dirname, "../myjsonfile.json"), "utf-8"));

    res.render('index', { title: 'Express', webmonitorHostInfo: webmonitorHostInfo, webadminHostInfo: webadminHostInfo, message: message, state: state});
  });
};