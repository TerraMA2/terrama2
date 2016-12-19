const KEY = 'terrama2.sid';

var express = require('express'),
    path = require('path'),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override'),
    app = express(),
    load = require('express-load'),
    swig = require('swig'),
    passport = require('./config/Passport'),
    session = require('express-session'),
    flash = require('connect-flash'),
    Database = require('./config/Database'),
    User = Database.getORM().import('./models/User.js'),
    // i18n = require('i18n-2'),
    i18n = require( "i18n" );
    i18nRoutes = require( "i18n-node-angular" );
    server = require('http').Server(app);

app.use(session({ secret: KEY, resave: false, saveUninitialized: false }));
app.use(function(req, res, next) {
  if(req.session.passport !== undefined && req.session.passport.user !== undefined) {
    User.findOne({
      where: { 'id': req.session.passport.user }
    }).then(function(userObj) {
      if(userObj != null) {
        res.locals.currentUser = {
          id: userObj.id,
          name: userObj.name,
          email: userObj.email,
          cellphone: userObj.cellphone,
          username: userObj.username,
          administrator: userObj.administrator
        };
      } else {
        res.locals.currentUser = null;
      }
    });
  } else {
    res.locals.currentUser = null;
  }

  next();
});

app.use(flash());

// Setting internationalization
i18n.configure( {
  locales        : [ "en_US", "pt_BR", "fr_FR", "es_ES"],
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

i18nRoutes.configure(app, {"extension": ".json", directory : __dirname + "/locales/"});

// set up the internacionalization middleware
app.use(function(req, res, next) {
  res.locals.errorMessage = req.flash('error');
  next();
});

app.use(bodyParser.json({ limit: '50mb' }));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(methodOverride('_method'));

app.use('/bower_components', express.static('bower_components'));
app.use(express.static(path.join(__dirname, 'public')));

passport.setupPassport(app);

load('controllers')
  .then('routes')
  .into(app);

module.exports = app;
