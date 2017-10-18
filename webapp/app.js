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

Application.setCurrentContext(process.argv[2]);

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







  app.use(app.locals.BASE_URL + 'externals/jquery', express.static(path.join(__dirname, 'node_modules/jquery')));
  app.use(app.locals.BASE_URL + 'externals/moment', express.static(path.join(__dirname, 'node_modules/moment')));
  app.use(app.locals.BASE_URL + 'externals/intl-tel-input', express.static(path.join(__dirname, 'node_modules/intl-tel-input')));
  app.use(app.locals.BASE_URL + 'externals/angular-material', express.static(path.join(__dirname, 'node_modules/angular-material')));
  app.use(app.locals.BASE_URL + 'externals/AdminLTE', express.static(path.join(__dirname, 'node_modules/admin-lte')));
  app.use(app.locals.BASE_URL + 'externals/font-awesome', express.static(path.join(__dirname, 'node_modules/font-awesome')));
  app.use(app.locals.BASE_URL + 'externals/datatables', express.static(path.join(__dirname, 'node_modules/datatables')));
  app.use(app.locals.BASE_URL + 'externals/angular', express.static(path.join(__dirname, 'node_modules/angular')));
  app.use(app.locals.BASE_URL + 'externals/i18n-node-angular', express.static(path.join(__dirname, 'node_modules/i18n-node-angular')));
  app.use(app.locals.BASE_URL + 'externals/ng-file-upload', express.static(path.join(__dirname, 'node_modules/ng-file-upload/dist')));
  app.use(app.locals.BASE_URL + 'externals/eonasdan-bootstrap-datetimepicker', express.static(path.join(__dirname, 'node_modules/eonasdan-bootstrap-datetimepicker')));
  app.use(app.locals.BASE_URL + 'externals/angular-ui-select', express.static(path.join(__dirname, 'node_modules/ui-select')));
  app.use(app.locals.BASE_URL + 'externals/eonasdan-bootstrap-datetimepicker', express.static(path.join(__dirname, 'node_modules/eonasdan-bootstrap-datetimepicker')));
  app.use(app.locals.BASE_URL + 'externals/angular-eonasdan-datetimepicker', express.static(path.join(__dirname, 'node_modules/angular-eonasdan-datetimepicker')));
  app.use(app.locals.BASE_URL + 'externals/tinycolor', express.static(path.join(__dirname, 'node_modules/tinycolor2')));
  app.use(app.locals.BASE_URL + 'externals/angular-color-picker', express.static(path.join(__dirname, 'node_modules/angularjs-color-picker')));
  app.use(app.locals.BASE_URL + 'externals/angular-tree-control', express.static(path.join(__dirname, 'node_modules/angular-tree-control')));
  app.use(app.locals.BASE_URL + 'externals/ace-builds', express.static(path.join(__dirname, 'node_modules/ace-builds')));
  app.use(app.locals.BASE_URL + 'externals/angular-sanitize', express.static(path.join(__dirname, 'node_modules/angular-sanitize')));
  app.use(app.locals.BASE_URL + 'externals/tv4', express.static(path.join(__dirname, 'node_modules/tv4')));
  app.use(app.locals.BASE_URL + 'externals/objectpath', express.static(path.join(__dirname, 'node_modules/objectpath')));
  app.use(app.locals.BASE_URL + 'externals/angular-schema-form', express.static(path.join(__dirname, 'node_modules/angular-schema-form')));
  app.use(app.locals.BASE_URL + 'externals/angular-aria', express.static(path.join(__dirname, 'node_modules/angular-aria')));
  app.use(app.locals.BASE_URL + 'externals/angular-animate', express.static(path.join(__dirname, 'node_modules/angular-animate')));
  app.use(app.locals.BASE_URL + 'externals/angular-wizard', express.static(path.join(__dirname, 'node_modules/angular-wizard')));
  app.use(app.locals.BASE_URL + 'externals/angular-xeditable', express.static(path.join(__dirname, 'node_modules/angular-xeditable')));
  app.use(app.locals.BASE_URL + 'externals/angular-ui-router', express.static(path.join(__dirname, 'node_modules/angular-ui-router')));









passport.setupPassport(app);

load('controllers')
  .then('routes')
  .into(app);

module.exports = app;
