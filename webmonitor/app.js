const KEY = 'terrama2Monitor.sid';

var express = require('express');
var path = require('path');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var fs = require("fs");
var swig = require('swig');
var passport = require('./config/Passport');
var session = require('express-session');
var load = require('express-load');
var io = require('socket.io')();
var Application = require('./core/Application');

var app = express();

var webMonitorSession = session({ secret: KEY, name: "TerraMA2WebMonitor_" + (process.argv[2] !== undefined ? process.argv[2] : "default"), resave: false, saveUninitialized: false });

// reading TerraMA² config.json
var config = Application.getContextConfig();

app.locals.BASE_URL = config.webmonitor.basePath;
app.locals.ADMIN_URL = config.webadmin.protocol + config.webadmin.host + (config.webadmin.port != "" ? ":" + config.webadmin.port : "") + config.webadmin.basePath;

// view engine setup
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'html');

app.use(logger('dev'));
app.use(cookieParser());
app.use(webMonitorSession);

passport.setupPassport(app);

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'bower_components')));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'public')));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, '../webcomponents/dist')));
app.use(app.locals.BASE_URL + 'require.js', express.static(path.join(__dirname, 'node_modules/requirejs/require.js')));
app.use(app.locals.BASE_URL + 'openlayers/ol.js', express.static(path.join(__dirname, 'node_modules/openlayers/dist/ol.js')));
app.use(app.locals.BASE_URL + 'openlayers/ol.css', express.static(path.join(__dirname, 'node_modules/openlayers/dist/ol.css')));

load('controllers').then('routes').into(app);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('index', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('index', {
    message: err.message,
    error: {}
  });
});

app.getIo = function() {
  return io;
};

module.exports = app;
