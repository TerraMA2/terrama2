const KEY = 'terrama2.sid';

var express = require('express'),
    path = require('path'),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override'),
    app = express(),
    load = require('express-load'),
    swig = require('swig'),
    // i18n = require('i18n-2'),
    i18n = require( "i18n" );
    i18nRoutes = require( "i18n-node-angular" );
    server = require('http').Server(app);


i18n.configure( {
  // setup some locales - other locales default to en silently
  locales        : [ "en", "pt", "fr", "es" ],
  directory      : __dirname + "/locales",
  objectNotation : true
} );


// Set SWIG template engine
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('view engine', 'html');

// Set template directories
app.set('views', __dirname + '/views');

// The order of these calls matters!
app.use(i18n.init);
app.use(i18nRoutes.getLocale);

// This line should be removed with express 4.0
i18nRoutes.configure(app, {"extension": ".js", directory : __dirname + "/locales/"});

// Set up the internacionalization
// i18n.expressBind(app, {
//   // setup some locales - other locales default to vi silently
//   locales: ['pt', 'en', 'es'],
//   // set the default locale
//   defaultLocale: 'en',
//   // set the cookie name
//   cookieName: 'locale',
//   // allow to change language by query string. i.e: url?lang=es
//   query: true
// });

// set up the internacionalization middleware
app.use(function(req, res, next) {
  // req.i18n.setLocaleFromQuery();
  // req.i18n.setLocaleFromCookie();
  next();
});

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(methodOverride('_method'));

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "X-Requested-With");
  next();
});

app.use(express.static(path.join(__dirname, 'public')));

load('controllers')
  .then('routes')
  .into(app);

module.exports = app;
