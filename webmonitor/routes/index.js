var fs = require("fs");
var path = require("path");
var Application = require("./../core/Application");

module.exports = function(app) {
  app.get(app.locals.BASE_URL, function(req, res, next) {
    // reading TerraMA² config.json
    var hostInfo = Application.getContextConfig();

    var userConfig = Application.getContextConfig("user_conf");

    var webmonitorHostInfo = hostInfo.webmonitor;
    var webadminHostInfo = hostInfo.webadmin;
    var message = {
      content: (req.query.message && req.query.message != "" ? req.query.message : "")
    };
    res.render('index', { title: 'Express', message: message, userConfig: userConfig});
  });

  app.post(app.locals.BASE_URL + 'languages', function(request, response) {
    request.session.userLocale = request.body.locale || "us";
    return response.json({status: 200});
  });
};
