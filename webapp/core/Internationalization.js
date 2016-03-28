var i18n = require('i18n-2');
var options = {
  // setup some locales - other locales default to vi silently
  locales: ['pt_BR', 'en_US', 'es_ES'],
  // set the default locale
  defaultLocale: 'en_US',
  // set the cookie name
  cookieName: 'locale',
  // allow to change language by query string. i.e: url?lang=es
  query: true
};

var tr = new i18n(options);
// Setting current OS locale
tr.setLocale(process.env.LANG.split('.')[0]);

module.exports = tr;