const KEY = 'terrama2Monitor.sid';

var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var fs = require("fs");
var swig = require('swig');
var routes = require('./routes/index');
var users = require('./routes/users');
var login = require('./routes/login');
var passport = require('./config/Passport');
var session = require('express-session');
var sharedsession = require('express-socket.io-session');
var io = require('socket.io')();

var app = express();

var webMonitorSession = session({ secret: KEY, resave: false, saveUninitialized: false });

// reading TerraMA² config.json
var config = JSON.parse(fs.readFileSync(path.join(__dirname, "./config/config.terrama2monitor"), "utf-8"));

app.locals.BASE_URL = config.webmonitor.basePath;
app.locals.ADMIN_URL = config.webadmin.protocol + config.webadmin.host + (config.webadmin.port != "" ? ":" + config.webadmin.port : "") + config.webadmin.basePath;

// view engine setup
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'html');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(cookieParser());
app.use(webMonitorSession);

io.use(sharedsession(webMonitorSession, {
  autoSave: true
}));

passport.setupPassport(app);

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'bower_components')));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'public')));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, '../webcomponents/dist')));

app.use(app.locals.BASE_URL, routes);
app.use(app.locals.BASE_URL + 'users', users);
app.use(app.locals.BASE_URL + 'login', login);

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
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});

app.getIo = function() {
  return io;
};

module.exports = app;
