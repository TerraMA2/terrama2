const KEY = 'terrama2.sid';

var express = require('express'),
    path = require('path'),
    fs = require('fs'),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override'),
    app = express(),
    load = require('express-load'),
    swig = require('swig'),
    passport = require('./config/Passport'),
    session = require('express-session'),
    flash = require('connect-flash'),
    // i18n = require('i18n-2'),
    i18n = require( "i18n" ),
    Application = require("./core/Application"),
    i18nRoutes = require( "i18n-node-angular" );
    server = require('http').Server(app);

var instance = (process.argv[2] !== undefined ? process.argv[2] : "default");

app.use(session({ secret: KEY, name: "TerraMA2WebApp_" + instance, resave: false, saveUninitialized: false }));

app.use(flash());

// Setting internationalization
i18n.configure({
  locales        : [ "en_US", "pt_BR", "fr_FR", "es_ES"],
  directory      : __dirname + "/locales",
  objectNotation : true
});

// Get base url from environment and store in Express.
app.locals.BASE_URL = Application.getContextConfig().basePath;

// Set SWIG template engine
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('view engine', 'html');

// Set template directories
app.set('views', __dirname + '/views');

// The order of these calls matters!
app.use(i18n.init);
app.use(i18nRoutes.getLocale);

i18nRoutes.configure(app, {"extension": ".json", directory : __dirname + "/locales/"});

app.use(function(req, res, next) {
  var configurations = JSON.parse(fs.readFileSync(path.join(__dirname, './config/instances/' + instance + '.json'), 'utf8'));
  res.locals.toolsMenu = configurations.toolsMenu;
  next();
});

// set up the internacionalization middleware
app.use(function(req, res, next) {
  res.locals.errorMessage = req.flash('error');
  next();
});

app.use(bodyParser.json({ limit: "50mb" }));
app.use(bodyParser.urlencoded({ limit: "50mb", extended: true }));
app.use(methodOverride('_method'));

app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'public')));
app.use(app.locals.BASE_URL + "helper", express.static(path.join(__dirname, './../helper')));

var externals = JSON.parse(fs.readFileSync(path.join(__dirname, './externals.json'), 'utf8'));

externals.forEach(function(external) {
  app.use(app.locals.BASE_URL + external.publicUrl, express.static(path.join(__dirname, external.path)));
});

passport.setupPassport(app);

load('controllers')
  .then('routes')
  .into(app);

module.exports = app;
