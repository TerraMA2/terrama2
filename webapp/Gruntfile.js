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
          generateSourceMaps: true,
          optimize: "none", // It does not minify
          paths: {
            TerraMA2WebApp: "angular",
            TerraMA2WebAppTemplates: "../dist"
          },
          include: [
            "../../bower_components/almond/almond",
            "TerraMA2WebApp/application"
          ],
          wrap: {
            startFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.start",
            endFile: SCRIPTS_PATH + "Wrap.TerraMA2WebApp.end"
          }
        }
      }
    },
    clean: [DEST_PATH],
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
    cssmin: {
      TerraMA2WebApp: {
        files: [{
          expand: true,
          cwd: 'public/stylesheets',
          src: ['*.css'],
          dest: DEST_PATH,
          ext: '.min.css'
        }]
      }
    },
    watch: {
      css: {
        files: ["public/stylesheets/**/*.css"],
        tasks: ["cssmin"],
        options: {
          // It allows to compile only when needed
          spawn: false
        }
      },
      Gruntfile: {
        files: ["Gruntfile.js"],
        tasks: ["default"],
        options: {
          spawn: false
        }
      },
      js: {
        files: [SCRIPTS_PATH + "/**/*.js"],
        tasks: ["requirejs"],
        options: {
          // It allows to compile only when needed
          spawn: false
        }
      },
      templates: {
        files: [SCRIPTS_PATH + "/angular/**/**.html"],
        tasks: ["copy"],
        options: {
          // It allows to compile only when needed
          spawn: false
        }
      }
    }  
  });
  // Print helper to detect which file has beed changed
  grunt.event.on('watch', function(action, filepath, target) {
    grunt.log.writeln(target + ': ' + filepath + ' has ' + action);
  });

  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-contrib-clean');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.registerTask('default', ['clean', 'copy', 'requirejs', 'cssmin']);
};