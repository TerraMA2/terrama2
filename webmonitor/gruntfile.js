module.exports = function(grunt) {

  // Project configuration
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    banner: '/*! <%= pkg.name %> <%= pkg.version %> | (C) 2007, <%= grunt.template.today("yyyy") %> National Institute For Space Research (INPE) - Brazil | https://github.com/TerraMA2/terrama2/blob/master/LICENSE */',
    requirejs: {
      TerraMA2WebMonitor: {
        options: {
          baseUrl: "public/javascripts",
          out: "public/dist/TerraMA2WebMonitor.min.js",
          preserveLicenseComments: false,
          paths: {
            TerraMA2WebComponents: "../../../webcomponents/dist/TerraMA2WebComponents.min"
          },
          include: [
            '../../node_modules/almond/almond',
            'Startup',
            'TerraMA2WebMonitor'
          ]
        }
      }
    },
    cssmin: {
      TerraMA2WebMonitor: {
        files: [{
          expand: true,
          cwd: 'public/stylesheets',
          src: ['*.css'],
          dest: 'public/dist',
          ext: '.min.css'
        }]
      }
    },
    usebanner: {
      TerraMA2WebMonitor: {
        options: {
          position: 'top',
          banner: '<%= banner %>',
          linebreak: true
        },
        files: {
          src: [ 'public/dist/*' ]
        }
      }
    },
    jsdoc : {
      TerraMA2WebMonitor: {
        src: ['README.md'],
        options: {
          configure: 'config/jsdoc/config.json',
          destination: 'public/docs'
        }
      }
    },
    watch: {
      css: {
        files: ["public/stylesheets/*.css"],
        tasks: ["cssmin", "usebanner"],
        options: {
          // It allows to compile only when needed
          spawn: false
        }
      },
      gruntfile: {
        files: ["gruntfile.js"],
        tasks: ["default"],
        options: {
          spawn: false
        }
      },
      js: {
        files: ["public/javascripts/*.js", "public/javascripts/components/*.js"],
        tasks: ["requirejs", "usebanner"],
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

  // Load the plugins.
  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-banner');
  grunt.loadNpmTasks('grunt-jsdoc');

  // Default tasks.
  grunt.registerTask('default', ['requirejs', 'cssmin', 'usebanner:TerraMA2WebMonitor', 'jsdoc']);

};
