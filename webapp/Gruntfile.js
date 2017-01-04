module.exports = function(grunt) {
  // Destination Path (terrama2/webapp/dist)
  var SCRIPTS_PATH = "public/javascripts/";
  var DEST_PATH = "public/dist/";
  // Project configuration
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    banner: '/*! <%= pkg.name %> <%= pkg.version %> | (C) 2007, <%= grunt.template.today("yyyy") %> National Institute For Space Research (INPE) - Brazil | https://github.com/TerraMA2/terrama2/blob/master/LICENSE */',
    requirejs: {
      TerraMA2WebApp: {
        options: {
          baseUrl: SCRIPTS_PATH,
          out: DEST_PATH + "terrama2-webapp.js",
          preserveLicenseComments: false,
          optimize: "none", // It does not minify
          paths: {
            TerraMA2WebApp: "angular",
            TerraMA2WebAppTemplates: "../dist"
          },
          include: [
            "../../bower_components/almond/almond",
            "TerraMA2WebApp/application",
            // "TerraMA2WebAppTemplates/templates.min"
          ],
          wrap: {
            startFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.start",
            endFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.end"
          }
        }
      },
      TerraMA2WebAppMin: {
        options: {
          baseUrl: SCRIPTS_PATH,
          out: DEST_PATH + "terrama2-webapp.min.js",
          preserveLicenseComments: false,
          optimize: "none", // It does not minify
          paths: {
            TerraMA2WebApp: "angular",
            TerraMA2WebAppTemplates: "../dist"
          },
          include: [
            "../../bower_components/almond/almond",
            "TerraMA2WebApp/application",
            // "TerraMA2WebAppTemplates/templates.min"
          ],
          wrap: {
            startFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.start",
            endFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.end"
          }
        }
      }
    },
    copy: {
      main: {
        files: [
          {
            cwd: SCRIPTS_PATH + 'angular',
            src: '**/*.html',
            dest: DEST_PATH + 'templates',
            expand: true
          }
        ]
      }
    },
    clean: ['public/dist/templates.min.js']
  });

  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.registerTask('default', ['copy', 'requirejs']);
};