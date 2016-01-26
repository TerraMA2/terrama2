const KEY = 'terrama2.sid';

const TERRAMA2_PATH = "/";

var express = require('express'),
    path = require('path'),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override'),
    app = express(),
    load = require('express-load'),
    swig = require('swig'),
    server = require('http').Server(app);

// Set SWIG template engine
app.engine('html', swig.renderFile);
app.set('view engine', 'html');

// Set template directories
app.set('views', __dirname + '/views');


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
