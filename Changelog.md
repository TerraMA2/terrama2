# 4.0.5

# New Features:
- Monitored object analysis operators for counting pixels by value or range of values
- Use geometry as filter in MO analysis (instead of a combination of bounding box and RTree)
- Time parameters in analysis can have a '+' modifier, this will make the data range be expanded to 00:00h of the start date
- Interpolate historical data
- Docker:
  - development environment
  - package generation


# Enhancements:
- Use Yarn instead of npm for web dependencies
- Smaller storage of log messages
- Allow temporal geotiff

# Bugfix:
- Improved limit of threads
- Fix legend for for raster views
- Fix automatic schedule

# 4.0.4

# New Features:
- Status selector on list screens
- Beta version of the interpolator
- WepApp id - allow more then one terrama2 instance in the same server
- Build on Windows (not working, just building - no interpolator)
- Analysis table at the WebMonitor
- DCP driver for storing data in a single table

# Enhancements:
- Improved error messages on errors of the raster analysis output
- Improved status timeout
- Improved tcp message system

# Bugfix:
- Fix DCP behavior - many bugs including activate and deactivate
- Fix input/output attribute names
- Fix alert "internal error"
- Fix recollecting data - mostly common in DCP
- Dynamic shapefile general review
- Fix problem with projects that had special characters in the name
- Update analysis output when changing monitored object DataProvider
- Fix status when alter Service of a process
- Crop raster of any projection
- Improve webapp-service comunication


# 4.0.3

# New Features:
- Allow static raster view without style
- Occurrence picture on the WebMonitor

# Enhancements:
- Validate projections (SRID) on the WebApp

# Bugfix:
- Export analysis shapefile from WebMonitor (fix wrong DateTime)
- Status circle flashing at different project on the WebMonitor
- View from raster with timezone
- Automatic schedule from analysis to other services
- Don't start disabled activities when scheduled as automatic
- Respect start time on automatic activities

# 4.0.2

# New Features:
- Terralib 5.3 - bugfixes and code improvements
- Auto update database - when updating TerraMA² to a new version the database is automatically updated

# Enhancements:
- Improved system timezone identification
- Using shp2pgsql to import shapefiles to the database - better geometry type identification
- Improved versioning system
- versioning debian packages names - won't remove older versions automatically

# Bugfix:
- Fix alert email tables
  - floating point numbers representation
  - Null values representation
  - Wrong identifiers of monitored objects
- Fix filter and crop raster files - only worked with EPGS:4326
- Fix sending emails on risk level
- Log collected date/time for each file
- Fix alert emails error with spaces and linebrakes

# 4.0.1

# New Features:
- Script to update database from v4.0.0

# Enhancements:
- Intersection using threads
- update doc

# Bugfix:
- Fix encoding when importing static data
- Improved status when collecting multiple files
- Fix owner when creating function for DCP view
- Fix pre-defined styles
- Remove invalid label from 'Notify on legend level'
- Fix crash when creating a view from a table with a GEOMETRY column

# 4.0.0

# New Features:
- Docker container
- Doc installer
- Vector data pre-defined styles
- External tools menu item
- Current situation example page

# Enhancements:
- Using ogr2ogr instead of shp2pgsql
- Review of wildfire program driver
- Review raster pre-defined styles
- Better raster layer names in geoserver

# Bugfix:
- Collect from postgis
- Analysis statistic operations
- Postgres numeric as double
- Fix creation of output table without original restrictions
- Fix status color for collectors

# RC4

# New Features:
- Sample Docker files
- View
  - DCP view
- Alert
  - Attach images to alert emails
- WebMonitor
  - Auto update layers
  - Uncheck all layers
  - Extra default template
- WebApp
  - Help and docs
  - Auto generate styles and alert levels
  - Auto complete distinct values from table
  - Import DataSeries from other projects

# Enhancements:
- Collector
  - Process one file at a time
- View
  - Improved geoserver layer names
- Services
  - Fix decompress
  - Improvements in error messages
  - Using EPSG:4326 for spatial filter (avoid local projections bugs)
- WebApp
  - Improved mask validation (regex was crashing the browser)
  - Advanced filter at status screen

# Bugfix:
- Collector
  - Fix spatial filter
- Analysis
  - Fix time units conversions
  - Fix log id
- WebApp
  - Update project cache on import project
  - Fix DCP import erros
  - Check dependencies of Data Server before removal
  - Fix timestamp error on filter
  - Import and export intersactions
  - Fix styles for one band raster images


# RC3

# New Features:
- Dynamic geometric object
  - Collect and analysis
- DCP analysis view
- Pre-defined styles library for tif - INPE WildFire program climate styles
- Upload shapefile to postgis

# Enhancements:
- WebApp
  - projects combo
  - icons and components reviews
- Drop use of bower, every dependency installed by npm
- Data servers validation
- examples review
- remove compilation warnings
- Make install for non root users
Splitting project and language selection per session

# Bugfix:
- icons in firefox
- copy python scripts during installation

# Known issues
- Collecting data from a postgis table to another crashes the app

# RC2

# New Features:
- Protected projects
- get_analysis_date operator
- Import project in command line
- Http Data provider
- Check service status to manipulate objects
- Import shapefile to database

# Enhancements:
- Update terralib to 5.2.2
- Wild fire risk analysis example
- Better python interpreter for usage outside analysis

# Bugfix:
- Fix raster band index in views
- Check objects name when importing a project
- Fix idle connection to the database
- Include missing dependencies


# RC1

# New Features:
- Analysis:
   - DCP analysis
   - get_analysis_date function
- WebMonitor:
   - Animation of temporal layers
   - Logged user can save current state
   - Export occurrences and raster
- View:
   - System styles support
      - WindBarbs
- General:
   - Data provider HTTP (single files only, no mask support)

# Enhancements:
- Integration test for analysis
- Crop raster by dataseries

# Bugfix
- Create view of read only files
- Review of database connections (some connections were never closed)
- Improved reading of CTL grads files (VARS field had some bugs)
- Occurrence postgis dataseries now have two required fields
- Fix projection in views of monitored objects
- Fix boundingbox of views of rasters (resolution problems)
- Fixed precision in analysis execution date

# Beta 3

## New Features:
- Start services in servers with no display
- Databases are never removed on uninstall
 - Created script to remove the database
- Ascii grid semantics
- Allow user CQL styles to view
- Add value operator allows string values
- Allow same risk for different alerts
- Renaming Risk to Legend
- HTTP server
- WebMonitor:
 - Disable layers when can't connect to geoserver
 - Get feature info with click
 - Data table
 - Export occurrences data
 - List layers by project

## Enhancements:
- Analysis code editor review
- Continue analysis even with invalid monitored objects (ie: invalid geometry)
- Processing time in status display
- Organizing semantics in different files
- Check connection with geoserver and smtp servers
- Allow equal names in different projects
- Automatic translation check
- Include dataseries identifier in alert report
- Opacity in view style colors
- Huge webmonitor refactor (backend)
- Won't automatically start process when dependency process fails or has invalid output
- Store NULL instead of NAN in analysis results

## Bugfix:
- Fix geometry raser instersection
- Review of installation scripts using `make install`
- Fix DCP influence operators
- Fix DCP statistics operators
- Fix auto complete operators
- Read lowercase tags in CTL grads files
- fix grads multiplier
- Using pm2 logrotate to avoid giant log files
- Fix notification conditions
- Fix drawing with NAN values
- Fix tiles maps with different styles per tile
- only include comparison column in the report if there are enough dates
- fix view creation with wrong database password
- fix alert levels

## Known bugs:
- When starting services in servers with no display, the services screen doesn't update correctly.
- HTTP server doesn't allow masks, only fixed names can be used.


# Beta 2

## New Features:
- Alert and notification service
- Conditional schedule
- Icons in the Webapp
- Pre-compiled packages for Ubuntu 14.04 & 16.04
- Generic driver for CSV files
  - Occurrence
  - DCP
- Email and PDF report as attachment

## Enhancements:
- Import DCP configuration file
- Better filter by value
  - Protection from sql injections
- Allow compilation of the service without all modules
  - Allow compiling modules without some libraries (VMime, Qt < 5.8)
- Update Terralib to 5.2.1
- Use of terralib CurlWrapper for FTP downloads
- New operators for analysis:
  - grid zonal forecast num and list
  - grid zonal forecast interval num and list
- New options for FTP:
  - Timeout
  - Active mode
- Analises operators menus
- New service status: Interrupted


## bugfix:
- Fix service error messages
  - Better messages
  - Don't give out of context errors
- Fix project export/import
- Fix Uri tags for map and email servers
- Using timezone from dataset in DataRetriever downloads
