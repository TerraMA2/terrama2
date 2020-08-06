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
var cors = require('cors')
const common = require('./utils/common');

var app = express();

app.use(cors())

var webMonitorSession = session({ secret: KEY, name: "TerraMA2WebMonitor_" + (process.argv[2] !== undefined ? process.argv[2] : "default"), resave: false, saveUninitialized: false });

// reading TerraMA² config.json
var config = Application.getContextConfig();

app.locals.BASE_URL = config.webmonitor.basePath;
app.locals.ADMIN_URL = common.urlResolve(config.webadmin.public_uri, '/');
app.locals.INTERNAL_ADMIN_URL = config.webadmin.internal_uri;

// view engine setup
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'html');

app.use(logger('dev'));
app.use(cookieParser());
app.use(webMonitorSession);

passport.setupPassport(app);

app.use(bodyParser.json({ limit: '50mb' }));
app.use(bodyParser.urlencoded({ limit: '50mb', parameterLimit: 100000, extended: false }));

app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'public')));
app.use(app.locals.BASE_URL + 'locales', express.static(path.join(__dirname, 'locales')));

var externals = JSON.parse(fs.readFileSync(path.join(__dirname, './externals.json'), 'utf8'));

externals.forEach(function(external) {
  app.use(app.locals.BASE_URL + external.publicUrl, express.static(path.join(__dirname, external.path)));
});

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
